cmake_minimum_required(VERSION 3.6)
include("cmake/message.cmake")

project(yuni)
ynmessage()
ynmessage(BOLD "The Yuni Framework")

# Package informations
set(YUNI_URL_WEBSITE             "http://www.libyuni.org")
set(YUNI_MAILING_LIST            "dev@libyuni.org")
# Version
set(YUNI_VERSION_HI              2)
set(YUNI_VERSION_LO              0)
set(YUNI_VERSION_REV             0)
set(YUNI_VERSION                 "${YUNI_VERSION_HI}.${YUNI_VERSION_LO}")
set(YUNI_TARGET                  "debug") # Target : debug/release - see Target.cmake
set(YUNI_URL_DEVPACK_REPOSITORY  "http://devpacks.libyuni.org/")
set(YUNI_URL_DEVPACK_SOURCE      "http://devpacks.libyuni.org/downloads")





#
# -----------------------------------------------------------------------------
#
# CMake special variables
set(VS_KEYWORD                   "YuniVS${YUNI_VERSION}")
set(VERSION                      "${YUNI_VERSION}")
set(VS_SCC_PROJECTNAME           "Yuni Framework")
set(PROJECT_LABEL                "Yuni Framework")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # vim / youcomplete / emacs / ...

if (WIN32 AND NOT WIN64)
	# We have to determine the platform architecture
	if (MINGW OR MSYS OR CYGWIN)
		execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "--version"
			OUTPUT_VARIABLE compiler_version ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
		string(FIND "${compiler_version}" "64" platform_64)
		if ("${platform_64}" GREATER 0)
			set(WIN64 true)
		endif()
	else()
		string(FIND "${CMAKE_GENERATOR}" "64" platform_64)
		if ("${platform_64}" GREATER 0)
			set(WIN64 true)
		endif()
	endif()
endif()


set(YUNI_INSTALL_CXX_FLAGS   "")
set(YUNI_INSTALL_CXX_DEFINES "")

include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)


# ICC Detection
# CMake does not handle ICC at all, mainly because ICC behaves like Visual Studio and
# it seems there is no reliable way to make the distinction
if (WIN32 OR WIN64)
	if (NOT MSVC AND NOT MINGW AND NOT CYGWIN)
		# Should behaves like Visual Studio, like ICC
		set(MSVC 1)
	endif()
endif()


# Target
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/target.cmake")
# Getting the latest revision number
ynmessage(BOLD "Version : v${YUNI_VERSION_HI}.${YUNI_VERSION_LO}-${YUNI_TARGET} (Rev: ${YUNI_VERSION_REV})")
ynmessage()
set(YUNI_VERSION_STRING "${YUNI_VERSION_HI}.${YUNI_VERSION_LO}.${YUNI_VERSION_REV}-${CMAKE_BUILD_TYPE}")
# Informations about the system
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/system-information.cmake")


#
# The target directory for static libs
# Various output directories
#
set(YUNI_VERSIONED_INST_PATH "yuni/${YUNI_VERSION}")
set(YUNI_OUTPUT_DIRECTORY    "${CMAKE_CURRENT_BINARY_DIR}/build/${YUNI_TARGET}/")
set(YUNI_SOURCE_TREE         "${CMAKE_CURRENT_SOURCE_DIR}/yuni/")

set(YUNI_SKIP_YUNI_CONFIG  false)

ynmessage(BOLD "Folders")
ynmessage("         Source: ${CMAKE_CURRENT_SOURCE_DIR}")
ynmessage("     Build into: ${CMAKE_CURRENT_BINARY_DIR}")


# Custom settings
if ("${PROFILE}" STREQUAL "")
	if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/ProfileBuild.cmake")
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProfileBuild.template.cmake
			${CMAKE_CURRENT_BINARY_DIR}/ProfileBuild.cmake)
		ynmessage()
		ynmessage("A default profile has been created (`<build_directory>/ProfileBuild.cmake`)")
		ynmessage("Please edit this file to override the default settings")
		ynmessage()
	endif()
	set(YUNI_PROFILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/ProfileBuild.cmake")
else()
	if (NOT EXISTS "${PROFILE}")
		ynerror("Could not find the specified profile file: (`${PROFILE}`)")
	endif()
	set(YUNI_PROFILE_PATH "${PROFILE}")
endif()

include("${YUNI_PROFILE_PATH}")
ynmessage("  Using profile: ${YUNI_PROFILE_NAME}")
ynmessage("       found in: ${YUNI_PROFILE_PATH}")

# (Re)Check the target
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/target.cmake")
# DevPacks utilities
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/devpack.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/devpack-standard-imports.cmake")
# Modules
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules.cmake")


# Sub Directories
add_subdirectory(yuni)
# tools (especially for yuni-config)
add_subdirectory(tools)


#
# Compiling yuni-config if required
#
if (YUNI_AUTO_COMPILE_YUNI_CONFIG)
	ynmessage("[yuni-config] Bootstraping")
	if (YUNI_TESTS)
		ynmessage("The bootstrapping is required to compile the testsuite")
	endif()
	# --
	ynmessage("Configuring yuni-config...")
	ynmessage("(in ${CMAKE_CURRENT_SOURCE_DIR}/tools/yuni-config)")
	# Create the yuni-config bootstrap directory.
	execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory
		"${CMAKE_CURRENT_BINARY_DIR}/tools/yuni-config-bootstrap"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
	# Configure yuni-config like us.
	execute_process(COMMAND "${CMAKE_COMMAND}"
			${CMAKE_CURRENT_SOURCE_DIR}/tools/yuni-config
			"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DYUNI_OUTPUT_DIRECTORY=${YUNI_OUTPUT_DIRECTORY}"
			"-DYUNI_BOOTSTRAP_YUNI_CONFIG=1"
			-G "${CMAKE_GENERATOR}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tools/yuni-config-bootstrap")
	# Compile it.
	ynmessage("Compiling yuni-config...")
	execute_process(COMMAND "${CMAKE_COMMAND}" --build
                              "${CMAKE_CURRENT_BINARY_DIR}/tools/yuni-config-bootstrap"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tools/yuni-config-bootstrap")
endif()


if (YUNI_BENCHMARKS)
	add_subdirectory(benchmarks)
endif()

if (__libyuni_error_has_occured)
	message(STATUS "")
	message(STATUS "")
	if (UNIX)
		message(SEND_ERROR "[1;31mAn error has occured. Aborting. [0m")
	else()
		message(SEND_ERROR "An error has occured. Aborting.")
	endif()
endif()




# Empty message, for beauty :)
ynmessage() # done
