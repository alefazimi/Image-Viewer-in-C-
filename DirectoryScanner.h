#ifndef DIRECTORY_SCANNER_H
#define DIRECTORY_SCANNER_H

#include <vector>
#include <filesystem>
#include <string>

// Scans a directory for supported image files and provides nav
class DirectoryScanner {
public:
    DirectoryScanner();
    bool openDirectory(const std::filesystem::path& filepath);
    std::filesystem::path next();
    std::filesystem::path previous();
    std::filesystem::path current() const;

private:
    std::vector<std::filesystem::path> imageFiles;
    int currentIndex_;
    //returns true for recognized image file extensions.
    bool isSupported(const std::filesystem::path& path);
};

#endif 
//directory scanner