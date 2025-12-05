#pragma once

#include <string>
#include <any>
#include <optional>
#include <vector>
#include <map>
#include <exception>

#include "test_lib_export.hpp"

class TEST_LIB_EXPORT TestUtils {

    private:

        struct Data {
            std::optional<std::string> executableFile;
            std::optional<std::string> executableDir;
            std::optional<std::string> projectBinaryDir;
        };

    private:

        inline static Data data = {};

    public:

        static void init(const std::vector<std::string>& input);

        static std::string getExecutableFile();

        static std::string getExecutableDir();

        static std::string getProjectBinaryDir();

        static std::string getTestOutputDir(const std::string& testGroup, const std::string& testCase);

        static std::string getProjectSourceDir();

        static std::string getTestInputDir(const std::string& testGroup, const std::string& testCase);

        static std::vector<std::string> toStringVector(const std::exception& exception, std::vector<std::string> previous = {});

        static std::vector<std::string> toStackTrace(const std::exception& exception);

        static std::string toString(const std::exception& exception);

        static std::string ltrim(const std::string& value);

        static std::string rtrim(const std::string& value);

        static std::string trim(const std::string& value);

        static std::string toUpper(const std::string& value);

        static std::string toLower(const std::string& value);

        static std::string replaceAll(const std::string& value, const std::string& target, const std::string& replacement);

        static std::vector<char> readFileBytes(const std::string& path);

        static std::string readFileString(const std::string& path);

        static std::vector<std::string> split(const std::string& value, const std::string& delimiter = "");

        static std::string join(const std::vector<std::string>& value, const std::string& delimiter = "", const std::string& prefix = "", const std::string& suffix = "");

        static std::string toString(const std::vector<std::string>& value);

        static std::string toString(const std::any& value);

        static std::string toString(const std::map<std::string, std::any>& value);

        static std::string toString(const std::vector<std::any>& value);

};
