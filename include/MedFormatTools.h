#ifndef MEDFORMATTOOLS_H
#define MEDFORMATTOOLS_H

#include "ArgumentParser.h"

class MedFormatTools {
public:
    MedFormatTools(int argc, char* argv[]);
    void run();

private:
    ArgumentParser argParser;
};

#endif
