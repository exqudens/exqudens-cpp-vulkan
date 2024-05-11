#include <functional>
#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Context.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  void Context::init(
      const std::vector<const char*>& enabledExtensionNames,
      const std::vector<const char*>& enabledLayerNames,
      const std::function<VkSurfaceKHR(VkInstance)>& createSurfaceFunction,
      const uint32_t& queryPoolSize,
      const uint32_t& width,
      const uint32_t& height,
      const std::vector<std::string>& paths
  ) {
    try {
      instance = Instance::builder()
          .setEnabledExtensionNames(enabledExtensionNames)
          .setEnabledLayerNames(enabledLayerNames)
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
        auto vkInstance = static_cast<VkInstance>(*instance.reference());
        VkSurfaceKHR vkSurface = createSurfaceFunction(vkInstance);
        surface = Surface::builder()
            .setInstance(instance.value)
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
      physicalDeviceFeatures = tmpPhysicalDeviceFeatures;

      PhysicalDevice::Builder physicalDeviceBuilder = PhysicalDevice::builder()
          .setInstance(instance.value)
          .addEnabledExtensionName(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)
          .addQueuePriority(1.0f)
          .setIsSuitableFunction([this](const vk::raii::PhysicalDevice& p) {
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
              if (surface.value) {
                presentValid = p.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface.reference());
              } else {
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
          });
      if (surface.value) {
        physicalDeviceBuilder = physicalDeviceBuilder
            .setSurface(surface.value)
            .addEnabledExtensionName(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
      }
      physicalDevice = physicalDeviceBuilder.build();

      physicalDeviceTimestampPeriod = physicalDevice.reference().getProperties().limits.timestampPeriod;

      physicalDeviceMsaaSamples = Utility::getMaxUsableSampleCount(
          {
              physicalDevice.reference().getProperties().limits.framebufferColorSampleCounts,
              physicalDevice.reference().getProperties().limits.framebufferDepthSampleCounts
          }
      );

      device = Device::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setCreateInfo(
              vk::DeviceCreateInfo()
                  .setQueueCreateInfos(physicalDevice.uniqueQueueCreateInfos)
                  //.setPEnabledFeatures(&physicalDevice.features)
                  .setPEnabledExtensionNames(physicalDevice.enabledExtensionNames)
                  .setPEnabledLayerNames(instance.enabledLayerNames)
                  //.setPNext(physicalDevice.hostQueryResetFeatures.has_value() ? &physicalDevice.hostQueryResetFeatures.value() : nullptr)
                  .setPNext(&physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>())
          )
          .build();

      transferQueue = Queue::builder()
          .setDevice(device.value)
          .setFamilyIndex(physicalDevice.transferQueueCreateInfos.front().queueFamilyIndex)
          .build();

      graphicsQueue = Queue::builder()
          .setDevice(device.value)
          .setFamilyIndex(physicalDevice.graphicsQueueCreateInfos.front().queueFamilyIndex)
          .build();

      if (surface.value) {
        presentQueue = Queue::builder()
            .setDevice(device.value)
            .setFamilyIndex(physicalDevice.presentQueueCreateInfos.front().queueFamilyIndex)
            .build();
      }

      transferCommandPool = CommandPool::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::CommandPoolCreateInfo()
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  .setQueueFamilyIndex(transferQueue.familyIndex)
          )
          .build();

      graphicsCommandPool = CommandPool::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::CommandPoolCreateInfo()
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  .setQueueFamilyIndex(graphicsQueue.familyIndex)
          )
          .build();

      transferCommandBuffer = CommandBuffer::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::CommandBufferAllocateInfo()
                  .setCommandPool(*transferCommandPool.reference())
                  .setCommandBufferCount(1)
                  .setLevel(vk::CommandBufferLevel::ePrimary)
          )
          .build();

      descriptorSetLayout = DescriptorSetLayout::builder()
          .setDevice(device.value)
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(0)
                  .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eFragment)
          )
          .build();

      queryPool = QueryPool::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::QueryPoolCreateInfo()
                  /*.setPipelineStatistics(vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations)*/
                  .setQueryType(vk::QueryType::eTimestamp)
                  .setQueryCount(queryPoolSize)
          )
          .build();

      initSwapchain(width, height, paths);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void Context::initSwapchain(
      const uint32_t& width,
      const uint32_t& height,
      const std::vector<std::string>& paths
  ) {
    try {
      device.reference().waitIdle();

      inFlightFences.clear();
      renderFinishedSemaphores.clear();
      imageAvailableSemaphores.clear();
      graphicsCommandBuffers.clear();
      descriptorSets.clear();
      sampler.value.reset();
      descriptorPool.value.reset();
      samplerImageViews.clear();
      samplerImages.clear();
      swapchainFramebuffers.clear();
      pipeline.value.reset();
      renderPass.value.reset();
      swapchainImageViews.clear();
      swapchainImages.clear();
      swapchain.value.reset();

      if (surface.value) {
        swapchain = Swapchain::builder()
            .setDevice(device.value)
            .addGraphicsQueueFamilyIndex(physicalDevice.graphicsQueueCreateInfos.front().queueFamilyIndex)
            .addPresentQueueFamilyIndex(physicalDevice.presentQueueCreateInfos.front().queueFamilyIndex)
            .setCreateInfo(
                Utility::swapChainCreateInfo(
                    physicalDevice.reference(),
                    surface.reference(),
                    width,
                    height
                )
            )
            .build();
        swapchainImageViews.resize(swapchain.reference().getImages().size());
      } else {
        swapchainImages.resize(1);
        swapchainImages.at(0) = Image::builder()
            .setPhysicalDevice(physicalDevice.value)
            .setDevice(device.value)
            .setCreateInfo(
                vk::ImageCreateInfo()
                    .setExtent(
                        vk::Extent3D()
                            .setWidth(width)
                            .setHeight(height)
                            .setDepth(1)
                    )
                    .setFormat(vk::Format::eR8G8B8A8Srgb)
                    .setImageType(vk::ImageType::e2D)
                    .setMipLevels(1)
                    .setArrayLayers(1)
                    .setSamples(vk::SampleCountFlagBits::e1)
                    .setTiling(vk::ImageTiling::eOptimal)
                    .setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc)
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setInitialLayout(vk::ImageLayout::eUndefined)
            )
            .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
            .build();
        swapchainImageViews.resize(swapchainImages.size());
      }

      for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        vk::Image image = {};
        vk::Format format = {};
        if (swapchain.value) {
          image = static_cast<vk::Image>(swapchain.reference().getImages().at(i));
          format = swapchain.createInfo.imageFormat;
        } else {
          image = static_cast<vk::Image>(*swapchainImages.at(i).reference());
          format = swapchainImages.at(i).createInfo.format;
        }
        swapchainImageViews.at(i) = ImageView::builder()
            .setDevice(device.value)
            .setCreateInfo(
                vk::ImageViewCreateInfo()
                    .setFlags({})
                    .setImage(image)
                    .setViewType(vk::ImageViewType::e2D)
                    .setFormat(format)
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

      vk::ImageLayout renderPassFinalLayout = {};
      if (surface.value) {
        renderPassFinalLayout = vk::ImageLayout::ePresentSrcKHR;
      } else {
        renderPassFinalLayout = vk::ImageLayout::eTransferSrcOptimal;
      }

      renderPass = RenderPass::builder()
          .setDevice(device.value)
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(swapchain.value ? swapchain.createInfo.imageFormat : swapchainImages.at(0).createInfo.format)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(renderPassFinalLayout)
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

      pipeline = Pipeline::builder()
          .setDevice(device.value)
          .addPath(paths.at(0))
          //.addStage(TestUtils::createStage(context.device, shaders, paths.at(0)))
          .addPath(paths.at(1))
          .addSetLayout(*descriptorSetLayout.reference())
          .setGraphicsCreateInfo(
              GraphicsPipelineCreateInfo()
                  .setRenderPass(*renderPass.reference())
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
                                  .setWidth((float) width)
                                  .setHeight((float) height)
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
                                          .setWidth(width)
                                          .setHeight(height)
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

      swapchainFramebuffers.resize(swapchainImageViews.size());
      for (size_t i = 0; i < swapchainFramebuffers.size(); i++) {
        swapchainFramebuffers.at(i) = Framebuffer::builder()
            .setDevice(device.value)
            .addAttachment(*swapchainImageViews.at(i).reference())
            .setCreateInfo(
                vk::FramebufferCreateInfo()
                    .setRenderPass(*renderPass.reference())
                    .setWidth(width)
                    .setHeight(height)
                    .setLayers(1)
            )
            .build();
      }

      samplerImages.resize(swapchainImageViews.size());
      for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        samplerImages.at(i) = Image::builder()
            .setPhysicalDevice(physicalDevice.value)
            .setDevice(device.value)
            .setCreateInfo(
                vk::ImageCreateInfo()
                    .setImageType(vk::ImageType::e2D)
                    .setFormat(vk::Format::eR8G8B8A8Srgb)
                    .setExtent(
                        vk::Extent3D()
                            .setWidth(width)
                            .setHeight(height)
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

      samplerImageViews.resize(swapchainImageViews.size());
      for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        samplerImageViews.at(i) = ImageView::builder()
            .setDevice(device.value)
            .setCreateInfo(
                vk::ImageViewCreateInfo()
                    .setImage(*samplerImages.at(i).reference())
                    .setFormat(samplerImages.at(i).createInfo.format)
                    .setViewType(vk::ImageViewType::e2D)
                    .setFlags({})
                    .setComponents({})
                    .setSubresourceRange(
                        vk::ImageSubresourceRange()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setBaseMipLevel(0)
                            .setLevelCount(samplerImages.at(i).createInfo.mipLevels)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            )
            .build();
      }

      descriptorPool = DescriptorPool::builder()
          .setDevice(device.value)
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(swapchainImageViews.size())
          )
          .setCreateInfo(
              vk::DescriptorPoolCreateInfo()
                  .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                  .setMaxSets(swapchainImageViews.size())
          )
          .build();

      sampler = Sampler::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::SamplerCreateInfo()
                  .setMagFilter(vk::Filter::eLinear)
                  .setMinFilter(vk::Filter::eLinear)
                  .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                  .setMinLod(0.0f)
                  //.setMinLod((float) textureImage.createInfo.mipLevels / 2)
                  .setMaxLod((float) samplerImages.at(0).createInfo.mipLevels)
                  .setMipLodBias(0.0f)
                  .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                  .setCompareOp(vk::CompareOp::eAlways)
                  .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                  .setUnnormalizedCoordinates(false)
                  .setCompareEnable(false)
                  .setAnisotropyEnable(physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy)
                  .setMaxAnisotropy(physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy ? physicalDevice.reference().getProperties().limits.maxSamplerAnisotropy : 0)
          )
          .build();

      descriptorSets.resize(swapchainImageViews.size());
      for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        descriptorSets.at(i) = DescriptorSet::builder()
            .setDevice(device.value)
            .addSetLayout(*descriptorSetLayout.reference())
            .setCreateInfo(
                vk::DescriptorSetAllocateInfo()
                    .setDescriptorPool(*descriptorPool.reference())
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
                            .setSampler(*sampler.reference())
                            .setImageView(*samplerImageViews.at(i).reference())
                            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    )
            )
            .build();
      }

      graphicsCommandBuffers.resize(swapchainImageViews.size());
      for (size_t i = 0; i < graphicsCommandBuffers.size(); i++) {
        graphicsCommandBuffers.at(i) = CommandBuffer::builder()
            .setDevice(device.value)
            .setCreateInfo(
                vk::CommandBufferAllocateInfo()
                    .setCommandPool(*graphicsCommandPool.reference())
                    .setCommandBufferCount(1)
                    .setLevel(vk::CommandBufferLevel::ePrimary)
            )
            .build();
      }

      imageAvailableSemaphores.resize(swapchainImageViews.size());
      for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
        imageAvailableSemaphores.at(i) = Semaphore::builder().setDevice(device.value).build();
      }

      renderFinishedSemaphores.resize(swapchainImageViews.size());
      for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
        renderFinishedSemaphores.at(i) = Semaphore::builder().setDevice(device.value).build();
      }

      inFlightFences.resize(swapchainImageViews.size());
      for (size_t i = 0; i < inFlightFences.size(); i++) {
        inFlightFences.at(i) = Fence::builder()
            .setDevice(device.value)
            .setCreateInfo(
                vk::FenceCreateInfo()
                    .setFlags(vk::FenceCreateFlagBits::eSignaled)
            )
            .build();
      }

      std::vector<vk::SubmitInfo> submits;
      submits.resize(graphicsCommandBuffers.size());
      for (size_t i = 0; i < graphicsCommandBuffers.size(); i++) {
        graphicsCommandBuffers.at(i).reference().begin({});
        graphicsCommandBuffers.at(i).reference().pipelineBarrier(
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::DependencyFlags(0),
            {},
            {},
            {
                vk::ImageMemoryBarrier()
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setImage(*samplerImages.at(i).reference())
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
        graphicsCommandBuffers.at(i).reference().end();
        submits.at(i) = vk::SubmitInfo()
            .setCommandBufferCount(1)
            .setPCommandBuffers(&(*graphicsCommandBuffers.at(i).reference()));
      }
      graphicsQueue.reference().submit(submits);
      graphicsQueue.reference().waitIdle();
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
