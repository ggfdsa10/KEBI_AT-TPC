###############################################
# cmake configuration file for Rave
# @author Fabian Moser, HEPHY
###############################################


# do not store find results in cache
SET( Rave_INCLUDE_DIRS Rave_INCLUDE_DIRS-NOTFOUND )

FIND_PATH( Rave_INCLUDE_DIRS
    NAMES rave/VertexFactory.h
    PATHS /usr/local/rave
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
)
IF( NOT Rave_INCLUDE_DIRS )
    MESSAGE( STATUS "Check for Rave: ${Rave_HOME}"
					" -- failed to find Rave include directory!!" )
ELSE( NOT Rave_INCLUDE_DIRS )
    MARK_AS_ADVANCED( Rave_INCLUDE_DIRS )
ENDIF( NOT Rave_INCLUDE_DIRS )


# do not store find results in cache
SET( Rave_LIB Rave_LIB-NOTFOUND )

FIND_LIBRARY( Rave_LIB
    NAMES RaveBase RaveCore RaveVertex RaveFlavorTag RaveVertexKinematics
    PATHS /usr/local/rave
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
)
IF( NOT Rave_LIB )
    MESSAGE( STATUS "Check for Rave: ${Rave_HOME}"
					" -- failed to find Rave library!!" )
ELSE( NOT Rave_LIB )
    MARK_AS_ADVANCED( Rave_LIB )
ENDIF( NOT Rave_LIB )


# set variables and display results
IF( Rave_INCLUDE_DIRS AND Rave_LIB )
    SET( Rave_LIBRARIES ${Rave_LIB} )
    MARK_AS_ADVANCED( Rave_LIBRARIES )
    SET( Rave_DEFINITIONS "-DRAVE -DRaveDllExport= -DWITH_FLAVORTAGGING -DWITH_KINEMATICS" )
    MARK_AS_ADVANCED( Rave_DEFINITIONS )
    MESSAGE( STATUS "Check for Rave: ${Rave_HOME} -- works" )
ELSE( Rave_INCLUDE_DIRS AND Rave_LIB )
    IF( Rave_FIND_REQUIRED )
        MESSAGE( FATAL_ERROR "Check for Rave: ${Rave_HOME} -- failed!!" )
    ELSE( Rave_FIND_REQUIRED )
        MESSAGE( STATUS "Check for Rave: ${Rave_HOME}"
                        " -- failed!! will skip this package..." )
    ENDIF( Rave_FIND_REQUIRED )
ENDIF( Rave_INCLUDE_DIRS AND Rave_LIB )
