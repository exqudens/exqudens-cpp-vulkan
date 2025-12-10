cmake_minimum_required(VERSION "3.25" FATAL_ERROR)

message(STATUS "CMAKE_COMMAND: '${CMAKE_COMMAND}'")
message(STATUS "CMAKE_VERSION: '${CMAKE_VERSION}'")
message(STATUS "PRESET_NAME: '${PRESET_NAME}'")

string(REPLACE "." ";" PRESET_NAME_ELEMENTS "${PRESET_NAME}")

set(CMAKE_UTIL_FILE "" CACHE STRING "...")
set(USE_CONAN "1" CACHE BOOL "...")

message(STATUS "USE_CONAN: '${USE_CONAN}'")

if("${USE_CONAN}")
    find_program(CONAN_COMMAND NAMES "conan.exe" "conan" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
endif()

if("${CMAKE_UTIL_FILE}" STREQUAL "")
    if("$ENV{CMAKE_UTIL_FILE}" STREQUAL "")
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/direct_deploy/exqudens-cmake/cmake/util.cmake")
            if("${USE_CONAN}")
                #[[execute_process(
                    COMMAND "${CONAN_COMMAND}" "user" "$ENV{CONAN_LOGIN_USERNAME}" "-r" "artifactory" "-p"
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                    ENCODING "UTF-8"
                    COMMAND_ERROR_IS_FATAL "ANY"
                )]]
                execute_process(
                    COMMAND "${CONAN_COMMAND}" "install" "--tool-requires=exqudens-cmake/0.0.1" "--deployer" "direct_deploy" "--deployer-folder" "${CMAKE_SOURCE_DIR}/build/dependencies"
                    COMMAND_ECHO "STDERR"
                    ENCODING "UTF-8"
                    COMMAND_ERROR_IS_FATAL "ANY"
                )
                set(CMAKE_UTIL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/direct_deploy/exqudens-cmake/cmake/util.cmake" CACHE STRING "..." FORCE)
            else()
                message(FATAL_ERROR "Unable to resolve CMAKE_UTIL_FILE: '${CMAKE_UTIL_FILE}'")
            endif()
        else()
            set(CMAKE_UTIL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/direct_deploy/exqudens-cmake/cmake/util.cmake" CACHE STRING "..." FORCE)
        endif()
    else()
        set(CMAKE_UTIL_FILE "$ENV{CMAKE_UTIL_FILE}" CACHE STRING "..." FORCE)
    endif()
endif()

message(STATUS "CMAKE_UTIL_FILE: '${CMAKE_UTIL_FILE}'")

include("${CMAKE_UTIL_FILE}")

string(REPLACE "." ";" PRESET_NAME_ELEMENTS "${PRESET_NAME}")
if("msvc-x64-x64" IN_LIST "PRESET_NAME_ELEMENTS")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/msvc-x64-x64-toolchain.cmake")
        set_msvc_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            VERSION "17"
            HOST "x64"
            TARGET "x64"
            SET_CMAKE_SYSTEM_NAME "0"
            OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/msvc-x64-x64-toolchain.cmake"
        )
    endif()
    find_program(CONAN_COMMAND NAMES "conan.exe" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
elseif("gcc" IN_LIST PRESET_NAME_ELEMENTS)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/gcc-toolchain.cmake")
        find_file(compilerFile NAMES "gcc" PATHS ENV GCC_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
        set_gnu_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            PATH "${compilerFile}"
            SET_CMAKE_SYSTEM_NAME "0"
            OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/gcc-toolchain.cmake"
        )
    endif()
    find_program(CONAN_COMMAND NAMES "conan" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
endif()

message(STATUS "CMAKE_TOOLCHAIN_FILE: '${CMAKE_TOOLCHAIN_FILE}'")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/name-version.json" NAME_VERSION_JSON)
string(JSON NAME GET "${NAME_VERSION_JSON}" "name")
string(JSON VERSION GET "${NAME_VERSION_JSON}" "version")
project("${NAME}" VERSION "${VERSION}" LANGUAGES "NONE")

message(STATUS "PROJECT_NAME: '${PROJECT_NAME}'")
message(STATUS "PROJECT_VERSION: '${PROJECT_VERSION}'")

string(REPLACE "_" "-" BASE_DIR "${PROJECT_NAME}")
string(REPLACE "-cpp-" "-" BASE_DIR "${BASE_DIR}")
string(REPLACE "-" "/" BASE_DIR "${BASE_DIR}")

message(STATUS "BASE_DIR: '${BASE_DIR}'")

enable_language("C")
enable_language("CXX")
include("CMakePackageConfigHelpers")
include("GenerateExportHeader")
enable_testing()
include("GoogleTest")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/cmake-install" CACHE PATH "..." FORCE)
set(CMAKE_VERBOSE_MAKEFILE "1" CACHE BOOL "..." FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS "1" CACHE BOOL "..." FORCE)

set(CMAKE_OBJECT_PATH_MAX 1000)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")
set(CMAKE_STAGING_PREFIX "")
set(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH "0")
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH "0")
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH "1")
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY "0")
set(CMAKE_FIND_USE_PACKAGE_REGISTRY "0")
set(CMAKE_FIND_USE_PACKAGE_ROOT_PATH "0")
set(CMAKE_FIND_USE_CMAKE_PATH "1")
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG "1")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "0")
set(CMAKE_BUILD_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_SKIP_BUILD_RPATH "1")
set(CMAKE_BUILD_RPATH_USE_ORIGIN "1")
set(CMAKE_BUILD_WITH_INSTALL_RPATH "1")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH "0")
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "1")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED "1")
set(CMAKE_C_EXTENSIONS "0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED "1")
set(CMAKE_CXX_EXTENSIONS "0")

#[[separate_arguments(CMAKE_CXX_FLAGS NATIVE_COMMAND "${CMAKE_CXX_FLAGS}")
if(MSVC)
    if(NOT "/EHa" IN_LIST CMAKE_CXX_FLAGS AND "/EHsc" IN_LIST CMAKE_CXX_FLAGS)
        list(REMOVE_ITEM CMAKE_CXX_FLAGS "/EHsc")
        list(APPEND CMAKE_CXX_FLAGS "/EHa")
    endif()
endif()
string(JOIN " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})]]

set(CONAN_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/conan-install")

file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/lib")

file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/test/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/test/lib")

if("${USE_CONAN}")
    set_conan_architecture(CONAN_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
    set_conan_os(CONAN_OS "${CMAKE_SYSTEM_NAME}")
    set_conan_compiler(CONAN_COMPILER "${CMAKE_CXX_COMPILER_ID}")
    set_conan_compiler_version(CONAN_COMPILER_VERSION "${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}" MAX_ELEMENTS "1")
    set_conan_compiler_runtime(CONAN_COMPILER_RUNTIME "${CMAKE_MSVC_RUNTIME_LIBRARY}")
    set_conan_compiler_runtime_type(CONAN_COMPILER_RUNTIME_TYPE "${CMAKE_MSVC_RUNTIME_LIBRARY}")

    set_python_boolean(CONAN_INTERFACE_ONLY "${INTERFACE_ONLY}")
    set_python_boolean(CONAN_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")

    set_conan_settings(CONAN_SETTINGS
        "arch=${CONAN_ARCH}"
        "os=${CONAN_OS}"
        "compiler=${CONAN_COMPILER}"
        "compiler.version=${CONAN_COMPILER_VERSION}"
        "compiler.cstd=${CMAKE_C_STANDARD}"
        "compiler.cppstd=${CMAKE_CXX_STANDARD}"
        "compiler.runtime=${CONAN_COMPILER_RUNTIME}"
        "compiler.runtime_type=${CONAN_COMPILER_RUNTIME_TYPE}"
        "compiler.libcxx=${CONAN_COMPILER_LIBCXX}"
        "build_type=${CMAKE_BUILD_TYPE}"
    )
    set_conan_options(CONAN_OPTIONS
        "&:shared=${CONAN_BUILD_SHARED_LIBS}"
        "*:shared=${CONAN_BUILD_SHARED_LIBS}"
    )
    if(EXISTS "${PROJECT_SOURCE_DIR}/conanfile.py")
        if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
            #[[execute_process(
                COMMAND "${CONAN_COMMAND}" "user" "$ENV{CONAN_LOGIN_USERNAME}" "-r" "artifactory" "-p"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL "ANY"
            )]]
            execute_process(
                COMMAND "${CONAN_COMMAND}" "install"
                        "--output-folder" "${CONAN_INSTALL_PREFIX}"
                        ${CONAN_SETTINGS}
                        ${CONAN_OPTIONS}
                        "${PROJECT_SOURCE_DIR}/conanfile.py"
                COMMAND_ECHO "STDERR"
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL "ANY"
            )
        endif()
        if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
            message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_PREFIX}/conan-packages.cmake'")
        endif()
        include("${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
        list(APPEND CMAKE_MODULE_PATH "${CONAN_INSTALL_PREFIX}")
        list(APPEND CMAKE_PREFIX_PATH "${CONAN_INSTALL_PREFIX}")
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

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION" "${cmakePackageVersion}")
            endif()

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH" "${cmakePackagePath}")
            endif()

            list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${cmakePackageName}")
            list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")
        endforeach()
    endif()
    if(NOT "${SKIP_TEST}" AND EXISTS "${PROJECT_SOURCE_DIR}/test-conanfile.py")
        if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake")
            #[[execute_process(
                COMMAND "${CONAN_COMMAND}" "user" "$ENV{CONAN_LOGIN_USERNAME}" "-r" "artifactory" "-p"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL "ANY"
            )]]
            execute_process(
                COMMAND "${CONAN_COMMAND}" "install"
                        "--output-folder" "${CONAN_INSTALL_PREFIX}/test"
                        ${CONAN_SETTINGS}
                        ${CONAN_OPTIONS}
                        "${PROJECT_SOURCE_DIR}/test-conanfile.py"
                COMMAND_ECHO "STDERR"
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL "ANY"
            )
        endif()
        if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake")
            message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake'")
        endif()
        include("${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake")
        list(APPEND CMAKE_MODULE_PATH "${CONAN_INSTALL_PREFIX}/test")
        list(APPEND CMAKE_PREFIX_PATH "${CONAN_INSTALL_PREFIX}/test")
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

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION" "${cmakePackageVersion}")
            endif()

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH" "${cmakePackagePath}")
            endif()

            list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${cmakePackageName}")
            list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")
        endforeach()
    endif()
endif()

if("${${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES}" STREQUAL "")
    #[[list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
        "exqudens-cpp-log-api"
    )]]
    if(NOT "${SKIP_TEST}")
        list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
            "GTest"
            "exqudens-cpp-log-api"
            "exqudens-cpp-log"
            "nlohmann_json"
            #"VulkanHeaders"
            #"vulkan-sdk"
            "glfw3"
            "glm"
        )
    endif()
endif()
list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
    "Vulkan"
)
set("${PROJECT_NAME}_CMAKE_PACKAGE_Vulkan_VERSION" "1.4.328.1")
if("windows" IN_LIST "PRESET_NAME_ELEMENTS")
    set("${PROJECT_NAME}_CMAKE_PACKAGE_Vulkan_PATH" "C:/VulkanSDK/${${PROJECT_NAME}_CMAKE_PACKAGE_Vulkan_VERSION}")
else()
    message(FATAL_ERROR "Unsupported preset: '${PRESET_NAME}'")
endif()
message(STATUS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES:")
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    message(STATUS "${cmakePackageName}")
endforeach()
unset(NOT_FOUND_PACKAGE_NAMES)
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    if("GTest" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    elseif("exqudens-cpp-log-api" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    elseif("exqudens-cpp-log" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    elseif("nlohmann_json" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        #list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        #find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
        if(NOT EXISTS "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}")
            message(FATAL_ERROR "Not exists: '${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}'")
        endif()
        add_library("nlohmann_json" INTERFACE IMPORTED)
        set_property(TARGET "nlohmann_json" PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/include")
        add_library("nlohmann_json::nlohmann_json" ALIAS "nlohmann_json")
    elseif("VulkanHeaders" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        #list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        #find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
        if(NOT EXISTS "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}")
            message(FATAL_ERROR "Not exists: '${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}'")
        endif()
        add_library("vulkan-headers" INTERFACE IMPORTED)
        set_property(TARGET "vulkan-headers" PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/include")
        add_library("vulkan-headers::vulkan-headers" ALIAS "vulkan-headers")
    elseif("vulkan-sdk" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
        add_library("vulkan-sdk" SHARED IMPORTED)
        set_property(TARGET "vulkan-sdk" PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/include")
        if("windows" IN_LIST "PRESET_NAME_ELEMENTS")
            set_property(TARGET "vulkan-sdk" PROPERTY IMPORTED_IMPLIB "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/lib/vulkan-1.lib")
        else()
            set_property(TARGET "vulkan-sdk" PROPERTY IMPORTED_LOCATION "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/bin/vulkan.so")
        endif()
        add_library("vulkan-sdk::vulkan-sdk" ALIAS "vulkan-sdk")
    elseif("Vulkan" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        #list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        #find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
        if(NOT EXISTS "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}")
            message(FATAL_ERROR "Not exists: '${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}'")
        endif()
        add_library("Vulkan" SHARED IMPORTED)
        set_property(TARGET "Vulkan" PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/include")
        if("windows" IN_LIST "PRESET_NAME_ELEMENTS")
            set_property(TARGET "Vulkan" PROPERTY IMPORTED_IMPLIB "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/lib/vulkan-1.lib")
        else()
            set_property(TARGET "Vulkan" PROPERTY IMPORTED_LOCATION "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/bin/vulkan.so")
        endif()
        add_library("Vulkan::Vulkan" ALIAS "Vulkan")
    elseif("glfw3" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    elseif("glm" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        #list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        #find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
        if(NOT EXISTS "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}")
            message(FATAL_ERROR "Not exists: '${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}'")
        endif()
        add_library("glm" INTERFACE IMPORTED)
        set_property(TARGET "glm" PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}/include")
        add_library("glm::glm" ALIAS "glm")
    else()
        message(WARNING "Ignore cmakePackageName: '${cmakePackageName}'")
    endif()
endforeach()
set_not_found_package_names(NOT_FOUND_PACKAGE_NAMES ${NOT_FOUND_PACKAGE_NAMES})
if(NOT "" STREQUAL "${NOT_FOUND_PACKAGE_NAMES}")
    foreach(package IN LISTS NOT_FOUND_PACKAGE_NAMES)
        message(SEND_ERROR "${package}_FOUND: '${${package}_FOUND}' ${package}_NOT_FOUND_MESSAGE: '${${package}_NOT_FOUND_MESSAGE}'")
    endforeach()
    message(FATAL_ERROR "Some package not found!")
endif()

configure_file("${PROJECT_SOURCE_DIR}/versions.hpp.in" "${PROJECT_BINARY_DIR}/generated/src/main/cpp/${BASE_DIR}/versions.hpp" @ONLY NEWLINE_STYLE UNIX)

add_library("${PROJECT_NAME}" INTERFACE
    "src/main/cpp/exqudens/vulkan/Instance.hpp"
    "src/main/cpp/exqudens/vulkan/DebugUtilsMessenger.hpp"
    "src/main/cpp/exqudens/vulkan/PhysicalDevice.hpp"
)
target_include_directories("${PROJECT_NAME}" INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/main/cpp>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/cpp>"
    "$<INSTALL_INTERFACE:include>"
)
#set_property(TARGET "${PROJECT_NAME}" PROPERTY "VERSION" "${PROJECT_VERSION}")
#set_property(TARGET "${PROJECT_NAME}" PROPERTY "SOVERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "INTERFACE_${PROJECT_NAME}_MAJOR_VERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" APPEND PROPERTY "COMPATIBLE_INTERFACE_STRING" "${PROJECT_NAME}_MAJOR_VERSION")
install(
    EXPORT "${PROJECT_NAME}-targets"
    DESTINATION "cmake"
    NAMESPACE "${PROJECT_NAME}::"
    FILE "${PROJECT_NAME}-targets.cmake"
)
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
    DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp/"
              "src/main/cpp/"
    DESTINATION "include"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)
install(
    TARGETS "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}-targets"
    INCLUDES DESTINATION "include"
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)

if(NOT "${SKIP_TEST}")
    add_library("test-lib"
        "src/test/cpp/TestUtils.hpp"
        "src/test/cpp/TestUtils.cpp"
        "src/test/cpp/TestGlfwUtils.hpp"
        "src/test/cpp/TestGlfwUtils.cpp"
        "src/test/cpp/TestApplication.hpp"
        "src/test/cpp/TestApplication.cpp"
        "src/test/cpp/StringVector.hpp"
        "src/test/cpp/unit/OtherUnitTests.hpp"
        "src/test/cpp/unit/StringVectorUnitTests.hpp"
        "src/test/cpp/gui/VulkanTutorial1GuiTests.hpp"
        "src/test/cpp/gui/VulkanTutorial2GuiTests.hpp"
        "src/test/cpp/gui/VulkanTutorial3GuiTests.hpp"
        "src/test/cpp/gui/VulkanTutorial4GuiTests.hpp"
    )
    generate_export_header("test-lib"
        BASE_NAME "TEST_LIB"
        EXPORT_FILE_NAME "generated/src/test/cpp/test_lib_export.hpp"
    )
    if(NOT "${BUILD_SHARED_LIBS}")
        target_compile_definitions("test-lib" PUBLIC
            "TEST_LIB_STATIC_DEFINE"
        )
    endif()
    target_include_directories("test-lib" PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/test/cpp>"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/test/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
    target_link_libraries("test-lib" PUBLIC
        "${PROJECT_NAME}"
        "GTest::gmock"
        "GTest::gtest"
        "GTest::gmock_main"
        "GTest::gtest_main"
        "exqudens-cpp-log::exqudens-cpp-log"
        "nlohmann_json::nlohmann_json"
        #"vulkan-headers::vulkan-headers"
        #"vulkan-sdk::vulkan-sdk"
        "Vulkan::Vulkan"
        "glfw"
        "glm::glm"
    )
    set_target_properties("test-lib" PROPERTIES
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

    add_executable("test-app"
        "src/test/cpp/main.cpp"
    )
    target_link_libraries("test-app"
        "test-lib"
    )
    set_target_properties("test-app" PROPERTIES
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
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                #COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                #COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/test/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                USES_TERMINAL
                VERBATIM
            )
        else()
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                #COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},LIBRARY_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                #COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/test/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                USES_TERMINAL
                VERBATIM
            )
        endif()
    endif()
    add_custom_command(TARGET "test-app"
        POST_BUILD
        COMMAND "$<TARGET_FILE:test-app>"
                "--gtest_list_tests"
                "--gtest_output=json:tests.json"
                "--project-binary-dir=${PROJECT_BINARY_DIR}"
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:test-app>"
        USES_TERMINAL
        VERBATIM
    )

    gtest_discover_tests("test-app"
        DISCOVERY_EXTRA_ARGS "--project-binary-dir=${PROJECT_BINARY_DIR}"
        EXTRA_ARGS "--project-binary-dir=${PROJECT_BINARY_DIR}"
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:test-app>"
        PROPERTIES ENVIRONMENT "PATH=$<TARGET_FILE_DIR:test-app>;${ENV_PATH}"
    )

    add_custom_command(
        OUTPUT "${PROJECT_BINARY_DIR}/junit.xml"
        COMMAND "${CMAKE_CTEST_COMMAND}" "--build-nocmake" "--output-junit" "${PROJECT_BINARY_DIR}/junit.xml" "--preset" "${PRESET_NAME}" "-R" "${TEST_REGEXP}"
        DEPENDS "test-app"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target("cmake-test"
        DEPENDS "${PROJECT_BINARY_DIR}/junit.xml"
    )
endif()

add_custom_command(
    OUTPUT "${CMAKE_INSTALL_PREFIX}/cmake/${PROJECT_NAME}-config.cmake"
    COMMAND "${CMAKE_COMMAND}" "--install" "${PROJECT_BINARY_DIR}" "--prefix" "${CMAKE_INSTALL_PREFIX}"
    DEPENDS ${TARGET_CMAKE_INSTALL_DEPENDS_ON}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target("cmake-install" ALL
    DEPENDS "${CMAKE_INSTALL_PREFIX}/cmake/${PROJECT_NAME}-config.cmake"
)

if("${USE_CONAN}")
    add_custom_target("conan-list"
        COMMAND "${CONAN_COMMAND}" "list" "${PROJECT_NAME}/${PROJECT_VERSION}:*"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target("conan-remove"
        COMMAND "${CONAN_COMMAND}" "remove" "-c" "${PROJECT_NAME}/${PROJECT_VERSION}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target("conan-export"
        COMMAND "${CONAN_COMMAND}" "export-pkg"
                "--output-folder" "${CMAKE_INSTALL_PREFIX}"
                ${CONAN_SETTINGS}
                #"--user" "gitlab-group+gitlab-sub-group+${PROJECT_NAME}"
                #"--channel" "stable"
                "${PROJECT_SOURCE_DIR}/conanfile.py"
        DEPENDS "cmake-install"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )
endif()

add_custom_target("vscode"
    COMMAND "${CMAKE_COMMAND}" "-P" "cmake/util.cmake" "--" "vscode"
            SOURCE_DIR "${PROJECT_SOURCE_DIR}"
            BINARY_DIR "${PROJECT_BINARY_DIR}"

            LAUNCH_GENERATE "1"
            LAUNCH_FILE_OVERWRITE "1"
            LAUNCH_TEMPLATE_FILE "${PROJECT_SOURCE_DIR}/src/test/resources/vscode/launch.json"
            LAUNCH_FILE "${PROJECT_SOURCE_DIR}/.vscode/launch.json"
            LAUNCH_TESTS_FILE "${PROJECT_BINARY_DIR}/test/bin/tests.json"
            LAUNCH_TESTS "*.*" "*UnitTests.*" "*GuiTests.*"
            LAUNCH_DEFAULT_TEST "*.*"
    DEPENDS "test-app"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target("clion"
    COMMAND "${CMAKE_COMMAND}" "-P" "cmake/util.cmake" "--" "clion"
            SOURCE_DIR "${PROJECT_SOURCE_DIR}"
            BINARY_DIR "${PROJECT_BINARY_DIR}"

            GTEST_GENERATE "1"
            GTEST_FILES_OVERWRITE "1"
            GTEST_TESTS_FILE "${PROJECT_BINARY_DIR}/test/bin/tests.json"
            GTEST_TEMPLATE_FILE "${PROJECT_SOURCE_DIR}/src/test/resources/clion/gtest/filter-run.xml"
            GTEST_CONFIG_NAME "${PRESET_NAME}"
            GTEST_PROJECT_NAME "${PROJECT_NAME}"
            GTEST_TARGET_NAME "test-app"
            GTEST_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/.clion/gtest"
            GTEST_TESTS "*.*" "*UnitTests.*" "*GuiTests.*"
            GTEST_PROGRAM_PARAMS "--project-binary-dir=${PROJECT_BINARY_DIR}"
    DEPENDS "test-app"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

set_target_names(ALL_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "all-targets:")
foreach(t IN LISTS ALL_TARGETS)
    message(STATUS "${t}")
endforeach()

set("${PROJECT_NAME}_ADDITIONAL_CLEAN_FILES"
    "${CMAKE_INSTALL_PREFIX}"
)
set_directory_properties(PROPERTIES
    ADDITIONAL_CLEAN_FILES "${${PROJECT_NAME}_ADDITIONAL_CLEAN_FILES}"
)
