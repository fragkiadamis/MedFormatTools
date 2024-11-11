#include "ArgParser.h"

// Constructor that parses the command-line arguments and validates paths
ArgParser::ArgParser(int argc, char* argv[]) {
    parseArguments(argc, argv);
}

// Parses a string into a Format enum value (DICOM, NIFTI, or UNKNOWN)
Format ArgParser::parseFormat(const std::string& format) {
    std::string upperFormat = format;
    std::transform(upperFormat.begin(), upperFormat.end(), upperFormat.begin(), ::toupper);

    if (upperFormat == "DICOM")
        return Format::DICOM;
    if (upperFormat == "NIFTI")
        return Format::NIFTI;
    return Format::UNKNOWN;
}

// Parses a string into a Modality enum value (CT, MR, CXR, default to CT)
Modality ArgParser::parseModality(const std::string& modality) {
    std::string upperModality = modality;  // Create a copy of the input string
    std::transform(upperModality.begin(), upperModality.end(), upperModality.begin(), ::toupper);

    if (upperModality == "CT") return Modality::CT;
    if (upperModality == "MR") return Modality::MR;
    if (upperModality == "CXR") return Modality::CXR;
    return Modality::CT;
}

// Get the parsed arguments
bool ArgParser::isVerbose() const { return verbose; }
const std::filesystem::path& ArgParser::getInputPath() const { return inputPath; }
const std::filesystem::path& ArgParser::getOutputPath() const { return outputPath; }
Format ArgParser::getTargetFormat() const { return targetFormat; }
Modality ArgParser::getModality() const { return modality; }

// Converts a Format enum value to a corresponding string
std::string ArgParser::toString(Format format) {
    switch (format) {
        case Format::DICOM: return "DICOM";
        case Format::NIFTI: return "NIFTI";
        default: return "Unknown";
    }
}

// Converts a Modality enum value to a corresponding string
std::string ArgParser::toString(Modality modality) {
    switch (modality) {
        case Modality::MR: return "MR";
        case Modality::CXR: return "XRay";
        default: return "CT";
    }
}

// Print all the parsed arguments to the console
void ArgParser::printArguments() const {
    std::cout << "Input Path: " << inputPath << std::endl;
    std::cout << "Output Path: " << outputPath << std::endl;
    std::cout << "Target Format: " << toString(targetFormat) << std::endl;
    std::cout << "Modality: " << toString(modality) << std::endl;
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
            std::cout << "Detected input format: " << toString(inputFormat) << std::endl;
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
            targetFormat = parseFormat(value);
            if (targetFormat == Format::UNKNOWN)
                throw std::runtime_error("Invalid target format. Must be either DICOM or NIFTI");
        });

    // Add modality argument with a default value of "CT"
    program.add_argument("-m", "--modality")
        .help("Specify the modality of the image (CT|MR|CXR)")
        .default_value("CT")
        .action([this](const std::string& value) {
            modality = parseModality(value);
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
    if (extension == ".dcm" or (strncmp(header, "DICM", 4) == 0))
        return Format::DICOM;
    
    // Check if the data contain the NIFTI magic number
    getFileHeader(inputPath, header, 344);
    if (extension == ".nii" or (strncmp(header, "n\x1e", 2) == 0))
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