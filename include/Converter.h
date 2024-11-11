#ifndef CONVERTER_H
#define CONVERTER_H

#include <iostream>
#include <filesystem>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageSeriesWriter.h>
#include <itkNumericSeriesFileNames.h>
#include <itkGDCMImageIO.h>

#include "Types.h"

constexpr unsigned int Dimension = 3;

class Converter {
public:
    void convertToDICOM(
        const std::filesystem::path inputPath,
        const std::filesystem::path outputPath,
        const Format inputFormat,
        const Modality modality,
        const bool verbose
    );

private:
    using PixelType = signed short;
    const std::filesystem::path createOutputDirectory(const std::filesystem::path& outputPath) const;
    const std::string directionToDICOMString(const itk::Image<signed short, 3>::DirectionType &direction) const;
    void printSpatialProperties(const itk::ImageBase<3>* image) const;
};

#endif // CONVERTER_H