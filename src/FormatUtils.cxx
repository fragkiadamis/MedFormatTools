#include "FormatUtils.h"
#include <zlib.h>

// Converts a Format enum value to a corresponding string
std::string FormatUtils::toString(Format format) {
    switch (format) {
        case Format::DICOM: return "DICOM";
        case Format::NIFTI: return "NIFTI";
        default: return "Unknown";
    }
}

// Converts a Modality enum value to a corresponding string
std::string FormatUtils::toString(Modality modality) {
    switch (modality) {
        case Modality::MR: return "MR";
        case Modality::CXR: return "XRay";
        default: return "CT";
    }
}

// Parses a string into a Format enum value (DICOM, NIFTI, or UNKNOWN)
Format FormatUtils::parseFormat(const std::string& format) {
    std::string upperFormat = format;
    std::transform(upperFormat.begin(), upperFormat.end(), upperFormat.begin(), ::toupper);

    if (upperFormat == "DICOM")
        return Format::DICOM;
    if (upperFormat == "NIFTI")
        return Format::NIFTI;
    return Format::UNKNOWN;
}

// Parses a string into a Modality enum value (CT, MR, CXR, default to CT)
Modality FormatUtils::parseModality(const std::string& modality) {
    std::string upperModality = modality;  // Create a copy of the input string
    std::transform(upperModality.begin(), upperModality.end(), upperModality.begin(), ::toupper);

    if (upperModality == "CT") return Modality::CT;
    if (upperModality == "MR") return Modality::MR;
    if (upperModality == "CXR") return Modality::CXR;
    return Modality::CT;
}

// Reads the first 128 bytes of a file into a buffer
void FormatUtils::getFileHeader(const std::filesystem::path& inputPath, char *header, int position) {
    std::ifstream file(inputPath, std::ios::binary);
    file.seekg(position, std::ios::beg);
    file.read(header, 4);
}

// Detects the format of an input file based on its extension or headercpp
Format FormatUtils::detectInputFormat(const std::filesystem::path& inputPath) {
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
