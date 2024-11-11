#include "ArgParser.h"

// Constructor that parses the command-line arguments and validates paths
ArgParser::ArgParser(int argc, char* argv[]) {
    parseArguments(argc, argv);
}

// Get the parsed arguments
bool ArgParser::isVerbose() const { return verbose; }
const std::filesystem::path& ArgParser::getInputPath() const { return inputPath; }
const std::filesystem::path& ArgParser::getOutputPath() const { return outputPath; }
const Format& ArgParser::getFormat() const { return inputFormat; }
const Modality& ArgParser::getModality() const { return modality; }

// Print all the parsed arguments to the console
void ArgParser::printArguments() const {
    std::cout << "Input path: " << std::filesystem::current_path() / inputPath << std::endl;
    std::cout << "Output path: " << std::filesystem::current_path() / outputPath << std::endl;
    std::cout << "Target format: " << Types::toString(targetFormat) << std::endl;
    std::cout << "Target modality: " << Types::toString(modality) << std::endl;
}

// Parse command-line arguments passed to the program
void ArgParser::parseArguments(int argc, char* argv[]) {
    // Initialize the argument parser from the argparse.hpp library with the program name
    argparse::ArgumentParser program("MedFormatTools", MEDFORMATTOOLS_VERSION);

    // Add the input argument, which is required
    program.add_argument("-i", "--input")
        .help("Specify the input file name")
        .required()
        .action([this](const std::filesystem::path& value) {
            inputPath = value;
            // Check if input file exists
            if (!std::filesystem::exists(inputPath))
                throw std::runtime_error("Input file does not exist: " + inputPath.string());

            inputFormat = detectInputFormat(inputPath);
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
            targetFormat = Types::parseFormat(value);
            if (targetFormat == Format::UNKNOWN)
                throw std::runtime_error("Invalid target format. Must be either DICOM or NIFTI");
        });

    // Add modality argument with a default value of "CT"
    program.add_argument("-m", "--modality")
        .help("Specify the modality of the image (CT|MR|CXR)")
        .default_value("CT")
        .action([this](const std::string& value) {
            modality = Types::parseModality(value);
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

// Reads the first 128 bytes of a file into a buffer
void ArgParser::getFileHeader(const std::filesystem::path& inputPath, char *header, int position) {
    std::ifstream file(inputPath, std::ios::binary);
    file.seekg(position, std::ios::beg);
    file.read(header, 4);
}

// Detects the format of an input file based on its extension or headercpp
Format ArgParser::detectInputFormat(const std::filesystem::path& inputPath) {
    std::string extension = inputPath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    // Check if the data contain the DICOM magic number
    char header[4] = {0};
    getFileHeader(inputPath, header, 128);
    if (extension == ".dcm" && (strncmp(header, "DICM", 4) == 0))
        return Format::DICOM;
    
    // Check if the data contain the NIFTI magic number
    getFileHeader(inputPath, header, 344);
    if (extension == ".nii" && (strncmp(header, "n\x1e", 2) == 0))
        return Format::NIFTI;

    // Decompress the file if it is a NIFTI file compressed with gzip
    if (inputPath.string().substr(inputPath.string().size() - 7) == ".nii.gz") {
        gzFile gzFile = gzopen(inputPath.c_str(), "rb");
        const size_t bufferSize = 1024;
        char buffer[bufferSize];
        std::string decompressedContent;
        
        // Decompress the file and store the content in the decompressedContent string
        int bytesRead;
        while ((bytesRead = gzread(gzFile, buffer, bufferSize)) > 0)
            decompressedContent.append(buffer, bytesRead);
        gzclose(gzFile);

        // Check if the decompressed data contain the NIFTI magic number
        if (strncmp(decompressedContent.c_str() + 344, "n\x1e", 2) == 0 || strncmp(decompressedContent.c_str() + 344, "n+1", 3) == 0)
            return Format::NIFTI;
    }

    return Format::UNKNOWN;
}