cmake_minimum_required(VERSION 3.1.0)

project(population_dynamics)

# Tell CMake to run moc when necessary
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake to always look for includes there
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Find the required Qt modules
find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)


# Include the headers
include_directories(include)

# Define the source files
#Uncomment line below once it is fixed
set(SOURCE_FILES_MAIN src/LVSimulator.cpp  src/main.cpp include/LVSimulator.h include/qcustomplot.cpp)

set(SOURCE_FILES_RPS_SAVE src/RPSSimulator.cpp src/RPS_save.cpp)
set(SOURCE_FILES_RPS_REALTIME src/RPSSimulator.cpp src/RPS_realtime.cpp)

#Uncomment line below once it is fixed
# set(HEADER_FILES_MAIN include/SimulationWorker.h)

# Add an executable with the above sources
#Uncomment line below once it is fixed
add_executable(population_dynamics ${SOURCE_FILES_MAIN} ${HEADER_FILES_MAIN})

add_executable(RPS_save ${SOURCE_FILES_RPS_SAVE})
add_executable(RPS_realtime ${SOURCE_FILES_RPS_REALTIME})

# Link the target to the necessary Qt libraries
#Uncomment line below once it is fixed
target_link_libraries(population_dynamics Qt5::Widgets Qt5::PrintSupport sfml-graphics)

target_link_libraries(RPS_save sfml-graphics)
target_link_libraries(RPS_realtime sfml-graphics)

