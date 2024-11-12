#ifndef TYPES_H
#define TYPES_H

#include <string>

// Forward declaration of the enums
enum class Format { UNKNOWN, DICOM, NIFTI, PNG };
enum class Modality { UNKNOWN, CT, MR, CXR };

// Function declarations (no function bodies here)
namespace Types {
    Format parseFormat(const std::string& value);
    Modality parseModality(const std::string& value);
    std::string toString(Format format);
    std::string toString(Modality modality);
}

#endif // TYPES_H
