#pragma once

class Reza {
public:
    int totalErrors;
    int maxErrors;
    int majorVersion;
    int minorVersion;
    int testVersion;
    std::string platformName;
public:
    Reza();
    int run(int argc, char** argv);
};
