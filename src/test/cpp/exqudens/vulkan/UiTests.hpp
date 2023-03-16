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

namespace exqudens::vulkan {

  class UiTests : public testing::Test {

    protected:

      class TestRenderer {

        public:

          bool resized = false;

        private:
          inline static const std::string OBJ_FILE_PATH = "";
          inline static const std::string PNG_FILE_PATH = "";
          inline static const bool ANIMATE = false;
          inline static const size_t MAX_FRAMES_IN_FLIGHT = 2;

          std::vector<Vertex> vertexVector = {};
          std::vector<uint16_t> indexVector = {};

          Instance instance = {};
          Surface surface = {};
          vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceHostQueryResetFeatures> physicalDeviceFeatures = {};
          PhysicalDevice physicalDevice = {};
          Device device = {};
          Queue transferQueue = {};
          Queue graphicsQueue = {};
          Queue presentQueue = {};
          CommandPool transferCommandPool = {};
          CommandPool graphicsCommandPool = {};
          CommandBuffer transferCommandBuffer = {};
          std::vector<CommandBuffer> graphicsCommandBuffers = std::vector<CommandBuffer>(MAX_FRAMES_IN_FLIGHT);
          DescriptorSetLayout descriptorSetLayout = {};
          Swapchain swapchain = {};
          std::vector<ImageView> swapchainImageViews = {};
          Image depthImage = {};
          ImageView depthImageView = {};
          Image colorImage = {};
          ImageView colorImageView = {};
          RenderPass renderPass = {};
          std::map<std::string, std::pair<vk::ShaderModuleCreateInfo, std::shared_ptr<vk::raii::ShaderModule>>> shaders;
          Pipeline pipeline = {};
          std::vector<Framebuffer> swapchainFramebuffers = {};
          Buffer textureBuffer = {};
          Image textureImage = {};
          ImageView textureImageView = {};
          Buffer vertexStagingBuffer = {};
          Buffer vertexBuffer = {};
          Buffer indexStagingBuffer = {};
          Buffer indexBuffer = {};
          std::vector<Buffer> uniformBuffers = std::vector<Buffer>(MAX_FRAMES_IN_FLIGHT);
          Sampler sampler = {};
          std::vector<Semaphore> imageAvailableSemaphores = std::vector<Semaphore>(MAX_FRAMES_IN_FLIGHT);
          std::vector<Semaphore> renderFinishedSemaphores = std::vector<Semaphore>(MAX_FRAMES_IN_FLIGHT);
          std::vector<Fence> inFlightFences = std::vector<Fence>(MAX_FRAMES_IN_FLIGHT);
          DescriptorPool descriptorPool = {};
          std::vector<DescriptorSet> descriptorSets = std::vector<DescriptorSet>(MAX_FRAMES_IN_FLIGHT);
          QueryPool queryPool = {};

          size_t currentFrame = 0;
          uint8_t timeDiffCounter = 0;
          float physicalDeviceTimestampPeriod = 0;
          vk::SampleCountFlagBits physicalDeviceMsaaSamples = {};

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
                  {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                  {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                  {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                  {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

                  {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                  {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                  {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                  {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
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
                    .append(/* "texture.png" */ "viking_room.png")
                    .make_preferred()
                    .string();
              } else {
                pngFilePath = std::filesystem::path(pngFilePath).make_preferred().string();
              }
              unsigned int tmpImageWidth, tmpImageHeight, tmpImageDepth, tmpImageMipLevels;
              std::vector<unsigned char> tmpImageData;
              TestUtils::readPng(
                  pngFilePath,
                  tmpImageWidth,
                  tmpImageHeight,
                  tmpImageDepth,
                  tmpImageData
              );
              tmpImageMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(tmpImageWidth, tmpImageHeight)))) + 1;

              Utility::setEnvironmentVariable("VK_LAYER_PATH", arguments.front());

              std::vector<const char*> enabledExtensionNames = glfwInstanceRequiredExtensions;
              enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
              enabledExtensionNames.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

              instance = Instance::builder()
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
              std::cout << std::format("instance: '{}'", (bool) instance.value) << std::endl;

              VkSurfaceKHR vkSurface = nullptr;
              auto vkInstance = static_cast<VkInstance>(*instance.reference());
              if (glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurface) != VK_SUCCESS) {
                throw std::runtime_error(CALL_INFO() + ": failed to create surface!");
              }
              if (vkSurface == nullptr) {
                throw std::runtime_error(CALL_INFO() + ": surface is null!");
              }
              surface = Surface::builder()
                  .setInstance(instance.value)
                  .setVkSurface(vkSurface)
              .build();
              std::cout << std::format("surface: '{}'", (bool) surface.value) << std::endl;

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
              physicalDevice = PhysicalDevice::builder()
                  .setInstance(instance.value)
                  .setSurface(surface.value)
                  .addEnabledExtensionName(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
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
                      if (surface.value && p.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface.reference())) {
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
              std::cout << std::format("physicalDevice: '{}'", (bool) physicalDevice.value) << std::endl;

              physicalDeviceTimestampPeriod = physicalDevice.reference().getProperties().limits.timestampPeriod;
              std::cout << std::format("physicalDeviceTimestampPeriod: '{}'", physicalDeviceTimestampPeriod) << std::endl;

              physicalDeviceMsaaSamples = Utility::getMaxUsableSampleCount({
                  physicalDevice.reference().getProperties().limits.framebufferColorSampleCounts,
                  physicalDevice.reference().getProperties().limits.framebufferDepthSampleCounts
              });
              std::cout << std::format("physicalDeviceMsaaSamples: '{}'", vk::to_string(physicalDeviceMsaaSamples)) << std::endl;

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
              std::cout << std::format("device: '{}'", (bool) device.value) << std::endl;

              transferQueue = Queue::builder()
                  .setDevice(device.value)
                  .setFamilyIndex(physicalDevice.transferQueueCreateInfos.front().queueFamilyIndex)
              .build();
              std::cout << std::format("transferQueue: '{}'", (bool) transferQueue.value) << std::endl;
              graphicsQueue = Queue::builder()
                  .setDevice(device.value)
                  .setFamilyIndex(physicalDevice.graphicsQueueCreateInfos.front().queueFamilyIndex)
              .build();
              std::cout << std::format("graphicsQueue: '{}'", (bool) graphicsQueue.value) << std::endl;
              presentQueue = Queue::builder()
                  .setDevice(device.value)
                  .setFamilyIndex(physicalDevice.presentQueueCreateInfos.front().queueFamilyIndex)
              .build();
              std::cout << std::format("presentQueue: '{}'", (bool) presentQueue.value) << std::endl;

              transferCommandPool = CommandPool::builder()
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::CommandPoolCreateInfo()
                          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                          .setQueueFamilyIndex(transferQueue.familyIndex)
                  )
              .build();
              std::cout << std::format("transferCommandPool: '{}'", (bool) transferCommandPool.value) << std::endl;
              graphicsCommandPool = CommandPool::builder()
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::CommandPoolCreateInfo()
                          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                          .setQueueFamilyIndex(graphicsQueue.familyIndex)
                  )
              .build();
              std::cout << std::format("graphicsCommandPool: '{}'", (bool) graphicsCommandPool.value) << std::endl;

              transferCommandBuffer = CommandBuffer::builder()
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::CommandBufferAllocateInfo()
                          .setCommandPool(*transferCommandPool.reference())
                          .setCommandBufferCount(1)
                          .setLevel(vk::CommandBufferLevel::ePrimary)
                  )
              .build();
              std::cout << std::format("transferCommandBuffer: '{}'", (bool) transferCommandBuffer.value) << std::endl;

              for (auto& commandBuffer : graphicsCommandBuffers) {
                commandBuffer = CommandBuffer::builder()
                    .setDevice(device.value)
                    .setCreateInfo(
                        vk::CommandBufferAllocateInfo()
                            .setCommandPool(*graphicsCommandPool.reference())
                            .setCommandBufferCount(1)
                            .setLevel(vk::CommandBufferLevel::ePrimary)
                    )
                .build();
              }
              std::ranges::for_each(graphicsCommandBuffers, [](const auto& o1) {std::cout << std::format("graphicsCommandBuffer: '{}'", (bool) o1.value) << std::endl;});

              descriptorSetLayout = DescriptorSetLayout::builder()
                  .setDevice(device.value)
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
              std::cout << std::format("descriptorSetLayout: '{}'", (bool) descriptorSetLayout.value) << std::endl;

              createSwapchain(width, height);

              textureBuffer = Buffer::builder()
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::BufferCreateInfo()
                          .setSize(tmpImageWidth * tmpImageHeight * tmpImageDepth)
                          .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                          .setSharingMode(vk::SharingMode::eExclusive)
                  )
                  .setMemoryCreateInfo(
                      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                  )
              .build();
              std::cout << std::format("textureBuffer: '{}'", (bool) textureBuffer.value) << std::endl;
              textureImage = Image::builder()
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setFormat(vk::Format::eR8G8B8A8Srgb)
                          .setExtent(
                              vk::Extent3D()
                                  .setWidth(tmpImageWidth)
                                  .setHeight(tmpImageHeight)
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
                  .setDevice(device.value)
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

              vertexStagingBuffer = Buffer::builder()
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
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
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
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
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
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
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::BufferCreateInfo()
                          .setSize(indexStagingBuffer.createInfo.size)
                          .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer)
                          .setSharingMode(vk::SharingMode::eExclusive)
                  )
                  .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
              .build();
              std::cout << std::format("indexBuffer: '{}'", (bool) indexBuffer.value) << std::endl;

              for (auto& uniformBuffer : uniformBuffers) {
                uniformBuffer = Buffer::builder()
                    .setPhysicalDevice(physicalDevice.value)
                    .setDevice(device.value)
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

              sampler = Sampler::builder()
                  .setDevice(device.value)
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
                          .setAnisotropyEnable(physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy)
                          .setMaxAnisotropy(physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy ? physicalDevice.reference().getProperties().limits.maxSamplerAnisotropy : 0)
                  )
              .build();
              std::cout << std::format("sampler: '{}'", (bool) sampler.value) << std::endl;

              for (auto& imageAvailableSemaphore : imageAvailableSemaphores) {
                imageAvailableSemaphore = Semaphore::builder().setDevice(device.value).build();
              }
              std::ranges::for_each(imageAvailableSemaphores, [](auto& o1) {std::cout << std::format("imageAvailableSemaphore: '{}'", (bool) o1.value) << std::endl;});

              for (auto& renderFinishedSemaphore : renderFinishedSemaphores) {
                renderFinishedSemaphore = Semaphore::builder().setDevice(device.value).build();
              }
              std::ranges::for_each(renderFinishedSemaphores, [](auto& o1) {std::cout << std::format("renderFinishedSemaphore: '{}'", (bool) o1.value) << std::endl;});

              for (auto& inFlightFence : inFlightFences) {
                inFlightFence = Fence::builder()
                    .setDevice(device.value)
                    .setCreateInfo(
                        vk::FenceCreateInfo()
                            .setFlags(vk::FenceCreateFlagBits::eSignaled)
                    )
                .build();
              }
              std::ranges::for_each(inFlightFences, [](auto& o1) {std::cout << std::format("inFlightFence: '{}'", (bool) o1.value) << std::endl;});

              descriptorPool = DescriptorPool::builder()
                  .setDevice(device.value)
                  .addPoolSize(
                      vk::DescriptorPoolSize()
                          .setType(vk::DescriptorType::eUniformBuffer)
                          .setDescriptorCount(MAX_FRAMES_IN_FLIGHT)
                  )
                  .addPoolSize(
                      vk::DescriptorPoolSize()
                          .setType(vk::DescriptorType::eCombinedImageSampler)
                          .setDescriptorCount(MAX_FRAMES_IN_FLIGHT)
                  )
                  .setCreateInfo(
                      vk::DescriptorPoolCreateInfo()
                          .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                          .setMaxSets(MAX_FRAMES_IN_FLIGHT)
                  )
              .build();
              std::cout << std::format("descriptorPool: '{}'", (bool) descriptorPool.value) << std::endl;

              for (size_t i = 0 ; i < descriptorSets.size(); i++) {
                descriptorSets[i] = DescriptorSet::builder()
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
                            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                            .addBufferInfo(
                                vk::DescriptorBufferInfo()
                                    .setBuffer(*uniformBuffers[i].reference())
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
                .build();
              }
              std::ranges::for_each(descriptorSets, [](auto& o1) {std::cout << std::format("descriptorSet: '{}'", (bool) o1.value) << std::endl;});

              queryPool = QueryPool::builder()
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::QueryPoolCreateInfo()
                          /*.setPipelineStatistics(vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations)*/
                          .setQueryType(vk::QueryType::eTimestamp)
                          .setQueryCount(2)
                  )
              .build();
              std::cout << std::format("queryPool: '{}'", (bool) queryPool.value) << std::endl;

              textureBuffer.fill(tmpImageData.data());
              vertexStagingBuffer.fill(vertexVector.data());
              indexStagingBuffer.fill(indexVector.data());

              transferCommandBuffer.reference().begin({});

              TestUtils::insertDepthImagePipelineBarrier(transferCommandBuffer, depthImage);

              TestUtils::copyBufferToImageAndGenerateMipmaps(physicalDevice, transferCommandBuffer, textureBuffer, textureImage);

              transferCommandBuffer.reference().copyBuffer(
                  *vertexStagingBuffer.reference(),
                  *vertexBuffer.reference(),
                  {vk::BufferCopy().setSize(vertexStagingBuffer.createInfo.size)}
              );

              transferCommandBuffer.reference().copyBuffer(
                  *indexStagingBuffer.reference(),
                  *indexBuffer.reference(),
                  {vk::BufferCopy().setSize(indexStagingBuffer.createInfo.size)}
              );

              transferCommandBuffer.reference().end();
              transferQueue.reference().submit(
                  {vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&(*transferCommandBuffer.reference()))}
              );
              transferQueue.reference().waitIdle();

              std::cout << std::format("{} ... done", CALL_INFO()) << std::endl;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void createSwapchain(uint32_t width, uint32_t height) {
            try {
              std::cout << std::format("{} ... call", CALL_INFO()) << std::endl;

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
              std::cout << std::format("swapchain: '{}'", (bool) swapchain.value) << std::endl;

              for (const VkImage& vkImage : swapchain.reference().getImages()) {
                swapchainImageViews.emplace_back(
                    ImageView::builder()
                        .setDevice(device.value)
                        .setCreateInfo(
                            vk::ImageViewCreateInfo()
                                .setFlags({})
                                .setImage(static_cast<vk::Image>(vkImage))
                                .setViewType(vk::ImageViewType::e2D)
                                .setFormat(swapchain.createInfo.imageFormat)
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
                    .build()
                );
              }
              std::ranges::for_each(swapchainImageViews, [](const auto& o1) {std::cout << std::format("swapchainImageView: '{}'", (bool) o1.value) << std::endl;});

              depthImage = Image::builder()
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setFormat(Utility::imageDepthFormat(physicalDevice.reference()))
                          .setExtent(
                              vk::Extent3D()
                                  .setWidth(swapchain.createInfo.imageExtent.width)
                                  .setHeight(swapchain.createInfo.imageExtent.height)
                                  .setDepth(1)
                          )
                          .setMipLevels(1)
                          .setArrayLayers(1)
                          .setSamples(physicalDeviceMsaaSamples)
                          .setTiling(vk::ImageTiling::eOptimal)
                          .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setQueueFamilyIndices({})
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                  )
                  .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
              .build();
              std::cout << std::format("depthImage: '{}'", (bool) depthImage.value) << std::endl;

              depthImageView = ImageView::builder()
                  .setDevice(device.value)
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
              std::cout << std::format("depthImageView: '{}'", (bool) depthImageView.value) << std::endl;

              colorImage = Image::builder()
                  .setPhysicalDevice(physicalDevice.value)
                  .setDevice(device.value)
                  .setCreateInfo(
                      vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setFormat(swapchain.createInfo.imageFormat)
                          .setExtent(
                              vk::Extent3D()
                                  .setWidth(swapchain.createInfo.imageExtent.width)
                                  .setHeight(swapchain.createInfo.imageExtent.height)
                                  .setDepth(1)
                          )
                          .setMipLevels(1)
                          .setArrayLayers(1)
                          .setSamples(physicalDeviceMsaaSamples)
                          .setTiling(vk::ImageTiling::eOptimal)
                          .setUsage(vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setQueueFamilyIndices({})
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                  )
                  .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
              .build();
              std::cout << std::format("colorImage: '{}'", (bool) colorImage.value) << std::endl;

              colorImageView = ImageView::builder()
                  .setDevice(device.value)
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
              std::cout << std::format("colorImageView: '{}'", (bool) colorImageView.value) << std::endl;

              renderPass = RenderPass::builder()
                  .setDevice(device.value)
                  .addAttachment(
                      vk::AttachmentDescription()
                          .setFormat(colorImage.createInfo.format)
                          .setSamples(colorImage.createInfo.samples)
                          .setLoadOp(vk::AttachmentLoadOp::eClear)
                          .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                          .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
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
                          .setFormat(swapchain.createInfo.imageFormat)
                          .setSamples(vk::SampleCountFlagBits::e1)
                          .setLoadOp(vk::AttachmentLoadOp::eDontCare)
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
                  .addDependency(
                      vk::SubpassDependency()
                          .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                          .setDstSubpass(0)
                          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                          .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                          .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
                          .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
                  )
              .build();
              std::cout << std::format("renderPass: '{}'", (bool) renderPass.value) << std::endl;

              pipeline = Pipeline::builder()
                  .setDevice(device.value)
                  //.addPath("resources/shader/shader-4.vert.spv")
                  .addStage(TestUtils::createStage(device, shaders, "resources/shader/shader-4.vert.spv"))
                  .addPath("resources/shader/shader-4.frag.spv")
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
                                          .setWidth((float) swapchain.createInfo.imageExtent.width)
                                          .setHeight((float) swapchain.createInfo.imageExtent.height)
                                          .setMinDepth(0.0)
                                          .setMaxDepth(1.0)
                                          .setX(0.0)
                                          .setY(0.0)
                                  )
                                  .addScissor(
                                      vk::Rect2D()
                                          .setOffset({0, 0})
                                          .setExtent(swapchain.createInfo.imageExtent)
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
                                  .setRasterizationSamples(physicalDeviceMsaaSamples)
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
              std::cout << std::format("pipeline: '{}'", (bool) pipeline.value) << std::endl;

              for (auto& imageView : swapchainImageViews) {
                swapchainFramebuffers.emplace_back(
                    Framebuffer::builder()
                        .setDevice(device.value)
                        .addAttachment(*colorImageView.reference())
                        .addAttachment(*depthImageView.reference())
                        .addAttachment(*imageView.reference())
                        .setCreateInfo(
                            vk::FramebufferCreateInfo()
                                .setRenderPass(*renderPass.reference())
                                .setWidth(swapchain.createInfo.imageExtent.width)
                                .setHeight(swapchain.createInfo.imageExtent.height)
                                .setLayers(1)
                        )
                    .build()
                );
              }
              std::ranges::for_each(swapchainFramebuffers, [](const auto& o1) {std::cout << std::format("swapchainFramebuffer: '{}'", (bool) o1.value) << std::endl;});

              std::cout << std::format("{} ... done", CALL_INFO()) << std::endl;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void reCreateSwapchain(int width, int height) {
            try {
              std::cout << std::format("{} ... call", CALL_INFO()) << std::endl;

              device.reference().waitIdle();

              std::ranges::for_each(swapchainFramebuffers, [](auto& o1) {o1.value.reset();});
              swapchainFramebuffers.clear();
              pipeline.value.reset();
              renderPass.value.reset();
              colorImageView.value.reset();
              colorImage.value.reset();
              depthImageView.value.reset();
              depthImage.value.reset();
              std::ranges::for_each(swapchainImageViews, [](auto& o1) {o1.value.reset();});
              swapchainImageViews.clear();
              swapchain.value.reset();

              createSwapchain(width, height);

              transferCommandBuffer.reference().begin({});

              TestUtils::insertDepthImagePipelineBarrier(transferCommandBuffer, depthImage);

              transferCommandBuffer.reference().end();
              transferQueue.reference().submit(
                  {
                      vk::SubmitInfo()
                          .setCommandBufferCount(1)
                          .setPCommandBuffers(&(*transferCommandBuffer.reference()))
                  }
              );
              transferQueue.reference().waitIdle();

              std::cout << std::format("{} ... done", CALL_INFO()) << std::endl;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void drawFrame(int width, int height) {
            try {
              if (
                  inFlightFences.size() < MAX_FRAMES_IN_FLIGHT
                  || imageAvailableSemaphores.size() < MAX_FRAMES_IN_FLIGHT
                  || graphicsCommandBuffers.size() < MAX_FRAMES_IN_FLIGHT
                  || swapchainFramebuffers.size() < MAX_FRAMES_IN_FLIGHT
                  || descriptorSets.size() < MAX_FRAMES_IN_FLIGHT
                  || imageAvailableSemaphores.size() < MAX_FRAMES_IN_FLIGHT
                  || renderFinishedSemaphores.size() < MAX_FRAMES_IN_FLIGHT
              ) {
                std::throw_with_nested(std::runtime_error(CALL_INFO() + ": vector.size() < MAX_FRAMES_IN_FLIGHT !!!"));
              }

              vk::Result result = device.reference().waitForFences(
                  {*inFlightFences.at(currentFrame).reference()},
                  true,
                  UINT64_MAX
              );
              if (vk::Result::eSuccess != result) {
                throw std::runtime_error(CALL_INFO() + ": failed to 'device.waitForFences(...)'!");
              }

              std::pair<vk::Result, uint32_t> swapchainNextImage = swapchain
                  .reference()
                  .acquireNextImage(UINT64_MAX, *imageAvailableSemaphores.at(currentFrame).reference());
              result = swapchainNextImage.first;
              if (vk::Result::eErrorOutOfDateKHR == result) {
                reCreateSwapchain(width, height);
                return;
              } else if (vk::Result::eSuccess != result && vk::Result::eSuboptimalKHR != result) {
                throw std::runtime_error("failed to 'swapChain.acquireNextImage(...)'!");
              }

              updateUniformBuffer(uniformBuffers.at(currentFrame));
              device.reference().resetFences({*inFlightFences.at(currentFrame).reference()});
              graphicsCommandBuffers.at(currentFrame).reference().reset();

              graphicsCommandBuffers.at(currentFrame).reference().begin({});
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

              graphicsCommandBuffers.at(currentFrame).reference().writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, *queryPool.reference(), 0);

              graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
                  vk::RenderPassBeginInfo()
                      .setRenderPass(*renderPass.reference())
                      .setFramebuffer(*swapchainFramebuffers.at(swapchainNextImage.second).reference())
                      .setRenderArea(
                          vk::Rect2D()
                              .setOffset(
                                  vk::Offset2D()
                                      .setX(0)
                                      .setY(0)
                              )
                              .setExtent(swapchain.createInfo.imageExtent)
                      )
                      .setClearValues(clearValues),
                  vk::SubpassContents::eInline
              );

              graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.reference());
              graphicsCommandBuffers.at(currentFrame).reference().bindVertexBuffers(0, {*vertexBuffer.reference()}, {0});
              graphicsCommandBuffers.at(currentFrame).reference().bindIndexBuffer(*indexBuffer.reference(), 0, vk::IndexType::eUint16);
              graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.layoutReference(), 0, {*descriptorSets.at(currentFrame).reference()}, {});
              graphicsCommandBuffers.at(currentFrame).reference().drawIndexed(indexVector.size(), 1, 0, 0, 0);

              graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

              graphicsCommandBuffers.at(currentFrame).reference().writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, *queryPool.reference(), 1);

              try {
                std::pair<vk::Result, std::vector<uint64_t>> queryResults = queryPool.reference().getResults<uint64_t>(0, 1, sizeof(uint64_t) * 2, sizeof(uint64_t), vk::QueryResultFlagBits::e64);
                if (vk::Result::eNotReady == queryResults.first || vk::Result::eSuccess == queryResults.first) {
                  if (vk::Result::eSuccess == queryResults.first && queryResults.second.size() > 1) {
                    if (timeDiffCounter == 9) {
                      float timeDiff = physicalDevice.reference().getProperties().limits.timestampPeriod * ((float) queryResults.second.at(1) - queryResults.second.at(0));
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

              queryPool.reference().reset(0, 2);

              graphicsCommandBuffers.at(currentFrame).reference().end();

              std::vector<vk::PipelineStageFlags> waitDstStageMask = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
              std::vector<vk::Semaphore> waitSemaphores = {*imageAvailableSemaphores.at(currentFrame).reference()};
              std::vector<vk::Semaphore> signalSemaphores = {*renderFinishedSemaphores.at(currentFrame).reference()};
              std::vector<vk::CommandBuffer> commandBuffers = {*graphicsCommandBuffers.at(currentFrame).reference()};

              graphicsQueue.reference().submit(
                  {
                      vk::SubmitInfo()
                          .setWaitDstStageMask(waitDstStageMask)
                          .setWaitSemaphores(waitSemaphores)
                          .setSignalSemaphores(signalSemaphores)
                          .setCommandBuffers(commandBuffers)
                  },
                  *inFlightFences.at(currentFrame).reference()
              );

              std::vector<vk::SwapchainKHR> swapchains = {*swapchain.reference()};

              result = presentQueue.reference().presentKHR(
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

              currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void waitIdle() {
            try {
              device.reference().waitIdle();
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void updateUniformBuffer(Buffer& uniformBuffer) {
            try {
              float angle = 0.0f;

              if (ANIMATE) {
                static auto startTime = std::chrono::high_resolution_clock::now();

                auto currentTime = std::chrono::high_resolution_clock::now();
                float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
                angle = time * glm::radians(90.0f);
              } else {
                angle = 0.5f * glm::radians(0.0f); // min 0 max 360
              }

              UniformBufferObject ubo = {};
              ubo.model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
              ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
              ubo.proj = glm::perspective(
                  glm::radians(45.0f),
                  (float) swapchain.createInfo.imageExtent.width / (float) swapchain.createInfo.imageExtent.height,
                  0.1f,
                  10.0f
              );
              ubo.proj[1][1] *= -1;

              uniformBuffer.fill(&ubo);
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
              //glfwSetKeyCallback(window, keyCallback);

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
                renderer->drawFrame(width, height);
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

  TEST_F(UiTests, test1) {
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
