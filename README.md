# Image Viewer in C++

A professional-grade desktop image viewer built with **C++**, **Qt6**, and **OpenCV**. It provides an intuitive interface for browsing, editing, and managing images with advanced image processing capabilities.

## Features

- **Image Navigation**: Browse images in directories with next/previous navigation
- **Zoom & Pan**: Zoom in/out and fit-to-window display modes
- **Image Editing**:
  - Brightness & Contrast adjustment with live sliders
  - Saturation control
  - Blur effects
  - Grayscale toggle
  - Sharpen filter
  - 90° rotation
- **Crop Tool**: Interactive crop mode with rubber band selection
- **Undo/Redo System**: Full edit history with unlimited undo/redo
- **File Operations**: Open, save, and save-as functionality
- **Settings Persistence**: Automatically saves window state and user preferences
- **Directory Scanner**: Automatic detection of all supported image formats in directories

## Supported Image Formats

JPEG, PNG, BMP, TIFF, GIF, and other OpenCV-supported formats

## Project Structure

├── CMakeLists.txt # Build configuration
├── main.cpp # Application entry point
├── MainWindow.h/cpp # Main UI window with editing features
├── DirectoryScanner.h/cpp # Directory browsing utility
├── ImageUtils.h # Image processing utilities
└── build/ # Build artifacts


Project Structure
├── CMakeLists.txt # Build configuration
├── main.cpp # Application entry point
├── MainWindow.h/cpp # Main UI window with editing features
├── DirectoryScanner.h/cpp # Directory browsing utility
├── ImageUtils.h # Image processing utilities
└── build/ # Build artifacts
## Requirements

- **C++17** or later
- **Qt6** (Widgets, Concurrent modules)
- **OpenCV** 4.0+
- **CMake** 3.16+

## Usage

Open Image: File → Open or drag-and-drop
Browse Directory: Navigate through images using next/previous buttons
Edit: Use sliders and tools to adjust brightness, contrast, saturation, blur
Rotate: Click the rotate button for 90° clockwise rotation
Crop: Toggle crop mode, select area with mouse, press crop
Undo/Redo: Revert or redo any edits
Save: Save edited images back to disk

## How to Build & Run

```bash
# Navigate to the project directory
cd ImageViewer

# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make

# Run the application
./ProImageViewer
