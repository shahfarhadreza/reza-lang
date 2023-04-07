#pragma once

class File {
public:
    std::string path;
    std::string buffer;
    File(const std::string& filename);
    void setBuffer(const std::string& buf);
    bool read();
    bool write();
};

