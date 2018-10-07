#include <iostream>
#include <getopt.h>
#include <inttypes.h>

#include "biohash/log.hpp"

#include "util/test.hpp"

namespace {

void usage(const char* cmd)
{
    std::cerr <<
        "\nusage: " << cmd << " [options]\n"
        "\n"
        "--help              Display usage.\n"
        "--log               Log level.\n"
        "--prefix PREFIX     Only run tests whose names start with PREFIX.\n"
        "--threads NUMBER    Number of threads.\n"
        "\n";
}

struct option longopts[] = {
    {"help", no_argument, nullptr, 1},
    {"log", required_argument, nullptr, 2},
    {"prefix", required_argument, nullptr, 3},
    {"threads", required_argument, nullptr, 4},
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
                biohash::log::Level level;
                if (!biohash::log::string_to_level(optarg, level)) {
                    std::cerr << "The log level is invalid\n";
                    usage(argv[0]);
                    return 1;
                }
                config.log_level = level;
                break;
            case 3:
                config.prefix = optarg;
                break;
            case 4:
                {
                    char* endptr;
                    long threads = strtol(optarg, &endptr, 10);
                    if (threads == 0 || *endptr != '\0') {
                        std::cerr << "The threads option is invalid\n";
                        usage(argv[0]);
                        return 1;
                    }
                    config.threads = threads;
                }
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
