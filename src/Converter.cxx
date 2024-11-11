#include "Converter.h"

void Converter::convertToDICOM(
    const std::filesystem::path inputPath,
    const std::filesystem::path outputPath,
    const Format inputFormat,
    const Modality modality,
    const bool verbose
) {
    using ImageType = itk::Image<PixelType, Dimension>;

    // Step 1: Read the input NIFTI file
    using ReaderType = itk::ImageFileReader<ImageType>;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputPath);

    try {
        reader->Update();
    } catch (itk::ExceptionObject &err) {
        throw std::runtime_error("Error reading input file: " + std::string(err.what()));
    }

    // Step 2: Get image properties: origin, spacing, direction
    ImageType::PointType volumeOrigin = reader->GetOutput()->GetOrigin();
    ImageType::SpacingType volumeSpacing = reader->GetOutput()->GetSpacing();
    ImageType::DirectionType volumeDirection = reader->GetOutput()->GetDirection();

    if (verbose)
        printSpatialProperties(reader->GetOutput());

    // Step 3: Extract the 2D slices from the 3D image
    using Image2DType = itk::Image<PixelType, 2>;
    using WriterType = itk::ImageSeriesWriter<ImageType, Image2DType>;

    // Set up the DICOM Image IO
    using ImageIOType = itk::GDCMImageIO;
    ImageIOType::Pointer dicomIO = ImageIOType::New();

    // Step 4: Setup the writer
    WriterType::Pointer writer = WriterType::New();
    writer->SetImageIO(dicomIO);

    // Step 5: Generate filenames for each DICOM slice
    using NamesGeneratorType = itk::NumericSeriesFileNames;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

    ImageType::RegionType region = reader->GetOutput()->GetLargestPossibleRegion();
    unsigned int startSlice = region.GetIndex()[2];
    unsigned int endSlice = startSlice + region.GetSize()[2] - 1;

    // Define the format for each slice's filename
    std::filesystem::path outputDir = createOutputDirectory(outputPath);
    nameGenerator->SetSeriesFormat(std::string(outputDir.string()) + "/slice%04d.dcm");
    nameGenerator->SetStartIndex(startSlice);
    nameGenerator->SetEndIndex(endSlice);
    nameGenerator->SetIncrementIndex(1);

    writer->SetFileNames(nameGenerator->GetFileNames());
    writer->SetInput(reader->GetOutput());

    if (verbose)
        std::cout << "Configuring DICOM metadata..." << std::endl;

    // Step 5: Add metadata to the DICOM header (patient information, etc.)
    using DictionaryType = itk::MetaDataDictionary;
    DictionaryType &dictionary = dicomIO->GetMetaDataDictionary();
    itk::EncapsulateMetaData<std::string>(dictionary, "0008|0060", Types::toString(modality));
    itk::EncapsulateMetaData<std::string>(dictionary, "0010|0010", "PatientName");
    itk::EncapsulateMetaData<std::string>(dictionary, "0010|0020", "PatientID");

    // Step 6: Set image orientation for DICOM (important for proper viewing)
    itk::EncapsulateMetaData(dictionary, "0020|0037", directionToDICOMString(volumeDirection));

    try {
        writer->Update();
        if (verbose)
            std::cout << "DICOM series written successfully!" << std::endl;
    } catch (itk::ExceptionObject &err) {
        throw std::runtime_error("Error writing DICOM series: " + std::string(err.what()));
    }
}

const std::string Converter::directionToDICOMString(const itk::Image<PixelType, Dimension>::DirectionType &direction) const {
    // Converts the ITK direction matrix into a DICOM-compatible string format
    std::ostringstream dicomOrientation;
    for (unsigned int i = 0; i < 3; ++i) {
        dicomOrientation << direction[i][0] << "\\" << direction[i][1] << "\\" << direction[i][2];
        if (i < 2) dicomOrientation << "\\";
    }

    return dicomOrientation.str();
}

void Converter::printSpatialProperties(const itk::ImageBase<3>* image) const {
    std::cout << "Image dimensions: " << image->GetImageDimension() << std::endl;
    std::cout << "Image size: " << image->GetLargestPossibleRegion().GetSize() << std::endl;
    std::cout << "Image origin: " << image->GetOrigin() << std::endl;
    std::cout << "Image spacing: " << image->GetSpacing() << std::endl;
    std::cout << "Image direction: [";
    for (unsigned int i = 0; i < image->GetImageDimension(); ++i) {
        std::cout << "[";
        for (unsigned int j = 0; j < image->GetImageDimension(); ++j)
            std::cout << image->GetDirection()[i][j] << (j < image->GetImageDimension() - 1 ? " " : "");
        std::cout << (i < image->GetImageDimension() - 1 ? "]," : "]") << (i < image->GetImageDimension() - 1 ? " " : "");
    }
    std::cout << "]" << std::endl;
}

const std::filesystem::path Converter::createOutputDirectory(const std::filesystem::path& outputPath) const {
    std::filesystem::path outputDir = std::filesystem::current_path() / outputPath;
    if (!std::filesystem::exists(outputDir))
        std::filesystem::create_directories(outputDir);
    
    return outputDir;
}