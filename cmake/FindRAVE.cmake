# - Find RAVE instalation
# This module tries to find the RAVE installation on your system.
#
# Variables defined by this module:
#
#   RAVE_FOUND               System has RAVE
#   RAVE_INCLUDE_DIR         RAVE include directories: not cached
#   RAVE_LIBRARY_DIR         The path to where the RAVE library files are.
#

Message(STATUS "Looking for RAVE...")

Set(RAVE_FOUND FALSE)
Set(RAVE_INCLUDE_DIR0 ${RAVE}/include/)
Set(RAVE_INCLUDE_DIR ${RAVE}/include/rave)
Set(RAVE_LIBRARY_DIR ${RAVE}/lib)
Set(RAVE_LDFLAGS "-L${RAVE_LIBRARY_DIR} -lRaveBase -lRaveCore -lRaveVertex -lRaveFlavorTag -lRaveVertexKinematics")

Find_Library(RAVE_LIBRARY  NAMES RaveBase PATHS ${RAVE_LIBRARY_DIR} NO_DEFAULT_PATH)
Find_Library(RAVE_LIBRARY1 NAMES RaveCore PATHS ${RAVE_LIBRARY_DIR} NO_DEFAULT_PATH)
Find_Library(RAVE_LIBRARY2 NAMES RaveFlavorTag PATHS ${RAVE_LIBRARY_DIR} NO_DEFAULT_PATH)
Find_Library(RAVE_LIBRARY3 NAMES RaveVertex PATHS ${RAVE_LIBRARY_DIR} NO_DEFAULT_PATH)
Find_Library(RAVE_LIBRARY4 NAMES RaveVertexKinematics PATHS ${RAVE_LIBRARY_DIR} NO_DEFAULT_PATH)

If(RAVE_LIBRARY)
  MESSAGE(STATUS "Found RAVE : ${RAVE_LIBRARY}")
  Mark_As_Advanced(RAVE_LIBRARY_DIR RAVE_INCLUDE_DIR)
  Set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${RAVE_LIBRARY_DIR})
  Set(TOOLS_LIBRARIES_LIST ${TOOLS_LIBRARIES_LIST} ${RAVE_LIBRARY})# ${RAVE_LIBRARY1} ${RAVE_LIBRARY2} ${RAVE_LIBRARY3} ${RAVE_LIBRARY4})
  Set(RAVE_FOUND TRUE)
Else(RAVE_LIBRARY)
  If(RAVE_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "RAVE Not found!")
  EndIf(RAVE_FIND_REQUIRED)
EndIf(RAVE_LIBRARY)
