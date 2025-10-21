cmake_minimum_required(VERSION 3.24)
project(betsbnd CXX)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	message("Found ccache!")
endif()

set(CMAKE_CXX_COMPILER /bin/clang++ CACHE STRING "")
set(CMAKE_C_COMPILER /bin/clang CACHE STRING "")

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(/usr/include/cpp)
include_directories(/usr/include/)

#add_subdirectory(ext)

include(GNUInstallDirs)

message(${CMAKE_CXX_COMPILER_ID})

set(OPT_FLAGS -O2)
if(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
	set(OPT_FLAGS ${OPT_FLAGS})
endif()
set(WARN_FLAGS
	-Wno-unused-but-set-variable
	-Wno-unknown-warning-option
	-Wno-unused-variable
	-Wno-nan-infinity-disabled
)
set(ARCH_FLAGS -mavx -mavx2 -march=native)
set(CMN_FLAGS -std=c++20)
set(STD_FLAGS ${CMN_FLAGS} ${WARN_FLAGS} ${ARCH_FLAGS} ${OPT_FLAGS})
set(DBG_FLAGS ${CMN_FLAGS} ${WARN_FLAGS} ${ARCH_FLAGS} -O0 -ggdb3)
set(VLG_FLAGS ${CMN_FLAGS} ${WARN_FLAGS} -march=broadwell -O2 -ggdb3 -fno-omit-frame-pointer)

set(STD_DEFS)
set(DBG_DEFS)
set(VLG_DEFS)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS})

remove_definitions(-DENABLE_SIMD)
remove_definitions(-DDEBUG_ARRAYS)
remove_definitions(-DDEBUG_TENSORS)
remove_definitions(-DENABLE_DEBUG_LOG)

set(SOURCES
	src/compression/oozle/bitknit.cpp
	src/compression/oozle/kraken.cpp
	src/compression/oozle/lzna.cpp
	src/compression/zlib_inf.cpp
)

set(LIBS
	ZLIB::ZLIB
)

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

function(create_bin)
	cmake_parse_arguments(PARSE_ARGV 0 BIN "" "NAME;PATH" "FLAGS;DEFS")
	add_executable(${BIN_NAME} ${BIN_PATH} ${SOURCES})
	target_compile_options(${BIN_NAME} PRIVATE ${BIN_FLAGS})
	target_compile_definitions(${BIN_NAME} PUBLIC ${BIN_DEFS})
	target_link_libraries(${BIN_NAME} PRIVATE ${LIBS})
endfunction()

create_bin(NAME betsbnd PATH src/main.cpp FLAGS ${STD_FLAGS} DEFS ${STD_DEFS})
create_bin(NAME betsbnd_dbg PATH src/main.cpp FLAGS ${DBG_FLAGS} DEFS ${DBG_DEFS})
create_bin(NAME betsbnd_vlg PATH src/main.cpp FLAGS ${VLG_FLAGS} DEFS ${VLG_DEFS})
