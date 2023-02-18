#pragma once

#include <string>
#include <filesystem>
#include <format>
#include <iostream>

#ifndef TO_STRING
#define TO_STRING(var) std::format("{}: '{}'", #var, var) // std::string(#var)
#endif

#ifndef FUNCTION_INFO
#define FUNCTION_INFO() std::string(__FUNCTION__)
#endif

#ifndef FILE_INFO
#define FILE_INFO() std::filesystem::path(__FILE__).make_preferred().string()
#endif

#ifndef LINE_INFO
#define LINE_INFO() std::to_string(__LINE__)
#endif

#ifndef CALL_INFO
#define CALL_INFO() FUNCTION_INFO() + "(" + FILE_INFO() + ":" + LINE_INFO() + ")"
#endif

#ifndef PRINT
#define PRINT(var) std::cout << TO_STRING(var)
#endif

#ifndef PRINTLN
#define PRINTLN(var) PRINT(var) << std::endl
#endif
