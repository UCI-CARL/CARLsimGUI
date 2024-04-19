
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)


add_library(carlsimoat SHARED)
#set_property(TARGET carlsimoat PROPERTY PROJECT_LABEL oat)

add_compile_definitions(carlsimoat NC_EXTENSIONS_CARLSIM) 

find_package(carlsim REQUIRED)  

find_package(boost 1.76 REQUIRED COMPONENTS thread) 

# dir /b /s src/*.h
set(HDRS
         src/gui/api.h
         src/gui/CarlsimOatConnectionReportDialog.h
         src/gui/CarlsimOatGroupReportDialog.h
         src/gui/CarlsimOatNeuronReportDialog.h
         src/gui/CarlsimOatReportDialog.h
         src/gui/CarlsimOatSpikeReportDialog.h
         src/gui/CarlsimOatTableView.h
         src/gui/CarlsimOatWidget.h
         src/managers/CarlsimOatManager.h
         src/models/CarlsimOatConnectionMonitor.h
         src/models/CarlsimOatGroupMonitor.h
         src/models/CarlsimOatModel.h
         src/models/CarlsimOatMonitor.h
         src/models/CarlsimOatNeuronMonitor.h
         src/models/CarlsimOatSpikeMonitor.h
         src/models/CarlsimOatXmlLoader.h
)

# dir /b /s src/*.*   PSpad Block Copy
set(SRCS
         src/gui/CarlsimOatConnectionReportDialog.cpp
         src/gui/CarlsimOatGroupReportDialog.cpp
         src/gui/CarlsimOatNeuronReportDialog.cpp
         src/gui/CarlsimOatReportDialog.cpp
         src/gui/CarlsimOatSpikeReportDialog.cpp
         src/gui/CarlsimOatTableView.cpp
         src/gui/CarlsimOatWidget.cpp
         src/managers/CarlsimOatManager.cpp
         src/models/CarlsimOatConnectionMonitor.cpp
         src/models/CarlsimOatGroupMonitor.cpp
         src/models/CarlsimOatModel.cpp
         src/models/CarlsimOatMonitor.cpp
         src/models/CarlsimOatNeuronMonitor.cpp
         src/models/CarlsimOatSpikeMonitor.cpp
         src/models/CarlsimOatXmlLoader.cpp
)

# Option: Source Hierarchical -> Better for Development and Design, harder for searching 
source_group("Header Files\\GUI" REGULAR_EXPRESSION src/gui/.*\.h)
source_group("Header Files\\Managers" REGULAR_EXPRESSION src/managers/.*\.h)
source_group("Header Files\\Models" REGULAR_EXPRESSION src/models/.*\.h)

# Option: Header Hierachical -> Better for Development, harder for DevOps, Migration, .. e.g. multiple select files
source_group("Source Files\\GUI" REGULAR_EXPRESSION src/gui/.*\.cpp)
source_group("Source Files\\Managers" REGULAR_EXPRESSION src/managers/.*\.cpp)
source_group("Source Files\\Models" REGULAR_EXPRESSION src/models/.*\.cpp)


target_sources(carlsimoat PRIVATE
        ${HDRS}
        ${SRCS}    
)


target_include_directories(carlsimoat PUBLIC
        .
        src
        src/gui
        src/managers
        src/models
        ../../include
        ../../../../library/include
        ../../../../applicationlibrary/include  
        ${CMAKE_INSTALL_PREFIX}/include    # same fix as in finspikes2
        ${Boost_INCLUDE_DIRS}
        ${CARLSIM_INCLUDE_DIR}
)


target_link_directories(carlsimoat PUBLIC
    ${Boost_LIBRARY_DIRS}
	  ${CMAKE_INSTALL_PREFIX}/bin  
    ${CARLSIM_LIB_DIR}
	../$<$<CONFIG:Debug>:Debug:Release>	 # carlsimwrapper
)

target_link_libraries(carlsimoat PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Gui
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
        #${CARLSIM_LIBRARY}
        carlsimwrapper
        ${Boost_LIBRARIES}
		
		carlsim$<$<CONFIG:Debug>:d>.lib
        carlsim-stopwatch$<$<CONFIG:Debug>:d>.lib
        carlsim-spike-generators$<$<CONFIG:Debug>:d>.lib
)

install(TARGETS carlsimoat
        EXPORT carlsimoat
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/simulation/carlsimmonitors
            COMPONENT carlsimoat
)      

