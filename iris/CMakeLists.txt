########################################################################
# Need to install the headers for the iris_core library
########################################################################
INSTALL(DIRECTORY . DESTINATION "${INCLUDE_DIR}/iris" 
	FILES_MATCHING 	PATTERN "*.h"
			PATTERN ".svn" EXCLUDE
)

