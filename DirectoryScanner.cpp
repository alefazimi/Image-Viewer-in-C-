#include "DirectoryScanner.h"
#include <algorithm>
#include <array>

namespace fs = std::filesystem;

DirectoryScanner::DirectoryScanner() : currentIndex_(-1) {}

// check if file has a supported image extension
bool DirectoryScanner::isSupported(const fs::path& path) {
    static const std::array<std::string_view, 7> supported = {
        ".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff", ".webp"
    };
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return std::any_of(supported.begin(), supported.end(),
                       [&ext](std::string_view s) { return ext == s; });
}

// open a directory or a single file. 
//if a file path is provided we open its parent directory and set the current index to the file's position

// Returns false when no supported images are found

bool DirectoryScanner::openDirectory(const fs::path& filepath) {
    if (!fs::exists(filepath)) return false;
    fs::path dir = fs::is_directory(filepath) ? filepath : filepath.parent_path();

    imageFiles.clear();
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && isSupported(entry.path())) {
            imageFiles.push_back(entry.path());
        }
    }

    if (imageFiles.empty()) return false;
    std::sort(imageFiles.begin(), imageFiles.end());

    currentIndex_ = 0;
    if (!fs::is_directory(filepath)) {
        auto it = std::find(imageFiles.begin(), imageFiles.end(), filepath);
        if (it != imageFiles.end()) {
            currentIndex_ = static_cast<int>(std::distance(imageFiles.begin(), it));
        }
    }
    return true;
}

//Move forward with wrap-around
fs::path DirectoryScanner::next() {
    if (imageFiles.empty()) return {};
    currentIndex_ = (currentIndex_ + 1) % static_cast<int>(imageFiles.size());
    return imageFiles[currentIndex_];
}

// Move backward with wrap-around
fs::path DirectoryScanner::previous() {
    if (imageFiles.empty()) return {};
    currentIndex_--;
    if (currentIndex_ < 0) currentIndex_ = static_cast<int>(imageFiles.size()) - 1;
    return imageFiles[currentIndex_];
}

fs::path DirectoryScanner::current() const {
    if (imageFiles.empty() || currentIndex_ < 0) return {};
    return imageFiles[currentIndex_];
}