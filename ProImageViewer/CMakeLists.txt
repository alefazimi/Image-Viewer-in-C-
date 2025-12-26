cmake_minimum_required(VERSION 3.16)

project(ProImageViewer VERSION 4.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Concurrent REQUIRED)
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    DirectoryScanner.cpp
    DirectoryScanner.h
    ImageUtils.h
)

add_executable(ProImageViewer ${PROJECT_SOURCES})

target_link_libraries(ProImageViewer PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
    ${OpenCV_LIBS}
)