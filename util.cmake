cmake_policy(PUSH)
cmake_policy(SET CMP0054 NEW)
cmake_policy(PUSH)
cmake_policy(SET CMP0007 NEW)
cmake_policy(PUSH)
cmake_policy(SET CMP0012 NEW)
cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW)

function(set_if_not_defined var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()
    if("" STREQUAL "${${var}}" AND NOT "" STREQUAL "${ARGN}")
        set("${var}" "${ARGN}" PARENT_SCOPE)
    endif()
endfunction()

function(substring_from var input fromExclusive)
    foreach(i var input fromExclusive)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()
    string(FIND "${input}" "${fromExclusive}" fromStartIndex)
    if("-1" STREQUAL "${fromStartIndex}")
        message(FATAL_ERROR "Can't find 'fromExclusive' in 'input'")
    endif()
    string(LENGTH "${input}" inputLength)
    string(LENGTH "${fromExclusive}" "fromLength")
    math(EXPR fromEndIndex "${fromStartIndex} + ${fromLength}")
    math(EXPR substringLength "${inputLength} - ${fromEndIndex}")
    string(SUBSTRING "${input}" "${fromEndIndex}" "${substringLength}" result)
    set("${var}" "${result}" PARENT_SCOPE)
endfunction()

function(substring_to var input toExclusive)
    foreach(i var input toExclusive)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()
    string(FIND "${input}" "${toExclusive}" toStartIndex)
    if("-1" STREQUAL "${toStartIndex}")
        message(FATAL_ERROR "Can't find 'toExclusive' in 'input'")
    endif()
    string(LENGTH "${input}" inputLength)
    string(SUBSTRING "${input}" 0 "${toStartIndex}" result)
    set("${var}" "${result}" PARENT_SCOPE)
endfunction()

function(string_replace_between prefix input fromExclusive toExclusive)
    foreach(i prefix input fromExclusive toExclusive)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    set(options
        "RESULT_ONLY"
        "REPLACED_ONLY"
        "BETWEEN_ONLY"
    )
    set(oneValueKeywords
        "WITH"
    )
    set(multiValueKeywords)
    cmake_parse_arguments("string_replace_between" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    if("" STREQUAL "${string_replace_between_WITH}")
        set(value "")
    else()
        set(value "${string_replace_between_WITH}")
    endif()

    if("${string_replace_between_REPLACED_ONLY}")
        substring_from(between "${input}" "${fromExclusive}")
        substring_to(between "${between}" "${toExclusive}")

        string(CONCAT replaced "${fromExclusive}" "${between}" "${toExclusive}")

        set("${prefix}" "${replaced}" PARENT_SCOPE)
    elseif("${string_replace_between_BETWEEN_ONLY}")
        substring_from(between "${input}" "${fromExclusive}")
        substring_to(between "${between}" "${toExclusive}")

        set("${prefix}" "${between}" PARENT_SCOPE)
    else()
        if("${string_replace_between_RESULT_ONLY}")
            substring_to(part1 "${input}" "${fromExclusive}")
            substring_from(part2 "${input}" "${toExclusive}")
            string(CONCAT result "${part1}" "${value}" "${part2}")

            set("${prefix}" "${result}" PARENT_SCOPE)
        else()
            substring_to(part1 "${input}" "${fromExclusive}")
            substring_from(part2 "${input}" "${toExclusive}")
            string(CONCAT result "${part1}" "${value}" "${part2}")

            substring_from(between "${input}" "${fromExclusive}")
            substring_to(between "${between}" "${toExclusive}")

            string(CONCAT replaced "${fromExclusive}" "${between}" "${toExclusive}")

            set("${prefix}" "${result}" PARENT_SCOPE)
            set("${prefix}_BETWEEN" "${between}" PARENT_SCOPE)
            set("${prefix}_REPLACED" "${replaced}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

function(find_file_in_parent prefix name path maxParentLevel)
    foreach(i prefix name path)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()
    set(func_max "${maxParentLevel}")
    if("" STREQUAL "${func_max}")
        set(func_max "1")
    endif()
    set(func_path "${path}")
    cmake_path(GET func_path ROOT_NAME func_path_root_name)
    cmake_path(GET func_path ROOT_DIRECTORY func_path_root_dir)
    set(func_path_root "${func_path_root_name}${func_path_root_dir}")
    foreach(i RANGE "1" "${func_max}")
        if("${func_path_root}" STREQUAL "${func_path}")
            break()
        endif()
        cmake_path(GET func_path PARENT_PATH func_path)
        file(GLOB_RECURSE func_files LIST_DIRECTORIES "TRUE" "${func_path}/*")
        foreach(func_file IN LISTS func_files)
            get_filename_component(func_file_name "${func_file}" NAME)
            if("${func_file_name}" STREQUAL "${name}")
                set(func_result "${func_file}")
                get_filename_component(func_result_dir "${func_result}" DIRECTORY)
                set(func_path "${func_path_root}")
                break()
            endif()
        endforeach()
    endforeach()
    set("${prefix}_FILE" "${func_result}" PARENT_SCOPE)
    set("${prefix}_DIR" "${func_result_dir}" PARENT_SCOPE)
endfunction()

function(find_file_in prefix name path)
    foreach(i prefix name path)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()
    file(GLOB_RECURSE func_files LIST_DIRECTORIES "TRUE" "${path}/*")
    foreach(func_file IN LISTS func_files)
        get_filename_component(func_file_name "${func_file}" NAME)
        if("${func_file_name}" STREQUAL "${name}")
            set(func_result "${func_file}")
            get_filename_component(func_result_dir "${func_result}" DIRECTORY)
            break()
        endif()
    endforeach()
    set("${prefix}_FILE" "${func_result}" PARENT_SCOPE)
    set("${prefix}_DIR" "${func_result_dir}" PARENT_SCOPE)
endfunction()

function(set_msvc_path var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    set(options)
    set(oneValueKeywords
        "COMMAND"
        "VERSION"
        "PROPERTY"
    )
    set(multiValueKeywords
        "PRODUCTS"
    )
    cmake_parse_arguments("set_msvc_path" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    set(command "${set_msvc_path_COMMAND}")
    set(version "${set_msvc_path_VERSION}")
    set(property "${set_msvc_path_PROPERTY}")
    set(products "${set_msvc_path_PRODUCTS}")

    if("" STREQUAL "${command}")
        set(command "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe")
    endif()

    if("" STREQUAL "${version}")
        set(version "-latest")
    endif()

    if("" STREQUAL "${property}")
        set(property "installationPath")
    endif()

    if("" STREQUAL "${products}")
        set(products
            "Microsoft.VisualStudio.Product.Enterprise"
            "Microsoft.VisualStudio.Product.Professional"
            "Microsoft.VisualStudio.Product.Community"
            "Microsoft.VisualStudio.Product.BuildTools"
            #"Microsoft.VisualStudio.Product.TeamExplorer"
            #"Microsoft.VisualStudio.Product.TestAgent"
            #"Microsoft.VisualStudio.Product.TestController"
        )
    endif()

    cmake_path(CONVERT "${command}" TO_CMAKE_PATH_LIST command NORMALIZE)

    if("17" STREQUAL "${version}" OR "2022" STREQUAL "${version}")
        set(versionArgs "-version" "[17.0, 18.0)")
    elseif("16" STREQUAL "${version}" OR "2019" STREQUAL "${version}")
        set(versionArgs "-version" "[16.0, 17.0)")
    elseif("15" STREQUAL "${version}" OR "2017" STREQUAL "${version}")
        set(versionArgs "-version" "[15.0, 16.0)")
    elseif("latest" STREQUAL "${version}" OR "-latest" STREQUAL "${version}")
        set(versionArgs "-latest")
    else()
        set(versionArgs "-version" "${version}")
    endif()

    set(propertyArgs "-property" "${property}")

    set(result "")

    foreach(i ${products})
        set(productsArgs "-products" "${i}")
        execute_process(
            COMMAND "${command}" ${versionArgs} ${productsArgs} ${propertyArgs}
            OUTPUT_VARIABLE "result"
            COMMAND_ECHO "STDERR"
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ENCODING "UTF-8"
            COMMAND_ERROR_IS_FATAL ANY
        )
        if(NOT "" STREQUAL "${result}")
            break()
        endif()
    endforeach()

    if("" STREQUAL "${result}")
        message(FATAL_ERROR "Empty result from: '${command}'.")
    endif()

    cmake_path(CONVERT "${result}" TO_CMAKE_PATH_LIST result NORMALIZE)
    string(REGEX REPLACE "[\r]" "" result "${result}")
    string(REGEX REPLACE "[\n]" ";" result "${result}")
    list(GET result 0 result)

    set("${var}" "${result}" PARENT_SCOPE)
endfunction()

function(set_msvc_env prefix)
    foreach(i prefix)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    set(options)
    set(oneValueKeywords
        "COMMAND"
        "VERSION"
        "PROPERTY"
        "HOST"
        "TARGET"
        "PATH"
    )
    set(multiValueKeywords
        "PRODUCTS"
    )
    cmake_parse_arguments("set_msvc_env" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    set(command "${set_msvc_env_COMMAND}")
    set(version "${set_msvc_env_VERSION}")
    set(property "${set_msvc_env_PROPERTY}")
    set(host "${set_msvc_env_HOST}")
    set(target "${set_msvc_env_TARGET}")
    set(path "${set_msvc_env_PATH}")
    set(products "${set_msvc_env_PRODUCTS}")

    if(NOT "" STREQUAL "${path}")
        if(NOT EXISTS "${path}")
            message(FATAL_ERROR "Not exists: '${path}'")
        elseif(IS_DIRECTORY "${path}")
            message(FATAL_ERROR "Is directory: '${path}'")
        endif()
    endif()

    if("" STREQUAL "${target}")
        get_filename_component(compilerDir "${path}" DIRECTORY)
        get_filename_component(compilerDirName "${compilerDir}" NAME)
        string(TOLOWER "${compilerDirName}" compilerDirNameLower)
        if("x86" STREQUAL "${compilerDirNameLower}")
            set(target "x86")
        elseif("x64" STREQUAL "${compilerDirNameLower}")
            set(target "x64")
        endif()
    endif()

    if("" STREQUAL "${host}" AND NOT "" STREQUAL "${compilerDir}")
        get_filename_component(compilerDirParent "${compilerDir}" DIRECTORY)
        get_filename_component(compilerDirParentName "${compilerDirParent}" NAME)
        string(TOLOWER "${compilerDirParentName}" compilerDirParentNameLower)
        if("hostx86" STREQUAL "${compilerDirParentNameLower}")
            set(host "x86")
        elseif("hostx64" STREQUAL "${compilerDirParentNameLower}")
            set(host "x64")
        endif()
    endif()

    if(NOT "x86" STREQUAL "${host}" AND NOT "x64" STREQUAL "${host}")
        string(JOIN " " errorMessage
            "Unsupported or not specified 'host': '${host}'."
            "Supported values ['x86', 'x64']."
        )
        message(FATAL_ERROR "${errorMessage}")
    endif()

    if(NOT "x86" STREQUAL "${target}" AND NOT "x64" STREQUAL "${target}")
        string(JOIN " " errorMessage
            "Unsupported or not specified 'target': '${target}'."
            "Supported values [ 'x86', 'x64' ]."
        )
        message(FATAL_ERROR "${errorMessage}")
    endif()

    if("${host}" STREQUAL "${target}")
        set(vcvarsallBatConf "${host}")
    else()
        set(vcvarsallBatConf "${host}_${target}")
    endif()

    set(vcvarsallBatName "vcvarsall.bat")

    if("" STREQUAL "${path}")
        set_msvc_path(msvcPath COMMAND "${command}" VERSION "${version}" PROPERTY "${property}" PRODUCTS "${products}")
        find_file_in(vcvarsall "${vcvarsallBatName}" "${msvcPath}")
        if("" STREQUAL "${vcvarsall_DIR}")
            message(FATAL_ERROR "Not found '${vcvarsallBatName}' in '${msvcPath}'")
        endif()
    elseif(NOT "" STREQUAL "${path}")
        if(NOT EXISTS "${path}")
            message(FATAL_ERROR "Not exists 'path': '${path}'")
        endif()
        find_file_in_parent(vcvarsall "${vcvarsallBatName}" "${path}" "9")
        if("" STREQUAL "${vcvarsall_DIR}")
            message(FATAL_ERROR "Not found '${vcvarsallBatName}' in '8' parent dirs of '${path}'")
        endif()
    endif()

    set(vcvarsallBatArgs
        "&&" "call" "echo" "INCLUDE_START"
        "&&" "call" "echo" "%INCLUDE%"
        "&&" "call" "echo" "INCLUDE_STOP"
        "&&" "call" "echo" "LIBPATH_START"
        "&&" "call" "echo" "%LIBPATH%"
        "&&" "call" "echo" "LIBPATH_STOP"
        "&&" "call" "echo" "LIB_START"
        "&&" "call" "echo" "%LIB%"
        "&&" "call" "echo" "LIB_STOP"
        "&&" "call" "echo" "CLPATH_START"
        "&&" "call" "where" "cl"
        "&&" "call" "echo" "CLPATH_STOP"
        "&&" "call" "echo" "RCPATH_START"
        "&&" "call" "where" "rc"
        "&&" "call" "echo" "RCPATH_STOP"
    )
    execute_process(
        COMMAND "cmd" "/c" "${vcvarsallBatName}" "${vcvarsallBatConf}" ${vcvarsallBatArgs}
        WORKING_DIRECTORY "${vcvarsall_DIR}"
        OUTPUT_VARIABLE "msvcEnv"
        COMMAND_ECHO "STDERR"
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ENCODING "UTF-8"
        COMMAND_ERROR_IS_FATAL ANY
    )

    string_replace_between(msvcInclude "${msvcEnv}" "INCLUDE_START" "INCLUDE_STOP" BETWEEN_ONLY)
    string(STRIP "${msvcInclude}" msvcInclude)
    string(REGEX REPLACE "[\r]" "" msvcInclude "${msvcInclude}")
    string(REGEX REPLACE "[\n]" "" msvcInclude "${msvcInclude}")

    string_replace_between(msvcLibPath "${msvcEnv}" "LIBPATH_START" "LIBPATH_STOP" BETWEEN_ONLY)
    string(STRIP "${msvcLibPath}" msvcLibPath)
    string(REGEX REPLACE "[\r]" "" msvcLibPath "${msvcLibPath}")
    string(REGEX REPLACE "[\n]" "" msvcLibPath "${msvcLibPath}")

    string_replace_between(msvcLib "${msvcEnv}" "LIB_START" "LIB_STOP" BETWEEN_ONLY)
    string(STRIP "${msvcLib}" msvcLib)
    string(REGEX REPLACE "[\r]" "" msvcLib "${msvcLib}")
    string(REGEX REPLACE "[\n]" "" msvcLib "${msvcLib}")

    string_replace_between(msvcClPath "${msvcEnv}" "CLPATH_START" "CLPATH_STOP" BETWEEN_ONLY)
    string(STRIP "${msvcClPath}" msvcClPath)
    string(REGEX REPLACE "[\r]" "" msvcClPath "${msvcClPath}")
    string(REGEX REPLACE "[\n]" ";" msvcClPath "${msvcClPath}")
    list(GET msvcClPath 0 msvcClPath)
    get_filename_component(msvcClPath "${msvcClPath}" DIRECTORY)
    cmake_path(CONVERT "${msvcClPath}" TO_NATIVE_PATH_LIST msvcClPath NORMALIZE)

    string_replace_between(msvcRcPath "${msvcEnv}" "RCPATH_START" "RCPATH_STOP" BETWEEN_ONLY)
    string(STRIP "${msvcRcPath}" msvcRcPath)
    string(REGEX REPLACE "[\r]" "" msvcRcPath "${msvcRcPath}")
    string(REGEX REPLACE "[\n]" ";" msvcRcPath "${msvcRcPath}")
    list(GET msvcRcPath 0 msvcRcPath)
    get_filename_component(msvcRcPath "${msvcRcPath}" DIRECTORY)
    cmake_path(CONVERT "${msvcRcPath}" TO_NATIVE_PATH_LIST msvcRcPath NORMALIZE)

    set("${prefix}_MSVC_INCLUDE" "${msvcInclude}" PARENT_SCOPE)
    set("${prefix}_MSVC_LIBPATH" "${msvcLibPath}" PARENT_SCOPE)
    set("${prefix}_MSVC_LIB" "${msvcLib}" PARENT_SCOPE)
    set("${prefix}_MSVC_CL_PATH" "${msvcClPath}" PARENT_SCOPE)
    set("${prefix}_MSVC_RC_PATH" "${msvcRcPath}" PARENT_SCOPE)
endfunction()

function(set_msvc_toolchain_content var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    set(options)
    set(oneValueKeywords
        "COMMAND"
        "VERSION"
        "PROPERTY"
        "HOST"
        "TARGET"
        "PATH"
        "PROCESSOR"
        "OS"
    )
    set(multiValueKeywords
        "PRODUCTS"
    )
    cmake_parse_arguments("set_msvc_toolchain_content" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    set(command "${set_msvc_toolchain_content_COMMAND}")
    set(version "${set_msvc_toolchain_content_VERSION}")
    set(property "${set_msvc_toolchain_content_PROPERTY}")
    set(host "${set_msvc_toolchain_content_HOST}")
    set(target "${set_msvc_toolchain_content_TARGET}")
    set(path "${set_msvc_toolchain_content_PATH}")
    set(processor "${set_msvc_toolchain_content_PROCESSOR}")
    set(os "${set_msvc_toolchain_content_OS}")
    set(products "${set_msvc_toolchain_content_PRODUCTS}")

    set_msvc_env("func"
        COMMAND "${command}"
        VERSION "${version}"
        PROPERTY "${property}"
        HOST "${host}"
        TARGET "${target}"
        PATH "${path}"
        PRODUCTS "${products}"
    )

    cmake_path(CONVERT "${func_MSVC_INCLUDE}" TO_CMAKE_PATH_LIST func_include NORMALIZE)
    list(FILTER func_include EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES func_include)
    set(func_include_cmake "${func_include}")
    cmake_path(CONVERT "${func_include}" TO_NATIVE_PATH_LIST func_include NORMALIZE)

    cmake_path(CONVERT "${func_MSVC_LIBPATH}" TO_CMAKE_PATH_LIST func_libpath NORMALIZE)
    list(FILTER func_libpath EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES func_libpath)
    set(func_libpath_cmake "${func_libpath}")
    cmake_path(CONVERT "${func_libpath}" TO_NATIVE_PATH_LIST func_libpath NORMALIZE)

    cmake_path(CONVERT "${func_MSVC_LIB}" TO_CMAKE_PATH_LIST func_lib NORMALIZE)
    list(FILTER func_lib EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES func_lib)
    cmake_path(CONVERT "${func_lib}" TO_NATIVE_PATH_LIST func_lib NORMALIZE)

    cmake_path(CONVERT "${func_MSVC_CL_PATH}" TO_CMAKE_PATH_LIST func_cl_cmake NORMALIZE)
    cmake_path(CONVERT "${func_MSVC_RC_PATH}" TO_CMAKE_PATH_LIST func_rc_cmake NORMALIZE)
    set(func_path "")
    list(PREPEND func_path "${func_rc_cmake}")
    list(PREPEND func_path "${func_cl_cmake}")
    list(APPEND func_path "\$ENV{PATH}")
    list(FILTER func_path EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES func_path)
    cmake_path(CONVERT "${func_path}" TO_NATIVE_PATH_LIST func_path NORMALIZE)

    if("Windows" STREQUAL "${os}")
        string(REPLACE "\\" "\\\\" func_include "${func_include}")

        string(REPLACE "\\" "\\\\" func_libpath "${func_libpath}")

        string(REPLACE "\\" "\\\\" func_lib "${func_lib}")

        string(REPLACE "\\" "\\\\" func_path "${func_path}")
    endif()

    cmake_path(CONVERT "${func_MSVC_INCLUDE}" TO_CMAKE_PATH_LIST func_cmake_include NORMALIZE)

    set(func_cmake_libpath "${func_MSVC_LIBPATH}" "${func_MSVC_LIB}")
    cmake_path(CONVERT "${func_cmake_libpath}" TO_CMAKE_PATH_LIST func_cmake_libpath NORMALIZE)

    string(JOIN "\n" content
        "set(CMAKE_SYSTEM_PROCESSOR \"${processor}\")"
        "set(CMAKE_SYSTEM_NAME \"${os}\")"
        ""
        "set(MSVC_CL_PATH \"${func_cl_cmake}\")"
        "set(MSVC_RC_PATH \"${func_rc_cmake}\")"
        ""
        "set(CMAKE_C_COMPILER   \"\${MSVC_CL_PATH}/cl.exe\")"
        "set(CMAKE_CXX_COMPILER \"\${MSVC_CL_PATH}/cl.exe\")"
        "set(CMAKE_AR           \"\${MSVC_CL_PATH}/lib.exe\")"
        "set(CMAKE_LINKER       \"\${MSVC_CL_PATH}/link.exe\")"
        "set(CMAKE_RC_COMPILER  \"\${MSVC_RC_PATH}/rc.exe\")"
        "set(CMAKE_MT           \"\${MSVC_RC_PATH}/mt.exe\")"
        ""
        "set(ENV{INCLUDE} \"${func_include}\")"
        "set(ENV{LIBPATH} \"${func_libpath}\")"
        "set(ENV{LIB} \"${func_lib}\")"
        "set(ENV{PATH} \"${func_path}\")"
        ""
        "set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES \"${func_include_cmake}\")"
        "set(CMAKE_C_STANDARD_LINK_DIRECTORIES \"${func_libpath_cmake}\")"
        ""
        "set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES \"${func_cmake_include}\")"
        "set(CMAKE_CXX_STANDARD_LINK_DIRECTORIES \"${func_cmake_libpath}\")"
        ""
        "set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)"
        ""
        "link_directories(\"\${CMAKE_CXX_STANDARD_LINK_DIRECTORIES}\") # remove when CMAKE_CXX_STANDARD_LINK_DIRECTORIES is supported"
        ""
    )
    set("${var}" "${content}" PARENT_SCOPE)
endfunction()

function(set_gnu_toolchain_content var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    set(options)
    set(oneValueKeywords
        "PATH"
        "PROCESSOR"
        "OS"
    )
    set(multiValueKeywords)
    cmake_parse_arguments("set_gnu_toolchain_content" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    set(path "${set_gnu_toolchain_content_PATH}")
    set(processor "${set_gnu_toolchain_content_PROCESSOR}")
    set(os "${set_gnu_toolchain_content_OS}")

    cmake_path(CONVERT "${path}" TO_CMAKE_PATH_LIST path NORMALIZE)

    get_filename_component(compilerDir "${path}" DIRECTORY)

    set(envPath "${compilerDir}")
    list(APPEND envPath "\$ENV{PATH}")
    list(FILTER envPath EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES envPath)
    cmake_path(CONVERT "${envPath}" TO_NATIVE_PATH_LIST envPathNative NORMALIZE)

    if("Windows" STREQUAL "${os}")
        string(REPLACE "\\" "\\\\" envPathNative "${envPathNative}")
    endif()

    string(JOIN "\n" content
        "set(CMAKE_SYSTEM_PROCESSOR \"${processor}\")"
        "set(CMAKE_SYSTEM_NAME \"${os}\")"
        ""
        "set(COMPILER_PATH \"${compilerDir}\")"
        ""
        "set(CMAKE_C_COMPILER   \"\${COMPILER_PATH}/gcc.exe\")"
        "set(CMAKE_CXX_COMPILER \"\${COMPILER_PATH}/g++.exe\")"
        "set(CMAKE_AR           \"\${COMPILER_PATH}/ar.exe\")"
        "set(CMAKE_LINKER       \"\${COMPILER_PATH}/ld.exe\")"
        "set(CMAKE_RC_COMPILER  \"\${COMPILER_PATH}/windres.exe\")"
        ""
        "set(ENV{PATH} \"${envPathNative}\")"
        ""
        "set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)"
        ""
    )
    set("${var}" "${content}" PARENT_SCOPE)
endfunction()

function(set_clang_toolchain_content var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    set(options)
    set(oneValueKeywords
        "PATH"
        "PROCESSOR"
        "OS"
        "TARGET"
    )
    set(multiValueKeywords)
    cmake_parse_arguments("set_clang_toolchain_content" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    set(path "${set_clang_toolchain_content_PATH}")
    set(processor "${set_clang_toolchain_content_PROCESSOR}")
    set(os "${set_clang_toolchain_content_OS}")
    set(target "${set_clang_toolchain_content_TARGET}")

    cmake_path(CONVERT "${path}" TO_CMAKE_PATH_LIST path NORMALIZE)

    get_filename_component(compilerDir "${path}" DIRECTORY)

    set(envPath "${compilerDir}")
    list(APPEND envPath "\$ENV{PATH}")
    list(FILTER envPath EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES envPath)
    cmake_path(CONVERT "${envPath}" TO_NATIVE_PATH_LIST envPathNative NORMALIZE)

    if("Windows" STREQUAL "${os}")
        string(REPLACE "\\" "\\\\" envPathNative "${envPathNative}")
    endif()

    string(JOIN "\n" content
        "set(CMAKE_SYSTEM_PROCESSOR \"${processor}\")"
        "set(CMAKE_SYSTEM_NAME \"${os}\")"
        ""
        "set(COMPILER_PATH \"${compilerDir}\")"
        ""
        "set(CMAKE_C_COMPILER          \"\${COMPILER_PATH}/clang.exe\")"
        "set(CMAKE_C_COMPILER_TARGET   \"${target}\")"
        "set(CMAKE_CXX_COMPILER        \"\${COMPILER_PATH}/clang++.exe\")"
        "set(CMAKE_CXX_COMPILER_TARGET \"${target}\")"
        ""
        "set(ENV{PATH} \"${envPathNative}\")"
        ""
        "set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)"
        "set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)"
        ""
    )
    set("${var}" "${content}" PARENT_SCOPE)
endfunction()

function(set_conan_architecture var cmakeSystemProcessor)
    foreach(i var cmakeSystemProcessor)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    if("x64" STREQUAL "${cmakeSystemProcessor}" OR "AMD64" STREQUAL "${cmakeSystemProcessor}" OR "IA64" STREQUAL "${cmakeSystemProcessor}")
        set(value "x86_64")
    elseif("x86" STREQUAL "${cmakeSystemProcessor}")
        set(value "x86")
    else()
        message(FATAL_ERROR "Unsupported 'cmakeSystemProcessor': '${cmakeSystemProcessor}'")
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_conan_compiler var cmakeCxxCompilerId)
    foreach(i var cmakeCxxCompilerId)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    if("MSVC" STREQUAL "${cmakeCxxCompilerId}")
        set(value "Visual Studio")
    elseif("GNU" STREQUAL "${cmakeCxxCompilerId}")
        set(value "gcc")
    elseif("Clang" STREQUAL "${cmakeCxxCompilerId}")
        set(value "clang")
    else()
        message(FATAL_ERROR "Unsupported 'cmakeCxxCompilerId': '${cmakeCxxCompilerId}'")
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_conan_compiler_version var cmakeCxxCompilerId cmakeCxxCompilerVersion)
    foreach(i var cmakeCxxCompilerId cmakeCxxCompilerVersion)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    if("MSVC" STREQUAL "${cmakeCxxCompilerId}")
        if("${cmakeCxxCompilerVersion}" VERSION_GREATER_EQUAL "19.30" AND "${cmakeCxxCompilerVersion}" VERSION_LESS "19.40")
            set(value "17")
        elseif("${cmakeCxxCompilerVersion}" VERSION_GREATER_EQUAL "19.20" AND "${cmakeCxxCompilerVersion}" VERSION_LESS "19.30")
            set(value "16")
        elseif("${cmakeCxxCompilerVersion}" VERSION_GREATER_EQUAL "19.10" AND "${cmakeCxxCompilerVersion}" VERSION_LESS "19.20")
            set(value "15")
        else()
            message(FATAL_ERROR "Unsupported 'cmakeCxxCompilerVersion': '${cmakeCxxCompilerVersion}'")
        endif()
    elseif("GNU" STREQUAL "${cmakeCxxCompilerId}")
        set(value "${cmakeCxxCompilerVersion}")
    elseif("Clang" STREQUAL "${cmakeCxxCompilerId}")
        set(value "${cmakeCxxCompilerVersion}")
    else()
        message(FATAL_ERROR "Unsupported 'cmakeCxxCompilerId': '${cmakeCxxCompilerId}'")
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_conan_compiler_runtime var cmakeMsvcRuntimeLibrary)
    foreach(i var cmakeMsvcRuntimeLibrary)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    if("${cmakeMsvcRuntimeLibrary}" STREQUAL "MultiThreaded")
        set(value "MT")
    elseif("${cmakeMsvcRuntimeLibrary}" STREQUAL "MultiThreadedDLL")
        set(value "MD")
    elseif("${cmakeMsvcRuntimeLibrary}" STREQUAL "MultiThreadedDebug")
        set(value "MTd")
    elseif("${cmakeMsvcRuntimeLibrary}" STREQUAL "MultiThreadedDebugDLL")
        set(value "MDd")
    else()
        message(FATAL_ERROR "Unsupported 'cmakeMsvcRuntimeLibrary': '${cmakeMsvcRuntimeLibrary}'")
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_conan_settings var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    # all settings
    if(NOT "" STREQUAL "${ARGN}")
        foreach(arg ${ARGN})
            list(APPEND value "--settings" "${arg}")
        endforeach()
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_python_boolean var cmakeBoolean)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    if("${cmakeBoolean}")
        set(value "True")
    else()
        set(value "False")
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_conan_options var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    # all options
    if(NOT "" STREQUAL "${ARGN}")
        foreach(arg ${ARGN})
            if(NOT "" STREQUAL "${arg}" AND NOT "${arg}" IN_LIST "value")
                list(APPEND value "--options" "${arg}")
            endif()
        endforeach()
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_not_found_package_names var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    if(NOT "" STREQUAL "${ARGN}")
        foreach(arg ${ARGN})
            if(NOT "${${arg}_FOUND}")
                list(APPEND value "${arg}")
            endif()
        endforeach()
    endif()

    set("${var}" "${value}" PARENT_SCOPE)
endfunction()

function(set_target_names var dir)
    foreach(i var dir)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    get_property(subdirectories DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        set_target_names(subTargets "${subdir}")
    endforeach()
    if(NOT "${subTargets}" STREQUAL "")
        list(APPEND targets "${subTargets}")
    endif()
    get_property(currentTargets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    if(NOT "${currentTargets}" STREQUAL "")
        list(APPEND targets "${currentTargets}")
    endif()
    list(APPEND targets
        "all"
        "help"
        "clean"
        "test"
        "install"
        "package"
        "package_source"
    )
    set(${var} "${targets}" PARENT_SCOPE)
endfunction()

function(generate_interface_only_files var)
    foreach(i var)
        if("" STREQUAL "${${i}}")
            message(FATAL_ERROR "Empty value not supported for '${i}'.")
        endif()
    endforeach()

    set(options)
    set(oneValueKeywords
        "SRC_DIRECTORY"
        "SRC_BASE_DIRECTORY"
        "DST_BASE_DIRECTORY"
        "HEADER_FILES_EXPRESSION"
        "SOURCE_FILES_EXPRESSION"
    )
    set(multiValueKeywords)
    cmake_parse_arguments("generate_interface_only_files" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    set(srcDirectory "${generate_interface_only_files_SRC_DIRECTORY}")
    set(srcBaseDirectory "${generate_interface_only_files_SRC_BASE_DIRECTORY}")
    set(dstBaseDirectory "${generate_interface_only_files_DST_BASE_DIRECTORY}")
    set(headerFilesExpression "${generate_interface_only_files_HEADER_FILES_EXPRESSION}")
    set(sourceFilesExpression "${generate_interface_only_files_SOURCE_FILES_EXPRESSION}")

    if(
        "${srcDirectory}" STREQUAL ""
        OR "${srcBaseDirectory}" STREQUAL ""
        OR "${dstBaseDirectory}" STREQUAL ""
        OR "${headerFilesExpression}" STREQUAL ""
        OR "${sourceFilesExpression}" STREQUAL ""
    )
        string(JOIN " " message
            "Invalid set of arguments:"
            "srcDirectory: '${srcDirectory}'"
            "srcBaseDirectory: '${srcBaseDirectory}'"
            "dstBaseDirectory: '${dstBaseDirectory}'"
            "headerFilesExpression: '${headerFilesExpression}'"
            "sourceFilesExpression: '${sourceFilesExpression}'"
        )
        message(FATAL_ERROR "${message}")
    endif()

    cmake_path(RELATIVE_PATH dstBaseDirectory BASE_DIRECTORY "${srcDirectory}" OUTPUT_VARIABLE dstRelativePath)

    file(GLOB_RECURSE headerFiles "${srcBaseDirectory}/${headerFilesExpression}")
    file(GLOB_RECURSE sourceFiles "${srcBaseDirectory}/${sourceFilesExpression}")

    list(SORT headerFiles)
    list(SORT sourceFiles)
    set(singleHeaderFiles "")
    set(pairedHeaderFiles "")
    set(pairedSourceFiles "")
    set(result "")
    foreach(header ${headerFiles})
        set(singleHeader "TRUE")
        get_filename_component(headerFileDir "${header}" DIRECTORY)
        get_filename_component(headerFileName "${header}" NAME_WE)
        foreach(source ${sourceFiles})
            get_filename_component(sourceFileDir "${source}" DIRECTORY)
            get_filename_component(sourceFileName "${source}" NAME_WE)
            if("${headerFileDir}" STREQUAL "${sourceFileDir}" AND "${headerFileName}" STREQUAL "${sourceFileName}")
                set(singleHeader "FALSE")
                if(NOT "${header}" IN_LIST pairedHeaderFiles)
                    list(APPEND pairedHeaderFiles "${header}")
                    list(APPEND pairedSourceFiles "${source}")
                endif()
            endif()
        endforeach()
        if("${singleHeader}" AND NOT "${header}" IN_LIST singleHeaderFiles)
            list(APPEND singleHeaderFiles "${header}")
        endif()
    endforeach()
    foreach(header ${singleHeaderFiles})
        cmake_path(RELATIVE_PATH header BASE_DIRECTORY "${srcBaseDirectory}" OUTPUT_VARIABLE newHeaderRelativePath)
        get_filename_component(newHeaderRelativeDir "${newHeaderRelativePath}" DIRECTORY)
        set(newHeaderDir "${dstBaseDirectory}/${newHeaderRelativeDir}")

        file(COPY "${header}" DESTINATION "${newHeaderDir}")

        if(NOT "${dstRelativePath}/${newHeaderRelativePath}" IN_LIST result)
            list(APPEND result "${dstRelativePath}/${newHeaderRelativePath}")
        endif()
    endforeach()
    foreach(header source IN ZIP_LISTS pairedHeaderFiles pairedSourceFiles)
        file(READ "${header}" headerContent)
        file(READ "${source}" sourceContent)

        string_replace_between(sourcePart1 "${sourceContent}" "\n" "namespace " BETWEEN_ONLY)
        substring_from(sourcePart2 "${sourceContent}" "namespace ")
        string(PREPEND sourcePart2 "namespace ")
        string(STRIP "${headerContent}" headerContent)
        string(STRIP "${sourcePart1}" sourcePart1)
        string(STRIP "${sourcePart2}" sourcePart2)
        set(newHeaderContent "")
        string(APPEND newHeaderContent "${headerContent}" "\n" "\n")
        if(NOT "" STREQUAL "${sourcePart1}")
            string(APPEND newHeaderContent "${sourcePart1}" "\n" "\n")
        endif()
        string(APPEND newHeaderContent "${sourcePart2}" "\n")
        cmake_path(RELATIVE_PATH header BASE_DIRECTORY "${srcBaseDirectory}" OUTPUT_VARIABLE newHeaderPath)
        set(newHeaderPath "${dstBaseDirectory}/${newHeaderPath}")
        cmake_path(RELATIVE_PATH newHeaderPath BASE_DIRECTORY "${srcDirectory}" OUTPUT_VARIABLE newHeaderRelativePath)

        file(WRITE "${newHeaderPath}" "${newHeaderContent}")

        if(NOT "${newHeaderRelativePath}" IN_LIST result)
            list(APPEND result "${newHeaderRelativePath}")
        endif()
    endforeach()

    set("${var}" "${result}" PARENT_SCOPE)
endfunction()

function(execute_script)
    set(options
        "help"
        "toolchain"
    )
    set(oneValueKeywords
        "processor"
        "os"
        "compiler"
        "version"
        "host"
        "target"
        "path"
        "file"
    )
    set(multiValueKeywords
        "products"
    )
    cmake_parse_arguments("execute_script" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    if(
        "${execute_script_toolchain}"
        AND "${execute_script_compiler}" STREQUAL "msvc"
        AND (NOT "${execute_script_path}" STREQUAL "" AND EXISTS "${execute_script_path}" AND NOT IS_DIRECTORY "${execute_script_path}")
    )
        set_msvc_toolchain_content(toolchain_content
            PROCESSOR "${execute_script_processor}"
            OS "${execute_script_os}"
            PATH "${execute_script_path}"
        )
        file(WRITE "${execute_script_file}" "${toolchain_content}")
        return()
    elseif(
        "${execute_script_toolchain}"
        AND "${execute_script_compiler}" STREQUAL "msvc"
        AND "${execute_script_path}" STREQUAL ""
        AND NOT "${execute_script_version}" STREQUAL ""
        AND NOT "${execute_script_host}" STREQUAL ""
        AND NOT "${execute_script_target}" STREQUAL ""
    )
        set_msvc_toolchain_content(toolchain_content
            PROCESSOR "${execute_script_processor}"
            OS "${execute_script_os}"
            VERSION "${execute_script_version}"
            HOST "${execute_script_host}"
            TARGET "${execute_script_target}"
            PRODUCTS "${execute_script_products}"
        )
        file(WRITE "${execute_script_file}" "${toolchain_content}")
        return()
    elseif(
        "${execute_script_toolchain}"
        AND "${execute_script_compiler}" STREQUAL "gnu"
        AND (NOT "${execute_script_path}" STREQUAL "" AND EXISTS "${execute_script_path}" AND NOT IS_DIRECTORY "${execute_script_path}")
    )
        set_gnu_toolchain_content(toolchain_content
            PROCESSOR "${execute_script_processor}"
            OS "${execute_script_os}"
            PATH "${execute_script_path}"
        )
        file(WRITE "${execute_script_file}" "${toolchain_content}")
        return()
    elseif(
        "${execute_script_toolchain}"
        AND "${execute_script_compiler}" STREQUAL "clang"
        AND (NOT "${execute_script_path}" STREQUAL "" AND EXISTS "${execute_script_path}" AND NOT IS_DIRECTORY "${execute_script_path}")
        AND NOT "${execute_script_target}" STREQUAL ""
    )
        set_clang_toolchain_content(toolchain_content
            PROCESSOR "${execute_script_processor}"
            OS "${execute_script_os}"
            PATH "${execute_script_path}"
            TARGET "${execute_script_target}"
        )
        file(WRITE "${execute_script_file}" "${toolchain_content}")
        return()
    elseif("${execute_script_help}")
        get_filename_component(execute_script_current_file_name "${CMAKE_CURRENT_LIST_FILE}" NAME)

        string(JOIN " " execute_script_usage_1 "cmake" "-P" "${execute_script_current_file_name}" "help")
        string(JOIN " " execute_script_usage_2 "cmake" "-P" "${execute_script_current_file_name}" "toolchain"
            "processor" "'<value>'"
            "os" "'Windows'"
            "compiler" "'msvc'"
            "version" "'16 (i.e. Visual Studio 2019)|17 (i.e. Visual Studio 2022)'"
            "host" "'x86|x64'"
            "target" "'x86|x64'"
            "["
                "products"
                    "Microsoft.VisualStudio.Product.Enterprise"
                    "|"
                    "Microsoft.VisualStudio.Product.Professional"
                    "|"
                    "Microsoft.VisualStudio.Product.Community"
                    "|"
                    "Microsoft.VisualStudio.Product.BuildTools"
            "]"
            "file" "'toolchain.cmake|build/toolchain.cmake'"
        )
        string(JOIN " " execute_script_usage_3 "cmake" "-P" "${execute_script_current_file_name}" "toolchain"
            "processor" "'<value>'"
            "os" "'Windows'"
            "compiler" "'msvc|gnu'"
            "path" "'path/to/cl.exe|path/to/gcc.exe'"
            "file" "'toolchain.cmake|build/toolchain.cmake'"
        )
        string(JOIN " " execute_script_usage_4 "cmake" "-P" "${execute_script_current_file_name}" "toolchain"
            "processor" "'<value>'"
            "os" "'Windows'"
            "compiler" "'clang'"
            "path" "'path/to/clang.exe'"
            "target" "'x86_64-pc-windows-msvc'"
            "file" "'toolchain.cmake|build/toolchain.cmake'"
        )

        string(JOIN "\n" execute_script_content
            "Usage:"
            "  ${execute_script_usage_1}"
            "  ${execute_script_usage_2}"
            "  ${execute_script_usage_3}"
            "  ${execute_script_usage_4}"
        )
        message("${execute_script_content}")
    endif()
endfunction()

math(EXPR MAX "${CMAKE_ARGC} - 1")
foreach(i RANGE "${MAX}")
    list(APPEND ARGS "${CMAKE_ARGV${i}}")
endforeach()
execute_script("${ARGS}")

cmake_policy(POP)
cmake_policy(POP)
cmake_policy(POP)
cmake_policy(POP)
