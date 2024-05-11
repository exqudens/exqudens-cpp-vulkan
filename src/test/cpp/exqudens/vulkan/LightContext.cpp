#include <functional>
#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/LightContext.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"
#include "exqudens/vulkan/Vertex.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  void LightContext::init(
      Context& root,
      std::vector<Buffer>& uniformBuffers,
      const std::vector<std::string>& paths
  ) {
    try {
      shadowImages.resize(root.swapchainImageViews.size());
      for (size_t i = 0; i < shadowImages.size(); i++) {
        shadowImages.at(i) = Image::builder()
            .setPhysicalDevice(root.physicalDevice.value)
            .setDevice(root.device.value)
            .setCreateInfo(
                vk::ImageCreateInfo()
                    .setImageType(vk::ImageType::e2D)
                    .setFormat(Utility::imageDepthFormat(root.physicalDevice.reference()))
                    .setExtent(
                        vk::Extent3D()
                            .setWidth(8192) // 32768, 16384, 8192, 4096, 2048, 1024
                            .setHeight(8192)
                            .setDepth(1)
                    )
                    .setMipLevels(1)
                    .setArrayLayers(1)
                    .setSamples(vk::SampleCountFlagBits::e1)
                    .setTiling(vk::ImageTiling::eOptimal)
                    .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled)
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setQueueFamilyIndices({})
                    .setInitialLayout(vk::ImageLayout::eUndefined)
            )
            .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
            .build();
      }

      shadowImageViews.resize(shadowImages.size());
      for (size_t i = 0; i < shadowImageViews.size(); i++) {
        shadowImageViews.at(i) = ImageView::builder()
            .setDevice(root.device.value)
            .setCreateInfo(
                vk::ImageViewCreateInfo()
                    .setImage(*shadowImages.at(i).reference())
                    .setFormat(shadowImages.at(i).createInfo.format)
                    .setViewType(vk::ImageViewType::e2D)
                    .setFlags({})
                    .setComponents(
                        vk::ComponentMapping()
                            .setR(vk::ComponentSwizzle::eR)
                            .setG(vk::ComponentSwizzle::eG)
                            .setB(vk::ComponentSwizzle::eB)
                            .setA(vk::ComponentSwizzle::eA)
                    )
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
      }

      sampler = Sampler::builder()
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::SamplerCreateInfo()
                  .setMagFilter(vk::Filter::eLinear)
                  .setMinFilter(vk::Filter::eLinear)
                  .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                  .setMinLod(0.0f)
                  .setMaxLod(1.0f)
                  .setMipLodBias(0.0f)
                  .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
                  .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                  .setCompareOp(vk::CompareOp::eAlways)
                  .setBorderColor(vk::BorderColor::eFloatOpaqueWhite)
                  .setUnnormalizedCoordinates(false)
                  .setCompareEnable(false)
                  .setAnisotropyEnable(root.physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy)
                  .setMaxAnisotropy(root.physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy ? root.physicalDevice.reference().getProperties().limits.maxSamplerAnisotropy : 0)
          )
          .build();

      descriptorSetLayout = DescriptorSetLayout::builder()
          .setDevice(root.device.value)
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(0)
                  .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eVertex)
          )
          .build();

      descriptorPool = DescriptorPool::builder()
          .setDevice(root.device.value)
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eUniformBuffer)
                  .setDescriptorCount(root.swapchainImageViews.size())
          )
          .setCreateInfo(
              vk::DescriptorPoolCreateInfo()
                  .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                  .setMaxSets(root.swapchainImageViews.size())
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
            .build();
      }

      renderPass = RenderPass::builder()
          .setDevice(root.device.value)
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(shadowImages.at(0).createInfo.format)
                  .setSamples(shadowImages.at(0).createInfo.samples)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
          )
          .addSubpass(
              SubpassDescription()
                  .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                  .setDepthStencilAttachment(
                      vk::AttachmentReference()
                          .setAttachment(0)
                          .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
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
          /*.addDependency(
              vk::SubpassDependency()
                  .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                  .setDstSubpass(0)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                  .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                  .setSrcAccessMask(vk::AccessFlagBits::eNone)
                  .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
          )*/
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
                                  .setWidth((float) shadowImages.at(0).createInfo.extent.width)
                                  .setHeight((float) shadowImages.at(0).createInfo.extent.height)
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
                                          .setWidth(shadowImages.at(0).createInfo.extent.width)
                                          .setHeight(shadowImages.at(0).createInfo.extent.height)
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
                          .setLineWidth(1.0f)
                          .setDepthBiasEnable(true)
                          .setDepthBiasConstantFactor(4.0f)
                          .setDepthBiasClamp(0.0f)
                          .setDepthBiasSlopeFactor(1.8f)
                  )
                  .setMultisampleState(
                      vk::PipelineMultisampleStateCreateInfo()
                          .setRasterizationSamples(shadowImages.at(0).createInfo.samples)
                          .setSampleShadingEnable(false)
                          .setMinSampleShading(0.0)
                          //.setSampleShadingEnable(true)
                          //.setMinSampleShading(0.2f)
                          .setPSampleMask(nullptr)
                          .setAlphaToCoverageEnable(false)
                          .setAlphaToOneEnable(false)
                  )
                  .setDepthStencilState(
                      vk::PipelineDepthStencilStateCreateInfo()
                          .setDepthTestEnable(true)
                          .setDepthWriteEnable(true)
                          .setDepthCompareOp(vk::CompareOp::eLessOrEqual)
                          .setDepthBoundsTestEnable(false)
                          .setStencilTestEnable(false)
                          .setFront(
                              vk::StencilOpState()
                                  .setFailOp(vk::StencilOp::eKeep)
                                  .setDepthFailOp(vk::StencilOp::eKeep)
                                  .setPassOp(vk::StencilOp::eKeep)
                                  .setCompareOp(vk::CompareOp::eAlways)
                                  .setCompareMask(0)
                                  .setWriteMask(0)
                                  .setReference(0)
                          )
                          .setBack(
                              vk::StencilOpState()
                                  .setFailOp(vk::StencilOp::eKeep)
                                  .setDepthFailOp(vk::StencilOp::eKeep)
                                  .setPassOp(vk::StencilOp::eKeep)
                                  .setCompareOp(vk::CompareOp::eAlways)
                                  .setCompareMask(0)
                                  .setWriteMask(0)
                                  .setReference(0)
                          )
                          .setMinDepthBounds(0.0)
                          .setMaxDepthBounds(1.0)
                  )
                  .setColorBlendState(
                      PipelineColorBlendStateCreateInfo()
                          .setLogicOpEnable(false)
                          .setLogicOp(vk::LogicOp::eCopy)
                          .setBlendConstants({1.0f, 1.0f, 1.0f, 1.0f})
                          .addAttachment(
                              vk::PipelineColorBlendAttachmentState()
                                  .setBlendEnable(false)
                                  .setColorBlendOp(vk::BlendOp::eAdd)
                                  .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
                                  .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
                                  .setAlphaBlendOp(vk::BlendOp::eAdd)
                                  .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                                  .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                                  .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
                          )
                  )
          )
          .build();

      framebuffers.resize(shadowImageViews.size());
      for (size_t i = 0; i < framebuffers.size(); i++) {
        framebuffers.at(i) = Framebuffer::builder()
            .setDevice(root.device.value)
            .addAttachment(*shadowImageViews.at(i).reference())
            .setCreateInfo(
                vk::FramebufferCreateInfo()
                    .setRenderPass(*renderPass.reference())
                    .setWidth(shadowImages.at(i).createInfo.extent.width)
                    .setHeight(shadowImages.at(i).createInfo.extent.height)
                    .setLayers(shadowImages.at(i).createInfo.arrayLayers)
            )
            .build();
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
