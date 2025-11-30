#pragma once

class TestApplication {

  private:

    inline static const char* LOGGER_ID = "TestApplication";

  public:

    static int run(int argc, char** argv) noexcept;

};
