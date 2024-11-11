#ifndef MEDFORMATTOOLS_H
#define MEDFORMATTOOLS_H

#include "ArgParser.h"
#include "Converter.h"

class MedFormatTools {
public:
    MedFormatTools(int argc, char* argv[]);
    void run();

private:
    ArgParser argParser;
    Converter converter;
};

#endif // MEDFORMATTOOLS_H
