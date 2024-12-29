cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE DEBUG)


find_package(SFML COMPONENTS System Graphics Window REQUIRED)

set(SFML_STATIC_LIBRARIES true)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE SFML::System SFML::Graphics SFML::Window)

