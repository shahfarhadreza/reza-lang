#include "stdheaders.h"
#include <fstream>
#include "file.h"

File::File(const std::string& filename)
    : path(filename) {
}

void File::setBuffer(const std::string& buf){
    buffer = buf;
}

bool File::read() {
    if (buffer.length()) {
        return true; // already read the file
    }
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    const auto read_size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto buf = std::string(read_size, '\0');

    file.read(buf.data(), read_size);

    buffer.append(buf, 0, file.gcount());
    return true;
}

bool File::write() {
    std::ofstream file(path, std::ios::binary | std::ios::ate);
    file.write(buffer.data(), buffer.length());
    return true;
}

