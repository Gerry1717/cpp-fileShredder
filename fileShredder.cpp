#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <random>

namespace fs = std::filesystem;

bool isDirectory(const fs::path& path) {
    return fs::is_directory(path);
}

void overwriteFile(const fs::path& filename, int numOverwrites) {
    std::streampos fileSize = fs::file_size(filename);
    std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "Failed to open the file for overwriting." << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    const int bufferSize = 1024;
    char buffer[bufferSize];

    for (int i = 0; i < numOverwrites; ++i) {
        std::streampos written = 0;
        while (written < fileSize) {
            int currentWriteSize = std::min(bufferSize, static_cast<int>(fileSize - written));
            for (int j = 0; j < currentWriteSize; ++j) {
                buffer[j] = static_cast<char>(dis(gen));
            }
            file.write(buffer, currentWriteSize);
            written += currentWriteSize;
        }
        file.seekp(0, std::ios::beg);
    }
    file.close();
}

void shredPath(const fs::path& path, int numOverwrites) {
    if (isDirectory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            shredPath(entry.path(), numOverwrites);
        }
    } else {
        overwriteFile(path, numOverwrites);
        if (fs::remove(path) == false) {
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
