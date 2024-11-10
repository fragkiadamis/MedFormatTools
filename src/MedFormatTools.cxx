#include <iostream>
#include <filesystem>

#include <argparse/argparse.hpp>
#include <itkVersion.h>

#include "../include/config.h.in"

using namespace std;
namespace fs = filesystem;

enum class Format {
    DICOM,
    NIFTI,
    NRRD,
    UNKNOWN
};

Format parseFormat(const std::string& format) {
    // Convert to uppercase for comparison
    string upperFormat = format;
    transform(upperFormat.begin(), upperFormat.end(), upperFormat.begin(), ::toupper);
    
    if (upperFormat == "DICOM") return Format::DICOM;
    if (upperFormat == "NIFTI") return Format::NIFTI;
    return Format::UNKNOWN;
}

void addArguments(argparse::ArgumentParser& program) {
    program.add_argument("-i", "--input")
        .help("Specify the input file name")
        .required();

    program.add_argument("-o", "--output")
        .help("Specify the output (file name or directory if it's DICOM)")
        .required();

    program.add_argument("-f", "--format")
        .help("Specify the target output format (DICOM|NIFTI|NRRD)")
        .required()
        .action([](const string& value) {
            Format frmt = parseFormat(value);
            if (frmt == Format::UNKNOWN) {
                throw runtime_error("Invalid format. Must be either 'DICOM', 'NIFTI', 'NRRD''");
            }
            return value;
        });
    
    program.add_argument("--verbose")
        .help("Increase output verbosity")
        .default_value(false)
        .implicit_value(true);
}

void validatePaths(const fs::path& inputPath, const fs::path& outputPath, const Format& targetFormat, bool verbosity) {
    // Validate input path
    if (!filesystem::exists(inputPath))
        throw runtime_error("Input file does not exist: " + inputPath.string());

    // Validate output path based on format
    if (targetFormat == Format::DICOM && !filesystem::exists(outputPath)) {
        filesystem::create_directories(outputPath);
        if (verbosity)
            cout << "Created output directory: " << outputPath << endl;
    } else {
        // For NIFTI or NRRD output, ensure parent directory exists
        filesystem::path outputDir = outputPath.parent_path();
        if (!outputDir.empty() && !filesystem::exists(outputDir)) {
            filesystem::create_directories(outputDir);
            if (verbosity)
                cout << "Created output directory: " << outputDir << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("MedFormatTools", MEDFORMATTOOLS_VERSION);
    addArguments(program);

    try {
        program.parse_args(argc, argv);

        const bool verbosity = program.get<bool>("--verbose");
        const filesystem::path inputPath = program.get<string>("--input");
        const filesystem::path outputPath = program.get<string>("--output");
        const Format targetFormat = parseFormat(program.get<string>("--format"));

        if (verbosity) {
            cout << "Verbosity enabled" << endl;
            cout << "MedFormatTools version " << MEDFORMATTOOLS_VERSION << endl;
            cout << "ITK Version: " << ITK_VERSION_STRING << endl;
            cout << "Input Path: " << inputPath << endl;
            cout << "Output Path: " << outputPath << endl;
        }

        validatePaths(inputPath, outputPath, targetFormat, verbosity);

    } catch (const exception& err) {
        cerr << err.what() << endl;
        cerr << program;
        exit(1);
    }

    return 0;
}
