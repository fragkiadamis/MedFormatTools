#include "ArgumentParser.h"

// Constructor that parses the command-line arguments and validates paths
ArgumentParser::ArgumentParser(int argc, char* argv[]) {
    parseArguments(argc, argv);  // Parse arguments from command line
    validatePaths();  // Ensure input and output paths are valid
}

// Return the verbosity flag indicating whether verbose output is requested
bool ArgumentParser::isVerbose() const { return verbose; }

// Return the input file path specified by the user
const std::filesystem::path& ArgumentParser::getInputPath() const { return inputPath; }

// Return the output file path specified by the user
const std::filesystem::path& ArgumentParser::getOutputPath() const { return outputPath; }

// Return the target format specified by the user (DICOM or NIFTI)
Format ArgumentParser::getTargetFormat() const { return targetFormat; }

// Return the image modality specified by the user (CT, MR, CXR)
Modality ArgumentParser::getModality() const { return modality; }

// Print all the parsed arguments to the console
void ArgumentParser::printArguments() const {
    std::cout << "Input Path: " << inputPath << std::endl;
    std::cout << "Output Path: " << outputPath << std::endl;
    std::cout << "Target Format: " << FormatUtils::toString(targetFormat) << std::endl;
    std::cout << "Modality: " << FormatUtils::toString(modality) << std::endl;
}

// Parse command-line arguments passed to the program
void ArgumentParser::parseArguments(int argc, char* argv[]) {
    // Initialize the argument parser with the program name
    argparse::ArgumentParser program("MedFormatTools", MEDFORMATTOOLS_VERSION);

    // Add the input argument, which is required
    program.add_argument("-i", "--input")
        .help("Specify the input file name")
        .required()
        .action([this](const std::filesystem::path& value) {
            inputPath = value;
            inputFormat = FormatUtils::detectInputFormat(inputPath);
            std::cout << "Detected input format: " << FormatUtils::toString(inputFormat) << std::endl;
            if (inputFormat == Format::UNKNOWN)
                throw std::runtime_error("Unknown input format. Supported formats are DICOM and NIFTI");
        });

    // Add the output argument, which is required
    program.add_argument("-o", "--output")
        .help("Specify the output path")
        .required()
        .action([this](const std::filesystem::path& value) {
            outputPath = value;
        });

    // Add the format argument to specify target output format (DICOM or NIFTI)
    program.add_argument("-f", "--format")
        .help("Specify the target output format [DICOM|NIFTI]")
        .required()
        .action([this](const std::string& value) {
            // Parse the target format and validate it
            targetFormat = FormatUtils::parseFormat(value);
            if (targetFormat == Format::UNKNOWN)
                throw std::runtime_error("Invalid target format. Must be either DICOM or NIFTI");
        });

    // Add modality argument with a default value of "CT"
    program.add_argument("-m", "--modality")
        .help("Specify the modality of the image (CT|MR|CXR)")
        .default_value("CT")
        .action([this](const std::string& value) {
            modality = FormatUtils::parseModality(value);
        });

    // Add the verbose flag to enable detailed output if specified
    program.add_argument("--verbose")
        .help("Increase output verbosity")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        throw std::runtime_error("Argument parsing failed: " + std::string(err.what()));
    }

    // Store verbosity
    verbose = program.get<bool>("--verbose");
}

// Validate the input file path and the output path
void ArgumentParser::validatePaths() {
    // Check if input file exists
    if (!std::filesystem::exists(inputPath))
        throw std::runtime_error("Input file does not exist: " + inputPath.string());

    // Validate output path for DICOM (check if directory exists, create if necessary)
    std::filesystem::path outputDir = (targetFormat == Format::DICOM) ? outputPath : outputPath.parent_path();
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    } else if (!std::filesystem::is_directory(outputDir))
        throw std::runtime_error("Output path exists but is not a directory: " + outputDir.string());
}
