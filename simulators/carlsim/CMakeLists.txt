
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

add_library(carlsimwrapper SHARED)

# CMake options for configuring CARLsim features.
add_compile_definitions(carlsimwrapper NC_EXTENSIONS_CARLSIM)  
add_compile_definitions(carlsimwrapper LN_I_CALC_TYPES)

# HINT: read config from CarlsimLib.h
#In file included from /home/ln/carlsimpp-t01/src/CARLsimGUI/simulators/carlsim/include/CarlsimLib.h:19,
# TODO Linux/Windows 
# check with CARLsim6 config  option(CARLSIM_LN_AXON_PLAST
# Windows 
#add_compile_definitions(carlsimwrapper CARLSIM_AXONPLAST)   
#add_compile_definitions(carlsimwrapper DEBUG_AXONPLAST)
#add_compile_definitions(carlsimwrapper LN_AXON_PLAST)
# Linux Pi5
if(CARLSIM_NO_CUDA)
  message("CARLSIM_NO_CUDA is on (carlsimwrapper)")
  add_compile_definitions(carlsimwrapper CARLSIM_NO_CUDA)
  add_compile_definitions(carlsimwrapper __NO_CUDA__)
endif()

find_package(carlsim REQUIRED)  
#find_package(carlsim REQUIRED HINTS ${CMAKE_INSTALL_PREFIX}/lib/carlsim )  

#find_package(boost 1.76 REQUIRED) 

set(HDRS
      include/AbstractSTDPFunction.h
      include/api.h
      include/CarlsimGenerator.h
      include/CarlsimGeneratorContainer.h
      include/CarlsimLib.h
      include/CarlsimLoader.h
      include/CarlsimParametersDialog.h
      include/CarlsimSpikeGenerator.h
      include/CarlsimAerSpikeGenerator.h
      include/CarlsimSpikeGeneratorContainer.h
      include/CarlsimWidget.h
      include/CarlsimWrapper.h
      include/carlsim_types.h
      include/DaSTDPFunction.h 
      include/DeviceLoaderWidget.h
      include/ExperimentLoaderWidget.h
      include/InjectorLoaderWidget.h
      include/ModelParametersDialog.h
      include/MonitorLoaderWidget.h
      include/NcMergedDockWidget.h
      include/NcMergedToolBar.h
      include/NcTimeDiffLabel.h
      include/Pattern.h
      include/PatternManager.h
      include/StandardSTDPFunction.h
      include/STDPFunctions.h
      include/StepSTDPFunction.h
      include/TimePanelWidget.h
      include/ui_TimePanel.h
)

# dir /b /s src\*.*   PSpad Block Copy
set(SRCS
    src/dialogs/CarlsimParametersDialog.cpp        
    src/dialogs/ModelParametersDialog.cpp      
        
    src/gui/CarlsimWidget.cpp                      
    src/gui/DeviceLoaderWidget.cpp                 
    src/gui/ExperimentLoaderWidget.cpp             
    src/gui/InjectorLoaderWidget.cpp               
    src/gui/MonitorLoaderWidget.cpp                
    src/gui/NcMergedDockWidget.cpp                 
    src/gui/NcMergedToolBar.cpp                    
    src/gui/NcTimeDiffLabel.cpp                    
    src/gui/TimePanelWidget.cpp  
                      
    src/managers/PatternManager.cpp  
                  
    src/model/AbstractSTDPFunction.cpp             
    src/model/CarlsimGenerator.cpp 
    src/model/CarlsimGeneratorContainer.cpp        
    src/model/CarlsimLib.cpp                       
    src/model/CarlsimLoader.cpp                    
    src/model/CarlsimSpikeGenerator.cpp            
    src/model/CarlsimAerSpikeGenerator.cpp 
    src/model/CarlsimSpikeGeneratorContainer.cpp   
    src/model/CarlsimWrapper.cpp                   
    src/model/carlsim_types.cpp                    
    src/model/DaSTDPFunction.cpp                   
    src/model/Pattern.cpp                          
    src/model/StandardSTDPFunction.cpp             
    src/model/STDPFunctions.cpp                    
    src/model/StepSTDPFunction.cpp 
)
        
source_group(Dialogs FILES
src/dialogs/CarlsimParametersDialog.cpp        
src/dialogs/ModelParametersDialog.cpp          
)
source_group(GUI FILES
src/gui/CarlsimWidget.cpp                      
src/gui/DeviceLoaderWidget.cpp                 
src/gui/ExperimentLoaderWidget.cpp             
src/gui/InjectorLoaderWidget.cpp               
src/gui/MonitorLoaderWidget.cpp                
src/gui/NcMergedDockWidget.cpp                 
src/gui/NcMergedToolBar.cpp                    
src/gui/NcTimeDiffLabel.cpp                    
src/gui/TimePanelWidget.cpp                    
)
source_group(Managers FILES
src/managers/PatternManager.cpp                
)
source_group(Model FILES
src/model/AbstractSTDPFunction.cpp             
src/model/CarlsimGenerator.cpp                 
src/model/CarlsimGeneratorContainer.cpp        
src/model/CarlsimLib.cpp                       
src/model/CarlsimLoader.cpp                    
src/model/CarlsimSpikeGenerator.cpp            
src/model/CarlsimAerSpikeGenerator.cpp            
src/model/CarlsimSpikeGeneratorContainer.cpp   
src/model/CarlsimWrapper.cpp                   
src/model/carlsim_types.cpp                    
src/model/DaSTDPFunction.cpp                   
src/model/Pattern.cpp                          
src/model/StandardSTDPFunction.cpp             
src/model/STDPFunctions.cpp                    
src/model/StepSTDPFunction.cpp
)               

target_sources(carlsimwrapper PRIVATE
        ${HDRS}
        ${SRCS}    
        carlsimwrapper.qrc 
        carlsimwrapper_de_DE.qm
        carlsimwrapper_en_US.qm		
)


target_include_directories(carlsimwrapper PUBLIC
        .
        include 
        ../../library/include
        ../../applicationlibrary/include  
        ${Boost_INCLUDE_DIR}
        ${CARLSIM_INCLUDE_DIR}
		#C:/Test/qt5/include/QtCore 
)


target_link_directories(carlsimwrapper PUBLIC
        ${Boost_LIBRARY_DIRS}
        ${CARLSIM_LIB_DIR}
        C:/Qwt-6.2.0/lib
)

target_link_libraries(carlsimwrapper PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Gui
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
        #${CARLSIM_LIBRARY}

#		# delegate CMake configuration to embedded lib CARLsim 
#        carlsim$<$<CONFIG:Debug>:d>.lib
#        carlsim-stopwatch$<$<CONFIG:Debug>:d>.lib
#        carlsim-spike-generators$<$<CONFIG:Debug>:d>.lib
#        carlsim-stopwatch$<$<CONFIG:Debug>:d>.lib

#		 # enforce released version 
#        carlsim.lib
#        carlsim-stopwatch.lib
#        carlsim-spike-generators.lib
#        carlsim-stopwatch.lib

		# delegate CMake configuration to embedded lib CARLsim 
        carlsim.so
        carlsim-stopwatch.so
        carlsim-spike-generators.so
        carlsim-stopwatch.so


#        "qwt$<$<CONFIG:Debug>:d>.lib"

# TODO Linux
#        C:\\Qwt-6.2.0\\lib\\qwtd.lib
# found in (base) ln@pi5:/usr/lib
	libqwt-qt5.so

)


install(TARGETS carlsimwrapper
        EXPORT carlsimwrapper
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/simulation
            COMPONENT carlsimwrapper
)      

