
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

add_library(vteexperiment SHARED)

set_property(TARGET vteexperiment PROPERTY PROJECT_LABEL carlsim_vteexperiment)

#add_compile_definitions(carlsimoat NC_EXTENSIONS_CARLSIM) 
add_compile_definitions(vteexperiment NC_EXTENSIONS_CARLSIM) 

find_package(carlsim REQUIRED)  

find_package(boost 1.75 REQUIRED COMPONENTS thread) 
 
target_sources(vteexperiment PRIVATE
    src/gui/VteExptWidget.h
    src/gui/VteExptWidget.cpp
    src/managers/VteExptManager.h
    src/managers/VteExptManager.cpp
)
       
target_include_directories(vteexperiment PUBLIC
    .
    src
    src/gui
    src/managers
    src/models
    ../../include
    ../../../../library/include
    ../../../../applicationlibrary/include  
    ${Boost_INCLUDE_DIRS}
    ${CARLSIM_INCLUDE_DIR}
)

target_link_directories(vteexperiment PUBLIC
    ${Boost_LIBRARY_DIRS}
    ${CARLSIM_LIB_DIR}
	  ../$<$<CONFIG:Debug>:Debug:Release>	 # carlsimwrapper
)

target_link_libraries(vteexperiment PUBLIC 
        spikestreamlib
        spikestreamapplication
        Qt::Gui
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
        ${CARLSIM_LIBRARY}
        carlsimwrapper
        carlsim-spike-generators$<$<CONFIG:Debug>:d>    
        ${Boost_LIBRARIES}
)

install(TARGETS vteexperiment
        EXPORT vteexperiment
        RUNTIME 
            DESTINATION plugins/simulation/carlsimexperiments
            COMPONENT vteexperiment
)      

