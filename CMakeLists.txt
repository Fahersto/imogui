cmake_minimum_required(VERSION "3.15")
project("imogui")

# include header files of hookFTW 
include_directories(hookftw/library/src)

# build hookFTW
add_subdirectory(hookftw)

# add header and source files to library
file(GLOB_RECURSE LIBRARY_SOURCES LIST_DIRECTORIES true src/*.h src/*.cpp)

# gather source files of this example
file(GLOB_RECURSE IMGUI_SOURCES LIST_DIRECTORIES true src/imgui/*.h src/imgui/*.cpp)

# add imogui.lib
add_library(imgui STATIC ${IMGUI_SOURCES})

# add imogui.lib
add_library(imogui STATIC ${LIBRARY_SOURCES})

# link hookFTW
target_link_libraries(imogui hookftw)

# link imgui
target_link_libraries(imogui imgui)

# link DirectX11
target_link_libraries(imogui d3d11)

# build examples
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
	add_library(steam_d3d9 SHARED src/examples/steam_d3d9.cpp)
	target_link_libraries(steam_d3d9 imgui)
	target_link_libraries(steam_d3d9 imogui)

	add_library(steam_opengl SHARED src/examples/steam_opengl.cpp)
	target_link_libraries(steam_opengl imgui)
	target_link_libraries(steam_opengl imogui)

	add_library(discord_d3d9 SHARED src/examples/discord_d3d9.cpp)
	target_link_libraries(discord_d3d9 imgui)
	target_link_libraries(discord_d3d9 imogui)

	add_library(discord_opengl SHARED src/examples/discord_opengl.cpp)
	target_link_libraries(discord_opengl imgui)
	target_link_libraries(discord_opengl imogui)

	add_library(obsoverlay_d3d9 SHARED src/examples/obsoverlay_d3d9.cpp)
	target_link_libraries(obsoverlay_d3d9 imgui)
	target_link_libraries(obsoverlay_d3d9 imogui)

	add_library(obsoverlay_opengl SHARED src/examples/obsoverlay_opengl.cpp)
	target_link_libraries(obsoverlay_opengl imgui)
	target_link_libraries(obsoverlay_opengl imogui)

	add_library(msiafterburner_opengl SHARED src/examples/msiafterburner_opengl.cpp)
	target_link_libraries(msiafterburner_opengl imgui)
	target_link_libraries(msiafterburner_opengl imogui)
endif()

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
	add_library(steam_d3d11 SHARED src/examples/steam_d3d11.cpp)
	target_link_libraries(steam_d3d11 imgui)
	target_link_libraries(steam_d3d11 imogui)

	add_library(native_d3d12 SHARED src/examples/native_d3d12.cpp)
	target_link_libraries(native_d3d12 imgui)
	target_link_libraries(native_d3d12 imogui)

	add_library(steam_opengl SHARED src/examples/steam_opengl.cpp)
	target_link_libraries(steam_opengl imgui)
	target_link_libraries(steam_opengl imogui)

	add_library(discord_d3d11 SHARED src/examples/discord_d3d11.cpp)
	target_link_libraries(discord_d3d11 imgui)
	target_link_libraries(discord_d3d11 imogui)

	add_library(discord_opengl SHARED src/examples/discord_opengl.cpp)
	target_link_libraries(discord_opengl imgui)
	target_link_libraries(discord_opengl imogui)

	add_library(msiafterburner_opengl SHARED src/examples/msiafterburner_opengl.cpp)
	target_link_libraries(msiafterburner_opengl imgui)
	target_link_libraries(msiafterburner_opengl imogui)

	add_library(msiafterburner_d3d11 SHARED src/examples/msiafterburner_d3d11.cpp)
	target_link_libraries(msiafterburner_d3d11 imgui)
	target_link_libraries(msiafterburner_d3d11 imogui)

	# not working
	#add_library(originoverlay_d3d11 SHARED src/examples/originoverlay_d3d11.cpp)
	#target_link_libraries(originoverlay_d3d11 imgui)
	#target_link_libraries(originoverlay_d3d11 imogui)

	add_library(obsoverlay_opengl SHARED src/examples/obsoverlay_opengl.cpp)
	target_link_libraries(obsoverlay_opengl imgui)
	target_link_libraries(obsoverlay_opengl imogui)

	add_library(obsoverlay_d3d11 SHARED src/examples/obsoverlay_d3d11.cpp)
	target_link_libraries(obsoverlay_d3d11 imgui)
	target_link_libraries(obsoverlay_d3d11 imogui)

	add_library(overwolfoverlay_d3d11 SHARED src/examples/overwolfoverlay_d3d11.cpp)
	target_link_libraries(overwolfoverlay_d3d11 imgui)
	target_link_libraries(overwolfoverlay_d3d11 imogui)
endif()

# link our project executable against Zydis.
target_compile_features(imogui PRIVATE cxx_std_17)