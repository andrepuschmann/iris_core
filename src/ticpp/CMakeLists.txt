########################################################################
# Add the subdirectories
########################################################################
# Recurse into the "src" and "lib" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
ADD_SUBDIRECTORY (include)
ADD_SUBDIRECTORY (src)
