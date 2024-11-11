#include "Types.h"

// Helper functions to convert enum values to strings for display or logging
namespace Types {
    Format parseFormat(const std::string& str) {
        if (str == "DICOM") return Format::DICOM;
        if (str == "NIFTI") return Format::NIFTI;
        return Format::UNKNOWN;
    }

    Modality parseModality(const std::string& str) {
        if (str == "CT") return Modality::CT;
        if (str == "MR") return Modality::MR;
        if (str == "CXR") return Modality::CXR;
        return Modality::UNKNOWN;
    }

    std::string toString(Format format) {
        switch (format) {
            case Format::DICOM: return "DICOM";
            case Format::NIFTI: return "NIFTI";
            default: return "UNKNOWN";
        }
    }

    std::string toString(Modality modality) {
        switch (modality) {
            case Modality::CT: return "CT";
            case Modality::MR: return "MR";
            case Modality::CXR: return "CXR";
            default: return "UNKNOWN";
        }
    }
}
