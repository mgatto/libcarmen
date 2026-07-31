# Copy SRC to DST only when DST does not already exist.
#
# Used by the `dist` target so re-running it never clobbers an edited
# settings file. Invoke via:
#   cmake -DSRC=<src> -DDST=<dst> -P cmake/copy_if_absent.cmake
if(NOT EXISTS "${DST}")
    configure_file("${SRC}" "${DST}" COPYONLY)
endif()
