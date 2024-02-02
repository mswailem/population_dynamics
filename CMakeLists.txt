cmake_minimum_required(VERSION 3.1.0)

project(population_dynamics)

# Find the required Qt modules
find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

# Tell CMake to run moc when necessary
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake to always look for includes there
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include the headers
include_directories(include)

# Define the source files
set(SOURCE_FILES src/main.cpp include/qcustomplot.cpp)

# Add an executable with the above sources
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link the target to the necessary Qt libraries
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport)

