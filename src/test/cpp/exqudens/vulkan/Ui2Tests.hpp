#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <format>

#include <gtest/gtest.h>
#include <vulkan/vulkan_raii.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TestUtils.hpp"
#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Vertex.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"
#include "exqudens/vulkan/Context.hpp"
#include "exqudens/vulkan/Functions.hpp"

namespace exqudens::vulkan {

  class Ui2Tests : public testing::Test {

    protected:

      struct CameraContext {

        DescriptorSetLayout descriptorSetLayout = {};
        Sampler sampler = {};
        Image depthImage = {};
        ImageView depthImageView = {};
        DescriptorPool descriptorPool = {};
        std::vector<DescriptorSet> descriptorSets = {};

        RenderPass renderPass = {};
        Pipeline pipeline = {};
        std::vector<Framebuffer> swapchainFramebuffers = {};

      };

      class TestRenderer {

        public:

          bool resized = false;
          bool left = false;
          bool right = false;
          bool up = false;
          bool down = false;

          float angleLeft = 0.0f;
          float angleUp = 0.0f;

        private:
          inline static const std::string OBJ_FILE_PATH = "";
          inline static const std::string PNG_FILE_PATH = "";
          inline static const std::string ANIMATE = "control"; // "", "auto", "control"

          std::vector<Vertex> vertexVector = {};
          std::vector<uint16_t> indexVector = {};
          unsigned int textureWidth;
          unsigned int textureHeight;
          unsigned int textureDepth;
          std::vector<unsigned char> textureVector = {};

          Context root = {};

          Buffer vertexStagingBuffer = {};
          Buffer vertexBuffer = {};
          Buffer indexStagingBuffer = {};
          Buffer indexBuffer = {};
          Buffer textureBuffer = {};
          Image textureImage = {};
          ImageView textureImageView = {};
          std::vector<Buffer> uniformBuffers = {};

          CameraContext camera = {};

          size_t currentFrame = 0;
          uint8_t timeDiffCounter = 0;

        public:

          void create(
              const std::vector<std::string>& arguments,
              const std::vector<const char*>& glfwInstanceRequiredExtensions,
              GLFWwindow* window,
              uint32_t width,
              uint32_t height
          ) {
            try {
              std::cout << std::format("{} ... call", CALL_INFO()) << std::endl;

              /*vertexVector = {
                  {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                  {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                  {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                  {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},

                  {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                  {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                  {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                  {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}
              };

              indexVector = {
                  0, 1, 2, 2, 3, 0,
                  4, 5, 6, 6, 7, 4
              };*/

              std::string objFilePath = OBJ_FILE_PATH;
              if (objFilePath.empty()) {
                objFilePath = std::filesystem::path(arguments.front())
                    .append("resources")
                    .append("obj")
                    .append("viking_room.obj")
                    .make_preferred()
                    .string();
              } else {
                objFilePath = std::filesystem::path(objFilePath).make_preferred().string();
              }
              TestUtils::readObj(
                  objFilePath,
                  vertexVector,
                  indexVector
              );

              std::string pngFilePath = PNG_FILE_PATH;
              if (pngFilePath.empty()) {
                pngFilePath = std::filesystem::path(arguments.front())
                    .append("resources")
                    .append("png")
                    .append("viking_room.png")
                    .make_preferred()
                    .string();
              } else {
                pngFilePath = std::filesystem::path(pngFilePath).make_preferred().string();
              }
              unsigned int tmpImageMipLevels;
              TestUtils::readPng(
                  pngFilePath,
                  textureWidth,
                  textureHeight,
                  textureDepth,
                  textureVector
              );
              tmpImageMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

              Utility::setEnvironmentVariable("VK_LAYER_PATH", arguments.front());

              std::vector<const char*> enabledExtensionNames = glfwInstanceRequiredExtensions;
              enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
              enabledExtensionNames.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

              std::function<VkSurfaceKHR(VkInstance)> createSurfaceFunction;
              createSurfaceFunction = [&window](VkInstance instance) -> VkSurfaceKHR {
                VkSurfaceKHR vkSurface = nullptr;
                if (glfwCreateWindowSurface(instance, window, nullptr, &vkSurface) != VK_SUCCESS || vkSurface == nullptr) {
                  throw std::runtime_error(CALL_INFO() + ": failed to create surface!");
                }
                return vkSurface;
              };

              Functions::init(root, enabledExtensionNames, createSurfaceFunction, 2, width, height/*, textureWidth, textureHeight, textureDepth*/);

              std::cout << std::format("root.instance: '{}'", (bool) root.instance.value) << std::endl;
              std::cout << std::format("root.surface: '{}'", (bool) root.surface.value) << std::endl;
              std::cout << std::format("root.physicalDevice: '{}'", (bool) root.physicalDevice.value) << std::endl;
              std::cout << std::format("root.physicalDeviceTimestampPeriod: '{}'", root.physicalDeviceTimestampPeriod) << std::endl;
              std::cout << std::format("root.physicalDeviceMsaaSamples: '{}'", vk::to_string(root.physicalDeviceMsaaSamples)) << std::endl;
              std::cout << std::format("root.device: '{}'", (bool) root.device.value) << std::endl;
              std::cout << std::format("root.transferQueue: '{}'", (bool) root.transferQueue.value) << std::endl;
              std::cout << std::format("root.graphicsQueue: '{}'", (bool) root.graphicsQueue.value) << std::endl;
              std::cout << std::format("root.presentQueue: '{}'", (bool) root.presentQueue.value) << std::endl;
              std::cout << std::format("root.transferCommandPool: '{}'", (bool) root.transferCommandPool.value) << std::endl;
              std::cout << std::format("root.graphicsCommandPool: '{}'", (bool) root.graphicsCommandPool.value) << std::endl;
              std::cout << std::format("root.transferCommandBuffer: '{}'", (bool) root.transferCommandBuffer.value) << std::endl;
              std::cout << std::format("root.descriptorSetLayout: '{}'", (bool) root.descriptorSetLayout.value) << std::endl;
              std::cout << std::format("root.queryPool: '{}'", (bool) root.queryPool.value) << std::endl;
              std::cout << std::format("root.swapchain: '{}'", (bool) root.swapchain.value) << std::endl;
              std::cout << std::format("root.swapchain.images.size: '{}'", root.swapchain.reference().getImages().size()) << std::endl;
              std::ranges::for_each(root.swapchainImageViews, [](const auto& o1) {std::cout << std::format("root.swapchainImageView: '{}'", (bool) o1.value) << std::endl;});
              std::cout << std::format("root.renderPass: '{}'", (bool) root.renderPass.value) << std::endl;
              std::cout << std::format("root.pipeline: '{}'", (bool) root.pipeline.value) << std::endl;
              std::ranges::for_each(root.swapchainFramebuffers, [](const auto& o1) {std::cout << std::format("root.swapchainFramebuffer: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.samplerImages, [](auto& o1) {std::cout << std::format("root.samplerImage: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.samplerImageViews, [](auto& o1) {std::cout << std::format("root.samplerImageView: '{}'", (bool) o1.value) << std::endl;});
              std::cout << std::format("root.descriptorPool: '{}'", (bool) root.descriptorPool.value) << std::endl;
              std::cout << std::format("root.sampler: '{}'", (bool) root.sampler.value) << std::endl;
              std::ranges::for_each(root.descriptorSets, [](auto& o1) {std::cout << std::format("root.descriptorSet: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.graphicsCommandBuffers, [](const auto& o1) {std::cout << std::format("root.graphicsCommandBuffer: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.imageAvailableSemaphores, [](auto& o1) {std::cout << std::format("root.imageAvailableSemaphore: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.renderFinishedSemaphores, [](auto& o1) {std::cout << std::format("root.renderFinishedSemaphore: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.inFlightFences, [](auto& o1) {std::cout << std::format("root.inFlightFence: '{}'", (bool) o1.value) << std::endl;});

              vertexStagingBuffer = Buffer::builder()
                  .setPhysicalDevice(root.physicalDevice.value)
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::BufferCreateInfo()
                          .setSize(sizeof(vertexVector[0]) * vertexVector.size())
                          .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                          .setSharingMode(vk::SharingMode::eExclusive)
                  )
                  .setMemoryCreateInfo(
                      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                  )
              .build();
              std::cout << std::format("vertexStagingBuffer: '{}'", (bool) vertexStagingBuffer.value) << std::endl;
              vertexBuffer = Buffer::builder()
                  .setPhysicalDevice(root.physicalDevice.value)
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::BufferCreateInfo()
                          .setSize(vertexStagingBuffer.createInfo.size)
                          .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer)
                          .setSharingMode(vk::SharingMode::eExclusive)
                  )
                  .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
              .build();
              std::cout << std::format("vertexBuffer: '{}'", (bool) vertexBuffer.value) << std::endl;

              indexStagingBuffer = Buffer::builder()
                  .setPhysicalDevice(root.physicalDevice.value)
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::BufferCreateInfo()
                          .setSize(sizeof(indexVector[0]) * indexVector.size())
                          .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                          .setSharingMode(vk::SharingMode::eExclusive)
                  )
                  .setMemoryCreateInfo(
                      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                  )
              .build();
              std::cout << std::format("indexStagingBuffer: '{}'", (bool) indexStagingBuffer.value) << std::endl;
              indexBuffer = Buffer::builder()
                  .setPhysicalDevice(root.physicalDevice.value)
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::BufferCreateInfo()
                          .setSize(indexStagingBuffer.createInfo.size)
                          .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer)
                          .setSharingMode(vk::SharingMode::eExclusive)
                  )
                  .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
              .build();
              std::cout << std::format("indexBuffer: '{}'", (bool) indexBuffer.value) << std::endl;

              textureBuffer = Buffer::builder()
                  .setPhysicalDevice(root.physicalDevice.value)
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::BufferCreateInfo()
                          .setSize(textureWidth * textureHeight * textureDepth)
                          .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                          .setSharingMode(vk::SharingMode::eExclusive)
                  )
                  .setMemoryCreateInfo(
                      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                  )
                  .build();
              std::cout << std::format("textureBuffer: '{}'", (bool) textureBuffer.value) << std::endl;
              textureImage = Image::builder()
                  .setPhysicalDevice(root.physicalDevice.value)
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setFormat(vk::Format::eR8G8B8A8Srgb)
                          .setExtent(
                              vk::Extent3D()
                                  .setWidth(textureWidth)
                                  .setHeight(textureHeight)
                                  .setDepth(1)
                          )
                          .setMipLevels(tmpImageMipLevels)
                          .setArrayLayers(1)
                          .setSamples(vk::SampleCountFlagBits::e1)
                          .setTiling(vk::ImageTiling::eOptimal)
                          .setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setQueueFamilyIndices({})
                  )
                  .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
              .build();
              std::cout << std::format("textureImage: '{}'", (bool) textureImage.value) << std::endl;
              textureImageView = ImageView::builder()
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::ImageViewCreateInfo()
                          .setImage(*textureImage.reference())
                          .setFormat(textureImage.createInfo.format)
                          .setViewType(vk::ImageViewType::e2D)
                          .setFlags({})
                          .setComponents({})
                          .setSubresourceRange(
                              vk::ImageSubresourceRange()
                                  .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                  .setBaseMipLevel(0)
                                  .setLevelCount(textureImage.createInfo.mipLevels)
                                  .setBaseArrayLayer(0)
                                  .setLayerCount(1)
                          )
                  )
              .build();
              std::cout << std::format("textureImageView: '{}'", (bool) textureImageView.value) << std::endl;

              uniformBuffers.resize(root.swapchainImageViews.size());
              for (auto& uniformBuffer : uniformBuffers) {
                uniformBuffer = Buffer::builder()
                    .setPhysicalDevice(root.physicalDevice.value)
                    .setDevice(root.device.value)
                    .setCreateInfo(
                        vk::BufferCreateInfo()
                            .setSize(sizeof(UniformBufferObject) * uniformBuffers.size())
                            .setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
                            .setSharingMode(vk::SharingMode::eExclusive)
                    )
                    .setMemoryCreateInfo(
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                    )
                .build();
              }
              std::ranges::for_each(uniformBuffers, [](auto& o1) {std::cout << std::format("uniformBuffer: '{}'", (bool) o1.value) << std::endl;});

              camera.descriptorSetLayout = DescriptorSetLayout::builder()
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
              .build();
              std::cout << std::format("camera.descriptorSetLayout: '{}'", (bool) camera.descriptorSetLayout.value) << std::endl;

              camera.sampler = Sampler::builder()
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::SamplerCreateInfo()
                          .setMagFilter(vk::Filter::eLinear)
                          .setMinFilter(vk::Filter::eLinear)
                          .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                          .setMinLod(0.0f)
                          //.setMinLod((float) textureImage.createInfo.mipLevels / 2)
                          .setMaxLod((float) textureImage.createInfo.mipLevels)
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
              std::cout << std::format("camera.sampler: '{}'", (bool) camera.sampler.value) << std::endl;

              camera.descriptorPool = DescriptorPool::builder()
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
                  .setCreateInfo(
                      vk::DescriptorPoolCreateInfo()
                          .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                          .setMaxSets(root.swapchainImageViews.size())
                  )
              .build();
              std::cout << std::format("camera.descriptorPool: '{}'", (bool) camera.descriptorPool.value) << std::endl;

              camera.descriptorSets.resize(root.swapchainImageViews.size());
              for (size_t i = 0; i < camera.descriptorSets.size(); i++) {
                camera.descriptorSets.at(i) = DescriptorSet::builder()
                    .setDevice(root.device.value)
                    .addSetLayout(*camera.descriptorSetLayout.reference())
                    .setCreateInfo(
                        vk::DescriptorSetAllocateInfo()
                            .setDescriptorPool(*camera.descriptorPool.reference())
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
                                    .setSampler(*camera.sampler.reference())
                                    .setImageView(*textureImageView.reference())
                                    .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                            )
                    )
                .build();
              }
              std::ranges::for_each(camera.descriptorSets, [](auto& o1) {std::cout << std::format("camera.descriptorSet: '{}'", (bool) o1.value) << std::endl;});

              vertexStagingBuffer.fill(vertexVector.data());
              indexStagingBuffer.fill(indexVector.data());
              textureBuffer.fill(textureVector.data());

              root.transferCommandBuffer.reference().begin({});
              root.transferCommandBuffer.reference().copyBuffer(
                  *vertexStagingBuffer.reference(),
                  *vertexBuffer.reference(),
                  {vk::BufferCopy().setSize(vertexStagingBuffer.createInfo.size)}
              );
              root.transferCommandBuffer.reference().copyBuffer(
                  *indexStagingBuffer.reference(),
                  *indexBuffer.reference(),
                  {vk::BufferCopy().setSize(indexStagingBuffer.createInfo.size)}
              );
              TestUtils::copyBufferToImageAndGenerateMipmaps(root.physicalDevice, root.transferCommandBuffer, textureBuffer, textureImage);
              root.transferCommandBuffer.reference().end();
              root.transferQueue.reference().submit(
                  {
                      vk::SubmitInfo()
                          .setCommandBufferCount(1)
                          .setPCommandBuffers(&(*root.transferCommandBuffer.reference()))
                  }
              );
              root.transferQueue.reference().waitIdle();

              reCreateSwapchain(width, height);

              std::cout << std::format("{} ... done", CALL_INFO()) << std::endl;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void reCreateSwapchain(uint32_t width, uint32_t height) {
            try {
              std::cout << std::format("{} ... call", CALL_INFO()) << std::endl;

              Functions::initSwapchain(root, width, height/*, textureWidth, textureHeight, textureDepth*/);

              root.device.reference().waitIdle();

              camera.swapchainFramebuffers.clear();
              camera.pipeline.value.reset();
              camera.renderPass.value.reset();
              camera.depthImageView.value.reset();
              camera.depthImage.value.reset();

              camera.depthImage = Image::builder()
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
                          .setSamples(vk::SampleCountFlagBits::e1)
                          .setTiling(vk::ImageTiling::eOptimal)
                          .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setQueueFamilyIndices({})
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                  )
                  .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
                  .build();
              std::cout << std::format("camera.depthImage: '{}'", (bool) camera.depthImage.value) << std::endl;

              camera.depthImageView = ImageView::builder()
                  .setDevice(root.device.value)
                  .setCreateInfo(
                      vk::ImageViewCreateInfo()
                          .setImage(*camera.depthImage.reference())
                          .setFormat(camera.depthImage.createInfo.format)
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
              std::cout << std::format("camera.depthImageView: '{}'", (bool) camera.depthImageView.value) << std::endl;

              camera.renderPass = RenderPass::builder()
                  .setDevice(root.device.value)
                  .addAttachment(
                      vk::AttachmentDescription()
                          .setFormat(root.samplerImages.at(0).createInfo.format)
                          .setSamples(root.samplerImages.at(0).createInfo.samples)
                          .setLoadOp(vk::AttachmentLoadOp::eClear)
                          .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                          .setStoreOp(vk::AttachmentStoreOp::eStore)
                          .setStencilStoreOp(vk::AttachmentStoreOp::eStore)
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                          .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                  )
                  .addAttachment(
                      vk::AttachmentDescription()
                          .setFormat(camera.depthImage.createInfo.format)
                          .setSamples(camera.depthImage.createInfo.samples)
                          .setLoadOp(vk::AttachmentLoadOp::eClear)
                          .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                          .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                          .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
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
              std::cout << std::format("camera.renderPass: '{}'", (bool) camera.renderPass.value) << std::endl;

              camera.pipeline = Pipeline::builder()
                  .setDevice(root.device.value)
                  .addPath("resources/shader/shader-3.vert.spv")
                  //.addStage(TestUtils::createStage(device, shaders, "resources/shader/shader-4.vert.spv"))
                  .addPath("resources/shader/shader-3.frag.spv")
                  .addSetLayout(*camera.descriptorSetLayout.reference())
                  .setGraphicsCreateInfo(
                      GraphicsPipelineCreateInfo()
                          .setRenderPass(*camera.renderPass.reference())
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
                                  .setRasterizationSamples(vk::SampleCountFlagBits::e1)
                                  .setSampleShadingEnable(false)
                                  .setMinSampleShading(1.0)
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
              std::cout << std::format("camera.pipeline: '{}'", (bool) camera.pipeline.value) << std::endl;

              camera.swapchainFramebuffers.resize(root.swapchainImageViews.size());
              for (size_t i = 0; i < camera.swapchainFramebuffers.size(); i++) {
                camera.swapchainFramebuffers.at(i) = Framebuffer::builder()
                    .setDevice(root.device.value)
                    .addAttachment(*root.samplerImageViews.at(i).reference())
                    .addAttachment(*camera.depthImageView.reference())
                    .setCreateInfo(
                        vk::FramebufferCreateInfo()
                            .setRenderPass(*camera.renderPass.reference())
                            .setWidth(root.samplerImages.at(i).createInfo.extent.width)
                            .setHeight(root.samplerImages.at(i).createInfo.extent.height)
                            .setLayers(1)
                    )
                .build();
              }
              std::ranges::for_each(camera.swapchainFramebuffers, [](const auto& o1) {std::cout << std::format("camera.swapchainFramebuffer: '{}'", (bool) o1.value) << std::endl;});

              root.transferCommandBuffer.reference().begin({});
              TestUtils::insertDepthImagePipelineBarrier(root.transferCommandBuffer, camera.depthImage);
              root.transferCommandBuffer.reference().end();
              root.transferQueue.reference().submit(
                  {
                      vk::SubmitInfo()
                          .setCommandBufferCount(1)
                          .setPCommandBuffers(&(*root.transferCommandBuffer.reference()))
                  }
              );
              root.transferQueue.reference().waitIdle();

              std::cout << std::format("{} ... done", CALL_INFO()) << std::endl;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void updateUniformBuffer(Buffer& uniformBuffer, unsigned int width, unsigned int height) {
            try {
              float angle1 = 0.0f;
              glm::vec3 axis1 = glm::vec3(0.0f, 0.0f, 1.0f);

              float angle2 = 0.0f;
              glm::vec3 axis2 = glm::vec3(0.0f, 1.0f, 0.0f);

              if (ANIMATE == std::string("auto")) {
                static auto startTime = std::chrono::high_resolution_clock::now();

                auto currentTime = std::chrono::high_resolution_clock::now();
                float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
                angle1 = time * glm::radians(90.0f);
                angle2 = 0.0f;
              } else if(ANIMATE == std::string("control")) {
                if (left) {
                  float tmpMod = angleLeft + 0.01f;
                  if (tmpMod >= 1.0f) {
                    tmpMod = 0.0f;
                  }
                  angleLeft = tmpMod;
                } else if (right) {
                  float tmpMod = angleLeft - 0.01f;
                  if (tmpMod <= 0.0f) {
                    tmpMod = 1.0f;
                  }
                  angleLeft = tmpMod;
                }

                angle1 = angleLeft * glm::radians(360.0f);

                if (up) {
                  float tmpMod = angleUp + 0.01f;
                  if (tmpMod >= 1.0f) {
                    tmpMod = 0.0f;
                  }
                  angleUp = tmpMod;
                } else if (down) {
                  float tmpMod = angleUp - 0.01f;
                  if (tmpMod <= 0.0f) {
                    tmpMod = 1.0f;
                  }
                  angleUp = tmpMod;
                }

                angle2 = angleUp * glm::radians(360.0f);
              } else {
                angle1 = 0.0f * glm::radians(360.0f); // min 0 max 360
                angle2 = 0.0f;
              }

              glm::vec3 lightPos = glm::vec3();
              float lightFOV = 45.0f;
              float zNear = 1.0f;
              float zFar = 96.0f;

              UniformBufferObject shadowUbo = {};

              shadowUbo.model = glm::rotate(glm::mat4(1.0f), angle1, axis1);
              shadowUbo.model = glm::rotate(shadowUbo.model, angle2, axis2);
              shadowUbo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
              shadowUbo.proj = glm::perspective(
                  glm::radians(45.0f),
                  (float) width / (float) height,
                  0.1f,
                  10.0f
              );
              shadowUbo.proj[1][1] *= -1;
              shadowUbo.lightPos = glm::vec4(lightPos, 1.0f);
              shadowUbo.lightSpace = glm::mat4(1.0f);
              shadowUbo.zNear = zNear;
              shadowUbo.zFar = zFar;

              UniformBufferObject ubo = {};

              ubo.model = glm::rotate(glm::mat4(1.0f), angle1, axis1);
              ubo.model = glm::rotate(ubo.model, angle2, axis2);
              ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
              ubo.proj = glm::perspective(
                  glm::radians(45.0f),
                  (float) width / (float) height,
                  0.1f,
                  10.0f
              );
              ubo.proj[1][1] *= -1;

              uniformBuffer.fill(&ubo);
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void drawFrame(int width, int height, int depth) {
            try {
              if (
                  root.inFlightFences.size() < root.swapchainImageViews.size()
                  || root.imageAvailableSemaphores.size() < root.swapchainImageViews.size()
                  || root.graphicsCommandBuffers.size() < root.swapchainImageViews.size()
                  || root.swapchainFramebuffers.size() < root.swapchainImageViews.size()
                  || root.descriptorSets.size() < root.swapchainImageViews.size()
                  || root.imageAvailableSemaphores.size() < root.swapchainImageViews.size()
                  || root.renderFinishedSemaphores.size() < root.swapchainImageViews.size()
              ) {
                std::throw_with_nested(std::runtime_error(CALL_INFO() + ": vector.size() < root.swapchainImageViews.size() !!!"));
              }

              vk::Result result = root.device.reference().waitForFences(
                  {*root.inFlightFences.at(currentFrame).reference()},
                  true,
                  UINT64_MAX
              );
              if (vk::Result::eSuccess != result) {
                throw std::runtime_error(CALL_INFO() + ": failed to 'device.waitForFences(...)'!");
              }

              std::pair<vk::Result, uint32_t> swapchainNextImage = root.swapchain
                  .reference()
                  .acquireNextImage(UINT64_MAX, *root.imageAvailableSemaphores.at(currentFrame).reference());
              result = swapchainNextImage.first;
              if (vk::Result::eErrorOutOfDateKHR == result) {
                reCreateSwapchain(width, height);
                return;
              } else if (vk::Result::eSuccess != result && vk::Result::eSuboptimalKHR != result) {
                throw std::runtime_error("failed to 'swapChain.acquireNextImage(...)'!");
              }

              updateUniformBuffer(uniformBuffers.at(currentFrame), width, height);
              root.device.reference().resetFences({*root.inFlightFences.at(currentFrame).reference()});
              root.graphicsCommandBuffers.at(currentFrame).reference().reset();

              root.graphicsCommandBuffers.at(currentFrame).reference().begin({});
              std::vector<vk::ClearValue> clearValues = {
                  vk::ClearValue()
                      .setColor(
                          vk::ClearColorValue()
                              .setFloat32({0.0f, 0.0f, 0.0f, 1.0f})
                      ),
                  vk::ClearValue()
                      .setDepthStencil(
                          vk::ClearDepthStencilValue()
                              .setDepth(1.0f)
                              .setStencil(0)
                      )
              };

              root.graphicsCommandBuffers.at(currentFrame).reference().writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, *root.queryPool.reference(), 0);

              root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
                  vk::RenderPassBeginInfo()
                      .setRenderPass(*camera.renderPass.reference())
                      .setFramebuffer(*camera.swapchainFramebuffers.at(swapchainNextImage.second).reference())
                      .setRenderArea(
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
                      .setClearValues(clearValues),
                  vk::SubpassContents::eInline
              );

              /*root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *camera.pipeline.reference());
              root.graphicsCommandBuffers.at(currentFrame).reference().draw(3, 1, 0, 0);*/
              root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *camera.pipeline.reference());
              root.graphicsCommandBuffers.at(currentFrame).reference().bindVertexBuffers(0, {*vertexBuffer.reference()}, {0});
              root.graphicsCommandBuffers.at(currentFrame).reference().bindIndexBuffer(*indexBuffer.reference(), 0, vk::IndexType::eUint16);
              root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *camera.pipeline.layoutReference(), 0, {*camera.descriptorSets.at(currentFrame).reference()}, {});
              root.graphicsCommandBuffers.at(currentFrame).reference().drawIndexed(indexVector.size(), 1, 0, 0, 0);

              root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

              root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
                  vk::RenderPassBeginInfo()
                      .setRenderPass(*root.renderPass.reference())
                      .setFramebuffer(*root.swapchainFramebuffers.at(swapchainNextImage.second).reference())
                      .setRenderArea(
                          vk::Rect2D()
                              .setOffset(
                                  vk::Offset2D()
                                      .setX(0)
                                      .setY(0)
                              )
                              .setExtent(root.swapchain.createInfo.imageExtent)
                      )
                      .setClearValues(clearValues),
                  vk::SubpassContents::eInline
              );

              root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *root.pipeline.reference());
              root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *root.pipeline.layoutReference(), 0, {*root.descriptorSets.at(currentFrame).reference()}, {});
              root.graphicsCommandBuffers.at(currentFrame).reference().draw(3, 1, 0, 0);

              root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

              root.graphicsCommandBuffers.at(currentFrame).reference().writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, *root.queryPool.reference(), 1);

              try {
                std::pair<vk::Result, std::vector<uint64_t>> queryResults = root.queryPool.reference().getResults<uint64_t>(0, 1, sizeof(uint64_t) * 2, sizeof(uint64_t), vk::QueryResultFlagBits::e64);
                if (vk::Result::eNotReady == queryResults.first || vk::Result::eSuccess == queryResults.first) {
                  if (vk::Result::eSuccess == queryResults.first && queryResults.second.size() > 1) {
                    if (timeDiffCounter == 9) {
                      float timeDiff = root.physicalDevice.reference().getProperties().limits.timestampPeriod * ((float) queryResults.second.at(1) - queryResults.second.at(0));
                      //std::cout << std::format("timeDiff: {}", timeDiff) << std::endl;
                      timeDiffCounter = 0;
                    } else {
                      timeDiffCounter++;
                    }
                  }
                } else {
                  throw std::runtime_error("failed to 'queryPool.getResults(...)'!");
                }
              } catch (...) {
                std::throw_with_nested(std::runtime_error(CALL_INFO() + ": exception in 'queryPool.getResults(...)'!"));
              }

              root.queryPool.reference().reset(0, 2);

              root.graphicsCommandBuffers.at(currentFrame).reference().end();

              std::vector<vk::PipelineStageFlags> waitDstStageMask = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
              std::vector<vk::Semaphore> waitSemaphores = {*root.imageAvailableSemaphores.at(currentFrame).reference()};
              std::vector<vk::Semaphore> signalSemaphores = {*root.renderFinishedSemaphores.at(currentFrame).reference()};
              std::vector<vk::CommandBuffer> commandBuffers = {*root.graphicsCommandBuffers.at(currentFrame).reference()};

              root.graphicsQueue.reference().submit(
                  {
                      vk::SubmitInfo()
                          .setWaitDstStageMask(waitDstStageMask)
                          .setWaitSemaphores(waitSemaphores)
                          .setSignalSemaphores(signalSemaphores)
                          .setCommandBuffers(commandBuffers)
                  },
                  *root.inFlightFences.at(currentFrame).reference()
              );

              std::vector<vk::SwapchainKHR> swapchains = {*root.swapchain.reference()};

              result = root.presentQueue.reference().presentKHR(
                  vk::PresentInfoKHR()
                      .setWaitSemaphores(signalSemaphores)
                      .setSwapchains(swapchains)
                      .setImageIndices(swapchainNextImage.second)
              );
              if (vk::Result::eErrorOutOfDateKHR == result || vk::Result::eSuboptimalKHR == result || resized) {
                resized = false;
                reCreateSwapchain(width, height);
              } else if (vk::Result::eSuccess != result) {
                throw std::runtime_error("failed to 'swapChain.acquireNextImage(...)'!");
              }

              currentFrame = (currentFrame + 1) % root.swapchainImageViews.size();
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void waitIdle() {
            try {
              root.device.reference().waitIdle();
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

      };

      class TestUiApplication {

        public:

          std::vector<std::string> arguments = {};
          TestRenderer* renderer = nullptr;

          TestUiApplication(const int& argc, char** argv) {
            try {
              for (std::size_t i = 0; i < argc; i++) {
                arguments.emplace_back(std::string(argv[i]));
              }
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          int run() {
            try {
              int width = 800;
              int height = 600;
              std::string title = "Vulkan";

              glfwInit();
              glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
              //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

              GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

              glfwSetWindowUserPointer(window, this);
              glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
              //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
              glfwSetKeyCallback(window, keyCallback);

              uint32_t glfwExtensionCount = 0;
              const char** glfwExtensions;
              glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
              std::vector<const char*> glfwInstanceRequiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

              renderer = new TestRenderer();
              renderer->create(
                  arguments,
                  glfwInstanceRequiredExtensions,
                  window,
                  static_cast<uint32_t>(width),
                  static_cast<uint32_t>(height)
              );

              while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                glfwGetFramebufferSize(window, &width, &height);
                while (width == 0 || height == 0) {
                  glfwGetFramebufferSize(window, &width, &height);
                  glfwWaitEvents();
                }
                renderer->drawFrame(width, height, 4);
              }
              renderer->waitIdle();

              delete renderer;
              glfwDestroyWindow(window);
              glfwTerminate();

              return EXIT_SUCCESS;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          static void frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
            try {
              auto* app = reinterpret_cast<TestUiApplication*>(glfwGetWindowUserPointer(window));
              app->renderer->resized = true;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            try {
              auto* app = reinterpret_cast<TestUiApplication*>(glfwGetWindowUserPointer(window));
              int aState = glfwGetKey(window, GLFW_KEY_A);
              int leftState = glfwGetKey(window, GLFW_KEY_LEFT);
              int dState = glfwGetKey(window, GLFW_KEY_D);
              int rightState = glfwGetKey(window, GLFW_KEY_RIGHT);
              int wState = glfwGetKey(window, GLFW_KEY_W);
              int upState = glfwGetKey(window, GLFW_KEY_UP);
              int sState = glfwGetKey(window, GLFW_KEY_S);
              int downState = glfwGetKey(window, GLFW_KEY_DOWN);
              if (aState == GLFW_PRESS || leftState == GLFW_PRESS) {
                app->renderer->left = true;
                app->renderer->right = false;
                app->renderer->up = false;
                app->renderer->down = false;
              } else if (dState == GLFW_PRESS || rightState == GLFW_PRESS) {
                app->renderer->left = false;
                app->renderer->right = true;
                app->renderer->up = false;
                app->renderer->down = false;
              } else if (wState == GLFW_PRESS || upState == GLFW_PRESS) {
                app->renderer->left = false;
                app->renderer->right = false;
                app->renderer->up = true;
                app->renderer->down = false;
              } else if (sState == GLFW_PRESS || downState == GLFW_PRESS) {
                app->renderer->left = false;
                app->renderer->right = false;
                app->renderer->up = false;
                app->renderer->down = true;
              } else {
                app->renderer->left = false;
                app->renderer->right = false;
                app->renderer->up = false;
                app->renderer->down = false;
              }
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

      };

      void SetUp() override {
        try {
          std::cout << std::format("{}", CALL_INFO()) << std::endl;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      void TearDown() override {
        try {
          std::cout << std::format("{}", CALL_INFO()) << std::endl;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  TEST_F(Ui2Tests, test1) {
    try {
      std::string executableDir = TestUtils::getExecutableDir();
      std::vector<char*> arguments = {executableDir.data()};
      int argc = static_cast<int>(arguments.size());
      char** argv = &arguments[0];
      int result = TestUiApplication(argc, argv).run();
      ASSERT_EQ(EXIT_SUCCESS, result);
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
