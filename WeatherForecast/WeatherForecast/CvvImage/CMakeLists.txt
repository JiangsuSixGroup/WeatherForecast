
#
# Source files
#
# Here is where you can add the name of your local classes.
# 

FILE( GLOB ALL_SOURCES "*.cpp" "*.c" )
FILE( GLOB ALL_INCLUDES "*.hpp" "*.h" )

SET( ALL_SRCS 
	${ALL_SOURCES}
	${ALL_INCLUDES}
)

# Use MFC in a Shared DLL
SET( CMAKE_MFC_FLAG 2 )

# Create the tinyxml C++ library
ADD_LIBRARY( CvvImage STATIC
	${ALL_SRCS}
)

# Add the extra link-libraries

# The installation path for the link-library
INSTALL_TARGETS( /libs/ExtLibs CvvImage )

# The installation path for the include-files
INSTALL(FILES ${ALL_INCLUDES} DESTINATION include/CvvImage)
