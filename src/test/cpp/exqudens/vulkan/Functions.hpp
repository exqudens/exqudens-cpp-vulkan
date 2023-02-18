#pragma once

#include <chrono>
#include <functional>
#include <stdexcept>

#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Vertex.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"
#include "exqudens/vulkan/Context.hpp"

namespace exqudens::vulkan {

  class Functions {

    public:

      inline static void init(
          Context& context,
          const std::vector<const char*>& enabledExtensionNames,
          const std::function<VkSurfaceKHR(VkInstance)>& createSurfaceFunction,
          const uint32_t& queryPoolSize,
          const uint32_t& width,
          const uint32_t& height
      );

      inline static void initSwapchain(
          Context& context,
          const uint32_t& width,
          const uint32_t& height
      );

  };

}

namespace exqudens::vulkan {

  void Functions::init(
      Context& context,
      const std::vector<const char*>& enabledExtensionNames,
      const std::function<VkSurfaceKHR(VkInstance)>& createSurfaceFunction,
      const uint32_t& queryPoolSize,
      const uint32_t& width,
      const uint32_t& height
  ) {
    try {
      context.instance = Instance::builder()
          .addEnabledLayerName("VK_LAYER_KHRONOS_validation")
          .setEnabledExtensionNames(enabledExtensionNames)
          .setApplicationInfo(
              vk::ApplicationInfo()
                  .setPApplicationName("Exqudens Application")
                  .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                  .setPEngineName("Exqudens Engine")
                  .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                  .setApiVersion(VK_API_VERSION_1_0)
          )
          .setMessengerCreateInfo(
              MessengerCreateInfo()
                  .setExceptionSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                  //.setOutSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
                  .setToStringFunction(&Utility::toString)
          )
          .setDebugUtilsMessengerCreateInfo(
              vk::DebugUtilsMessengerCreateInfoEXT()
                  .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                  .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
          )
      .build();

      if (createSurfaceFunction) {
        auto vkInstance = static_cast<VkInstance>(*context.instance.reference());
        VkSurfaceKHR vkSurface = createSurfaceFunction(vkInstance);
        context.surface = Surface::builder()
            .setInstance(context.instance.value)
            .setVkSurface(vkSurface)
        .build();
      }

      vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceHostQueryResetFeatures> tmpPhysicalDeviceFeatures = {
          vk::PhysicalDeviceFeatures2()
              .setFeatures(
                  vk::PhysicalDeviceFeatures()
                      .setSamplerAnisotropy(true)
                      .setSampleRateShading(true)
              ),
          vk::PhysicalDeviceHostQueryResetFeatures()
              .setHostQueryReset(true)
      };
      context.physicalDeviceFeatures = tmpPhysicalDeviceFeatures;

      context.physicalDevice = PhysicalDevice::builder()
          .setInstance(context.instance.value)
          .setSurface(context.surface.value)
          .addEnabledExtensionName(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
          .addEnabledExtensionName(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)
          .addQueuePriority(1.0f)
          .setIsSuitableFunction([&context](const vk::raii::PhysicalDevice& p) {
            bool result = false;
            std::vector<vk::QueueFamilyProperties> properties = p.getQueueFamilyProperties();
            bool computeValid = false;
            bool transferValid = false;
            bool graphicsValid = false;
            bool presentValid = false;
            for (size_t i = 0; i < properties.size(); i++) {
              if (properties[i].queueFlags & vk::QueueFlagBits::eCompute) {
                computeValid = true;
              }
              if (properties[i].queueFlags & vk::QueueFlagBits::eTransfer) {
                transferValid = true;
              }
              if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics && properties[i].timestampValidBits > 0) {
                graphicsValid = true;
              }
              if (context.surface.value && p.getSurfaceSupportKHR(static_cast<uint32_t>(i), *context.surface.reference())) {
                presentValid = true;
              }

              result = computeValid && transferValid && graphicsValid && presentValid;
              if (result) {
                break;
              }
            }
            if (!result) {
              return result;
            }
            auto f = p.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceHostQueryResetFeatures>();
            result = f.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy;
            if (!result) {
              return result;
            }
            result = f.get<vk::PhysicalDeviceFeatures2>().features.sampleRateShading;
            if (!result) {
              return result;
            }
            result = f.get<vk::PhysicalDeviceHostQueryResetFeatures>().hostQueryReset;
            if (!result) {
              return result;
            }
            return result;
          })
      .build();

      context.physicalDeviceTimestampPeriod = context.physicalDevice.reference().getProperties().limits.timestampPeriod;

      context.physicalDeviceMsaaSamples = Utility::getMaxUsableSampleCount(
          {
              context.physicalDevice.reference().getProperties().limits.framebufferColorSampleCounts,
              context.physicalDevice.reference().getProperties().limits.framebufferDepthSampleCounts
          }
      );

      context.device = Device::builder()
          .setPhysicalDevice(context.physicalDevice.value)
          .setCreateInfo(
              vk::DeviceCreateInfo()
                  .setQueueCreateInfos(context.physicalDevice.uniqueQueueCreateInfos)
                      //.setPEnabledFeatures(&physicalDevice.features)
                  .setPEnabledExtensionNames(context.physicalDevice.enabledExtensionNames)
                  .setPEnabledLayerNames(context.instance.enabledLayerNames)
                      //.setPNext(physicalDevice.hostQueryResetFeatures.has_value() ? &physicalDevice.hostQueryResetFeatures.value() : nullptr)
                  .setPNext(&context.physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>())
          )
      .build();

      context.transferQueue = Queue::builder()
          .setDevice(context.device.value)
          .setFamilyIndex(context.physicalDevice.transferQueueCreateInfos.front().queueFamilyIndex)
      .build();

      context.graphicsQueue = Queue::builder()
          .setDevice(context.device.value)
          .setFamilyIndex(context.physicalDevice.graphicsQueueCreateInfos.front().queueFamilyIndex)
      .build();

      context.presentQueue = Queue::builder()
          .setDevice(context.device.value)
          .setFamilyIndex(context.physicalDevice.presentQueueCreateInfos.front().queueFamilyIndex)
      .build();

      context.transferCommandPool = CommandPool::builder()
          .setDevice(context.device.value)
          .setCreateInfo(
              vk::CommandPoolCreateInfo()
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  .setQueueFamilyIndex(context.transferQueue.familyIndex)
          )
      .build();

      context.graphicsCommandPool = CommandPool::builder()
          .setDevice(context.device.value)
          .setCreateInfo(
              vk::CommandPoolCreateInfo()
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  .setQueueFamilyIndex(context.graphicsQueue.familyIndex)
          )
      .build();

      context.transferCommandBuffer = CommandBuffer::builder()
          .setDevice(context.device.value)
          .setCreateInfo(
              vk::CommandBufferAllocateInfo()
                  .setCommandPool(*context.transferCommandPool.reference())
                  .setCommandBufferCount(1)
                  .setLevel(vk::CommandBufferLevel::ePrimary)
          )
      .build();

      context.descriptorSetLayout = DescriptorSetLayout::builder()
          .setDevice(context.device.value)
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(0)
                  .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eFragment)
          )
      .build();

      context.queryPool = QueryPool::builder()
          .setDevice(context.device.value)
          .setCreateInfo(
              vk::QueryPoolCreateInfo()
                  /*.setPipelineStatistics(vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations)*/
                  .setQueryType(vk::QueryType::eTimestamp)
                  .setQueryCount(queryPoolSize)
          )
      .build();

      initSwapchain(context, width, height/*, textureWidth, textureHeight, textureDepth*/);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::initSwapchain(
      Context& context,
      const uint32_t& width,
      const uint32_t& height
  ) {
    try {
      context.device.reference().waitIdle();

      context.inFlightFences.clear();
      context.renderFinishedSemaphores.clear();
      context.imageAvailableSemaphores.clear();
      context.graphicsCommandBuffers.clear();
      context.descriptorSets.clear();
      context.sampler.value.reset();
      context.descriptorPool.value.reset();
      context.samplerImageViews.clear();
      context.samplerImages.clear();
      context.swapchainFramebuffers.clear();
      context.pipeline.value.reset();
      context.renderPass.value.reset();
      context.swapchainImageViews.clear();
      context.swapchain.value.reset();

      context.swapchain = Swapchain::builder()
          .setDevice(context.device.value)
          .addGraphicsQueueFamilyIndex(context.physicalDevice.graphicsQueueCreateInfos.front().queueFamilyIndex)
          .addPresentQueueFamilyIndex(context.physicalDevice.presentQueueCreateInfos.front().queueFamilyIndex)
          .setCreateInfo(
              Utility::swapChainCreateInfo(
                  context.physicalDevice.reference(),
                  context.surface.reference(),
                  width,
                  height
              )
          )
      .build();

      context.swapchainImageViews.resize(context.swapchain.reference().getImages().size());
      for (size_t i = 0; i < context.swapchainImageViews.size(); i++) {
        context.swapchainImageViews.at(i) = ImageView::builder()
            .setDevice(context.device.value)
            .setCreateInfo(
                vk::ImageViewCreateInfo()
                    .setFlags({})
                    .setImage(static_cast<vk::Image>(context.swapchain.reference().getImages().at(i)))
                    .setViewType(vk::ImageViewType::e2D)
                    .setFormat(context.swapchain.createInfo.imageFormat)
                    .setComponents({})
                    .setSubresourceRange(
                        vk::ImageSubresourceRange()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setBaseMipLevel(0)
                            .setLevelCount(1)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            )
        .build();
      }

      context.renderPass = RenderPass::builder()
          .setDevice(context.device.value)
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(context.swapchain.createInfo.imageFormat)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
          )
          .addSubpass(
              SubpassDescription()
                  .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                  .addColorAttachment(
                      vk::AttachmentReference()
                          .setAttachment(0)
                          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                  )
          )
          .addDependency(
              vk::SubpassDependency()
                  .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                  .setDstSubpass(0)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                  .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
                  .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                  .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
          )
      .build();

      context.pipeline = Pipeline::builder()
          .setDevice(context.device.value)
          .addPath("resources/shader/shader-10.vert.spv")
          //.addStage(TestUtils::createStage(context.device, shaders, "resources/shader/shader-4.vert.spv"))
          .addPath("resources/shader/shader-10.frag.spv")
          .addSetLayout(*context.descriptorSetLayout.reference())
          .setGraphicsCreateInfo(
              GraphicsPipelineCreateInfo()
                  .setRenderPass(*context.renderPass.reference())
                  .setSubpass(0)
                  .setVertexInputState(
                      PipelineVertexInputStateCreateInfo()
                  )
                  .setInputAssemblyState(
                      vk::PipelineInputAssemblyStateCreateInfo()
                          .setTopology(vk::PrimitiveTopology::eTriangleList)
                          .setPrimitiveRestartEnable(false)
                  )
                  .setViewportState(
                      PipelineViewportStateCreateInfo()
                          .addViewport(
                              vk::Viewport()
                                  .setWidth((float) context.swapchain.createInfo.imageExtent.width)
                                  .setHeight((float) context.swapchain.createInfo.imageExtent.height)
                                  .setMinDepth(0.0)
                                  .setMaxDepth(1.0)
                                  .setX(0.0)
                                  .setY(0.0)
                          )
                          .addScissor(
                              vk::Rect2D()
                                  .setOffset(
                                      vk::Offset2D()
                                          .setX(0)
                                          .setY(0)
                                  )
                                  .setExtent(
                                      vk::Extent2D()
                                          .setWidth(context.swapchain.createInfo.imageExtent.width)
                                          .setHeight(context.swapchain.createInfo.imageExtent.height)
                                  )
                          )
                  )
                  .setRasterizationState(
                      vk::PipelineRasterizationStateCreateInfo()
                          .setDepthClampEnable(false)
                          .setRasterizerDiscardEnable(false)
                          .setPolygonMode(vk::PolygonMode::eFill)
                          .setCullMode(vk::CullModeFlagBits::eBack)
                          .setFrontFace(vk::FrontFace::eClockwise)
                          .setLineWidth(1.0)
                          .setDepthBiasEnable(false)
                          .setDepthBiasConstantFactor(0.0)
                          .setDepthBiasClamp(0.0)
                          .setDepthBiasSlopeFactor(0.0)
                  )
                  .setMultisampleState(
                      vk::PipelineMultisampleStateCreateInfo()
                          .setRasterizationSamples(vk::SampleCountFlagBits::e1)
                          .setSampleShadingEnable(false)
                          .setMinSampleShading(1.0)
                          //.setSampleShadingEnable(true)
                          //.setMinSampleShading(0.2f)
                          .setPSampleMask(nullptr)
                          .setAlphaToCoverageEnable(false)
                          .setAlphaToOneEnable(false)
                  )
                  .setColorBlendState(
                      PipelineColorBlendStateCreateInfo()
                          .setLogicOpEnable(false)
                          .setLogicOp(vk::LogicOp::eCopy)
                          .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f})
                          .addAttachment(
                              vk::PipelineColorBlendAttachmentState()
                                  .setBlendEnable(false)
                                  .setColorBlendOp(vk::BlendOp::eAdd)
                                  .setSrcColorBlendFactor(vk::BlendFactor::eOne)
                                  .setDstColorBlendFactor(vk::BlendFactor::eZero)
                                  .setAlphaBlendOp(vk::BlendOp::eAdd)
                                  .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                                  .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                                  .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
                          )
                  )
          )
      .build();

      context.swapchainFramebuffers.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.swapchainFramebuffers.size(); i++) {
        context.swapchainFramebuffers.at(i) = Framebuffer::builder()
            .setDevice(context.device.value)
            .addAttachment(*context.swapchainImageViews.at(i).reference())
            .setCreateInfo(
                vk::FramebufferCreateInfo()
                    .setRenderPass(*context.renderPass.reference())
                    .setWidth(context.swapchain.createInfo.imageExtent.width)
                    .setHeight(context.swapchain.createInfo.imageExtent.height)
                    .setLayers(1)
            )
        .build();
      }

      context.samplerImages.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.swapchainImageViews.size(); i++) {
        context.samplerImages.at(i) = Image::builder()
            .setPhysicalDevice(context.physicalDevice.value)
            .setDevice(context.device.value)
            .setCreateInfo(
                vk::ImageCreateInfo()
                    .setImageType(vk::ImageType::e2D)
                    .setFormat(vk::Format::eR8G8B8A8Srgb)
                    .setExtent(
                        vk::Extent3D()
                            .setWidth(/*textureWidth*/ context.swapchain.createInfo.imageExtent.width)
                            .setHeight(/*textureHeight*/ context.swapchain.createInfo.imageExtent.height)
                            .setDepth(1)
                    )
                    .setMipLevels(1)
                    .setArrayLayers(1)
                    .setSamples(vk::SampleCountFlagBits::e1)
                    .setTiling(vk::ImageTiling::eOptimal)
                    .setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled)
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setQueueFamilyIndices({})
            )
            .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
            .build();
      }

      context.samplerImageViews.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.swapchainImageViews.size(); i++) {
        context.samplerImageViews.at(i) = ImageView::builder()
            .setDevice(context.device.value)
            .setCreateInfo(
                vk::ImageViewCreateInfo()
                    .setImage(*context.samplerImages.at(i).reference())
                    .setFormat(context.samplerImages.at(i).createInfo.format)
                    .setViewType(vk::ImageViewType::e2D)
                    .setFlags({})
                    .setComponents({})
                    .setSubresourceRange(
                        vk::ImageSubresourceRange()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setBaseMipLevel(0)
                            .setLevelCount(context.samplerImages.at(i).createInfo.mipLevels)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            )
            .build();
      }

      context.descriptorPool = DescriptorPool::builder()
          .setDevice(context.device.value)
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(context.swapchainImageViews.size())
          )
          .setCreateInfo(
              vk::DescriptorPoolCreateInfo()
                  .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                  .setMaxSets(context.swapchainImageViews.size())
          )
          .build();

      context.sampler = Sampler::builder()
          .setDevice(context.device.value)
          .setCreateInfo(
              vk::SamplerCreateInfo()
                  .setMagFilter(vk::Filter::eLinear)
                  .setMinFilter(vk::Filter::eLinear)
                  .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                  .setMinLod(0.0f)
                      //.setMinLod((float) textureImage.createInfo.mipLevels / 2)
                  .setMaxLod((float) context.samplerImages.at(0).createInfo.mipLevels)
                  .setMipLodBias(0.0f)
                  .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                  .setCompareOp(vk::CompareOp::eAlways)
                  .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                  .setUnnormalizedCoordinates(false)
                  .setCompareEnable(false)
                  .setAnisotropyEnable(context.physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy)
                  .setMaxAnisotropy(context.physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy ? context.physicalDevice.reference().getProperties().limits.maxSamplerAnisotropy : 0)
          )
          .build();

      context.descriptorSets.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.swapchainImageViews.size(); i++) {
        context.descriptorSets.at(i) = DescriptorSet::builder()
            .setDevice(context.device.value)
            .addSetLayout(*context.descriptorSetLayout.reference())
            .setCreateInfo(
                vk::DescriptorSetAllocateInfo()
                    .setDescriptorPool(*context.descriptorPool.reference())
                    .setDescriptorSetCount(1)
            )
            .addWrite(
                WriteDescriptorSet()
                    .setDstBinding(0)
                    .setDstArrayElement(0)
                    .setDescriptorCount(1)
                    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                    .addImageInfo(
                        vk::DescriptorImageInfo()
                            .setSampler(*context.sampler.reference())
                            .setImageView(*context.samplerImageViews.at(i).reference())
                            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    )
            )
            .build();
      }

      context.graphicsCommandBuffers.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.graphicsCommandBuffers.size(); i++) {
        context.graphicsCommandBuffers.at(i) = CommandBuffer::builder()
            .setDevice(context.device.value)
            .setCreateInfo(
                vk::CommandBufferAllocateInfo()
                    .setCommandPool(*context.graphicsCommandPool.reference())
                    .setCommandBufferCount(1)
                    .setLevel(vk::CommandBufferLevel::ePrimary)
            )
        .build();
      }

      context.imageAvailableSemaphores.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.imageAvailableSemaphores.size(); i++) {
        context.imageAvailableSemaphores.at(i) = Semaphore::builder().setDevice(context.device.value).build();
      }

      context.renderFinishedSemaphores.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.renderFinishedSemaphores.size(); i++) {
        context.renderFinishedSemaphores.at(i) = Semaphore::builder().setDevice(context.device.value).build();
      }

      context.inFlightFences.resize(context.swapchainImageViews.size());
      for (size_t i = 0; i < context.inFlightFences.size(); i++) {
        context.inFlightFences.at(i) = Fence::builder()
            .setDevice(context.device.value)
            .setCreateInfo(
                vk::FenceCreateInfo()
                    .setFlags(vk::FenceCreateFlagBits::eSignaled)
            )
            .build();
      }

      std::vector<vk::SubmitInfo> submits;
      submits.resize(context.graphicsCommandBuffers.size());
      for (size_t i = 0; i < context.graphicsCommandBuffers.size(); i++) {
        context.graphicsCommandBuffers.at(i).reference().begin({});
        context.graphicsCommandBuffers.at(i).reference().pipelineBarrier(
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::DependencyFlags(0),
            {},
            {},
            {
                vk::ImageMemoryBarrier()
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setImage(*context.samplerImages.at(i).reference())
                    .setOldLayout(vk::ImageLayout::eUndefined)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                    .setSubresourceRange(
                        vk::ImageSubresourceRange()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setBaseMipLevel(0)
                            .setLevelCount(1)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            }
        );
        context.graphicsCommandBuffers.at(i).reference().end();
        submits.at(i) = vk::SubmitInfo()
            .setCommandBufferCount(1)
            .setPCommandBuffers(&(*context.graphicsCommandBuffers.at(i).reference()));
      }
      context.graphicsQueue.reference().submit(submits);
      context.graphicsQueue.reference().waitIdle();
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

}
