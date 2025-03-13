cmake_minimum_required(VERSION 3.21)
set(NAME "po3_SpellPerkItemDistributorF4")
set(VERSION 3.0.0)
set(VR_VERSION 1)
set(NG_VERSION 1)

# ---- Options ----
option(COPY_BUILD "Copy the build output to the Fallout 4 directory." TRUE)
option(ENABLE_FALLOUT_F4 "Build for Fallout F4" OFF)
option(ENABLE_FALLOUT_NG "Build for Fallout NG" ON)
option(ENABLE_FALLOUT_VR "Build for Fallout VR" OFF)

# ---- Cache build vars ----
macro(set_from_environment VARIABLE)
	if(NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
		set(${VARIABLE} $ENV{${VARIABLE}})
	endif()
endmacro()

macro(find_commonlib_path)
	if(CommonLibName AND NOT ${CommonLibName} STREQUAL "")
		# Check extern
		find_path(CommonLibPath
			include/REL/Relocation.h
			PATHS external/${CommonLibName}/${CommonLibName}
			extern/${CommonLibName}/${CommonLibName})

		if(${CommonLibPath} STREQUAL "CommonLibPath-NOTFOUND")
			# Check path
			set_from_environment(${CommonLibName}Path)
			set(CommonLibPath ${${CommonLibName}Path})
		endif()
	endif()
endmacro()

set_from_environment(VCPKG_ROOT)

set(CommonLibName "CommonLibF4")

if(ENABLE_FALLOUT_NG)
	set_from_environment(Fallout4Path)
	add_compile_definitions(ENABLE_FALLOUT_NG)
	set(FalloutPath ${Fallout4Path})
	set(FalloutVersion "Fallout 4 NG")
	set(VERSION ${VERSION}.${NG_VERSION})
elseif(ENABLE_FALLOUT_VR)
	set_from_environment(Fallout4VRPath)
	add_compile_definitions(ENABLE_FALLOUT_VR)
	set(FalloutPath ${FalloutVRPath})
	set(FalloutVersion "Fallout VR")
	set(VERSION ${VERSION}.${VR_VERSION})
else()
	set_from_environment(Fallout4Path)
	add_compile_definitions(ENABLE_FALLOUT_F4)
	set(FalloutPath ${Fallout4Path})
	set(FalloutVersion "Fallout 4")
	set(VERSION ${VERSION}.0)
endif()

find_commonlib_path()

message(
	STATUS
	"Building ${NAME} ${VERSION} for ${FalloutVersion} at ${FalloutPath} with ${CommonLibName} at ${CommonLibPath}."
)

if(DEFINED VCPKG_ROOT)
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
	set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
else()
	message(
		WARNING
		"Variable VCPKG_ROOT is not set. Continuing without vcpkg."
	)
endif()

set(Boost_USE_STATIC_RUNTIME OFF CACHE BOOL "")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "")

# ---- Project ----
project(
	${NAME}
	VERSION ${VERSION}
	LANGUAGES CXX
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
		"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
	)
endif()

# ---- Globals ----
if(MSVC)
	if(NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
		add_compile_options(
			/MP # Build with Multiple Processes
		)
	endif()
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

set(Boost_USE_STATIC_LIBS ON)

# ---- Dependencies ----
if(DEFINED CommonLibPath AND NOT ${CommonLibPath} STREQUAL "" AND IS_DIRECTORY ${CommonLibPath})
	add_subdirectory(${CommonLibPath} ${CommonLibName})
else()
	message(
		FATAL_ERROR
		"Variable ${CommonLibName}Path is not set."
	)
endif()

find_path(SRELL_INCLUDE_DIRS "srell.hpp")
find_path(CLIBUTIL_INCLUDE_DIRS "CLibUtil/string.hpp")

find_package(spdlog REQUIRED CONFIG)
find_package(unordered_dense CONFIG REQUIRED)

# ---- Add source files ----
include(cmake/sourcelist.cmake)

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}
	FILES ${SOURCES}
)

source_group(
	TREE ${CMAKE_CURRENT_BINARY_DIR}
	FILES ${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
)

# ---- Create DLL ----
add_library(
	${PROJECT_NAME}
	SHARED
	${SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	.clang-format
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
	_UNICODE
)

target_compile_definitions(
	CommonLibF4
	PUBLIC
	F4SE_SUPPORT_XBYAK
)

target_compile_features(
	${PROJECT_NAME}
	PRIVATE
	cxx_std_23
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${SRELL_INCLUDE_DIRS}
	${CLIBUTIL_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	CommonLibF4::CommonLibF4
	spdlog::spdlog
	unordered_dense::unordered_dense
)

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
	include/PCH.h
)

if(MSVC)
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
		"/sdl" # Enable Additional Security Checks
		"/utf-8" # Set Source and Executable character sets to UTF-8
		"/Zi" # Debug Information Format

		"/permissive-" # Standards conformance
		"/Zc:preprocessor" # Enable preprocessor conformance mode

		/W4 # Warning level
		/WX # Warning level (warnings are errors)

		# disable warnings
		"/wd4200" # nonstandard extension used: zero-sized array in struct/union
		"/wd4201" # nonstandard extension used: nameless struct/union
		"/wd4324" # 'struct_name' : structure was padded due to __declspec(align())

		"$<$<CONFIG:DEBUG>:>"
		"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		${PROJECT_NAME}
		PRIVATE
		/WX # Treat Linker Warnings as Errors

		"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
		"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

# ---- File copying ----
if(COPY_BUILD)
	if(DEFINED FalloutPath)
		string(REGEX REPLACE "[/\\][Dd][Aa][Tt][Aa][/\\]?$" "" FalloutPath "${FalloutPath}") # remove " /data " if exists; cmake regex doesn't have /i
		add_custom_command(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${FalloutPath}/Data/F4SE/Plugins/
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> ${FalloutPath}/Data/F4SE/Plugins/
		)
	else()
		message(
			WARNING
			"Variable ${FalloutPath} is not defined. Skipping post-build copy command."
		)
	endif()
endif()

# ---- Build artifacts ----
set(SCRIPT "scripts/archive_artifacts.py")

add_custom_target(
	create_artifact
	COMMAND
	"${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}"
	"--name=\"${PROJECT_NAME}\""
	"--dll=\"$<TARGET_FILE:${PROJECT_NAME}>\""
	"--pdb=\"$<TARGET_PDB_FILE:${PROJECT_NAME}>\""
	WORKING_DIRECTORY
	${CMAKE_CURRENT_BINARY_DIR}
	SOURCES
	${SCRIPT}
)

add_dependencies(
	create_artifact
	${PROJECT_NAME}
)

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}
	FILES ${SCRIPT}
)
