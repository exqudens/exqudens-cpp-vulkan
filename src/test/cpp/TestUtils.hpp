#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

#include <lodepng.h>

#include "TestMacros.hpp"
#include "TestConfiguration.hpp"
#include "exqudens/vulkan/Buffer.hpp"

class TestUtils {

  public:

    static std::string getExecutableFile() {
      try {
        return TestConfiguration::getExecutableFile();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::string getExecutableDir() {
      try {
        return TestConfiguration::getExecutableDir();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::optional<std::string> getEnvironmentVariable(const std::string& name) {
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

    static void setEnvironmentVariable(const std::string& name, const std::string& value) {
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

    static std::vector<std::string> toStringVector(
        const std::exception& exception,
        std::vector<std::string> previous = {}
    ) {
      previous.emplace_back(exception.what());
      try {
        std::rethrow_if_nested(exception);
        return previous;
      } catch (const std::exception& e) {
        return toStringVector(e, previous);
      } catch (...) {
        if (previous.empty()) {
          previous.emplace_back(CALL_INFO() + ": Empty stack!");
        }
        return previous;
      }
    }

    static std::vector<std::string> toStackTrace(const std::exception& exception) {
      try {
        std::vector<std::string> elements = toStringVector(exception);
        if (elements.size() > 1) {
          std::ranges::reverse(elements);
        }
        return elements;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::string toString(const std::exception& e) {
      try {
        std::vector<std::string> stackTrace = toStackTrace(e);
        std::ostringstream out;
        for (size_t i = 0; i < stackTrace.size(); i++) {
          out << stackTrace[i];
          if (i < stackTrace.size() - 1) {
            out << std::endl;
          }
        }
        return out.str();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void readPng(
        const std::string& path,
        unsigned int& width,
        unsigned int& height,
        unsigned int& depth,
        std::vector<unsigned char>& data
    ) {
      try {
        unsigned int widthIn = 0;
        unsigned int heightIn = 0;
        unsigned int depthIn = 4;
        std::vector<unsigned char> dataIn;
        unsigned int error = lodepng::decode(dataIn, widthIn, heightIn, path);
        if (error) {
          throw std::runtime_error(
              CALL_INFO() + ": failed to read image '" + std::to_string(error) + "': " + lodepng_error_text(error)
          );
        }
        width = widthIn;
        height = heightIn;
        depth = depthIn;
        data = dataIn;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::vector<std::vector<std::vector<unsigned char>>> readPng(const std::string& path) {
      try {
        std::vector<std::vector<std::vector<unsigned char>>> image;
        std::vector<unsigned char> pixels;
        unsigned int width, height, depth;
        readPng(path, width, height, depth, pixels);
        unsigned int yMultiplier = width * depth;
        image.resize(height);
        for (unsigned int y = 0; y < height; y++) {
          image[y].resize(width);
          for (unsigned int x = 0; x < width; x++) {
            image[y][x].resize(depth);
            for (unsigned int z = 0; z < depth; z++) {
              unsigned int yOffset = y * yMultiplier;
              unsigned int xOffset = x * depth;
              unsigned int zOffset = z;
              unsigned int offset = yOffset + xOffset + zOffset;
              unsigned int i = offset;
              image[y][x][z] = pixels[i];
            }
          }
        }
        return image;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void writePng(
        const std::string& path,
        unsigned int& width,
        unsigned int& height,
        unsigned int& depth,
        std::vector<unsigned char>& data
    ) {
      try {
        unsigned int widthIn = width;
        unsigned int heightIn = height;
        unsigned int depthIn = depth;
        unsigned int error = lodepng::encode(path, data, widthIn, heightIn);
        if (error) {
          throw std::runtime_error(
              CALL_INFO() + ": failed to write image '" + std::to_string(error) + "': " + lodepng_error_text(error)
          );
        }
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void writePng(const std::vector<std::vector<std::vector<unsigned char>>>& image, const std::string& path) {
      try {
        if (image.empty() || image[0].empty() || image[0][0].empty()) {
          throw std::runtime_error(
              CALL_INFO() + ": image.empty() || image[0].empty() || image[0][0].empty()"
          );
        }
        unsigned int height = image.size();
        unsigned int width = image[0].size();
        unsigned int depth = image[0][0].size();
        std::vector<unsigned char> pixels;
        pixels.resize(width * height * depth);
        unsigned int i = 0;
        for (unsigned int y = 0; y < height; y++) {
          for (unsigned int x = 0; x < width; x++) {
            for (unsigned int z = 0; z < depth; z++) {
              pixels[i] = image[y][x][z];
              i++;
            }
          }
        }
        writePng(path, width, height, depth, pixels);
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void copyTo(exqudens::vulkan::Buffer& buffer, const void* data) {
      try {
        void* tmpData = buffer.memoryReference().mapMemory(0, buffer.createInfo.size);
        std::memcpy(tmpData, data, static_cast<size_t>(buffer.createInfo.size));
        buffer.memoryReference().unmapMemory();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

};
