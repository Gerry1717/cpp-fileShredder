#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

bool isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR);
}

void overwriteFile(const std::string& filename, int numOverwrites) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open the file for overwriting." << std::endl;
        return;
    }

    for (int i = 0; i < numOverwrites; ++i) {
        const int bufferSize = 1024;
        char buffer[bufferSize];
        for (int j = 0; j < bufferSize; ++j) {
            buffer[j] = rand() % 256;
        }

        file.write(buffer, bufferSize);
    }

    file.close();
}

void shredPath(const std::string& path, int numOverwrites) {
    if (isDirectory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            shredPath(entry.path().string(), numOverwrites);
        }
    } else {
        overwriteFile(path, numOverwrites);
        if (remove(path.c_str()) != 0) {
            std::cerr << "Failed to delete the file: " << path << std::endl;
        } else {
            std::cout << "File has been securely shredded: " << path << std::endl;
        }
    }
}

int main() {
    std::string path;
    int numOverwrites;

    std::cout << "Enter the path to the file or directory: ";
    std::cin >> path;

    std::cout << "Enter the number of overwrites: ";
    std::cin >> numOverwrites;

    if (numOverwrites <= 0) {
        std::cerr << "Number of overwrites must be a positive integer." << std::endl;
        return 1;
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    shredPath(path, numOverwrites);

    return 0;
}
