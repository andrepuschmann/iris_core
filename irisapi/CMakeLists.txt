########################################################################
# Need to install the api for component developers
########################################################################
INSTALL(DIRECTORY . DESTINATION "${INCLUDE_DIR}/irisapi" 
	FILES_MATCHING 	PATTERN "*.h"
			PATTERN ".svn" EXCLUDE
)

