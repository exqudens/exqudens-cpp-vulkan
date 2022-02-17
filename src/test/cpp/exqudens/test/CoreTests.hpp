#pragma once

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <sstream>
#include <thread>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

#include <gtest/gtest.h>

#include <vulkan/vulkan.h>

#include "exqudens/TestConfiguration.hpp"
#include "exqudens/TestUtils.hpp"
#include "exqudens/vulkan/utility/Factory.hpp"

namespace exqudens::vulkan {

  class CoreTests : public testing::Test {

    protected:

      void submitBlocking(VkDevice& device, VkCommandBuffer& cmdBuffer, VkQueue& queue) {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkFence fence = nullptr;

        if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VkResult::VK_SUCCESS || fence == nullptr) {
          throw std::runtime_error(CALL_INFO() + ": failed to create fence!");
        }

        vkResetFences(device, 1, &fence);

        if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VkResult::VK_SUCCESS) {
          throw std::runtime_error(CALL_INFO() + ": failed to submit queue!");
        }

        if (vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX) != VkResult::VK_SUCCESS) {
          throw std::runtime_error(CALL_INFO() + ": failed to wait for fences!");
        }

        vkDestroyFence(device, fence, nullptr);
      }

  };

  TEST_F(CoreTests, test1) {
    try {
      Factory factory;
      Configuration configuration = factory.createConfiguration();

      ASSERT_EQ(true, configuration.validationLayersEnabled);
      ASSERT_EQ(std::string("VK_LAYER_KHRONOS_validation"), std::string(configuration.validationLayers[0]));
      ASSERT_EQ(std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME), std::string(configuration.extensions[0]));
      ASSERT_EQ(std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME), std::string(configuration.deviceExtensions[0]));

    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(CoreTests, test2) {
    try {
      Factory factory;

      std::map<std::string, std::string> environmentVariables = factory.createEnvironmentVariables(TestConfiguration::getExecutableDir());

      for (auto const& [name, value] : environmentVariables) {
        factory.setEnvironmentVariable(name, value);
      }

      Configuration configuration = factory.createConfiguration();
      configuration.presentQueueFamilyRequired = false;
      configuration.deviceExtensions = {};
      std::ostringstream stream;
      Logger logger = factory.createLogger(stream);

      VkInstance instance = factory.createInstance(configuration, logger);
      VkDebugUtilsMessengerEXT debugUtilsMessenger = factory.createDebugUtilsMessenger(instance, logger);
      PhysicalDevice physicalDevice = factory.createPhysicalDevice(instance, configuration);
      VkDevice device = factory.createDevice(physicalDevice.value, configuration, physicalDevice.queueFamilyIndexInfo);

      Shader vertexShader1 = factory.createShader(device, "resources/shader/shader.vert.spv");
      Shader fragmentShader1 = factory.createShader(device, "resources/shader/shader.frag.spv");

      ASSERT_TRUE(vertexShader1.shaderModule != nullptr);
      ASSERT_EQ(vertexShader1.pipelineShaderStageCreateInfo.stage, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
      ASSERT_TRUE(fragmentShader1.shaderModule != nullptr);
      ASSERT_EQ(fragmentShader1.pipelineShaderStageCreateInfo.stage, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

      Shader vertexShader2 = vertexShader1;
      Shader fragmentShader2 = fragmentShader1;

      ASSERT_TRUE(vertexShader2.shaderModule != nullptr);
      ASSERT_EQ(vertexShader2.pipelineShaderStageCreateInfo.stage, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
      ASSERT_TRUE(fragmentShader2.shaderModule != nullptr);
      ASSERT_EQ(fragmentShader2.pipelineShaderStageCreateInfo.stage, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

      factory.destroyShader(vertexShader1, device);
      factory.destroyShader(fragmentShader1, device);

      ASSERT_TRUE(vertexShader2.shaderModule != nullptr);
      ASSERT_TRUE(fragmentShader2.shaderModule != nullptr);

      factory.destroyDevice(device);
      factory.destroyPhysicalDevice(physicalDevice);
      factory.destroyDebugUtilsMessenger(debugUtilsMessenger, instance);
      factory.destroyInstance(instance);
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(CoreTests, test3) {
    try {
      Factory factory;

      std::map<std::string, std::string> environmentVariables = factory.createEnvironmentVariables(TestConfiguration::getExecutableDir());

      for (auto const& [name, value] : environmentVariables) {
        factory.setEnvironmentVariable(name, value);
      }

      Configuration configuration = factory.createConfiguration();
      configuration.presentQueueFamilyRequired = false;
      configuration.deviceExtensions = {};
      std::ostringstream stream;
      Logger logger = factory.createLogger(stream);

      VkInstance instance = factory.createInstance(configuration, logger);
      VkDebugUtilsMessengerEXT debugUtilsMessenger = factory.createDebugUtilsMessenger(instance, logger);
      PhysicalDevice physicalDevice = factory.createPhysicalDevice(instance, configuration);
      VkDevice device = factory.createDevice(physicalDevice.value, configuration, physicalDevice.queueFamilyIndexInfo);
      Queue graphicsQueue = factory.createQueue(device, physicalDevice.queueFamilyIndexInfo.graphicsFamily.value(), 0);
      VkCommandPool graphicsCommandPool = factory.createCommandPool(device, graphicsQueue.familyIndex);
      Image image = factory.createImage(physicalDevice.value, device, 800, 600, VkFormat::VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      VkImageView imageView = factory.createImageView(device, image.value, image.format);
      VkRenderPass renderPass = factory.createRenderPass(device, image.format);
      VkFramebuffer frameBuffer = factory.createFrameBuffer(device, imageView, renderPass, image.width, image.height);
      Pipeline graphicsPipeline = factory.createGraphicsPipeline(device, {.width = image.width, .height = image.height}, {"resources/shader/shader.vert.spv", "resources/shader/shader.frag.spv"}, renderPass);
      VkCommandBuffer graphicsCommandBuffer = factory.createCommandBuffer(device, graphicsCommandPool);
      Image imageOut = factory.createImage(physicalDevice.value, device, 800, 600, image.format, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      VkCommandBuffer copyCommandBuffer = factory.createCommandBuffer(device, graphicsCommandPool);

      VkCommandBufferBeginInfo beginInfo = {};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

      if (vkBeginCommandBuffer(graphicsCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error(CALL_INFO() + ": failed to begin graphics command buffer!");
      }

      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = renderPass;
      renderPassInfo.framebuffer = frameBuffer;
      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = {.width = image.width, .height = image.height};

      VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      vkCmdBeginRenderPass(graphicsCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.value);

      vkCmdDraw(graphicsCommandBuffer, 3, 1, 0, 0);

      vkCmdEndRenderPass(graphicsCommandBuffer);

      if (vkEndCommandBuffer(graphicsCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error(CALL_INFO() + ": failed to end graphics command buffer!");
      }

      submitBlocking(device, graphicsCommandBuffer, graphicsQueue.value);
      vkDeviceWaitIdle(device);

      /*VkImageSubresourceRange subresourceRange = {};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.baseArrayLayer = 0;
      subresourceRange.layerCount = 1;

      VkImageMemoryBarrier memoryBarrier = {};
      memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      memoryBarrier.image = imageOut.value;
      memoryBarrier.srcAccessMask = 0;
      memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      memoryBarrier.subresourceRange = subresourceRange;*/

      if (vkBeginCommandBuffer(copyCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error(CALL_INFO() + ": failed to begin copy command buffer!");
      }

      VkImageMemoryBarrier imageMemoryBarrier1 = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .srcAccessMask = 0,
          .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .image = imageOut.value,
          .subresourceRange = {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1
          }
      };

      vkCmdPipelineBarrier(
          copyCommandBuffer,
          VK_PIPELINE_STAGE_TRANSFER_BIT,
          VK_PIPELINE_STAGE_TRANSFER_BIT,
          0,
          0,
          nullptr,
          0,
          nullptr,
          1,
          &imageMemoryBarrier1
      );

      VkImageCopy imageCopyRegion = {};
      imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      imageCopyRegion.srcSubresource.layerCount = 1;
      imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      imageCopyRegion.dstSubresource.layerCount = 1;
      imageCopyRegion.extent.width = image.width;
      imageCopyRegion.extent.height = image.height;
      imageCopyRegion.extent.depth = 1;

      vkCmdCopyImage(
          copyCommandBuffer,
          image.value,
          VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
          imageOut.value,
          VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          1,
          &imageCopyRegion
      );

      VkImageMemoryBarrier imageMemoryBarrier2 = {
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_GENERAL,
          .image = imageOut.value,
          .subresourceRange = {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1
          }
      };

      vkCmdPipelineBarrier(
          copyCommandBuffer,
          VK_PIPELINE_STAGE_TRANSFER_BIT,
          VK_PIPELINE_STAGE_TRANSFER_BIT,
          0,
          0,
          nullptr,
          0,
          nullptr,
          1,
          &imageMemoryBarrier2
      );

      if (vkEndCommandBuffer(copyCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error(CALL_INFO() + ": failed to end copy command buffer!!");
      }

      submitBlocking(device, copyCommandBuffer, graphicsQueue.value);

      ///

      VkImageSubresource subResource = {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
      };
      VkSubresourceLayout subResourceLayout;

      vkGetImageSubresourceLayout(device, imageOut.value, &subResource, &subResourceLayout);

      unsigned char* imageData;
      vkMapMemory(device, imageOut.memory, 0, imageOut.memorySize, 0, (void**) &imageData);
      imageData += subResourceLayout.offset;

      //////
      std::vector<std::vector<std::vector<unsigned char>>> imageDataVector;
      std::size_t depth = 4;
      imageDataVector.resize(imageOut.height);
      for (uint32_t y = 0; y < imageOut.height; y++) {
        imageDataVector[y].resize(imageOut.width);
        for (uint32_t x = 0; x < imageOut.width; x++) {
          imageDataVector[y][x].resize(depth);
          for (uint32_t z = 0; z < depth; z++) {
            std::size_t yOffset = y * subResourceLayout.rowPitch;
            std::size_t xOffset = x * depth;
            std::size_t zOffset = z;
            std::size_t offset = yOffset + xOffset + zOffset;
            imageDataVector[y][x][z] = imageData[offset];
          }
        }
      }
      TestUtils::writePng(
          imageDataVector,
          std::filesystem::path(TestConfiguration::getExecutableDir())
            .append("resources")
            .append("png")
            .append("core-tests-test3-result.png")
            .make_preferred()
            .string()
      );
      //////

      vkUnmapMemory(device, imageOut.memory);

      ///

      vkQueueWaitIdle(graphicsQueue.value);

      factory.destroyCommandBuffer(copyCommandBuffer, graphicsCommandPool, device);
      factory.destroyImage(imageOut, device);
      factory.destroyCommandBuffer(graphicsCommandBuffer, graphicsCommandPool, device);
      factory.destroyPipeline(graphicsPipeline, device);
      factory.destroyFrameBuffer(frameBuffer, device);
      factory.destroyRenderPass(renderPass, device);
      factory.destroyImageView(imageView, device);
      factory.destroyImage(image, device);
      factory.destroyCommandPool(graphicsCommandPool, device);
      factory.destroyQueue(graphicsQueue);
      factory.destroyDevice(device);
      factory.destroyPhysicalDevice(physicalDevice);
      factory.destroyDebugUtilsMessenger(debugUtilsMessenger, instance);
      factory.destroyInstance(instance);

      std::cout << stream.str();

      std::vector<std::vector<std::vector<unsigned char>>> image1 = TestUtils::readPng(
          std::filesystem::path(TestConfiguration::getExecutableDir())
              .append("resources")
              .append("png")
              .append("core-tests-test3-expected.png")
              .make_preferred()
              .string()
      );
      std::vector<std::vector<std::vector<unsigned char>>> image2 = TestUtils::readPng(
          std::filesystem::path(TestConfiguration::getExecutableDir())
              .append("resources")
              .append("png")
              .append("core-tests-test3-result.png")
              .make_preferred()
              .string()
      );
      ASSERT_EQ(image1.size(), image2.size());
      ASSERT_EQ(image1[0].size(), image2[0].size());
      ASSERT_EQ(image1[0][0].size(), image2[0][0].size());
      ASSERT_EQ(image1, image2);
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
