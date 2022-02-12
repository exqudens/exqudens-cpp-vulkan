#include "exqudens/vulkan/utility/Functions.hpp"
#include "exqudens/vulkan/utility/Macros.hpp"
#include "exqudens/vulkan/model/Vertex.hpp"

#include <array>
#include <set>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <cstdlib>

namespace exqudens::vulkan {

  Functions::Functions(
      std::function<VkSurfaceKHR(VkInstance&)> createSurfaceFunction
  ):
      createSurfaceFunction(std::move(createSurfaceFunction))
  {
  }

  Functions::Functions() = default;

  Functions::Functions(const Functions& object): Functions(
      object.createSurfaceFunction
  ) {
  }

  Functions::Functions(Functions&& object) noexcept: Functions() {
    swap(*this, object);
  }

  Functions& Functions::operator=(const Functions& object) {
    return *this = Functions(object);
  }

  Functions& Functions::operator=(Functions&& object) noexcept {
    swap(*this, object);
    return *this;
  }

  void swap(Functions& first, Functions& second) {
    std::swap(first.createSurfaceFunction, second.createSurfaceFunction);
  }

  // utility

  void Functions::setEnvironmentVariable(const std::string& name, const std::string& value) {
    try {
#ifdef _WIN32
      _putenv_s(name.c_str(), value.c_str());
#elif _WIN64
      _putenv_s(name.c_str(), value.c_str());
#endif
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  std::optional<std::string> Functions::getEnvironmentVariable(const std::string& name) {
    try {
      std::optional<std::string> value;
#ifdef _WIN32
      char* buffer;
      size_t size;
      errno_t error = _dupenv_s(&buffer, &size, name.c_str());
      if (error) {
        return value;
      }
      if (buffer != nullptr) {
        value.emplace(std::string(buffer));
      }
#elif _WIN64
      char* buffer;
    size_t size;
    errno_t error = _dupenv_s(&buffer, &size, name.c_str());
    if (error) {
      return value;
    }
    if (buffer != nullptr) {
      value.emplace(std::string(buffer));
    }
#endif
      return value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  std::vector<char> Functions::readFile(const std::string& path) {
    try {
      std::ifstream file(path, std::ios::ate | std::ios::binary);

      if (!file.is_open()) {
        throw std::runtime_error(CALL_INFO() + ": failed to open file: '" + path + "'!");
      }

      std::streamsize fileSize = file.tellg();
      std::vector<char> buffer(fileSize);

      file.seekg(0);
      file.read(buffer.data(), fileSize);

      file.close();

      return buffer;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  bool Functions::checkValidationLayerSupport(const std::vector<const char*>& validationLayers) {
    try {
      uint32_t layerCount;
      vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

      std::vector<VkLayerProperties> availableLayers(layerCount);
      vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

      for (const std::string& layerName: validationLayers) {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers) {
          if (std::string(layerName) == std::string(layerProperties.layerName)) {
            layerFound = true;
            break;
          }
        }

        if (!layerFound) {
          return false;
        }
      }

      return true;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& object, Logger& logger) {
    try {
      object = {};
      object.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      object.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      object.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      object.pfnUserCallback = &Logger::call;
      object.pUserData = &logger;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  bool Functions::isDeviceSuitable(
      VkPhysicalDevice& physicalDevice,
      Configuration& configuration,
      VkSurfaceKHR& surface
  ) {
    try {
      if (configuration.presentQueueFamilyRequired && surface == nullptr) {
        throw std::runtime_error(
            CALL_INFO()
            + ": '"
            + TO_STRING(configuration.presentQueueFamilyRequired)
            + "': true but '"
            + TO_STRING(surface)
            + "' is null!"
        );
      }

      QueueFamilyIndices familyIndices = findQueueFamilies(
          physicalDevice,
          configuration.computeQueueFamilyRequired,
          configuration.transferQueueFamilyRequired,
          configuration.graphicsQueueFamilyRequired,
          surface
      );

      bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice, configuration.deviceExtensions);

      bool swapChainAdequate = surface == nullptr;
      if (extensionsSupported && surface != nullptr) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
      }

      VkPhysicalDeviceFeatures supportedFeatures;
      vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

      bool anisotropyAdequate = true;
      if (configuration.anisotropyRequired) {
        anisotropyAdequate = supportedFeatures.samplerAnisotropy;
      }

      return isQueueFamilyIndicesComplete(familyIndices) &&
             extensionsSupported &&
             swapChainAdequate &&
             anisotropyAdequate;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  QueueFamilyIndices Functions::findQueueFamilies(
      VkPhysicalDevice& physicalDevice,
      bool computeFamilyRequired,
      bool transferFamilyRequired,
      bool graphicsFamilyRequired,
      VkSurfaceKHR& surface
  ) {
    try {
      QueueFamilyIndices familyIndices;
      familyIndices.computeFamilyRequired = computeFamilyRequired;
      familyIndices.transferFamilyRequired = transferFamilyRequired;
      familyIndices.graphicsFamilyRequired = graphicsFamilyRequired;
      familyIndices.presentFamilyRequired = surface != nullptr;

      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

      std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

      int i = 0;
      for (const auto& queueFamily : queueFamilies) {
        if (familyIndices.computeFamilyRequired) {
          if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            familyIndices.computeFamily = i;
          }
        }

        if (familyIndices.transferFamilyRequired) {
          if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            familyIndices.transferFamily = i;
          }
        }

        if (familyIndices.graphicsFamilyRequired) {
          if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            familyIndices.graphicsFamily = i;
          }
        }

        if (familyIndices.presentFamilyRequired) {
          VkBool32 presentSupport = false;
          vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
          if (presentSupport) {
            familyIndices.presentFamily = i;
          }
        }

        if (isQueueFamilyIndicesComplete(familyIndices)) {
          break;
        }

        i++;
      }

      return familyIndices;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  bool Functions::isQueueFamilyIndicesComplete(QueueFamilyIndices& value) {
    try {
      bool computeFamilyComplete = !value.computeFamilyRequired || value.computeFamily.has_value();
      bool transferFamilyComplete = !value.transferFamilyRequired || value.transferFamily.has_value();
      bool graphicsFamilyComplete = !value.graphicsFamilyRequired || value.graphicsFamily.has_value();
      bool presentFamilyComplete = !value.presentFamilyRequired || value.presentFamily.has_value();
      return computeFamilyComplete && transferFamilyComplete && graphicsFamilyComplete && presentFamilyComplete;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  std::set<uint32_t> Functions::uniqueQueueFamilyIndices(QueueFamilyIndices& value) {
    try {
      std::set<uint32_t> result;
      if (value.computeFamilyRequired) {
        result.insert(value.computeFamily.value());
      }
      if (value.transferFamilyRequired) {
        result.insert(value.transferFamily.value());
      }
      if (value.graphicsFamilyRequired) {
        result.insert(value.graphicsFamily.value());
      }
      if (value.presentFamilyRequired) {
        result.insert(value.presentFamily.value());
      }
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  bool Functions::checkDeviceExtensionSupport(VkPhysicalDevice& physicalDevice, const std::vector<const char*>& deviceExtensions) {
    try {
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

      std::vector<VkExtensionProperties> availableExtensions(extensionCount);
      vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

      std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

      for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
      }

      return requiredExtensions.empty();
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  SwapChainSupportDetails Functions::querySwapChainSupport(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface) {
    try {
      if (surface == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to get swap chain support details!");
      }

      SwapChainSupportDetails details;

      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

      if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
      }

      uint32_t presentModeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

      if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
      }

      return details;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkSurfaceFormatKHR Functions::chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR>& availableFormats) {
    try {
      for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
          return availableFormat;
        }
      }
      return availableFormats[0];
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkPresentModeKHR Functions::chooseSwapPresentMode(std::vector<VkPresentModeKHR>& availablePresentModes) {
    try {
      for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
          return availablePresentMode;
        }
      }
      return VK_PRESENT_MODE_FIFO_KHR;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkExtent2D Functions::chooseSwapExtent(VkSurfaceCapabilitiesKHR& capabilities, const int& width, const int& height) {
    try {
      if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
      } else {
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  uint32_t Functions::getSwapChainImageCount(SwapChainSupportDetails& swapChainSupport) {
    try {
      uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

      if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
      }

      return imageCount;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Shader Functions::createShader(
      VkDevice& device,
      const std::string& path
  ) {
    try {
      VkShaderModule shaderModule = nullptr;

      const std::vector<char>& code = readFile(path);

      if (code.empty()) {
        throw std::runtime_error(CALL_INFO() + ": failed to create shader module code is empty!");
      }

      VkShaderModuleCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

      if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS || shaderModule == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create shader module!");
      }

      VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};
      pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

      ShaderType type = ShaderType::NONE;

      if (path.ends_with(".vert.spv")) {
        type = ShaderType::VERTEX;
      } else if (path.ends_with(".frag.spv")) {
        type = ShaderType::FRAGMENT;
      }

      if (type == ShaderType::VERTEX) {
        pipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
      } else if (type == ShaderType::FRAGMENT) {
        pipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      } else {
        throw std::invalid_argument(CALL_INFO() + ": failed to create shader!");
      }

      pipelineShaderStageCreateInfo.module = shaderModule;
      pipelineShaderStageCreateInfo.pName = "main";

      Shader shader;

      shader.type = type;
      shader.shaderModule = shaderModule;
      shader.pipelineShaderStageCreateInfo = pipelineShaderStageCreateInfo;

      return shader;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  // create

  std::map<std::string, std::string> Functions::createEnvironmentVariables(const std::string& executableDirPath) {
    try {
      std::map<std::string, std::string> environmentVariables;
      environmentVariables["VK_LAYER_PATH"] = std::filesystem::path(executableDirPath).make_preferred().string();
      return environmentVariables;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Configuration Functions::createConfiguration() {
    try {
      std::string applicationName = "Exqudens Application";
      unsigned int applicationVersionMajor = 1;
      unsigned int applicationVersionMinor = 0;
      unsigned int applicationVersionPatch = 0;
      std::string engineName = "Engine Application";
      unsigned int engineVersionMajor = 1;
      unsigned int engineVersionMinor = 0;
      unsigned int engineVersionPatch = 0;
      bool validationLayersEnabled = true;
      std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
      std::vector<const char*> extensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
      std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
      bool computeQueueFamilyRequired = true;
      bool transferQueueFamilyRequired = true;
      bool graphicsQueueFamilyRequired = true;
      bool presentQueueFamilyRequired = true;
      bool anisotropyRequired = true;
      return {
          applicationName,
          applicationVersionMajor,
          applicationVersionMinor,
          applicationVersionPatch,
          engineName,
          engineVersionMajor,
          engineVersionMinor,
          engineVersionPatch,
          validationLayersEnabled,
          validationLayers,
          extensions,
          deviceExtensions,
          computeQueueFamilyRequired,
          transferQueueFamilyRequired,
          graphicsQueueFamilyRequired,
          presentQueueFamilyRequired,
          anisotropyRequired
      };
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Logger Functions::createLogger() {
    try {
      return createLogger(std::cout);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Logger Functions::createLogger(std::ostream& stream) {
    try {
      std::function < void(
          VkDebugUtilsMessageSeverityFlagBitsEXT,
          VkDebugUtilsMessageTypeFlagsEXT,
          std::string
      ) > function = [&stream](
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
          VkDebugUtilsMessageTypeFlagsEXT messageType,
          const std::string& message
      ) {
        //std::string function = std::string("(") + __FUNCTION__ + ")";

        std::string level;
        if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT == messageSeverity) {
          level = "[VERBOSE]";
        } else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT == messageSeverity) {
          level = "[INFO]";
        } else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT == messageSeverity) {
          level = "[WARNING]";
        } else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT == messageSeverity) {
          level = "[ERROR]";
        } else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT == messageSeverity) {
          level = "[MAX]";
        }

        std::string type;
        if (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT == messageType) {
          type = "(GENERAL)";
        } else if (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT == messageType) {
          type = "(VALIDATION)";
        } else if (VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT == messageType) {
          type = "(PERFORMANCE)";
        } else if (VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT == messageType) {
          type = "(MAX)";
        }

        std::string line;

        //line += function;
        //line += " ";
        line += level;
        line += " ";
        line += type;
        line += " ";
        line += "validation layer:";
        line += " ";
        line += message;

        stream << line << std::endl;
      };

      return Logger(function);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Logger Functions::createLogger(
      const std::function<void(
          VkDebugUtilsMessageSeverityFlagBitsEXT,
          VkDebugUtilsMessageTypeFlagsEXT,
          const std::string&
      )>& function
  ) {
    try {
      return Logger(function);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkInstance Functions::createInstance(Configuration& configuration, Logger& logger) {
    try {
      VkInstance instance = nullptr;

      if (configuration.validationLayersEnabled && !checkValidationLayerSupport(configuration.validationLayers)) {
        throw std::runtime_error(CALL_INFO() + ": validation layers requested, but not available!");
      }

      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Exqudens Application";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "Exqudens Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_0;

      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;
      createInfo.enabledExtensionCount = static_cast<uint32_t>(configuration.extensions.size());
      createInfo.ppEnabledExtensionNames = configuration.extensions.data();

      // declared before if statement for to not be destroyed before call vkCreateInstance
      VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
      if (configuration.validationLayersEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(configuration.validationLayers.size());
        createInfo.ppEnabledLayerNames = configuration.validationLayers.data();
        populateDebugMessengerCreateInfo(debugCreateInfo, logger);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
      } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
      }

      if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS || instance == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create instance!");
      }

      return instance;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkDebugUtilsMessengerEXT Functions::createDebugUtilsMessenger(VkInstance& instance, Logger& logger) {
    try {
      VkDebugUtilsMessengerEXT debugUtilsMessenger = nullptr;

      VkResult result;

      auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

      if (func != nullptr) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo, logger);
        result = func(instance, &createInfo, nullptr, &debugUtilsMessenger);
      } else {
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
      }

      if (result != VK_SUCCESS || debugUtilsMessenger == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed create debug messenger!");
      }

      return debugUtilsMessenger;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkSurfaceKHR Functions::createSurface(VkInstance& instance) {
    try {
      VkSurfaceKHR surface = nullptr;

      try {
        surface = createSurfaceFunction(instance);
      } catch (const std::bad_function_call& e) {
        surface = nullptr;
      }

      if (surface == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed create surface!");
      }

      return surface;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkPhysicalDevice Functions::createPhysicalDevice(
      VkInstance& instance,
      Configuration& configuration,
      VkSurfaceKHR& surface
  ) {
    try {
      VkPhysicalDevice physicalDevice = nullptr;

      if (configuration.presentQueueFamilyRequired && surface == nullptr) {
        throw std::runtime_error(
            CALL_INFO()
            + ": '"
            + TO_STRING(configuration.presentQueueFamilyRequired)
            + "': true but '"
            + TO_STRING(surface)
            + "' is null!"
        );
      }

      uint32_t deviceCount = 0;
      vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

      if (deviceCount == 0) {
        throw std::runtime_error(CALL_INFO() + ": failed to find GPUs with Vulkan support!");
      }

      std::vector<VkPhysicalDevice> devices(deviceCount);
      vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

      for (VkPhysicalDevice& object : devices) {
        if (isDeviceSuitable(object, configuration, surface)) {
          physicalDevice = object;
          break;
        }
      }

      if (physicalDevice == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to find a suitable GPU!");
      }

      return physicalDevice;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkDevice Functions::createDevice(
      VkPhysicalDevice& physicalDevice,
      Configuration& configuration,
      VkSurfaceKHR& surface
  ) {
    try {
      VkDevice device = nullptr;

      if (configuration.presentQueueFamilyRequired && surface == nullptr) {
        throw std::runtime_error(
            CALL_INFO()
            + ": '"
            + TO_STRING(configuration.presentQueueFamilyRequired)
            + "': true but '"
            + TO_STRING(surface)
            + "' is null!"
        );
      }

      QueueFamilyIndices familyIndices = findQueueFamilies(
          physicalDevice,
          configuration.computeQueueFamilyRequired,
          configuration.transferQueueFamilyRequired,
          configuration.graphicsQueueFamilyRequired,
          surface
      );

      std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

      std::set<uint32_t> uniqueQueueFamilies = uniqueQueueFamilyIndices(familyIndices);
      float queuePriority = 1.0f;
      for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
      }

      VkPhysicalDeviceFeatures deviceFeatures{};
      deviceFeatures.samplerAnisotropy = VK_TRUE;

      VkDeviceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

      createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
      createInfo.pQueueCreateInfos = queueCreateInfos.data();

      createInfo.pEnabledFeatures = &deviceFeatures;

      createInfo.enabledExtensionCount = static_cast<uint32_t>(configuration.deviceExtensions.size());
      createInfo.ppEnabledExtensionNames = configuration.deviceExtensions.data();

      if (configuration.validationLayersEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(configuration.validationLayers.size());
        createInfo.ppEnabledLayerNames = configuration.validationLayers.data();
      } else {
        createInfo.enabledLayerCount = 0;
      }

      if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS || device == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create logical device!");
      }

      return device;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkQueue Functions::createComputeQueue(
      VkPhysicalDevice& physicalDevice,
      Configuration& configuration,
      VkSurfaceKHR& surface,
      VkDevice& device,
      uint32_t queueIndex
  ) {
    try {
      VkQueue queue = nullptr;

      QueueFamilyIndices familyIndices = findQueueFamilies(
          physicalDevice,
          configuration.computeQueueFamilyRequired,
          configuration.transferQueueFamilyRequired,
          configuration.graphicsQueueFamilyRequired,
          surface
      );

      vkGetDeviceQueue(device, familyIndices.computeFamily.value(), queueIndex, &queue);

      if (queue == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create compute queue!");
      }

      return queue;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkQueue Functions::createTransferQueue(
      VkPhysicalDevice& physicalDevice,
      Configuration& configuration,
      VkSurfaceKHR& surface,
      VkDevice& device,
      uint32_t queueIndex
  ) {
    try {
      VkQueue queue = nullptr;

      QueueFamilyIndices familyIndices = findQueueFamilies(
          physicalDevice,
          configuration.computeQueueFamilyRequired,
          configuration.transferQueueFamilyRequired,
          configuration.graphicsQueueFamilyRequired,
          surface
      );

      vkGetDeviceQueue(device, familyIndices.transferFamily.value(), queueIndex, &queue);

      if (queue == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create transfer queue!");
      }

      return queue;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkQueue Functions::createGraphicsQueue(
      VkPhysicalDevice& physicalDevice,
      Configuration& configuration,
      VkSurfaceKHR& surface,
      VkDevice& device,
      uint32_t queueIndex
  ) {
    try {
      VkQueue queue = nullptr;

      QueueFamilyIndices familyIndices = findQueueFamilies(
          physicalDevice,
          configuration.computeQueueFamilyRequired,
          configuration.transferQueueFamilyRequired,
          configuration.graphicsQueueFamilyRequired,
          surface
      );

      vkGetDeviceQueue(device, familyIndices.graphicsFamily.value(), queueIndex, &queue);

      if (queue == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create graphics queue!");
      }

      return queue;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkQueue Functions::createPresentQueue(
      VkPhysicalDevice& physicalDevice,
      Configuration& configuration,
      VkSurfaceKHR& surface,
      VkDevice& device,
      uint32_t queueIndex
  ) {
    try {
      VkQueue queue = nullptr;

      QueueFamilyIndices familyIndices = findQueueFamilies(
          physicalDevice,
          configuration.computeQueueFamilyRequired,
          configuration.transferQueueFamilyRequired,
          configuration.graphicsQueueFamilyRequired,
          surface
      );

      vkGetDeviceQueue(device, familyIndices.presentFamily.value(), queueIndex, &queue);

      if (queue == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create present queue!");
      }

      return queue;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkSwapchainKHR Functions::createSwapChain(
      VkPhysicalDevice& physicalDevice,
      Configuration& configuration,
      VkSurfaceKHR& surface,
      VkDevice& device,
      const int& width,
      const int& height
  ) {
    try {
      if (surface == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": surface is null failed to create swap chain!");
      }

      VkSwapchainKHR swapChain = nullptr;

      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

      VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
      VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
      VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

      uint32_t imageCount = getSwapChainImageCount(swapChainSupport);

      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = surface;

      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = surfaceFormat.format;
      createInfo.imageColorSpace = surfaceFormat.colorSpace;
      createInfo.imageExtent = extent;
      createInfo.imageArrayLayers = 1;
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      QueueFamilyIndices familyIndices = findQueueFamilies(
          physicalDevice,
          configuration.computeQueueFamilyRequired,
          configuration.transferQueueFamilyRequired,
          configuration.graphicsQueueFamilyRequired,
          surface
      );
      uint32_t queueFamilyIndices[] = {familyIndices.graphicsFamily.value(), familyIndices.presentFamily.value()};

      if (familyIndices.graphicsFamily != familyIndices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
      } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      }

      createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      createInfo.presentMode = presentMode;
      createInfo.clipped = VK_TRUE;

      createInfo.oldSwapchain = nullptr;

      if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS || swapChain == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create swap chain!");
      }

      return swapChain;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  std::vector<VkImage> Functions::createSwapChainImages(
      VkPhysicalDevice& physicalDevice,
      VkSurfaceKHR& surface,
      VkDevice& device,
      VkSwapchainKHR& swapChain
  ) {
    try {
      if (surface == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": surface is null failed to create swap chain images!");
      }

      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
      uint32_t imageCount = getSwapChainImageCount(swapChainSupport);
      std::vector<VkImage> swapChainImages;
      vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
      swapChainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
      return swapChainImages;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkImageView Functions::createImageView(VkDevice& device, VkImage& image, VkFormat& format) {
    try {
      VkImageViewCreateInfo viewInfo{};
      viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      viewInfo.image = image;
      viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      viewInfo.format = format;
      viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      viewInfo.subresourceRange.baseMipLevel = 0;
      viewInfo.subresourceRange.levelCount = 1;
      viewInfo.subresourceRange.baseArrayLayer = 0;
      viewInfo.subresourceRange.layerCount = 1;

      VkImageView imageView = nullptr;

      if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS || imageView == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create texture image view!");
      }

      return imageView;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkRenderPass Functions::createRenderPass(VkDevice& device, VkFormat& format) {
    try {
      VkRenderPass renderPass = nullptr;

      VkAttachmentDescription colorAttachment{};
      colorAttachment.format = format;
      colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
      colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      VkAttachmentReference colorAttachmentRef{};
      colorAttachmentRef.attachment = 0;
      colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      VkSubpassDescription subPass{};
      subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subPass.colorAttachmentCount = 1;
      subPass.pColorAttachments = &colorAttachmentRef;

      VkSubpassDependency dependency{};
      dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
      dependency.dstSubpass = 0;
      dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependency.srcAccessMask = 0;
      dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      VkRenderPassCreateInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassInfo.attachmentCount = 1;
      renderPassInfo.pAttachments = &colorAttachment;
      renderPassInfo.subpassCount = 1;
      renderPassInfo.pSubpasses = &subPass;
      renderPassInfo.dependencyCount = 1;
      renderPassInfo.pDependencies = &dependency;

      if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS || renderPass == nullptr) {
        throw std::runtime_error(CALL_INFO() + ": failed to create render pass!");
      }

      return renderPass;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  VkDescriptorSetLayout Functions::createDescriptorSetLayout(VkDevice& device) {
    try {
      VkDescriptorSetLayout descriptorSetLayout = nullptr;

      VkDescriptorSetLayoutBinding uboLayoutBinding{};
      uboLayoutBinding.binding = 0;
      uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      uboLayoutBinding.descriptorCount = 1;
      uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
      uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

      VkDescriptorSetLayoutBinding samplerLayoutBinding{};
      samplerLayoutBinding.binding = 1;
      samplerLayoutBinding.descriptorCount = 1;
      samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      samplerLayoutBinding.pImmutableSamplers = nullptr;
      samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

      std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

      VkDescriptorSetLayoutCreateInfo layoutInfo{};
      layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
      layoutInfo.pBindings = bindings.data();

      if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error(CALL_INFO() + ": failed to create descriptor set layout!");
      }

      return descriptorSetLayout;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Pipeline Functions::createGraphicsPipeline(
      VkDevice& device,
      VkExtent2D& extent,
      VkDescriptorSetLayout& descriptorSetLayout,
      const std::vector<std::string>& shaderPaths,
      VkRenderPass& renderPass
  ) {
    try {
      VkPipelineLayout pipelineLayout = nullptr;

      VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
      vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

      auto bindingDescription = Vertex::getBindingDescription();
      auto attributeDescriptions = Vertex::getAttributeDescriptions();

      vertexInputInfo.vertexBindingDescriptionCount = 1;
      vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
      vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
      vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

      VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
      inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      inputAssembly.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float) extent.width;
      viewport.height = (float) extent.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = extent;

      VkPipelineViewportStateCreateInfo viewportState{};
      viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo rasterizer{};
      rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizer.depthClampEnable = VK_FALSE;
      rasterizer.rasterizerDiscardEnable = VK_FALSE;
      rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizer.lineWidth = 1.0f;
      rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
      rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      rasterizer.depthBiasEnable = VK_FALSE;
      rasterizer.depthBiasConstantFactor = 0.0f; // Optional
      rasterizer.depthBiasClamp = 0.0f; // Optional
      rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

      VkPipelineMultisampleStateCreateInfo multisampling{};
      multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampling.sampleShadingEnable = VK_FALSE;
      multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      multisampling.minSampleShading = 1.0f; // Optional
      multisampling.pSampleMask = nullptr; // Optional
      multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
      multisampling.alphaToOneEnable = VK_FALSE; // Optional

      VkPipelineColorBlendAttachmentState colorBlendAttachment{};
      colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      colorBlendAttachment.blendEnable = VK_FALSE;
      colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
      colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
      colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
      colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
      colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
      colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

      VkPipelineColorBlendStateCreateInfo colorBlending{};
      colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlending.logicOpEnable = VK_FALSE;
      colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
      colorBlending.attachmentCount = 1;
      colorBlending.pAttachments = &colorBlendAttachment;
      colorBlending.blendConstants[0] = 0.0f; // Optional
      colorBlending.blendConstants[1] = 0.0f; // Optional
      colorBlending.blendConstants[2] = 0.0f; // Optional
      colorBlending.blendConstants[3] = 0.0f; // Optional

      VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.setLayoutCount = 1;
      pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
      pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
      pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

      if (
          vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS
          || pipelineLayout == nullptr
          ) {
        throw std::runtime_error(CALL_INFO() + ": failed to create pipeline layout!");
      }

      VkPipeline pipeline = nullptr;

      std::vector<Shader> vertexShaders;
      std::vector<Shader> fragmentShaders;

      for (const std::string& path : shaderPaths) {
        Shader shader = createShader(device, path);
        if (shader.type == ShaderType::VERTEX) {
          vertexShaders.emplace_back(shader);
        } else if (shader.type == ShaderType::FRAGMENT) {
          fragmentShaders.emplace_back(shader);
        }
      }

      std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
      shaderStages.resize(vertexShaders.size() + fragmentShaders.size());
      for (const Shader& shader : vertexShaders) {
        shaderStages.emplace_back(shader.pipelineShaderStageCreateInfo);
      }
      for (const Shader& shader : fragmentShaders) {
        shaderStages.emplace_back(shader.pipelineShaderStageCreateInfo);
      }

      VkGraphicsPipelineCreateInfo pipelineInfo{};
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
      pipelineInfo.pStages = shaderStages.data();
      pipelineInfo.pVertexInputState = &vertexInputInfo;
      pipelineInfo.pInputAssemblyState = &inputAssembly;
      pipelineInfo.pViewportState = &viewportState;
      pipelineInfo.pRasterizationState = &rasterizer;
      pipelineInfo.pMultisampleState = &multisampling;
      pipelineInfo.pDepthStencilState = nullptr; // Optional
      pipelineInfo.pColorBlendState = &colorBlending;
      pipelineInfo.pDynamicState = nullptr; // Optional
      pipelineInfo.layout = pipelineLayout;
      pipelineInfo.renderPass = renderPass;
      pipelineInfo.subpass = 0;
      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
      pipelineInfo.basePipelineIndex = -1; // Optional

      if (
          vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS
          || pipeline == nullptr
      ) {
        throw std::runtime_error(CALL_INFO() + ": failed to create graphics pipeline!");
      }

      for (const Shader& shader : vertexShaders) {
        vkDestroyShaderModule(device, shader.shaderModule, nullptr);
      }
      for (const Shader& shader : fragmentShaders) {
        vkDestroyShaderModule(device, shader.shaderModule, nullptr);
      }

      Pipeline result;
      result.layout = pipelineLayout;
      result.value = pipeline;
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  // destroy

  void Functions::destroyPipeline(Pipeline& pipeline, VkDevice& device) {
    try {
      if (pipeline.value != nullptr) {
        vkDestroyPipeline(device, pipeline.value, nullptr);
        pipeline.value = nullptr;
      }
      if (pipeline.layout != nullptr) {
        vkDestroyPipelineLayout(device, pipeline.layout, nullptr);
        pipeline.layout = nullptr;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroyDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout, VkDevice& device) {
    try {
      if (descriptorSetLayout != nullptr) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        descriptorSetLayout = nullptr;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroyRenderPass(VkRenderPass& renderPass, VkDevice& device) {
    try {
      if (renderPass != nullptr) {
        vkDestroyRenderPass(device, renderPass, nullptr);
        renderPass = nullptr;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroyImageView(VkImageView& imageView, VkDevice& device) {
    try {
      if (imageView != nullptr) {
        vkDestroyImageView(device, imageView, nullptr);
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroySwapChain(VkSwapchainKHR& swapChain, VkDevice& device) {
    try {
      if (swapChain != nullptr) {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        swapChain = nullptr;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroyDevice(VkDevice& device) {
    try {
      if (device != nullptr) {
        vkDestroyDevice(device, nullptr);
        device = nullptr;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroyPhysicalDevice(VkPhysicalDevice& physicalDevice) {
    try {
      physicalDevice = nullptr;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroyDebugUtilsMessenger(VkDebugUtilsMessengerEXT& debugUtilsMessenger, VkInstance& instance) {
    try {
      if (debugUtilsMessenger != nullptr) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        func(instance, debugUtilsMessenger, nullptr);
        debugUtilsMessenger = nullptr;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  void Functions::destroyInstance(VkInstance& instance) {
    try {
      if (instance != nullptr) {
        vkDestroyInstance(instance, nullptr);
        instance = nullptr;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Functions::~Functions() = default;

}
