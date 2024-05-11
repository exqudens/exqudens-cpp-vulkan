#pragma once

#include <exception>
#include <vector>
#include <unordered_map>

#include <gtest/gtest.h>
#include <tiny_obj_loader.h>

#include "TestUtils.hpp"
#include "TestLogging.hpp"
#include "exqudens/vulkan/Vertex.hpp"

namespace exqudens::vulkan {

  class TinyObjLoaderTests : public testing::Test {

    protected:

      inline static const char* LOGGER_ID = "exqudens.vulkan.TinyObjLoaderTests";

  };

  TEST_F(TinyObjLoaderTests, test1) {
    try {
      std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
      std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' start";

      std::string executableDir = TestUtils::getExecutableDir();

      tinyobj::attrib_t attrib;
      std::vector<tinyobj::shape_t> shapes;
      std::vector<tinyobj::material_t> materials;
      std::string err, modelPath;

      modelPath = std::filesystem::path(executableDir).append("resources").append("obj").append("viking_room.obj").make_preferred().string();

      if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.c_str())) {
        throw std::runtime_error(err);
      }

      std::vector<Vertex> vertices;
      std::vector<uint16_t> indices;

      std::unordered_map<Vertex, uint32_t> uniqueVertices{};

      for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
          Vertex vertex = {};

          vertex.position = {
              attrib.vertices[3 * index.vertex_index + 0],
              attrib.vertices[3 * index.vertex_index + 1],
              attrib.vertices[3 * index.vertex_index + 2]
          };

          vertex.texCoord = {
              attrib.texcoords[2 * index.texcoord_index + 0],
              1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
          };

          vertex.color = {1.0f, 1.0f, 1.0f};

          if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.emplace_back(vertex);
          }

          indices.emplace_back(uniqueVertices[vertex]);
        }
      }

      SUCCEED();

      TEST_LOG_I(LOGGER_ID) << "'" << testGroup << "." << testCase << "' end";
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
