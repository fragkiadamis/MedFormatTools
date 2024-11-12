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
                throw std::runtime_error("Unknown input format. Supported formats are DICOM, NIFTI or PNG");
        });

    // Add the output argument, which is required
    program.add_argument("-o", "--output")
        .help("Specify the output path")
        .required()
        .action([this](const std::filesystem::path& value) {
            outputPath = value;
        });

    // Add the format argument to specify target output format (DICOM, NIFTI or PNG)
    program.add_argument("-f", "--format")
        .help("Specify the target output format [DICOM|NIFTI|PNG]")
        .required()
        .action([this](const std::string& value) {
            // Parse the target format and validate it
            targetFormat = Types::parseFormat(value);
            if (targetFormat == Format::UNKNOWN)
                throw std::runtime_error("Invalid target format. Must be either DICOM, NIFTI or PNG");
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
void ArgParser::getFileHeader(const std::filesystem::path& inputPath, char *header, unsigned short position, size_t size) {
    std::ifstream file(inputPath, std::ios::binary);
    file.seekg(position, std::ios::beg);
    file.read(header, 4);
}

bool ArgParser::verifiedFormat(const std::filesystem::path& inputPath, std::string& extension, unsigned short position, const char *formatSignature, size_t signatureSize) {
    char fileHeader[signatureSize] = {0};
    getFileHeader(inputPath, fileHeader, position, signatureSize);

    // Check if the header matches the first 4 bytes of the PNG signature
    return (strncmp(fileHeader, formatSignature, signatureSize) == 0);
}

std::string ArgParser::decompressGzFile(const std::filesystem::path& inputPath) {
    gzFile gzFile = gzopen(inputPath.c_str(), "rb");
    const size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::string decompressedContent;
    
    // Decompress the file and store the content in the decompressedContent string
    int bytesRead;
    while ((bytesRead = gzread(gzFile, buffer, bufferSize)) > 0)
        decompressedContent.append(buffer, bytesRead);
    gzclose(gzFile);

    return decompressedContent;
}

// Detects the format of an input file based on its extension or headercpp
Format ArgParser::detectInputFormat(const std::filesystem::path& inputPath) {
    std::string extension = inputPath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == ".png" && verifiedFormat(inputPath, extension, 0, "\x89PNG", 4))
        return Format::PNG;
    if (extension == ".dcm" && verifiedFormat(inputPath, extension, 128, "DICM", 4))
        return Format::DICOM;
    if (extension == ".nii" && verifiedFormat(inputPath, extension, 344, "n\x1e", 4))
        return Format::NIFTI;
    if (inputPath.string().substr(inputPath.string().size() - 7) == ".nii.gz") {
        std::string decompressedContent = decompressGzFile(inputPath);
        if (strncmp(decompressedContent.c_str() + 344, "n\x1e", 2) == 0 || strncmp(decompressedContent.c_str() + 344, "n+1", 3) == 0)
            return Format::NIFTI;
    }

    return Format::UNKNOWN;
}