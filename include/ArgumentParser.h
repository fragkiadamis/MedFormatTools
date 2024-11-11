#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <argparse/argparse.hpp>

#include "FormatUtils.h"

class ArgumentParser {
public:
    ArgumentParser(int argc, char* argv[]);
    
    bool isVerbose() const;
    const std::filesystem::path& getInputPath() const;
    const std::filesystem::path& getOutputPath() const;
    Format getTargetFormat() const;
    Modality getModality() const;

    void printArguments() const;

private:
    std::filesystem::path inputPath;
    std::filesystem::path outputPath;
    Format inputFormat;
    Format targetFormat;
    Modality modality;
    bool verbose;

    void parseArguments(int argc, char* argv[]);
    void validatePaths();
};

#endif
