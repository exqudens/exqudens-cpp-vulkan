#pragma once

#include "test_lib_export.hpp"

class TEST_LIB_EXPORT TestApplication {

    private:

        inline static const char* LOGGER_ID = "TestApplication";

    public:

        static int run(int argc, char** argv);

};
