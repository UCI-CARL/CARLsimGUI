
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)


add_library(noiseinjector SHARED)
set_property(TARGET noiseinjector PROPERTY PROJECT_LABEL carlsimnoiseinjector)

# TODO: read config from CarlsimLib.h
#add_compile_definitions(noiseinjector NC_EXTENSIONS_CARLSIM) 

find_package(carlsim REQUIRED)  

find_package(Boost 1.74 EXACT REQUIRED COMPONENTS thread) # 1.74 Pi OS 5


set(HDRS
  src/gui/NoiseInjectorTableView.h
  src/gui/NoiseInjectorWidget.h
  src/managers/NoiseInjectorManager.h
  src/models/CarlsimSpikeGenerator.h
  src/models/CarlsimSpikeGeneratorContainer.h
  src/models/NoiseInjectorModel.h
  src/models/NoiseInjectorXmlLoader.h
)

# dir /b /s src/*.*   PSpad Block Copy
set(SRCS
  src/gui/NoiseInjectorTableView.cpp
  src/gui/NoiseInjectorWidget.cpp
  src/managers/NoiseInjectorManager.cpp
  src/models/CarlsimSpikeGenerator.cpp
  src/models/CarlsimSpikeGeneratorContainer.cpp
  src/models/NoiseInjectorModel.cpp
  src/models/NoiseInjectorXmlLoader.cpp
)
        
source_group(GUI FILES
  src/gui/NoiseInjectorTableView.cpp
  src/gui/NoiseInjectorWidget.cpp
)

source_group(Managers FILES
  src/managers/NoiseInjectorManager.cpp
)

source_group(Model FILES
  src/models/CarlsimSpikeGenerator.cpp
  src/models/CarlsimSpikeGeneratorContainer.cpp
  src/models/NoiseInjectorModel.cpp
  src/models/NoiseInjectorXmlLoader.cpp
)               

target_sources(noiseinjector PRIVATE
        ${HDRS}
        ${SRCS}    
)

target_include_directories(noiseinjector PUBLIC
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

target_link_directories(noiseinjector PUBLIC
    ${Boost_LIBRARY_DIRS}
    ${CARLSIM_LIB_DIR}
	../$<$<CONFIG:Debug>:Debug:Release>
)

target_link_libraries(noiseinjector PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Gui
        Qt::Xml
        Qt::OpenGL
        Qt::Sql

        carlsimwrapper

        #${CARLSIM_LIBRARY}
#		carlsim$<$<CONFIG:Debug>:d>.lib
#        carlsim-spike-generators$<$<CONFIG:Debug>:d>

	# Patch Linux Pi5
        carlsim.so
        carlsim-spike-generators.so

        ${Boost_LIBRARIES}
)


install(TARGETS noiseinjector
        EXPORT noiseinjector
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/simulation/carlsiminjectors
            COMPONENT noiseinjector
)      

