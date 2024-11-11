#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include <filesystem>
#include <algorithm>

enum class Format {
    DICOM,
    NIFTI,
    UNKNOWN
};

enum class Modality {
    CT,
    MR,
    CXR,
};

class FormatUtils {
public:
    static std::string toString(Format format);
    static std::string toString(Modality modality);
    static Format parseFormat(const std::string& format);
    static Modality parseModality(const std::string& modality);
    // static Format detectInputFormat(const std::filesystem::path& inputPath);
};

#endif // FORMATUTILS_H