
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

add_library(epropexperiment SHARED)

set_property(TARGET epropexperiment PROPERTY PROJECT_LABEL carlsim_epropexperiment)


add_compile_definitions(epropexperiment NC_EXTENSIONS_CARLSIM)   #\todo cmake option set by superbuild

#FIXME this must be sent int the superbuild
# features related to CARLsim axonal plasticity branch
add_compile_definitions(epropexperiment CARLSIM_AXONPLAST)   #\todo cmake option set by superbuild
add_compile_definitions(epropexperiment DEBUG_AXONPLAST)

add_compile_definitions(epropexperiment LN_AXON_PLAST)
add_compile_definitions(epropexperiment LN_I_CALC_TYPES)


find_package(carlsim REQUIRED)  

find_package(boost 1.75 REQUIRED COMPONENTS thread) 
 
target_sources(epropexperiment PRIVATE
    src/gui/EpropExptWidget.h
    src/gui/EpropExptWidget.cpp
    src/managers/EpropExptManager.h
    src/managers/EpropExptManager.cpp
    src/models/Maze.h
    src/models/Maze.cpp
    src/models/ExpCarlsimWrapper.h
    src/models/ExpCarlsimWrapper.cpp
    src/models/ResultsCursor.h
    #src/models/ResultsCursor.cpp
)
       
target_include_directories(epropexperiment PUBLIC
    .
    src
    src/gui
    src/managers
    src/models

	# CAUTION might induce a lib dependency that prevents loading the widget
    ../../monitors/oat/src
    ../../monitors/oat/src/gui
    ../../monitors/oat/src/managers
    ../../monitors/oat/src/models

    ../../../../library/include
    ../../../../applicationlibrary/include  
    ${Boost_INCLUDE_DIRS}
    ${CARLSIM_INCLUDE_DIR}
)

target_link_directories(epropexperiment PUBLIC
    ${Boost_LIBRARY_DIRS}
    ${CARLSIM_LIB_DIR}
	  ../$<$<CONFIG:Debug>:Debug:Release>	 # carlsimwrapper
	  ../monitors/oat/$<$<CONFIG:Debug>:Debug:Release>	 # oat lib
)

target_link_libraries(epropexperiment PUBLIC 
        spikestreamlib
        spikestreamapplication
        Qt::Gui
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
        ${Boost_LIBRARIES}
		carlsimwrapper
		carlsimoat  # as expected
        #${CARLSIM_LIBRARY}  # ISSUE could not load plugin
		carlsim$<$<CONFIG:Debug>:d>  # FIX provide Debug suffix
        carlsim-spike-generators$<$<CONFIG:Debug>:d>    		
)

install(TARGETS epropexperiment
        EXPORT epropexperiment
        RUNTIME 
            DESTINATION plugins/simulation/carlsimexperiments
            COMPONENT epropexperiment
)      

install( FILES 
  scripts/eprop_boone_train_loop_segment.m  
  scripts/eprop_boone_train_convergence.m
  scripts/eprop_boone_trial_landmark.m  
  scripts/eprop_morris_simulation_session_trial.m  
  DESTINATION m )