#include "stdheaders.h"
#include "reza.h"
#include "lexer.h"
#include "module.h"
#include "asmwriter.h"

const bool TEST_MODE = false;
const std::string TEST_FILE = "helloworld.reza";

Reza::Reza() {
    totalErrors = 0;
    maxErrors = 5;
    majorVersion = 0;
    minorVersion = 0;
    testVersion = 1;
    platformName = "Windows 64-bit";
    Lexer::init();
    AsmWriter::init();
}

int Reza::run(int argc, char** argv) {
    std::vector<std::string> files;
    if (TEST_MODE == true && TEST_FILE.empty() == false) {
        files.push_back(TEST_FILE);
    } else {
        for (int i = 1; i < argc; i++) {
            const char* p = argv[i];
            // check for compiler options
            if ( *p == '-' || *p == '/' ) {
                // TODO: Need to add compiler options
            } else {
                const std::string& filename = argv[i];
                files.push_back(filename);
            }
        }
    }
    std::cout << "Reza Compiler v"
            << majorVersion << "."
            << minorVersion << "."
            << testVersion << " for " << platformName << std::endl;
    if (!files.size()) {
        std::cout << "No input file is specified." << std::endl;
        return 0;
    }

    std::vector<Module*> modules;
    modules.reserve(files.size());

    // Load files/modules
    for (const auto& file : files) {
        Module* m = new Module(file);
        modules.push_back(m);
    }

    // Read all the modules and then parse them
    for (Module* m : modules) {
        if (m->read()) {
            std::cout << m->srcFileName << ": Parsing..." << std::endl;
            m->parse();
        }
    }

    // Semantical analysis on the modules
    for (Module* m : modules) {
        std::cout << m->srcFileName << ": Analysing..." << std::endl;
        m->semantic();
    }

    // Generate the assembly code
    for (Module* m : modules) {
        std::cout << m->srcFileName << ": Generating assembly..." << std::endl;
        m->genAsmFile();
    }
    std::cout << "Done." << std::endl;
    return 0;
}

int main(int argc, char** argv) {
    Reza reza;
    // Run the compiler!
    return reza.run(argc, argv);
}




