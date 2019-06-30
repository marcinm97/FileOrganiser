cmake_minimum_required(VERSION 3.13)
project(FileOrganiser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS -static)
link_libraries(stdc++fs)
add_executable(FileOrganiser main.cpp FileOrganiser.cpp FileOrganiser.h FileChecker.h)