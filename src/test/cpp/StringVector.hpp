#pragma once

#include <cstddef>
#include <string>
#include <optional>
#include <vector>
#include <utility>

class StringVector : public std::vector<std::string> {

    private:

        std::optional<std::vector<const char*>> optionalData = {};

    public:

        inline static StringVector fromStdObject(const std::vector<std::string>& value) {
            return StringVector(value);
        }

        using std::vector<std::string>::vector;

        StringVector(const std::vector<std::string>& other) : std::vector<std::string>(other) {}

        StringVector(std::vector<std::string>&& other) noexcept : std::vector<std::string>(std::move(other)) {}

        inline char** c_data() {
            if (!optionalData.has_value()) {
                std::vector<const char*> data = std::vector<const char*>(size() + 1);
                for (size_t i = 0; i < size(); i++) {
                    data.at(i) = at(i).c_str();
                }
                data.at(size()) = nullptr;
                optionalData = data;
            }
            return const_cast<char**>(optionalData.value().data());
        }

        inline std::vector<std::string> toStdObject() {
            return std::vector<std::string>(begin(), end());
        }

};
