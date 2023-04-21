#pragma once

#include <string>
#include <optional>
#include <utility>

class TestConfiguration {

    friend class TestApplication;

  private:

    inline static std::optional<std::string> executableFile;
    inline static std::optional<std::string> executableDir;

  public:

    static std::string getExecutableFile();

    static std::string getExecutableDir();

  private:

    static std::pair<int, std::string> setExecutableFile(const char* value);

};
