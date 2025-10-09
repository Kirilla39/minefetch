cmake_minimum_required(VERSION 3.10)

project(minefetch)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USER_CONFIG_DIR $ENV{HOME}/.config/aseprite-wal)

add_executable(minefetch
  src/main.cpp
  src/minecraft_motd.cpp
)

target_include_directories(minefetch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
