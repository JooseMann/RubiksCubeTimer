cmake_minimum_required(VERSION 3.14)

project(QubeTimer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)

include_directories(include/QubeTimer)

add_executable(QubeTimer)

target_sources(QubeTimer
    PRIVATE
        # Headers (Qt Widgets)
        include/QubeTimer/UI/Averages.hpp
        include/QubeTimer/UI/Timer.hpp
        include/QubeTimer/UI/Window.hpp
        # Source files
        src/main.cpp
        src/RubiksCube.cpp
        src/SolveManager.cpp
        src/UI/Averages.cpp
        src/UI/Timer.cpp
        src/UI/Window.cpp
)
target_link_libraries(QubeTimer Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)
