cmake_minimum_required (VERSION 3.0)

find_package(PkgConfig)

set(MY_PROJECT_NAME Siv3D)
project(${MY_PROJECT_NAME}.app CXX)
enable_language(C)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wno-unknown-pragmas -fPIC -msse4.1")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	add_compile_options ("-fcolor-diagnostics")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_options ("-fdiagnostics-color=always")
endif()

#set(CMAKE_BUILD_TYPE Release)

include_directories(
    "./include/"
    "./include/ThirdParty/"
)

link_directories(
    "./lib/macOS/"
    "./lib/macOS/glew/"
    "./lib/macOS/opencv/"
    "./lib/macOS/libjpeg-turbo/"
    "./lib/macOS/libogg/"
    "./lib/macOS/boost/"
    "./lib/macOS/libwebp/"
    "./lib/macOS/harfbuzz/"
    "./lib/macOS/libvorbis/"
    "./lib/macOS/zlib/"
    "./lib/macOS/freetype/"
    "./lib/macOS/libgif/"
    "./lib/macOS/libpng/"
)

set(SOURCE_FILES
	"./Main.cpp"
)

set(APP_PROJECT_NAME ${MY_PROJECT_NAME})
set(EXECUTABLE_NAME ${MY_PROJECT_NAME})

file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS RELATIVE "${PROJECT_SOURCE_DIR}" "Resources/*")

add_executable(${MY_PROJECT_NAME} ${SOURCE_FILES} ${RESOURCE_FILES})

foreach(RESOURCE_FILE ${RESOURCE_FILES})
    get_filename_component(RESOURCE_DIR ${RESOURCE_FILE} DIRECTORY)
    set_source_files_properties(${RESOURCE_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "${RESOURCE_DIR}")
endforeach(RESOURCE_FILE)

set_target_properties(${MY_PROJECT_NAME} PROPERTIES
  MACOSX_BUNDLE TRUE
)

target_link_libraries(${MY_PROJECT_NAME}
    -lSiv3D
    "-framework CoreMedia"
    "-framework CoreAudio"
    "-framework AVFoundation"
    "-framework Foundation"
    "-framework AudioToolbox"
    "-framework OpenGL"
    "-framework OpenAL"
    -lopencv_imgcodecs
    -lopencv_photo
    -lopencv_videoio
    -lopencv_objdetect
    -lopencv_core
    -lopencv_imgproc
    -lGLEW
	-lpthread
	-ldl
	-lfreetype
	-lharfbuzz
	-lpng
	-lturbojpeg
	-lgif
    -lwebp
	-logg
	-lvorbis
	-lvorbisenc
	-lvorbisfile
	-lboost_filesystem
    -lz
    -lutil
)
