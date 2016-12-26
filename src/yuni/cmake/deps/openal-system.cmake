
ynmessage("      Using OpenAL from system")

# Use find_package for automatic detection
find_package(OpenAL)
if (OPENAL_FOUND)
	set(CMAKE_REQUIRED_INCLUDES "${OPENAL_INCLUDE_DIR}")
	ynmessage("OpenAL found : ${OPENAL_INCLUDE_DIR}")

	# Mac OS X
	if(APPLE)
		# Frameworks
		LIBYUNI_CONFIG_FRAMEWORK("both" "media" OpenAL)
	# Others
	else(APPLE)
		# Libs
		LIBYUNI_CONFIG_LIB("both" "media" "${OPENAL_LIBRARY}")
	endif()

	LIBYUNI_CONFIG_INCLUDE_PATH("both" "media" "${OPENAL_INCLUDE_DIR}")
	include_directories("${OPENAL_INCLUDE_DIR}")

else()
	set(YUNI_CMAKE_ERROR 1)
	ynmessage("[!!] Impossible to find OpenAL. Please check your profile.")
	ynmessage(" * Packages needed on Debian: libopenal-dev")
	ynmessage(" * Packages needed on redhat: openal-devel")
endif()


