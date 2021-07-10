cmake_minimum_required(VERSION "3.15")
project("imogui")


# include header files of hookFTW 
include_directories(hookftw/library/src)

# build hookFTW
add_subdirectory(hookftw)

# Add header and source files to library
file(GLOB_RECURSE LIBRARY_SOURCES LIST_DIRECTORIES true src/*.h src/*.cpp)

# Add imogui.lib
add_library(imogui STATIC ${LIBRARY_SOURCES})

# link hookFTW
target_link_libraries(imogui hookftw)

# Link our project executable against Zydis.
target_compile_features(imogui PRIVATE cxx_std_17)