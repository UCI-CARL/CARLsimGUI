
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

add_library(wmexperiment SHARED)

set_property(TARGET wmexperiment PROPERTY PROJECT_LABEL carlsim_wmexperiment)

#FIXME this must be sent int the superbuild
add_compile_definitions(wmexperiment NC_EXTENSIONS_CARLSIM)   #\todo cmake option set by superbuild
add_compile_definitions(wmexperiment LN_I_CALC_TYPES)

find_package(carlsim REQUIRED)  

find_package(boost 1.75 REQUIRED COMPONENTS thread) 
 
target_sources(wmexperiment PRIVATE
    src/gui/WmExptWidget.h
    src/gui/WmExptWidget.cpp
    src/managers/WmExptManager.h
    src/managers/WmExptManager.cpp
    src/models/WmExpCarlsimWrapper.h
    src/models/WmExpCarlsimWrapper.cpp
)
       
target_include_directories(wmexperiment PUBLIC
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

target_link_directories(wmexperiment PUBLIC
    ${Boost_LIBRARY_DIRS}
    ${CARLSIM_LIB_DIR}
	../$<$<CONFIG:Debug>:Debug:Release>	 # carlsimwrapper
	../monitors/oat/$<$<CONFIG:Debug>:Debug:Release>	 # oat lib
)

target_link_libraries(wmexperiment PUBLIC 
	spikestreamlib
	spikestreamapplication
	Qt::Gui
	Qt::Xml
	Qt::OpenGL
	Qt::Sql
	${Boost_LIBRARIES}
	carlsimwrapper
	#carlsimoat$<$<CONFIG:Debug>:d>  # as expected
	carlsimoat

	${CARLSIM_LIBRARY}  # ISSUE could not load plugin
	carlsim$<$<CONFIG:Debug>:d>  # FIX provide Debug suffix
	carlsim-spike-generators$<$<CONFIG:Debug>:d>   
)

install(TARGETS wmexperiment
        EXPORT wmexperiment
        RUNTIME 
            DESTINATION plugins/simulation/carlsimexperiments
            COMPONENT wmexperiment
)      

#install( FILES 
#  scripts/eprop_boone_train_loop_segment.m  
#  DESTINATION m )