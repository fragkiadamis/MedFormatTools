#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <zlib.h>

#include <argparse/argparse.hpp>

#include "Types.h"

class ArgParser {
public:
    ArgParser(int argc, char* argv[]);
    
    bool isVerbose() const;
    const std::filesystem::path& getInputPath() const;
    const std::filesystem::path& getOutputPath() const;
    const Format& getFormat() const;
    const Modality& getModality() const;

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

#endif // ARGPARSER_H
