cmake_minimum_required(VERSION 3.10)

project(minefetch)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)


set(USER_CONFIG_DIR $ENV{HOME}/.config/minefetch)

set(SOURCES src/fetch_out.cpp src/main.cpp src/minecraft_motd.cpp src/motd_config.cpp src/utils.cpp)

add_executable(minefetch ${SOURCES})

target_include_directories(minefetch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS minefetch RUNTIME DESTINATION bin)
