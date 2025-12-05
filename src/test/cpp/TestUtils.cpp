#include <cstddef>
#include <cstdint>
#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <filesystem>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <fstream>

#include "TestUtils.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

void TestUtils::init(const std::vector<std::string>& input) {
    try {
        for (size_t i = 0; i < input.size(); i++) {
            if (!data.executableFile.has_value() && i == 0) {
                std::filesystem::path path = std::filesystem::path(input.at(i));
                data.executableFile = path.generic_string();
                if (!path.parent_path().empty()) {
                    data.executableDir = path.parent_path().generic_string();
                }
            }
            if (!data.projectBinaryDir.has_value() && i != 0 && input.at(i).starts_with("--project-binary-dir=")) {
                std::vector<std::string> parts = split(input.at(i), "=");
                if (parts.size() > 1) {
                    std::filesystem::path path = std::filesystem::path(parts.at(1));
                    data.projectBinaryDir = path.generic_string();
                }
            }
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::getExecutableFile() {
    try {
        return data.executableFile.value();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::getExecutableDir() {
    try {
        return data.executableDir.value();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::getProjectBinaryDir() {
    try {
        return data.projectBinaryDir.value();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::getTestOutputDir(const std::string& testGroup, const std::string& testCase) {
    try {
        std::filesystem::path result(getProjectBinaryDir());
        result = result / "test" / "output" / testGroup / testCase;
        return result.generic_string();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::getProjectSourceDir() {
    try {
        return std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path().generic_string();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::getTestInputDir(const std::string& testGroup, const std::string& testCase) {
    try {
        std::filesystem::path result(getProjectSourceDir());
        result = result / "src" / "test" / "resources" / testGroup / testCase;
        return result.generic_string();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::vector<std::string> TestUtils::toStringVector(
        const std::exception& exception,
        std::vector<std::string> previous
) {
    previous.emplace_back(exception.what());
    try {
        std::rethrow_if_nested(exception);
        return previous;
    } catch (const std::exception& e) {
        return toStringVector(e, previous);
    } catch (...) {
        if (previous.empty()) {
            previous.emplace_back(CALL_INFO + ": Empty stack!");
        }
        return previous;
    }
}

std::vector<std::string> TestUtils::toStackTrace(const std::exception& exception) {
    try {
        std::vector<std::string> elements = toStringVector(exception);
        if (elements.size() > 1) {
            std::ranges::reverse(elements);
        }
        return elements;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toString(const std::exception& exception) {
    try {
        std::vector<std::string> stackTrace = toStackTrace(exception);
        std::ostringstream out;
        for (size_t i = 0; i < stackTrace.size(); i++) {
            out << stackTrace[i];
            if (i < stackTrace.size() - 1) {
                out << std::endl;
            }
        }
        return out.str();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::ltrim(const std::string& value) {
    try {
        std::string str = value;
        str.erase(
                str.begin(),
                std::find_if(str.begin(), str.end(), [](char c) {return !std::isspace(c);})
        );
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::rtrim(const std::string& value) {
    try {
        std::string str = value;
        str.erase(
                std::find_if(str.rbegin(), str.rend(), [](char c) {return !std::isspace(c);}).base(),
                str.end()
        );
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::trim(const std::string& value) {
    try {
        std::string str = value;
        str = ltrim(str);
        str = rtrim(str);
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toUpper(const std::string& value) {
    try {
        std::string str = value;
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toLower(const std::string& value) {
    try {
        std::string str = value;
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::replaceAll(const std::string& value, const std::string& target, const std::string& replacement) {
    try {
        std::string str = value;
        if (target == replacement) {
            return str;
        }
        std::size_t foundAt = std::string::npos;
        while ((foundAt = str.find(target, foundAt + 1)) != std::string::npos) {
            str.replace(foundAt, target.length(), replacement);
        }
        return str;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::vector<char> TestUtils::readFileBytes(const std::string& path) {
    try {
        std::filesystem::path filePath = std::filesystem::path(path).make_preferred();
        std::size_t fileSizeRaw = std::filesystem::file_size(filePath);

        if (fileSizeRaw > LLONG_MAX) {
            throw std::runtime_error(CALL_INFO + ": File size is larger than read max size: '" + std::to_string(fileSizeRaw) + " > " + std::to_string(LLONG_MAX) + "'!");
        }

        auto fileSize = (std::streamsize) fileSizeRaw;
        std::vector<char> buffer(fileSize);
        std::ifstream inputStream(filePath.string(), std::ios::binary);

        if (!inputStream.is_open()) {
            throw std::runtime_error(CALL_INFO + ": Failed to open file: '" + path + "'!");
        }

        inputStream.read(buffer.data(), fileSize);
        inputStream.close();

        return buffer;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::readFileString(const std::string& path) {
    try {
        std::vector<char> bytes = readFileBytes(path);
        std::string result(bytes.begin(), bytes.end());

        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::vector<std::string> TestUtils::split(const std::string& value, const std::string& delimiter) {
    try {
        std::vector<std::string> result = {};

        if (value.empty()) {
            return result;
        }

        if (delimiter.empty()) {
            for (size_t i = 0; i < value.size(); i++) {
                std::string token = value.substr(i, 1);
                result.emplace_back(token);
            }
        } else {
            std::string s = value;
            size_t p = 0;
            std::string token = "";
            while ((p = s.find(delimiter)) != std::string::npos) {
                token = s.substr(0, p);
                result.emplace_back(token);
                s.erase(0, p + delimiter.size());
            }
            token = s.substr(0, p);
            result.emplace_back(token);
        }

        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::join(const std::vector<std::string>& value, const std::string& delimiter, const std::string& prefix, const std::string& suffix) {
    try {
        std::string result;
        if (value.empty()) {
            return result;
        }
        result = std::accumulate(
                value.begin(),
                value.end(),
                std::string(),
                [&delimiter](const std::string& a, const std::string& b) {
                    return a + (a.empty() ? "" : delimiter) + b;
                }
        );
        result = prefix + result + suffix;
        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toString(const std::vector<std::string>& value) {
    try {
        return TestUtils::join(value, "', '", "['", "']");
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toString(const std::any& value) {
    try {
        std::ostringstream out;
        if (typeid(nullptr_t) == value.type()) {
            nullptr_t v = std::any_cast<nullptr_t>(value);
            out << '"';
            out << "nullptr";
            out << '"';
        } else if (typeid(intmax_t) == value.type()) {
            intmax_t v = std::any_cast<intmax_t>(value);
            out << '"';
            out << v;
            out << '"';
        } else if (typeid(size_t) == value.type()) {
            size_t v = std::any_cast<size_t>(value);
            out << '"';
            out << v;
            out << '"';
        } else if (typeid(long double) == value.type()) {
            long double v = std::any_cast<long double>(value);
            out << '"';
            out << v;
            out << '"';
        } else if (typeid(const char *) == value.type()) {
            const char * v = std::any_cast<const char *>(value);
            out << '"';
            out << v;
            out << '"';
        } else if (typeid(std::string) == value.type()) {
            std::string v = std::any_cast<std::string>(value);
            out << '"';
            out << v;
            out << '"';
        } else if (typeid(std::map<std::string, std::any>) == value.type()) {
            std::map<std::string, std::any> v = std::any_cast<std::map<std::string, std::any>>(value);
            out << toString(v);
        } else if (typeid(std::vector<std::any>) == value.type()) {
            std::vector<std::any> v = std::any_cast<std::vector<std::any>>(value);
            out << toString(v);
        } else {
            throw std::runtime_error(CALL_INFO + ": unsupported type: '" + value.type().name() + "'");
        }
        return out.str();
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toString(const std::map<std::string, std::any>& value) {
    try {
        std::string result = "{";
        size_t i = 0;
        for (const std::pair<std::string, std::any>& entry : value) {
            i++;
            result += "'";
            result += entry.first;
            result += "': ";
            result += toString(entry.second);
            if (i != value.size()) {
                result += ", ";
            }
        }
        result += "}";
        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

std::string TestUtils::toString(const std::vector<std::any>& value) {
    try {
        std::string result = "[";
        for (size_t i = 0; i < value.size(); i++) {
            result += toString(value.at(i));
            if (i + 1 != value.size()) {
                result += ", ";
            }
        }
        result += "]";
        return result;
    } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
}

#undef CALL_INFO
