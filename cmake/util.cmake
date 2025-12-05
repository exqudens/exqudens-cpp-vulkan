cmake_minimum_required(VERSION "3.25" FATAL_ERROR)

if(NOT "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "" AND "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    cmake_policy(PUSH)
    cmake_policy(SET CMP0007 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0010 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0012 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0054 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0057 NEW)
endif()

function(vscode)
    set(options)
    set(oneValueKeywords
        "SOURCE_DIR"
        "BINARY_DIR"

        "LAUNCH_GENERATE"
        "LAUNCH_FILE_OVERWRITE"
        "LAUNCH_TEMPLATE_FILE"
        "LAUNCH_FILE"
        "LAUNCH_TESTS_FILE"
        "LAUNCH_DEFAULT_TEST"
    )
    set(multiValueKeywords
        "LAUNCH_ENV_PATH"
        "LAUNCH_TESTS"
    )

    foreach(v IN LISTS "options" "oneValueKeywords" "multiValueKeywords")
        unset("_${v}")
    endforeach()

    cmake_parse_arguments("" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    if(NOT "${_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "UNPARSED_ARGUMENTS: '${_UNPARSED_ARGUMENTS}'")
    endif()

    if("${_SOURCE_DIR}" STREQUAL "")
        cmake_path(GET "CMAKE_CURRENT_LIST_DIR" PARENT_PATH "_SOURCE_DIR")
    endif()
    if(NOT EXISTS "${_SOURCE_DIR}")
        message(FATAL_ERROR "not exists SOURCE_DIR: '${_SOURCE_DIR}'")
    endif()

    if("${_BINARY_DIR}" STREQUAL "")
        message(FATAL_ERROR "empty BINARY_DIR: '${_BINARY_DIR}'")
    endif()

    # .vscode/launch.json
    if("${_LAUNCH_GENERATE}" STREQUAL "")
        set(_LAUNCH_GENERATE "0")
    endif()
    if("${_LAUNCH_GENERATE}" STREQUAL "")
        message(FATAL_ERROR "empty LAUNCH_GENERATE: '${_LAUNCH_GENERATE}'")
    endif()

    if("${_LAUNCH_FILE_OVERWRITE}" STREQUAL "")
        set(_LAUNCH_FILE_OVERWRITE "1")
    endif()
    if("${_LAUNCH_FILE_OVERWRITE}" STREQUAL "")
        message(FATAL_ERROR "empty LAUNCH_FILE_OVERWRITE: '${_LAUNCH_FILE_OVERWRITE}'")
    endif()

    if("${_LAUNCH_TEMPLATE_FILE}" STREQUAL "")
        set(_LAUNCH_TEMPLATE_FILE "${_SOURCE_DIR}/src/test/resources/vscode/launch.json")
    endif()
    if(NOT EXISTS "${_LAUNCH_TEMPLATE_FILE}")
        message(FATAL_ERROR "not exists LAUNCH_TEMPLATE_FILE: '${_LAUNCH_TEMPLATE_FILE}'")
    endif()

    if("${_LAUNCH_FILE}" STREQUAL "")
        set(_LAUNCH_FILE "${_SOURCE_DIR}/.vscode/launch.json")
    endif()
    if("${_LAUNCH_FILE}" STREQUAL "")
        message(FATAL_ERROR "empty LAUNCH_FILE: '${_LAUNCH_FILE}'")
    endif()

    if("${_LAUNCH_TESTS_FILE}" STREQUAL "")
        set(_LAUNCH_TESTS_FILE "${_BINARY_DIR}/test/bin/tests.json")
    endif()
    if("${_LAUNCH_TESTS_FILE}" STREQUAL "")
        message(FATAL_ERROR "empty LAUNCH_TESTS_FILE: '${_LAUNCH_TESTS_FILE}'")
    endif()

    if("${_LAUNCH_DEFAULT_TEST}" STREQUAL "")
        set(_LAUNCH_DEFAULT_TEST "-")
    endif()
    if("${_LAUNCH_DEFAULT_TEST}" STREQUAL "")
        message(FATAL_ERROR "empty LAUNCH_DEFAULT_TEST: '${_LAUNCH_DEFAULT_TEST}'")
    endif()

    if("${_LAUNCH_ENV_PATH}" STREQUAL "")
        set(_LAUNCH_ENV_PATH "-")
    endif()
    if("${_LAUNCH_ENV_PATH}" STREQUAL "")
        message(FATAL_ERROR "empty LAUNCH_ENV_PATH: '${_LAUNCH_ENV_PATH}'")
    endif()

    if("${_LAUNCH_TESTS}" STREQUAL "")
        set(_LAUNCH_TESTS "-")
    endif()
    if("${_LAUNCH_TESTS}" STREQUAL "")
        message(FATAL_ERROR "empty LAUNCH_TESTS: '${_LAUNCH_TESTS}'")
    endif()

    foreach(v IN LISTS "options" "oneValueKeywords" "multiValueKeywords")
        list(LENGTH "_${v}" l)
        if("${l}" GREATER "1")
            message(STATUS "${v} (${l}):")
            foreach(i IN LISTS "_${v}")
                message(STATUS "'${i}'")
            endforeach()
        else()
            message(STATUS "${v}: '${_${v}}'")
        endif()
    endforeach()

    # .vscode/launch.json
    if("${_LAUNCH_GENERATE}" AND (NOT EXISTS "${_LAUNCH_FILE}" OR "${_LAUNCH_FILE_OVERWRITE}"))
        if("${_LAUNCH_TESTS}" STREQUAL "-")
            set(_LAUNCH_TESTS "")
        endif()
        if(EXISTS "${_LAUNCH_TESTS_FILE}")
            file(READ "${_LAUNCH_TESTS_FILE}" json)
            string(JSON testGroups GET "${json}" "testsuites")
            string(JSON testGroupsLength LENGTH "${testGroups}")
            if("${testGroupsLength}" GREATER "0")
                math(EXPR testGroupMaxIndex "${testGroupsLength} - 1")
                foreach(testGroupIndex RANGE "0" "${testGroupMaxIndex}")
                    string(JSON testGroup GET "${testGroups}" "${testGroupIndex}")
                    string(JSON testGroupName GET "${testGroup}" "name")
                    string(JSON testCases GET "${testGroup}" "testsuite")
                    string(JSON testCasesLength LENGTH "${testCases}")
                    if("${testCasesLength}" GREATER "0")
                        math(EXPR testCaseMaxIndex "${testCasesLength} - 1")
                        foreach(testCaseIndex RANGE "0" "${testCaseMaxIndex}")
                            string(JSON testCase GET "${testCases}" "${testCaseIndex}")
                            string(JSON testCaseName GET "${testCase}" "name")
                            if(NOT "${testGroupName}.*" IN_LIST "_LAUNCH_TESTS")
                                list(APPEND _LAUNCH_TESTS "${testGroupName}.*")
                            endif()
                            if(NOT "${testGroupName}.${testCaseName}" IN_LIST "_LAUNCH_TESTS")
                                list(APPEND _LAUNCH_TESTS "${testGroupName}.${testCaseName}")
                            endif()
                        endforeach()
                    endif()
                endforeach()
            endif()
        endif()
        if("${_LAUNCH_TESTS}" STREQUAL "")
            set(_LAUNCH_TESTS "*.*")
        endif()

        if("${_LAUNCH_ENV_PATH}" STREQUAL "-")
            set(_LAUNCH_ENV_PATH "${_BINARY_DIR}")
        else()
            cmake_path(CONVERT "${_LAUNCH_ENV_PATH}" TO_NATIVE_PATH_LIST "_LAUNCH_ENV_PATH" NORMALIZE)
            string(REPLACE "\\" "\\\\" "_LAUNCH_ENV_PATH" "${_LAUNCH_ENV_PATH}")
        endif()
        if("${_LAUNCH_DEFAULT_TEST}" STREQUAL "-")
            list(GET "_LAUNCH_TESTS" "0" "_LAUNCH_DEFAULT_TEST")
        endif()
        string(JOIN "\",\n                \"" "_LAUNCH_TESTS" ${_LAUNCH_TESTS})

        configure_file("${_LAUNCH_TEMPLATE_FILE}" "${_LAUNCH_FILE}" @ONLY)

        message(STATUS "generated: '${_LAUNCH_FILE}'")
    endif()
endfunction()

function(clion)
    set(options)
    set(oneValueKeywords
        "SOURCE_DIR"
        "BINARY_DIR"

        "GTEST_GENERATE"
        "GTEST_FILES_OVERWRITE"
        "GTEST_TESTS_FILE"
        "GTEST_TEMPLATE_FILE"
        "GTEST_CONFIG_NAME"
        "GTEST_PROJECT_NAME"
        "GTEST_TARGET_NAME"
        "GTEST_OUTPUT_DIR"
    )
    set(multiValueKeywords
        "GTEST_TESTS"
        "GTEST_PROGRAM_PARAMS"
    )

    foreach(v IN LISTS "options" "oneValueKeywords" "multiValueKeywords")
        unset("_${v}")
    endforeach()

    cmake_parse_arguments("" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    if(NOT "${_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "UNPARSED_ARGUMENTS: '${_UNPARSED_ARGUMENTS}'")
    endif()

    if("${_SOURCE_DIR}" STREQUAL "")
        cmake_path(GET "CMAKE_CURRENT_LIST_DIR" PARENT_PATH "_SOURCE_DIR")
    endif()
    if(NOT EXISTS "${_SOURCE_DIR}")
        message(FATAL_ERROR "not exists SOURCE_DIR: '${_SOURCE_DIR}'")
    endif()

    if("${_BINARY_DIR}" STREQUAL "")
        message(FATAL_ERROR "empty BINARY_DIR: '${_BINARY_DIR}'")
    endif()

    if("${_GTEST_GENERATE}" STREQUAL "")
        set(_GTEST_GENERATE "0")
    endif()
    if("${_GTEST_GENERATE}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_GENERATE: '${_GTEST_GENERATE}'")
    endif()

    if("${_GTEST_FILES_OVERWRITE}" STREQUAL "")
        set(_GTEST_FILES_OVERWRITE "1")
    endif()
    if("${_GTEST_FILES_OVERWRITE}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_FILES_OVERWRITE: '${_GTEST_FILES_OVERWRITE}'")
    endif()

    if("${_GTEST_TESTS_FILE}" STREQUAL "")
        set(_GTEST_TESTS_FILE "${_BINARY_DIR}/test/bin/tests.json")
    endif()
    if("${_GTEST_TESTS_FILE}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_TESTS_FILE: '${_GTEST_TESTS_FILE}'")
    endif()

    if("${_GTEST_TEMPLATE_FILE}" STREQUAL "")
        set(_GTEST_TEMPLATE_FILE "${_SOURCE_DIR}/src/test/resources/clion/gtest/filter-run.xml")
    endif()
    if(NOT EXISTS "${_GTEST_TEMPLATE_FILE}")
        message(FATAL_ERROR "not exists GTEST_TEMPLATE_FILE: '${_GTEST_TEMPLATE_FILE}'")
    endif()

    if("${_GTEST_CONFIG_NAME}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_CONFIG_NAME: '${_GTEST_CONFIG_NAME}'")
    endif()

    if("${_GTEST_PROJECT_NAME}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_PROJECT_NAME: '${_GTEST_PROJECT_NAME}'")
    endif()

    if("${_GTEST_TARGET_NAME}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_TARGET_NAME: '${_GTEST_TARGET_NAME}'")
    endif()

    if("${_GTEST_OUTPUT_DIR}" STREQUAL "")
        set(_GTEST_OUTPUT_DIR "${_SOURCE_DIR}/.clion/gtest")
    endif()
    if("${_GTEST_OUTPUT_DIR}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_OUTPUT_DIR: '${_GTEST_OUTPUT_DIR}'")
    endif()

    if("${_GTEST_TESTS}" STREQUAL "")
        set(_GTEST_TESTS "-")
    endif()
    if("${_GTEST_TESTS}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_TESTS: '${_GTEST_TESTS}'")
    endif()

    if("${_GTEST_PROGRAM_PARAMS}" STREQUAL "")
        set(_GTEST_PROGRAM_PARAMS "-")
    endif()
    if("${_GTEST_PROGRAM_PARAMS}" STREQUAL "")
        message(FATAL_ERROR "empty GTEST_PROGRAM_PARAMS: '${_GTEST_PROGRAM_PARAMS}'")
    endif()

    foreach(v IN LISTS "options" "oneValueKeywords" "multiValueKeywords")
        list(LENGTH "_${v}" l)
        if("${l}" GREATER "1")
            message(STATUS "${v} (${l}):")
            foreach(i IN LISTS "_${v}")
                message(STATUS "'${i}'")
            endforeach()
        else()
            message(STATUS "${v}: '${_${v}}'")
        endif()
    endforeach()

    # ${BINARY_DIR}/clion/google-test/${TEST_NAME}.run.xml
    if("${_GTEST_GENERATE}" AND (NOT EXISTS "${_GTEST_OUTPUT_DIR}" OR "${_GTEST_FILES_OVERWRITE}"))
        if("${_GTEST_TESTS}" STREQUAL "-")
            set(_GTEST_TESTS "")
        endif()

        if(EXISTS "${_GTEST_TESTS_FILE}")
            file(READ "${_GTEST_TESTS_FILE}" json)
            string(JSON testGroups GET "${json}" "testsuites")
            string(JSON testGroupsLength LENGTH "${testGroups}")
            if("${testGroupsLength}" GREATER "0")
                math(EXPR testGroupMaxIndex "${testGroupsLength} - 1")
                foreach(testGroupIndex RANGE "0" "${testGroupMaxIndex}")
                    string(JSON testGroup GET "${testGroups}" "${testGroupIndex}")
                    string(JSON testGroupName GET "${testGroup}" "name")
                    string(JSON testCases GET "${testGroup}" "testsuite")
                    string(JSON testCasesLength LENGTH "${testCases}")
                    if("${testCasesLength}" GREATER "0")
                        math(EXPR testCaseMaxIndex "${testCasesLength} - 1")
                        foreach(testCaseIndex RANGE "0" "${testCaseMaxIndex}")
                            string(JSON testCase GET "${testCases}" "${testCaseIndex}")
                            string(JSON testCaseName GET "${testCase}" "name")
                            if(NOT "${testGroupName}.*" IN_LIST "_GTEST_TESTS")
                                list(APPEND _GTEST_TESTS "${testGroupName}.*")
                            endif()
                            if(NOT "${testGroupName}.${testCaseName}" IN_LIST "_GTEST_TESTS")
                                list(APPEND _GTEST_TESTS "${testGroupName}.${testCaseName}")
                            endif()
                        endforeach()
                    endif()
                endforeach()
            endif()
        endif()

        cmake_path(RELATIVE_PATH "_BINARY_DIR" BASE_DIRECTORY "${_SOURCE_DIR}" OUTPUT_VARIABLE "_BINARY_DIR_REL")
        set(_CONFIG_NAME "${_GTEST_CONFIG_NAME}")
        set(_PROJECT_NAME "${_GTEST_PROJECT_NAME}")
        set(_TARGET_NAME "${_GTEST_TARGET_NAME}")

        if("${_GTEST_PROGRAM_PARAMS}" STREQUAL "-")
            set(_REDIRECT_INPUT "REDIRECT_INPUT=\"false\"")
        else()
            string(JOIN " " _PROGRAM_PARAMS ${_GTEST_PROGRAM_PARAMS})
            set(_PROGRAM_PARAMS "PROGRAM_PARAMS=\"${_PROGRAM_PARAMS}\"")
            set(_REDIRECT_INPUT "${_PROGRAM_PARAMS}\n        REDIRECT_INPUT=\"false\"")
        endif()

        file(MAKE_DIRECTORY "${_GTEST_OUTPUT_DIR}")
        foreach(v IN LISTS "_GTEST_TESTS")
            string(REPLACE "." ";" vSplit "${v}")
            list(GET "vSplit" "0" _TEST_CLASS)
            list(GET "vSplit" "1" _TEST_METHOD)
            set(_TEST_PATTERN "${_TEST_CLASS}.${_TEST_METHOD}")

            if("${_TEST_METHOD}" STREQUAL "*")
                set(_TEST_NAME "${_TEST_CLASS}")
            else()
                set(_TEST_NAME "${_TEST_CLASS}.${_TEST_METHOD}")
            endif()

            string(REPLACE "*" "_ALL_" _TEST_NAME "${_TEST_NAME}")

            configure_file("${_GTEST_TEMPLATE_FILE}" "${_GTEST_OUTPUT_DIR}/${_TEST_NAME}.run.xml" @ONLY NEWLINE_STYLE UNIX)
            message(STATUS "generated: '${_GTEST_OUTPUT_DIR}/${_TEST_NAME}.run.xml'")
        endforeach()
    endif ()
endfunction()

block()
    if(NOT "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "" AND "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
        set(args)
        set(argsStarted "FALSE")
        math(EXPR argIndexMax "${CMAKE_ARGC} - 1")

        foreach(i RANGE "0" "${argIndexMax}")
            if("${argsStarted}")
                list(APPEND args "${CMAKE_ARGV${i}}")
            elseif(NOT "${argsStarted}" AND "${CMAKE_ARGV${i}}" STREQUAL "--")
                set(argsStarted "TRUE")
            endif()
        endforeach()

        set(noEscapeBackslashOption "--no-escape-backslash")

        if("${args}" STREQUAL "")
            cmake_path(GET CMAKE_CURRENT_LIST_FILE FILENAME fileName)
            message(FATAL_ERROR "Usage: cmake -P ${fileName} -- [${noEscapeBackslashOption}] function_name args...")
        endif()

        list(POP_FRONT args firstArg)
        set(escapeBackslash "TRUE")

        if("${firstArg}" STREQUAL "${noEscapeBackslashOption}")
            set(escapeBackslash "FALSE")
            list(POP_FRONT args func)
        else()
            set(func "${firstArg}")
        endif()

        set(wrappedArgs "")

        if(NOT "${args}" STREQUAL "")
            foreach(arg IN LISTS args)
                set(wrappedArg "${arg}")
                string(FIND "${wrappedArg}" " " firstIndexOfSpace)

                if(NOT "${firstIndexOfSpace}" EQUAL "-1")
                    set(wrappedArg "\"${wrappedArg}\"")
                endif()

                if("${escapeBackslash}")
                    string(REPLACE "\\" "\\\\" wrappedArg "${wrappedArg}")
                endif()

                if("${wrappedArgs}" STREQUAL "")
                    string(APPEND wrappedArgs "${wrappedArg}")
                else()
                    string(APPEND wrappedArgs " ${wrappedArg}")
                endif()
            endforeach()
        endif()

        cmake_language(EVAL CODE "${func}(${wrappedArgs})")
    endif()
endblock()

if(NOT "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "" AND "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    cmake_policy(POP)
    cmake_policy(POP)
    cmake_policy(POP)
    cmake_policy(POP)
    cmake_policy(POP)
endif()
