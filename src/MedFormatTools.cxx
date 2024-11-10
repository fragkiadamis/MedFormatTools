#include <iostream>
#include <string>

#include <argparse/argparse.hpp>
#include <itkVersion.h>

#include "../include/config.h.in"

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("MedFormatTools", MEDFORMATTOOLS_VERSION);

    program.add_argument("-i", "--input")
        .help("Specify the input file name")
        .required();

    program.add_argument("-o", "--output")
        .help("Specify the output file name")
        .required();

    program.add_argument("--verbose")
        .help("Increase output verbosity")
        .default_value(false)
        .implicit_value(true);

    bool verbosity = false;

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    verbosity = program.get<bool>("--verbose");
    std::string outputPath = program.get<std::string>("--input");
    std::string inputPath = program.get<std::string>("--output");

    if (verbosity) {
        std::cout << "Verbosity enabled" << std::endl;
        std::cout << "MedFormatTools version " << MEDFORMATTOOLS_VERSION << std::endl;
        std::cout << "ITK Version: " << ITK_VERSION_STRING << std::endl;
        std::cout << "Input Path: " << inputPath << std::endl;
        std::cout << "Output Path: " << outputPath << std::endl;
    }

    return 0;
}
