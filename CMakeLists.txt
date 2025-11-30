cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_VERBOSE_MAKEFILE "TRUE" CACHE BOOL "..." FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE" CACHE BOOL "..." FORCE)
set(COMPILER "None" CACHE STRING "...")
set(VULKAN_PACKAGE_NAME "VulkanHeaders" CACHE STRING "...")
set(VULKAN_PACKAGE_VERSION "1.3.231.1" CACHE STRING "...")
set(USE_CONAN "TRUE" CACHE BOOL "...")
set(CONAN_VULKAN_REFERENCE "vulkan-headers/${VULKAN_PACKAGE_VERSION}" CACHE STRING "...")
set(CONAN_GTEST_REFERENCE "gtest/1.11.0" CACHE STRING "...")
set(GLSLC_CONAN "TRUE" CACHE BOOL "...")
set(VVL_CONAN "TRUE" CACHE BOOL "...")
set(SKIP_TEST "FALSE" CACHE BOOL "...")
set(TEST_GROUP "all" CACHE STRING "...")
set(TEST_CASE "all" CACHE STRING "...")

block()
    set(fileName "util.cmake")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../cmake/${fileName}")
        file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}")
        file(DOWNLOAD
            "https://raw.githubusercontent.com/exqudens/exqudens-cmake/1.0.1/src/main/cmake/${fileName}"
            "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}"
            EXPECTED_MD5 "d7dd546adf3830d9eb6cca7b0dda7d4e"
            STATUS downloadStatus
        )
        if(NOT "${downloadStatus}" STREQUAL "0;\"No error\"")
            message(FATAL_ERROR "downloadStatus: '${downloadStatus}'")
        endif()
        file(COPY "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../cmake")
    endif()
endblock()

include("${CMAKE_CURRENT_BINARY_DIR}/../cmake/util.cmake")

string(REPLACE "." ";" PRESET_NAME_ELEMENTS "${PRESET_NAME}")
if("msvc-16-x64-x64" IN_LIST PRESET_NAME_ELEMENTS OR "${COMPILER}" STREQUAL "msvc-16-x64-x64")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../cmake/msvc-16-x64-x64-toolchain.cmake")
        set_msvc_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            VERSION "16"
            HOST "x64"
            TARGET "x64"
            #NO_CACHE "FALSE"
            OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/../cmake/msvc-16-x64-x64-toolchain.cmake"
        )
    endif()
    if("${USE_CONAN}")
        find_program(CONAN_COMMAND NAMES "conan.exe" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
    if(NOT "${GLSLC_CONAN}")
        find_program(GLSLC_COMMAND NAMES "glslc.exe" PATHS ENV GLSLC_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
    #[[if(NOT "${VVL_CONAN}")
        if("${VK_LAYER_PATH}" STREQUAL "")
            set(VK_LAYER_PATH "$ENV{VK_LAYER_PATH}")
        elseif()
            set(ENV{VK_LAYER_PATH} "${VK_LAYER_PATH}")
        endif()
        if("${VK_LAYER_PATH}" STREQUAL "")
            message(FATAL_ERROR "'VK_LAYER_PATH' is empty!")
        endif()
    endif()]]
elseif("gcc" IN_LIST PRESET_NAME_ELEMENTS OR "${COMPILER}" STREQUAL "gcc")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../cmake/gcc-toolchain.cmake")
        find_file(COMPILER_FILE NAMES "gcc" PATHS ENV GCC_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
        set_gnu_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            PATH "${COMPILER_FILE}"
            NO_CACHE "FALSE"
            OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/../cmake/gcc-toolchain.cmake"
        )
    endif()
    if("${USE_CONAN}")
        find_program(CONAN_COMMAND NAMES "conan" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
    if(NOT "${GLSLC_CONAN}")
        find_program(GLSLC_COMMAND NAMES "glslc" PATHS ENV GLSLC_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
endif()

message("CMAKE_TOOLCHAIN_FILE: '${CMAKE_TOOLCHAIN_FILE}'")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/name-version.txt" INFO)
string(REPLACE ":" ";" INFO_LIST "${INFO}")
list(GET INFO_LIST 0 RAW_NAME)
list(GET INFO_LIST 1 RAW_VERSION)
string(STRIP "${RAW_NAME}" NAME)
string(STRIP "${RAW_VERSION}" VERSION)
project("${NAME}" VERSION "${VERSION}" LANGUAGES "NONE")

message("project: '${PROJECT_NAME}-${PROJECT_VERSION}'")

include("GenerateExportHeader")
enable_testing()
include("GoogleTest")

find_program(CONAN_COMMAND NAMES "conan.exe" "conan" REQUIRED)
find_program(DOXYGEN_COMMAND NAMES "doxygen.exe" "doxygen" REQUIRED)

enable_language("C")
enable_language("CXX")

set(CMAKE_OBJECT_PATH_MAX 1000)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")
set(CMAKE_STAGING_PREFIX "")
set(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH TRUE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_ROOT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_PATH TRUE)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG FALSE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
set(CMAKE_BUILD_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if("${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}")
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/cmake-install" CACHE PATH "..." FORCE)
endif()

separate_arguments(CMAKE_CXX_FLAGS NATIVE_COMMAND "${CMAKE_CXX_FLAGS}")
if(MSVC)
    if(NOT "/EHa" IN_LIST CMAKE_CXX_FLAGS AND "/EHsc" IN_LIST CMAKE_CXX_FLAGS)
        list(REMOVE_ITEM CMAKE_CXX_FLAGS "/EHsc")
        list(APPEND CMAKE_CXX_FLAGS "/EHa")
    endif()
endif()
string(JOIN " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

if("${BUILD_SHARED_LIBS}")
    if(NOT "${CMAKE_MSVC_RUNTIME_LIBRARY}" STREQUAL "")
        set(CONAN_RELEASE_COMPILER_RUNTIME "MD")
    endif()
else()
    if(NOT "${CMAKE_MSVC_RUNTIME_LIBRARY}" STREQUAL "")
        set(CONAN_RELEASE_COMPILER_RUNTIME "MT")
    endif()
    if("${MINGW}")
        set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()
endif()

set(TARGET_CMAKE_INSTALL_DEPENDS_ON "${PROJECT_NAME}")
if(NOT "${SKIP_TEST}")
    set(TARGET_CMAKE_INSTALL_DEPENDS_ON "cmake-test")
    if("${TEST_GROUP}" STREQUAL "all" AND "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^.+\\..+$")
    elseif(NOT "${TEST_GROUP}" STREQUAL "all" AND "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^${TEST_GROUP}\\..+$")
    elseif("${TEST_GROUP}" STREQUAL "all" AND NOT "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^.+\\.${TEST_CASE}$")
    elseif(NOT "${TEST_GROUP}" STREQUAL "all" AND NOT "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^${TEST_GROUP}\\.${TEST_CASE}$")
    endif()
endif()

set(CONAN_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/conan-install")
set(CONAN_INSTALL_TEST_PREFIX "${PROJECT_BINARY_DIR}/conan-install-test")
set(CONAN_INSTALL_GLSLC_PREFIX "${PROJECT_BINARY_DIR}/conan-install-glslc")
set(CONAN_INSTALL_VVL_PREFIX "${PROJECT_BINARY_DIR}/conan-install-vvl")

file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/lib")
file(MAKE_DIRECTORY "${CONAN_INSTALL_TEST_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_TEST_PREFIX}/lib")
file(MAKE_DIRECTORY "${CONAN_INSTALL_GLSLC_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_GLSLC_PREFIX}/lib")
file(MAKE_DIRECTORY "${CONAN_INSTALL_VVL_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_VVL_PREFIX}/lib")

if("${USE_CONAN}")
    set_conan_architecture(CONAN_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
    set_conan_compiler(CONAN_COMPILER "${CMAKE_CXX_COMPILER_ID}")
    set_conan_compiler_version(CONAN_COMPILER_VERSION "${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}" MAX_ELEMENTS "1")
    set_conan_compiler_runtime(CONAN_COMPILER_RUNTIME "${CMAKE_MSVC_RUNTIME_LIBRARY}")

    set_python_boolean(CONAN_INTERFACE_ONLY "${INTERFACE_ONLY}")
    set_python_boolean(CONAN_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")

    if("${GLSLC_CONAN}")
        set_conan_compiler_version(CONAN_GLSLC_COMPILER_VERSION "${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}" MAX_ELEMENTS "1")

        if("${CONAN_COMPILER}" STREQUAL "gcc" AND "${CONAN_GLSLC_COMPILER_VERSION}" GREATER "10")
            set(CONAN_GLSLC_COMPILER_VERSION "10")
        endif()

        set_conan_settings(CONAN_GLSLC_SETTINGS
            "arch=${CONAN_ARCH}"
            "os=${CMAKE_SYSTEM_NAME}"
            "compiler=${CONAN_COMPILER}"
            "compiler.version=${CONAN_GLSLC_COMPILER_VERSION}"
            "compiler.runtime=${CONAN_RELEASE_COMPILER_RUNTIME}"
            "compiler.libcxx=${CONAN_COMPILER_LIBCXX}"
            "build_type=Release"
        )

        if(NOT EXISTS "${CONAN_INSTALL_GLSLC_PREFIX}/conan-packages.cmake")
            execute_process(
                COMMAND "${CONAN_COMMAND}" install
                        --install-folder "${CONAN_INSTALL_GLSLC_PREFIX}"
                        ${CONAN_GLSLC_SETTINGS}
                        "${PROJECT_SOURCE_DIR}/glslc-conanfile.py"
                COMMAND_ECHO "STDERR"
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL ANY
            )
        endif()

        if(NOT EXISTS "${CONAN_INSTALL_GLSLC_PREFIX}/conan-packages.cmake")
            message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_GLSLC_PREFIX}/conan-packages.cmake'")
        endif()

        include("${CONAN_INSTALL_GLSLC_PREFIX}/conan-packages.cmake")
        list(GET "${PROJECT_NAME}_CMAKE_PACKAGE_PATHS" "0" cmakePackagePath)
        find_program(GLSLC_COMMAND NAMES "glslc.exe" "glslc" PATHS "${cmakePackagePath}/bin" REQUIRED NO_DEFAULT_PATH)
    endif()

    if("${VVL_CONAN}")
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
            if(NOT "${CMAKE_MSVC_RUNTIME_LIBRARY}" STREQUAL "")
                set(CONAN_COMPILER_RUNTIME_MD "MD")
            endif()

            set_conan_settings(CONAN_VVL_SETTINGS
                "arch=${CONAN_ARCH}"
                "os=${CMAKE_SYSTEM_NAME}"
                "compiler=${CONAN_COMPILER}"
                "compiler.version=${CONAN_COMPILER_VERSION}"
                "compiler.runtime=${CONAN_COMPILER_RUNTIME_MD}"
                "compiler.libcxx=${CONAN_COMPILER_LIBCXX}"
                "build_type=Release"
            )

            if(NOT EXISTS "${CONAN_INSTALL_VVL_PREFIX}/conan-packages.cmake")
                execute_process(
                    COMMAND "${CONAN_COMMAND}" install
                            --install-folder "${CONAN_INSTALL_VVL_PREFIX}"
                            ${CONAN_VVL_SETTINGS}
                            --options "vulkan_reference=vulkan-validationlayers/${VULKAN_PACKAGE_VERSION}"
                            "${PROJECT_SOURCE_DIR}/vvl-conanfile.py"
                    COMMAND_ECHO "STDERR"
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ENCODING "UTF-8"
                    COMMAND_ERROR_IS_FATAL ANY
                )
            endif()

            if(NOT EXISTS "${CONAN_INSTALL_VVL_PREFIX}/conan-packages.cmake")
                message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_VVL_PREFIX}/conan-packages.cmake'")
            endif()

            file(COPY "${CONAN_INSTALL_VVL_PREFIX}/bin/" DESTINATION "${CONAN_INSTALL_TEST_PREFIX}/bin")
            file(COPY "${CONAN_INSTALL_VVL_PREFIX}/lib/" DESTINATION "${CONAN_INSTALL_TEST_PREFIX}/lib")
        endif()
    endif()

    set_conan_settings(CONAN_SETTINGS
        "arch=${CONAN_ARCH}"
        "os=${CMAKE_SYSTEM_NAME}"
        "compiler=${CONAN_COMPILER}"
        "compiler.version=${CONAN_COMPILER_VERSION}"
        "compiler.runtime=${CONAN_COMPILER_RUNTIME}"
        "compiler.libcxx=${CONAN_COMPILER_LIBCXX}"
        "build_type=${CMAKE_BUILD_TYPE}"
        "vulkan:build_type=${CONAN_VULKAN_BUILD_TYPE}"
        "vulkan:compiler.runtime=${CONAN_VULKAN_COMPILER_RUNTIME}"
        "glfw:build_type=Release"
        "glfw:compiler.runtime=${CONAN_RELEASE_COMPILER_RUNTIME}"
    )
    set_conan_options(CONAN_OPTIONS
        "interface=${CONAN_INTERFACE_ONLY}"
        "shared=${CONAN_BUILD_SHARED_LIBS}"
        "vulkan_reference=${CONAN_VULKAN_REFERENCE}"
    )
    message("CONAN_SETTINGS: '${CONAN_SETTINGS}'")
    message("CONAN_OPTIONS: '${CONAN_OPTIONS}'")
    if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
        execute_process(
            COMMAND "${CONAN_COMMAND}" install
                    --install-folder "${CONAN_INSTALL_PREFIX}"
                    ${CONAN_SETTINGS}
                    ${CONAN_OPTIONS}
                    "${PROJECT_SOURCE_DIR}/conanfile.py"
            COMMAND_ECHO "STDERR"
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ENCODING "UTF-8"
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
        message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_PREFIX}/conan-packages.cmake'")
    endif()
    include("${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
    list(APPEND CMAKE_MODULE_PATH "${CONAN_INSTALL_PREFIX}")
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

        if("Vulkan" STREQUAL "${cmakePackageName}")
            list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${VULKAN_PACKAGE_NAME}")
        else()
            list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${cmakePackageName}")
        endif()
        list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")
    endforeach()

    if(NOT "${SKIP_TEST}")
        if(NOT EXISTS "${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake")
            execute_process(
                COMMAND "${CONAN_COMMAND}" install
                        --install-folder "${CONAN_INSTALL_TEST_PREFIX}"
                        ${CONAN_SETTINGS}
                        ${CONAN_OPTIONS}
                        --options "gtest_reference=${CONAN_GTEST_REFERENCE}"
                        "${PROJECT_SOURCE_DIR}/test-conanfile.py"
                COMMAND_ECHO "STDERR"
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL ANY
            )
        endif()
        if(NOT EXISTS "${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake")
            message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake'")
        endif()
        include("${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake")
        list(APPEND CMAKE_MODULE_PATH "${CONAN_INSTALL_TEST_PREFIX}")
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

            if("glfw" STREQUAL "${cmakePackageName}")
                list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "glfw3")
            else()
                list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${cmakePackageName}")
            endif()
            list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")
        endforeach()
    endif()
endif()

if("${${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES}" STREQUAL "")
    set("${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
        "${VULKAN_PACKAGE_NAME}"
    )
    if(NOT "${SKIP_TEST}")
        list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
            "GTest"
            "Easyloggingpp"
            "glfw3"
            "glm"
            "tinyobjloader"
            "lodepng"
        )
    endif()
endif()
message("${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES:")
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    message("  ${cmakePackageName}")
endforeach()
unset(NOT_FOUND_PACKAGE_NAMES)
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    if("VulkanHeaders" STREQUAL "${cmakePackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        if(
            "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" STREQUAL ""
            AND
            NOT "${${PROJECT_NAME}_CMAKE_PACKAGE_Vulkan_VERSION}" STREQUAL ""
        )
            set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION" "${${PROJECT_NAME}_CMAKE_PACKAGE_Vulkan_VERSION}")
        endif()
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    elseif("Vulkan" STREQUAL "${cmakePackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" STREQUAL "")
            find_package("${cmakePackageName}" QUIET)
        else()
            find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
        endif()
    elseif("GTest" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_TEST_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET CONFIG)
    elseif("Easyloggingpp" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_TEST_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET CONFIG)
    elseif("glfw3" STREQUAL "${cmakePackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" QUIET)
    elseif("glm" STREQUAL "${cmakePackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" QUIET)
    elseif("tinyobjloader" STREQUAL "${cmakePackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    elseif("lodepng" STREQUAL "${cmakePackageName}")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" QUIET)
    else()
        message("Ignore cmakePackageName: '${cmakePackageName}'")
    endif()
endforeach()
set_not_found_package_names(NOT_FOUND_PACKAGE_NAMES ${NOT_FOUND_PACKAGE_NAMES})
if(NOT "" STREQUAL "${NOT_FOUND_PACKAGE_NAMES}")
    foreach(package IN LISTS NOT_FOUND_PACKAGE_NAMES)
        message(SEND_ERROR "${package}_FOUND: '${${package}_FOUND}' ${package}_NOT_FOUND_MESSAGE: '${${package}_NOT_FOUND_MESSAGE}'")
    endforeach()
    message(FATAL_ERROR "Some package not found!")
endif()

set(FUNCTION_ATTRIBUTES_DEFINE_NAME "EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES")
set(FUNCTION_ATTRIBUTES_DEFINE "#define ${FUNCTION_ATTRIBUTES_DEFINE_NAME}")
if("${INTERFACE_ONLY}")
    set(FUNCTION_ATTRIBUTES_DEFINE "#define ${FUNCTION_ATTRIBUTES_DEFINE_NAME} inline")
endif()
string(JOIN "\n" FUNCTION_ATTRIBUTES_DEFINE_CONTENT
    "#ifndef ${FUNCTION_ATTRIBUTES_DEFINE_NAME}"
    "${FUNCTION_ATTRIBUTES_DEFINE}"
    "#endif /* ${FUNCTION_ATTRIBUTES_DEFINE_NAME} */\n"
)

if("${INTERFACE_ONLY}")
    generate_interface_only_files(GENERATED_SOURCES
        SRC_DIRECTORY "${PROJECT_SOURCE_DIR}"
        SRC_BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/cpp"
        DST_BASE_DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp"
        HEADER_FILES_EXPRESSIONS "**/*.hpp"
        SOURCE_FILES_EXPRESSIONS "**/*.cpp"
    )

    add_library("${PROJECT_NAME}" INTERFACE
        ${GENERATED_SOURCES}
    )
    file(WRITE "${PROJECT_BINARY_DIR}/generated/src/main/cpp/exqudens/vulkan/export.hpp"
        "#ifndef EXQUDENS_VULKAN_EXPORT_H\n"
        "#define EXQUDENS_VULKAN_EXPORT_H\n"
        "\n"
        "#ifndef EXQUDENS_VULKAN_EXPORT\n"
        "#define EXQUDENS_VULKAN_EXPORT\n"
        "#endif /* EXQUDENS_VULKAN_EXPORT */\n"
        "\n"
        "${FUNCTION_ATTRIBUTES_DEFINE_CONTENT}"
        "\n"
        "#endif /* EXQUDENS_VULKAN_EXPORT_H */\n"
    )
    target_include_directories("${PROJECT_NAME}" INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/main/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
    target_link_libraries("${PROJECT_NAME}" INTERFACE
        "Vulkan::Vulkan"
    )
    if(MSVC AND "Debug" STREQUAL "${CMAKE_BUILD_TYPE}")
        target_compile_options("${PROJECT_NAME}" INTERFACE
            "-bigobj"
        )
    endif()
else()
    add_library("${PROJECT_NAME}"
        "src/main/cpp/exqudens/vulkan/all.hpp"

        "src/main/cpp/exqudens/vulkan/Utility.hpp"
        "src/main/cpp/exqudens/vulkan/Utility.cpp"

        "src/main/cpp/exqudens/vulkan/Instance.hpp"
        "src/main/cpp/exqudens/vulkan/Instance.cpp"
        "src/main/cpp/exqudens/vulkan/MessengerCreateInfo.hpp"
        "src/main/cpp/exqudens/vulkan/MessengerCreateInfo.cpp"
        "src/main/cpp/exqudens/vulkan/Messenger.hpp"
        "src/main/cpp/exqudens/vulkan/Messenger.cpp"
        "src/main/cpp/exqudens/vulkan/QueueInfo.hpp"
        "src/main/cpp/exqudens/vulkan/QueueInfo.cpp"
        "src/main/cpp/exqudens/vulkan/PhysicalDevice.hpp"
        "src/main/cpp/exqudens/vulkan/PhysicalDevice.cpp"
        "src/main/cpp/exqudens/vulkan/Device.hpp"
        "src/main/cpp/exqudens/vulkan/Device.cpp"
        "src/main/cpp/exqudens/vulkan/Image.hpp"
        "src/main/cpp/exqudens/vulkan/Image.cpp"
        "src/main/cpp/exqudens/vulkan/ImageView.hpp"
        "src/main/cpp/exqudens/vulkan/ImageView.cpp"
        "src/main/cpp/exqudens/vulkan/Buffer.hpp"
        "src/main/cpp/exqudens/vulkan/Buffer.cpp"
        "src/main/cpp/exqudens/vulkan/Sampler.hpp"
        "src/main/cpp/exqudens/vulkan/Sampler.cpp"
        "src/main/cpp/exqudens/vulkan/Semaphore.hpp"
        "src/main/cpp/exqudens/vulkan/Semaphore.cpp"
        "src/main/cpp/exqudens/vulkan/Fence.hpp"
        "src/main/cpp/exqudens/vulkan/Fence.cpp"
        "src/main/cpp/exqudens/vulkan/SubpassDescription.hpp"
        "src/main/cpp/exqudens/vulkan/SubpassDescription.cpp"
        "src/main/cpp/exqudens/vulkan/RenderPass.hpp"
        "src/main/cpp/exqudens/vulkan/RenderPass.cpp"
        "src/main/cpp/exqudens/vulkan/DescriptorSetLayout.hpp"
        "src/main/cpp/exqudens/vulkan/DescriptorSetLayout.cpp"
        "src/main/cpp/exqudens/vulkan/PipelineVertexInputStateCreateInfo.hpp"
        "src/main/cpp/exqudens/vulkan/PipelineVertexInputStateCreateInfo.cpp"
        "src/main/cpp/exqudens/vulkan/PipelineViewportStateCreateInfo.hpp"
        "src/main/cpp/exqudens/vulkan/PipelineViewportStateCreateInfo.cpp"
        "src/main/cpp/exqudens/vulkan/PipelineColorBlendStateCreateInfo.hpp"
        "src/main/cpp/exqudens/vulkan/PipelineColorBlendStateCreateInfo.cpp"
        "src/main/cpp/exqudens/vulkan/GraphicsPipelineCreateInfo.hpp"
        "src/main/cpp/exqudens/vulkan/GraphicsPipelineCreateInfo.cpp"
        "src/main/cpp/exqudens/vulkan/Pipeline.hpp"
        "src/main/cpp/exqudens/vulkan/Pipeline.cpp"
        "src/main/cpp/exqudens/vulkan/DescriptorPool.hpp"
        "src/main/cpp/exqudens/vulkan/DescriptorPool.cpp"
        "src/main/cpp/exqudens/vulkan/WriteDescriptorSet.hpp"
        "src/main/cpp/exqudens/vulkan/WriteDescriptorSet.cpp"
        "src/main/cpp/exqudens/vulkan/DescriptorSet.hpp"
        "src/main/cpp/exqudens/vulkan/DescriptorSet.cpp"
        "src/main/cpp/exqudens/vulkan/Framebuffer.hpp"
        "src/main/cpp/exqudens/vulkan/Framebuffer.cpp"
        "src/main/cpp/exqudens/vulkan/Queue.hpp"
        "src/main/cpp/exqudens/vulkan/Queue.cpp"
        "src/main/cpp/exqudens/vulkan/CommandPool.hpp"
        "src/main/cpp/exqudens/vulkan/CommandPool.cpp"
        "src/main/cpp/exqudens/vulkan/CommandBuffer.hpp"
        "src/main/cpp/exqudens/vulkan/CommandBuffer.cpp"
        "src/main/cpp/exqudens/vulkan/QueryPool.hpp"
        "src/main/cpp/exqudens/vulkan/QueryPool.cpp"
        "src/main/cpp/exqudens/vulkan/Surface.hpp"
        "src/main/cpp/exqudens/vulkan/Surface.cpp"
        "src/main/cpp/exqudens/vulkan/Swapchain.hpp"
        "src/main/cpp/exqudens/vulkan/Swapchain.cpp"
    )
    generate_export_header("${PROJECT_NAME}"
        BASE_NAME "EXQUDENS_VULKAN"
        EXPORT_FILE_NAME "generated/src/main/cpp/exqudens/vulkan/export.hpp"
        CUSTOM_CONTENT_FROM_VARIABLE "FUNCTION_ATTRIBUTES_DEFINE_CONTENT"
    )
    if(NOT "${BUILD_SHARED_LIBS}")
        target_compile_definitions("${PROJECT_NAME}" PUBLIC
            #"__BASE_FILE__=\"${PROJECT_SOURCE_DIR}\""
            "EXQUDENS_VULKAN_STATIC_DEFINE"
        )
    endif()
    target_include_directories("${PROJECT_NAME}" PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/main/cpp>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
    target_link_libraries("${PROJECT_NAME}" PUBLIC
        "Vulkan::Vulkan"
    )
    if(MSVC AND "Debug" STREQUAL "${CMAKE_BUILD_TYPE}")
        target_compile_options("${PROJECT_NAME}" PUBLIC
            "-bigobj"
        )
    endif()
    set_target_properties("${PROJECT_NAME}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"
    )
endif()
set_property(TARGET "${PROJECT_NAME}" PROPERTY "VERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "SOVERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "INTERFACE_${PROJECT_NAME}_MAJOR_VERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" APPEND PROPERTY "COMPATIBLE_INTERFACE_STRING" "${PROJECT_NAME}_MAJOR_VERSION")
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
if("${INTERFACE_ONLY}")
    install(
        DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp/"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
    )
else()
    install(
        DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp/"
                  "src/main/cpp/"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
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

if(NOT "${SKIP_TEST}")
    add_library("test-lib" STATIC
        "src/test/cpp/TestApplication.hpp"
        "src/test/cpp/TestApplication.cpp"
        "src/test/cpp/TestConfiguration.hpp"
        "src/test/cpp/TestConfiguration.cpp"
        "src/test/cpp/TestUtils.hpp"
        "src/test/cpp/TestUtils.cpp"
        "src/test/cpp/TestLogging.hpp"
        "src/test/cpp/TestLogging.cpp"
        "src/test/cpp/exqudens/vulkan/DrawingTriangleSystemTests.hpp"
        "src/test/cpp/exqudens/vulkan/Vertex.hpp"
        "src/test/cpp/exqudens/vulkan/UniformBufferObject.hpp"
        "src/test/cpp/exqudens/vulkan/Context.hpp"
        "src/test/cpp/exqudens/vulkan/Context.cpp"
        "src/test/cpp/exqudens/vulkan/DataContext.hpp"
        "src/test/cpp/exqudens/vulkan/DataContext.cpp"
        "src/test/cpp/exqudens/vulkan/CameraContext.hpp"
        "src/test/cpp/exqudens/vulkan/CameraContext.cpp"
        "src/test/cpp/exqudens/vulkan/LightContext.hpp"
        "src/test/cpp/exqudens/vulkan/LightContext.cpp"
        "src/test/cpp/exqudens/vulkan/OtherTests.hpp"
        "src/test/cpp/exqudens/vulkan/Tests.hpp"
        "src/test/cpp/exqudens/vulkan/TestUtilsTests.hpp"
        "src/test/cpp/exqudens/vulkan/TinyObjLoaderTests.hpp"
        "src/test/cpp/exqudens/vulkan/UiTests.hpp"
    )
    target_include_directories("test-lib" PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/test/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
    target_link_libraries("test-lib" PUBLIC
        "${PROJECT_NAME}"
        "GTest::gmock"
        "GTest::gtest"
        "GTest::gmock_main"
        "GTest::gtest_main"
        "Easyloggingpp::Easyloggingpp"
        "glm::glm"
        "lodepng::lodepng"
        "glfw"
        "tinyobjloader::tinyobjloader"
    )
    set_target_properties("test-lib" PROPERTIES
        COMPILE_DEFINITIONS "__BASE_FILE__=\"${PROJECT_SOURCE_DIR}\""

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

    add_custom_command(
        OUTPUT "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.vert.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-1.frag.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.vert.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-2.frag.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.vert.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-3.frag.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.vert.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-4.frag.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shadow-shader-5.vert.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shadow-shader-5.frag.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-5.vert.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-5.frag.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-10.vert.spv"
               "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-10.frag.spv"
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
        COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shadow-shader-5.vert" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shadow-shader-5.vert.spv"
        COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shadow-shader-5.frag" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shadow-shader-5.frag.spv"
        COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-5.vert" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-5.vert.spv"
        COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-5.frag" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-5.frag.spv"
        COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-10.vert" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-10.vert.spv"
        COMMAND "${GLSLC_COMMAND}" "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-10.frag" -o "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-10.frag.spv"
        DEPENDS "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-1.vert"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-1.frag"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-2.vert"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-2.frag"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-3.vert"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-3.frag"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-4.vert"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-4.frag"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shadow-shader-5.vert"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shadow-shader-5.frag"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-5.vert"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-5.frag"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-10.vert"
                "${PROJECT_SOURCE_DIR}/src/test/resources/shader/shader-10.frag"
        USES_TERMINAL
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
        "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shadow-shader-5.vert.spv"
        "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shadow-shader-5.frag.spv"
        "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-5.vert.spv"
        "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-5.frag.spv"
        "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-10.vert.spv"
        "${PROJECT_BINARY_DIR}/test/bin/resources/shader/shader-10.frag.spv"
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
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_TEST_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/src/test/resources" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                USES_TERMINAL
                VERBATIM
            )
        else()
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_TEST_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},LIBRARY_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/src/test/resources" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                USES_TERMINAL
                VERBATIM
            )
        endif()
    else()
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_TEST_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/src/test/resources" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                USES_TERMINAL
                VERBATIM
            )
        else()
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_TEST_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/src/test/resources" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>/resources"
                USES_TERMINAL
                VERBATIM
            )
        endif()
    endif()

    gtest_discover_tests("test-app"
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/bin"
        #PROPERTIES ENVIRONMENT "PARENT_PATH=;PATH="
    )

    add_custom_target("cmake-test"
        COMMAND "${CMAKE_CTEST_COMMAND}" --preset "${PRESET_NAME}" "-R" "${TEST_REGEXP}"
        DEPENDS "test-app"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )
endif()

add_custom_target("cmake-install" ALL
    COMMAND "${CMAKE_COMMAND}" --install "${PROJECT_BINARY_DIR}" --prefix "${CMAKE_INSTALL_PREFIX}"
    DEPENDS ${TARGET_CMAKE_INSTALL_DEPENDS_ON}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target("conan-export"
    COMMAND "${CONAN_COMMAND}" export-pkg
            --force
            --package-folder "${CMAKE_INSTALL_PREFIX}"
            ${CONAN_SETTINGS}
            ${CONAN_OPTIONS}
            "${PROJECT_SOURCE_DIR}/conanfile.py"
            #"gitlab-group+gitlab-sub-group+${PROJECT_NAME}/stable"
    DEPENDS "cmake-install"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

set_target_names(ALL_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}")
message("ALL_TARGETS:")
foreach(t IN LISTS ALL_TARGETS)
    message("  ${t}")
endforeach()
