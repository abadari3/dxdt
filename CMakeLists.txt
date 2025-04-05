cmake_minimum_required(VERSION 3.16)
project(aime CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# Find Boost with the needed components including system.
find_package(Boost REQUIRED COMPONENTS date_time log log_setup system)

# Add the src/log directory to the project.
add_subdirectory(src/log)

# Define the main executable.
add_executable(aime src/main.cpp)

# Include Boost headers.
target_include_directories(aime PRIVATE ${Boost_INCLUDE_DIRS})

# Ensure BOOST_LOG_DYN_LINK is defined.
target_compile_definitions(aime PRIVATE BOOST_LOG_DYN_LINK)

# Link dependencies. Note that log propagates its INTERFACE link libraries (Boost and pthread),
# so here we add log first and then add additional system libraries.
target_link_libraries(aime PRIVATE
    log
    dl       # Link against libdl
    rt       # Add librt if necessary (optional)
)