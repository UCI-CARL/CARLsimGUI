
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

add_library(exampleexperiement SHARED)

set_property(TARGET exampleexperiement PROPERTY PROJECT_LABEL carlsim_exampleexperiement)

add_compile_definitions(carlsimoat NC_EXTENSIONS_CARLSIM) 

find_package(carlsim REQUIRED)  

find_package(boost 1.75 REQUIRED COMPONENTS thread) 
 
target_sources(exampleexperiement PRIVATE
    src/gui/ExampleExptWidget.h
    src/gui/ExampleExptWidget.cpp
    src/managers/ExampleExptManager.h
    src/managers/ExampleExptManager.cpp
)
       
target_include_directories(exampleexperiement PUBLIC
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

target_link_directories(exampleexperiement PUBLIC
    ${Boost_LIBRARY_DIRS}
    C:/Test/FinSpikes-3.0/bin
    ${CARLSIM_LIB_DIR}
	  ../$<$<CONFIG:Debug>:Debug:Release>	 # carlsimwrapper
)

target_link_libraries(exampleexperiement PUBLIC 
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

install(TARGETS exampleexperiement
        EXPORT exampleexperiement
        LIBRARY 
            DESTINATION plugins/simulation/carlsimexperiments
            COMPONENT exampleexperiement
)      

#install(DIRECTORY 
#        scripts 
#        DESTINATION plugins/simulation/nemoexperiments/temporalcodingexpt1 )
