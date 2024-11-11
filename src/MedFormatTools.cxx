#include "MedFormatTools.h"

MedFormatTools::MedFormatTools(int argc, char* argv[])
    : argParser(argc, argv) { }

void MedFormatTools::run() {
    if (argParser.isVerbose()) {
        argParser.printArguments();
    }
}

int main(int argc, char* argv[]) {
    MedFormatTools app(argc, argv);
    app.run();
    
    return 0;
}
