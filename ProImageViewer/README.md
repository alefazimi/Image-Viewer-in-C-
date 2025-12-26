# ProImageViewer

**Version:** 4.0.0

**Description:**
A professional image viewer and editor built with C++17, Qt6, and OpenCV. It offers a range of features for image manipulation and viewing, including brightness/contrast adjustment, saturation control, blur, grayscale conversion, sharpening, cropping, and rotation. The application supports undo/redo functionality and can scan directories for images.

**Features:**
- **Image Viewing:** Open and navigate through images in a directory.
- **Image Editing:**
    - Brightness and Contrast
    - Saturation
    - Blur
    - Grayscale
    - Sharpen
    - Rotate
    - Crop
- **Undo/Redo:** Multi-level undo and redo for all edits.
- **Fit to Window:** Toggle between fitting the image to the window and viewing at actual size.
- **Directory Scanning:** Automatically finds all supported image files in a directory.
- **Session Persistence:** Remembers window geometry and settings between sessions.

**Dependencies:**
- C++17 compiler
- CMake 3.16+
- Qt6 (Widgets, Concurrent)
- OpenCV

**Build Instructions:**
1. Create a build directory: `mkdir build`
2. Navigate into the build directory: `cd build`
3. Run CMake: `cmake ..`
4. Compile the project: `make`
5. Run the application: `./ProImageViewer`
