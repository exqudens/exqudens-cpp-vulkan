cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/name-version.txt" INFO)
string(REPLACE ":" ";" INFO_LIST "${INFO}")
list(GET INFO_LIST 0 RAW_NAME)
list(GET INFO_LIST 1 RAW_VERSION)
string(STRIP "${RAW_NAME}" NAME)
string(STRIP "${RAW_VERSION}" VERSION)
project("${NAME}" VERSION "${VERSION}" LANGUAGES "NONE")

message("project: '${PROJECT_NAME}-${PROJECT_VERSION}'")

include("util.cmake")
include("config.cmake")
enable_testing()
include("GoogleTest")

set_conan_settings(CONAN_SETTINGS
    "${CMAKE_SYSTEM_NAME}"
    "${CMAKE_SYSTEM_PROCESSOR}"
    "${CMAKE_CXX_COMPILER_ID}"
    "${CMAKE_CXX_COMPILER_VERSION}"
    "${CMAKE_MSVC_RUNTIME_LIBRARY}"
    "${CMAKE_CXX_STANDARD}"
    "${CMAKE_BUILD_TYPE}"
    "vulkan:build_type=Release"
    "vulkan:compiler.runtime=MD"
    "glfw:build_type=Release"
    "glfw:compiler.runtime=${CONAN_RELEASE_COMPILER_RUNTIME}"
)
set_conan_options(CONAN_OPTIONS
    "vulkan:shared=True"
    "gtest:shared=${CONAN_BUILD_SHARED_LIBS}"
    "lodepng:shared=${CONAN_BUILD_SHARED_LIBS}"
    "glfw:shared=${CONAN_BUILD_SHARED_LIBS}"
)
set("CONAN_INSTALL_PREFIX" "${PROJECT_BINARY_DIR}/conan-install")
if(NOT EXISTS "${CONAN_INSTALL_PREFIX}")
    execute_process(
        COMMAND "${CONAN_COMMAND}"
                install
                "${PROJECT_SOURCE_DIR}/conanfile.py"
                --install-folder
                "${CONAN_INSTALL_PREFIX}"
                ${CONAN_SETTINGS}
                ${CONAN_OPTIONS}
        COMMAND_ECHO "STDERR"
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ENCODING "UTF-8"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
    message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_PREFIX}/conan-packages.cmake'!")
endif()
include("${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
set(CMAKE_MODULE_PATH "${CONAN_INSTALL_PREFIX}")
foreach(
    conanPackageName
    cmakePackageName
    cmakePackageVersion
    cmakePackagePath
    IN ZIP_LISTS
    "${PROJECT_NAME}_CONAN_PACKAGE_NAMES"
    "${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"
    "${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"
    "${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"
)
    if(
        "" STREQUAL "${conanPackageName}"
        OR "" STREQUAL "${cmakePackageName}"
        OR "" STREQUAL "${cmakePackageVersion}"
        OR "" STREQUAL "${cmakePackagePath}"
    )
        string(JOIN " " errorMessage
            "Empty value 'conanPackageName': '${conanPackageName}'"
            "or 'cmakePackageName': '${cmakePackageName}'"
            "or 'cmakePackageVersion': '${cmakePackageVersion}'"
            "or 'cmakePackagePath': '${cmakePackagePath}'!"
        )
        message(FATAL_ERROR "${errorMessage}")
    elseif(NOT IS_DIRECTORY "${cmakePackagePath}")
        message(FATAL_ERROR "Not is directory 'cmakePackagePath': '${cmakePackagePath}'!")
    endif()

    list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")

    if("gtest" STREQUAL "${conanPackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET CONFIG)
    elseif("vulkan" STREQUAL "${conanPackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET)
        find_program(GLSLC_COMMAND NAMES "glslc.exe" "glslc" PATHS "${cmakePackagePath}/bin")
    elseif("tinyobjloader" STREQUAL "${conanPackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET)
    elseif("glm" STREQUAL "${conanPackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET)
    elseif("lodepng" STREQUAL "${conanPackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" QUIET)
    elseif("glfw" STREQUAL "${conanPackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET CONFIG)
    else()
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET CONFIG)
    endif()
endforeach()
set_not_found_package_names(NOT_FOUND_PACKAGE_NAMES ${NOT_FOUND_PACKAGE_NAMES})
if(NOT "" STREQUAL "${NOT_FOUND_PACKAGE_NAMES}")
    foreach(package ${NOT_FOUND_PACKAGE_NAMES})
        message(WARNING "${package}_FOUND: '${${package}_FOUND}'")
    endforeach()
    return()
endif()

add_library("${PROJECT_NAME}" INTERFACE
    "src/main/cpp/exqudens/vulkan/all.hpp"

    "src/main/cpp/exqudens/vulkan/Macros.hpp"
    "src/main/cpp/exqudens/vulkan/Utility.hpp"

    "src/main/cpp/exqudens/vulkan/Instance.hpp"
    "src/main/cpp/exqudens/vulkan/MessengerCreateInfo.hpp"
    "src/main/cpp/exqudens/vulkan/Messenger.hpp"
    "src/main/cpp/exqudens/vulkan/QueueInfo.hpp"
    "src/main/cpp/exqudens/vulkan/PhysicalDevice.hpp"
    "src/main/cpp/exqudens/vulkan/Device.hpp"
    "src/main/cpp/exqudens/vulkan/Image.hpp"
    "src/main/cpp/exqudens/vulkan/ImageView.hpp"
    "src/main/cpp/exqudens/vulkan/Buffer.hpp"
    "src/main/cpp/exqudens/vulkan/Sampler.hpp"
    "src/main/cpp/exqudens/vulkan/Semaphore.hpp"
    "src/main/cpp/exqudens/vulkan/Fence.hpp"
    "src/main/cpp/exqudens/vulkan/SubpassDescription.hpp"
    "src/main/cpp/exqudens/vulkan/RenderPass.hpp"
    "src/main/cpp/exqudens/vulkan/DescriptorSetLayout.hpp"
    "src/main/cpp/exqudens/vulkan/PipelineVertexInputStateCreateInfo.hpp"
    "src/main/cpp/exqudens/vulkan/PipelineViewportStateCreateInfo.hpp"
    "src/main/cpp/exqudens/vulkan/PipelineColorBlendStateCreateInfo.hpp"
    "src/main/cpp/exqudens/vulkan/GraphicsPipelineCreateInfo.hpp"
    "src/main/cpp/exqudens/vulkan/Pipeline.hpp"
    "src/main/cpp/exqudens/vulkan/DescriptorPool.hpp"
    "src/main/cpp/exqudens/vulkan/WriteDescriptorSet.hpp"
    "src/main/cpp/exqudens/vulkan/DescriptorSet.hpp"
    "src/main/cpp/exqudens/vulkan/Framebuffer.hpp"
    "src/main/cpp/exqudens/vulkan/Queue.hpp"
    "src/main/cpp/exqudens/vulkan/CommandPool.hpp"
    "src/main/cpp/exqudens/vulkan/CommandBuffer.hpp"
    "src/main/cpp/exqudens/vulkan/QueryPool.hpp"
    "src/main/cpp/exqudens/vulkan/Surface.hpp"
    "src/main/cpp/exqudens/vulkan/Swapchain.hpp"
)
target_include_directories("${PROJECT_NAME}" INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/main>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/cpp>"
    "$<INSTALL_INTERFACE:include>"
)
target_link_libraries("${PROJECT_NAME}" INTERFACE
    "Vulkan::Vulkan"
)
set_property(TARGET "${PROJECT_NAME}" PROPERTY "VERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "SOVERSION" "${PROJECT_VERSION_MAJOR}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "INTERFACE_${PROJECT_NAME}_MAJOR_VERSION" "${PROJECT_VERSION_MAJOR}")
set_property(TARGET "${PROJECT_NAME}" APPEND PROPERTY "COMPATIBLE_INTERFACE_STRING" "${PROJECT_NAME}_MAJOR_VERSION")
if(MSVC AND "Debug" STREQUAL "${CMAKE_BUILD_TYPE}")
    target_compile_options("${PROJECT_NAME}" INTERFACE
        "-bigobj"
    )
endif()
install(
    TARGETS "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}-targets"
    INCLUDES DESTINATION "include"
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)
install(
    EXPORT "${PROJECT_NAME}-targets"
    DESTINATION "cmake"
    NAMESPACE "${PROJECT_NAME}::"
    FILE "${PROJECT_NAME}-targets.cmake"
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/package-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "cmake"
)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY "ExactVersion"
)
install(
    FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION "cmake"
)
install(
    DIRECTORY "src/main/cpp/"
    DESTINATION "include"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)

add_library("test-lib" INTERFACE
    "src/test/cpp/TestApplication.hpp"
    "src/test/cpp/TestConfiguration.hpp"
    "src/test/cpp/TestMacros.hpp"
    "src/test/cpp/TestUtils.hpp"
    "src/test/cpp/exqudens/vulkan/Vertex.hpp"
    "src/test/cpp/exqudens/vulkan/UniformBufferObject.hpp"
    "src/test/cpp/exqudens/vulkan/TestUtilsTests.hpp"
    "src/test/cpp/exqudens/vulkan/TinyObjLoaderTests.hpp"
    "src/test/cpp/exqudens/vulkan/OtherTests.hpp"
    #"src/test/cpp/exqudens/vulkan/ConfigurationTests.hpp"
    #"src/test/cpp/exqudens/vulkan/ShaderTests.hpp"
    #"src/test/cpp/exqudens/vulkan/FactoryTests.hpp"
    "src/test/cpp/exqudens/vulkan/UiTestsA.hpp"
    #"src/test/cpp/exqudens/vulkan/UiTestsB.hpp"
    #"src/test/cpp/exqudens/vulkan/UiTestsC.hpp"
    #"src/test/cpp/exqudens/vulkan/UiTestsD.hpp"
    #"src/test/cpp/exqudens/vulkan/UiTestsE.hpp"
)
target_include_directories("test-lib" INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/test/cpp>"
    "$<INSTALL_INTERFACE:include>"
)
target_link_libraries("test-lib" INTERFACE
    "${PROJECT_NAME}"
    "GTest::gmock"
    "GTest::gtest"
    "GTest::gmock_main"
    "GTest::gtest_main"
    "glm::glm"
    "lodepng::lodepng"
    "glfw"
    "tinyobjloader::tinyobjloader"
)
set_target_properties("test-lib" PROPERTIES
    CXX_STANDARD 23
)

add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.vert.spv"
           "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.frag.spv"
           "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.vert.spv"
           "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.frag.spv"
           "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.vert.spv"
           "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.frag.spv"
           "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.vert.spv"
           "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.frag.spv"
    COMMAND "${CMAKE_COMMAND}" "-E" "rm" "-rf" "${PROJECT_BINARY_DIR}/test/bin/resources/shader"
    COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${PROJECT_BINARY_DIR}/test/bin/resources/shader"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-1.vert" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.vert.spv"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-1.frag" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.frag.spv"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-2.vert" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.vert.spv"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-2.frag" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.frag.spv"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-3.vert" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.vert.spv"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-3.frag" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.frag.spv"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-4.vert" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.vert.spv"
    COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-4.frag" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.frag.spv"
    VERBATIM
)
add_executable("test-app"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.vert.spv"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.frag.spv"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.vert.spv"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.frag.spv"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.vert.spv"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.frag.spv"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.vert.spv"
    "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.frag.spv"
    "src/test/cpp/main.cpp"
)
target_link_libraries("test-app" PRIVATE
    "test-lib"
)
set_target_properties("test-app" PROPERTIES
    CXX_STANDARD 23

    RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

    ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

    LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
)
if("${BUILD_SHARED_LIBS}")
    add_custom_command(TARGET "test-app"
        POST_BUILD
        #COMMAND "${CMAKE_COMMAND}" -E rm -f "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/*"
        #COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/src/test/resources" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
        VERBATIM
    )
else()
    add_custom_command(TARGET "test-app"
        POST_BUILD
        #COMMAND "${CMAKE_COMMAND}" -E rm -f "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/*"
        #COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/src/test/resources" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
        VERBATIM
    )
endif()

gtest_discover_tests("test-app"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/bin"
    PROPERTIES ENVIRONMENT "PARENT_PATH=;PATH="
)

add_custom_target("cmake-test"
    COMMAND "${CMAKE_CTEST_COMMAND}" --preset "${PRESET_NAME}"
    DEPENDS "test-app"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
)

add_custom_target("cmake-install" ALL
    COMMAND "${CMAKE_COMMAND}" --install "${PROJECT_BINARY_DIR}" --prefix "${CMAKE_INSTALL_PREFIX}"
    DEPENDS ${TARGET_CMAKE_INSTALL_DEPENDS_ON}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
)

add_custom_target("conan-export"
    COMMAND "${CONAN_COMMAND}"
            export-pkg
            --force
            "${PROJECT_SOURCE_DIR}/conanfile.py"
            #"gitlab-group+gitlab-sub-group+${PROJECT_NAME}/stable"
            --package-folder
            "${CMAKE_INSTALL_PREFIX}"
            ${CONAN_SETTINGS}
            ${CONAN_OPTIONS}
    DEPENDS "cmake-install"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
)

set_target_names(ALL_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}")
message("all-targets:")
foreach(t ${ALL_TARGETS})
    message("  ${t}")
endforeach()
