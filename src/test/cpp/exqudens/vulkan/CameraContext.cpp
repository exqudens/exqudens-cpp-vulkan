#include <functional>
#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/CameraContext.hpp"
#include "exqudens/vulkan/Vertex.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  void CameraContext::init(
      Context& root,
      LightContext& light,
      uint32_t& mipLevels,
      std::vector<Buffer>& uniformBuffers,
      ImageView& textureImageView
  ) {
    try {
      descriptorSetLayout = DescriptorSetLayout::builder()
          .setDevice(root.device.value)
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(0)
                  .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eVertex)
          )
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(1)
                  .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eFragment)
          )
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(2)
                  .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eFragment)
          )
          .build();

      sampler = Sampler::builder()
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::SamplerCreateInfo()
                  .setMagFilter(vk::Filter::eLinear)
                  .setMinFilter(vk::Filter::eLinear)
                  .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                  .setMinLod(0.0f)
                  //.setMinLod((float) mipLevels / 2)
                  .setMaxLod((float) mipLevels)
                  .setMipLodBias(0.0f)
                  .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                  .setCompareOp(vk::CompareOp::eAlways)
                  .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                  .setUnnormalizedCoordinates(false)
                  .setCompareEnable(false)
                  .setAnisotropyEnable(root.physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy)
                  .setMaxAnisotropy(root.physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy ? root.physicalDevice.reference().getProperties().limits.maxSamplerAnisotropy : 0)
          )
          .build();

      descriptorPool = DescriptorPool::builder()
          .setDevice(root.device.value)
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eUniformBuffer)
                  .setDescriptorCount(root.swapchainImageViews.size())
          )
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(root.swapchainImageViews.size())
          )
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(root.swapchainImageViews.size())
          )
          .setCreateInfo(
              vk::DescriptorPoolCreateInfo()
                  .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                  .setMaxSets(3u)
          )
          .build();

      descriptorSets.resize(root.swapchainImageViews.size());
      for (size_t i = 0; i < descriptorSets.size(); i++) {
        descriptorSets.at(i) = DescriptorSet::builder()
            .setDevice(root.device.value)
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
                    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                    .addBufferInfo(
                        vk::DescriptorBufferInfo()
                            .setBuffer(*uniformBuffers.at(i).reference())
                            .setOffset(0)
                            .setRange(sizeof(UniformBufferObject))
                    )
            )
            .addWrite(
                WriteDescriptorSet()
                    .setDstBinding(1)
                    .setDstArrayElement(0)
                    .setDescriptorCount(1)
                    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                    .addImageInfo(
                        vk::DescriptorImageInfo()
                            .setSampler(*sampler.reference())
                            .setImageView(*textureImageView.reference())
                            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    )
            )
            .addWrite(
                WriteDescriptorSet()
                    .setDstBinding(2)
                    .setDstArrayElement(0)
                    .setDescriptorCount(1)
                    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                    .addImageInfo(
                        vk::DescriptorImageInfo()
                            .setSampler(*light.sampler.reference())
                            .setImageView(*light.shadowImageViews.at(i).reference())
                            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    )
            )
            .build();
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void CameraContext::initSwapchain(
      Context& root,
      const std::vector<std::string>& paths
  ) {
    try {
      framebuffers.clear();
      pipeline.value.reset();
      renderPass.value.reset();
      depthImageView.value.reset();
      depthImage.value.reset();
      colorImageView.value.reset();
      colorImage.value.reset();

      colorImage = Image::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setImageType(vk::ImageType::e2D)
                  .setFormat(root.samplerImages.at(0).createInfo.format)
                  .setExtent(
                      vk::Extent3D()
                          .setWidth(root.samplerImages.at(0).createInfo.extent.width)
                          .setHeight(root.samplerImages.at(0).createInfo.extent.height)
                          .setDepth(1)
                  )
                  .setMipLevels(1)
                  .setArrayLayers(1)
                  .setSamples(root.physicalDeviceMsaaSamples)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setUsage(vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setQueueFamilyIndices({})
                  .setInitialLayout(vk::ImageLayout::eUndefined)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();

      colorImageView = ImageView::builder()
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::ImageViewCreateInfo()
                  .setImage(*colorImage.reference())
                  .setFormat(colorImage.createInfo.format)
                  .setViewType(vk::ImageViewType::e2D)
                  .setFlags({})
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

      depthImage = Image::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setImageType(vk::ImageType::e2D)
                  .setFormat(Utility::imageDepthFormat(root.physicalDevice.reference()))
                  .setExtent(
                      vk::Extent3D()
                          .setWidth(root.samplerImages.at(0).createInfo.extent.width)
                          .setHeight(root.samplerImages.at(0).createInfo.extent.height)
                          .setDepth(1)
                  )
                  .setMipLevels(1)
                  .setArrayLayers(1)
                  .setSamples(root.physicalDeviceMsaaSamples)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setQueueFamilyIndices({})
                  .setInitialLayout(vk::ImageLayout::eUndefined)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();

      depthImageView = ImageView::builder()
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::ImageViewCreateInfo()
                  .setImage(*depthImage.reference())
                  .setFormat(depthImage.createInfo.format)
                  .setViewType(vk::ImageViewType::e2D)
                  .setFlags({})
                  .setComponents({})
                  .setSubresourceRange(
                      vk::ImageSubresourceRange()
                          .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                          .setBaseMipLevel(0)
                          .setLevelCount(1)
                          .setBaseArrayLayer(0)
                          .setLayerCount(1)
                  )
          )
          .build();

      renderPass = RenderPass::builder()
          .setDevice(root.device.value)
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(colorImage.createInfo.format)
                  .setSamples(colorImage.createInfo.samples)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eStore)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
          )
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(depthImage.createInfo.format)
                  .setSamples(depthImage.createInfo.samples)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
          )
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(root.samplerImages.at(0).createInfo.format)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eStore)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
          )
          .addSubpass(
              SubpassDescription()
                  .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                  .addColorAttachment(
                      vk::AttachmentReference()
                          .setAttachment(0)
                          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                  )
                  .setDepthStencilAttachment(
                      vk::AttachmentReference()
                          .setAttachment(1)
                          .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                  )
                  .addResolveAttachment(
                      vk::AttachmentReference()
                          .setAttachment(2)
                          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                  )
          )
          /*.addDependency(
              vk::SubpassDependency()
                  .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                  .setDstSubpass(0)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                  .setSrcAccessMask(vk::AccessFlagBits::eShaderRead)
                  .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                  .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
          )
          .addDependency(
              vk::SubpassDependency()
                  .setSrcSubpass(0)
                  .setDstSubpass(VK_SUBPASS_EXTERNAL)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                  .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                  .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                  .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
          )*/
          .addDependency(
              vk::SubpassDependency()
                  .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                  .setDstSubpass(0)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                  .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                  .setSrcAccessMask(vk::AccessFlagBits::eNone)
                  .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
          )
          .build();

      pipeline = Pipeline::builder()
          .setDevice(root.device.value)
          .addPath(paths.at(0))
          //.addStage(TestUtils::createStage(device, shaders, paths.at(0)))
          .addPath(paths.at(1))
          .addSetLayout(*descriptorSetLayout.reference())
          .setGraphicsCreateInfo(
              GraphicsPipelineCreateInfo()
                  .setRenderPass(*renderPass.reference())
                  .setSubpass(0)
                  .setVertexInputState(
                      PipelineVertexInputStateCreateInfo()
                          .setVertexBindingDescriptions({Vertex::getBindingDescription()})
                          .setVertexAttributeDescriptions(Vertex::getAttributeDescriptions())
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
                                  .setWidth((float) root.samplerImages.at(0).createInfo.extent.width)
                                  .setHeight((float) root.samplerImages.at(0).createInfo.extent.height)
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
                                          .setWidth(root.samplerImages.at(0).createInfo.extent.width)
                                          .setHeight(root.samplerImages.at(0).createInfo.extent.height)
                                  )
                          )
                  )
                  .setRasterizationState(
                      vk::PipelineRasterizationStateCreateInfo()
                          .setDepthClampEnable(false)
                          .setRasterizerDiscardEnable(false)
                          .setPolygonMode(vk::PolygonMode::eFill)
                          .setCullMode(vk::CullModeFlagBits::eBack)
                          .setFrontFace(vk::FrontFace::eCounterClockwise)
                          .setLineWidth(1.0)
                          .setDepthBiasEnable(false)
                          .setDepthBiasConstantFactor(0.0)
                          .setDepthBiasClamp(0.0)
                          .setDepthBiasSlopeFactor(0.0)
                  )
                  .setMultisampleState(
                      vk::PipelineMultisampleStateCreateInfo()
                          .setRasterizationSamples(colorImage.createInfo.samples)
                          //.setSampleShadingEnable(false)
                          //.setMinSampleShading(1.0)
                          .setSampleShadingEnable(true)
                          .setMinSampleShading(0.2f)
                          .setPSampleMask(nullptr)
                          .setAlphaToCoverageEnable(false)
                          .setAlphaToOneEnable(false)
                  )
                  .setDepthStencilState(
                      vk::PipelineDepthStencilStateCreateInfo()
                          .setDepthTestEnable(true)
                          .setDepthWriteEnable(true)
                          .setDepthCompareOp(vk::CompareOp::eLess)
                          .setDepthBoundsTestEnable(false)
                          .setStencilTestEnable(false)
                          .setFront({})
                          .setBack({})
                          .setMinDepthBounds(0.0)
                          .setMaxDepthBounds(1.0)
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

      framebuffers.resize(root.swapchainImageViews.size());
      for (size_t i = 0; i < framebuffers.size(); i++) {
        framebuffers.at(i) = Framebuffer::builder()
            .setDevice(root.device.value)
            .addAttachment(*colorImageView.reference())
            .addAttachment(*depthImageView.reference())
            .addAttachment(*root.samplerImageViews.at(i).reference())
            .setCreateInfo(
                vk::FramebufferCreateInfo()
                    .setRenderPass(*renderPass.reference())
                    .setWidth(root.samplerImages.at(i).createInfo.extent.width)
                    .setHeight(root.samplerImages.at(i).createInfo.extent.height)
                    .setLayers(1)
            )
            .build();
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
