#include <iostream>
#include <getopt.h>

#include "util/test.hpp"

namespace {

void usage(const char* cmd)
{
    std::cerr <<
        "\nusage: " << cmd << " [options]\n"
        "\n"
        "--help              Display usage.\n"
        "--prefix PREFIX     Only run tests whose names start with PREFIX.\n"
        "\n";
}

struct option longopts[] = {
    {"help", no_argument, nullptr, 1},
    {"prefix", required_argument, nullptr, 2},
    {nullptr, 0, nullptr, 0}
};

int parse_args(int argc, char** argv, biohash::test::TestRunner::Config& config)
{
    int ch;
    while ((ch = getopt_long_only(argc, argv, "", longopts, nullptr)) != -1) {
        switch(ch) {
            case 1:
                usage(argv[0]);
                return 1;
            case 2:
                config.prefix = optarg;
                break;
            default:
                usage(argv[0]);
                return 1;
        }
    }
    return 0;
}

}

int main(int argc, char** argv)
{
    biohash::test::TestRunner::Config config;
    if (parse_args(argc, argv, config))
        return 1;

    std::vector<biohash::test::TestBase*>& tests = biohash::test::get_default_tests();

    biohash::test::TestRunner test_runner {tests, config};

    bool success = test_runner.run();
    test_runner.report();
    return success ? 0 : 1;
}
