#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <zlib.h>

#include <argparse/argparse.hpp>

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

class ArgParser {
public:
    ArgParser(int argc, char* argv[]);
    static Format parseFormat(const std::string& format);
    static Modality parseModality(const std::string& modality);
    
    bool isVerbose() const;
    const std::filesystem::path& getInputPath() const;
    const std::filesystem::path& getOutputPath() const;
    Format getTargetFormat() const;
    Modality getModality() const;
    static std::string toString(Format format);
    static std::string toString(Modality modality);

    void printArguments() const;

private:
    std::filesystem::path inputPath;
    std::filesystem::path outputPath;
    Format inputFormat;
    Format targetFormat;
    Modality modality;
    bool verbose;

    void parseArguments(int argc, char* argv[]);
    static void getFileHeader(const std::filesystem::path& inputPath, char *header, int position);
    static Format detectInputFormat(const std::filesystem::path& inputPath);
};

#endif
