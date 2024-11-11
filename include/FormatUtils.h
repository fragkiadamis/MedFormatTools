#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <cstring>

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
    static Format detectInputFormat(const std::filesystem::path& inputPath);

    private:
        static void getFileHeader(const std::filesystem::path& inputPath, char *header, int position);
};

#endif // FORMATUTILS_H