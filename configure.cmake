# KEBI BUILD Configurations

set(ACTIVATE_EVE      ON  CACHE INTERNAL "")  # Activate Root Eve package
<<<<<<< HEAD
set(CREATE_GIT_LOG    OFF  CACHE INTERNAL "")  # Create Git log
=======
set(CREATE_GIT_LOG    ON  CACHE INTERNAL "")  # Create Git log
>>>>>>> 2079b1e984042cd756ba71efe7768120e39f4343
set(BUILD_GEANT4_SIM  ON  CACHE INTERNAL "")  # Build Geant4 simulation
set(BUILD_DOXYGEN_DOC OFF CACHE INTERNAL "")  # Build Doxygen API with HTML


set(KEBI_PROJECT_LIST
  # ADD KEBI PROJECT DIRECOTRIES BELOW
  #===================================
  LAMPS-HighEnergy
  PrototypeTPC
  at-tpc
  newTPC
  #===================================
  # ADD KEBI PROJECT DIRECOTRIES ABOVE
  CACHE INTERNAL ""
)
