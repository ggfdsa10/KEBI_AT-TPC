# - Find GENFIT2 instalation
# This module tries to find the GENFIT2 installation on your system.
#
# Variables defined by this module:
#
#   GENFIT2_FOUND               System has GENFIT2
#   GENFIT2_INCLUDE_DIR         GENFIT2 include directories: not cached
#   GENFIT2_LIBRARY_DIR         The path to where the GENFIT2 library files are.
#

Message(STATUS "Looking for GENFIT2...")
Set(GENFIT2 /home/shlee/kebi/GenFit)

Set(GENFIT2_FOUND FALSE)
Set(GENFIT2_INCLUDE_DIR ${GENFIT2}/install/include)
Set(GENFIT2_LIBRARY_DIR ${GENFIT2}/install/lib)
Set(GENFIT2_LDFLAGS "-lgenfit2 -L${GENFIT2_LIBRARY_DIR}")

Find_Library(GENFIT2_LIBRARY NAMES libgenfit2.so.2.2.0
             PATHS ${GENFIT2_LIBRARY_DIR}
             NO_DEFAULT_PATH
            )

If(GENFIT2_LIBRARY)
  MESSAGE(STATUS "Found GENFIT2 : ${GENFIT2_LIBRARY}")
  Mark_As_Advanced(GENFIT2_LIBRARY_DIR GENFIT2_INCLUDE_DIR)
  Set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${GENFIT2_LIBRARY_DIR})
  Set(TOOLS_LIBRARIES_LIST ${TOOLS_LIBRARIES_LIST} ${GENFIT2_LIBRARY})
  Set(GENFIT2_FOUND TRUE)
Else(GENFIT2_LIBRARY)
  If(GENFIT2_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "GENFIT2 Not found!")
  EndIf(GENFIT2_FIND_REQUIRED)
EndIf(GENFIT2_LIBRARY)
