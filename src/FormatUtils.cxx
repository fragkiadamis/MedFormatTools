#include "FormatUtils.h"

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

    if (upperFormat == "DICOM") return Format::DICOM;
    if (upperFormat == "NIFTI") return Format::NIFTI;
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

// The following function is commented out but would detect the format of an input file based on its extension and content.
// It would attempt to identify the format as either DICOM or NIFTI by checking file extensions and magic bytes.
 
// Format FormatUtils::detectInputFormat(const fs::path& inputPath) {
//     if (!fs::exists(inputPath)) return Format::UNKNOWN;  // If the file doesn't exist, return UNKNOWN format

//     string ext = inputPath.extension().string();  // Get the file extension (e.g., ".dcm", ".nii")
//     transform(ext.begin(), ext.end(), ext.begin(), ::tolower);  // Convert extension to lowercase for comparison

//     if (ext == ".dcm") return Format::DICOM;  // If the extension is ".dcm", return DICOM format
//     if (ext == ".nii" || ext == ".nii.gz") return Format::NIFTI;  // If the extension is ".nii" or ".nii.gz", return NIFTI format

//     ifstream file(inputPath, ios::binary);  // Open the file in binary mode to inspect its contents
//     if (file) {
//         char header[4] = {0};  // Create a buffer to hold the header data

//         file.seekg(128, ios::beg);  // Seek to position 128 (DICOM file header offset)
//         file.read(header, 4);  // Read the first 4 bytes of the DICOM header
//         if (strncmp(header, "DICM", 4) == 0) return Format::DICOM;  // If the header starts with "DICM", return DICOM format

//         file.seekg(0, ios::beg);  // Seek to the beginning of the file
//         file.read(header, 4);  // Read the first 4 bytes of the file
//         if (header[0] == 0x6E && header[1] == 0x1E) return Format::NIFTI;  // If the header matches the NIFTI magic bytes, return NIFTI format
//     }

//     return Format::UNKNOWN;  // Return UNKNOWN if the format could not be determined
// }
