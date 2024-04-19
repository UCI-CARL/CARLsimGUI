
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

add_library(cns SHARED)
set_property(TARGET cns PROPERTY PROJECT_LABEL carlsimcns)

add_compile_definitions(cns NC_EXTENSIONS_CARLSIM) 

find_package(carlsim REQUIRED)  

find_package(boost 1.76 REQUIRED COMPONENTS thread) 

set(HDRS
src/dialogs/AddChannelDialog.h
src/dialogs/EditPropertiesDialog.h
src/gui/CnsWidget.h
src/managers/CnsManager.h
src/managers/CnsXmlLoader.h
src/models/ChannelModel.h
src/views/ChannelTableView.h
)

# dir /b /s src/*.*   PSpad Block Copy
set(SRCS
src/dialogs/AddChannelDialog.cpp
src/dialogs/EditPropertiesDialog.cpp
src/gui/CnsWidget.cpp
src/managers/CnsManager.cpp
src/managers/CnsXmlLoader.cpp
src/models/ChannelModel.cpp
src/views/ChannelTableView.cpp 
)
        
source_group(Dialogs FILES
src/dialogs/AddChannelDialog.cpp
src/dialogs/EditPropertiesDialog.cpp
)

source_group(GUI FILES
src/gui/CnsWidget.cpp
)
source_group(Managers FILES
src/managers/CnsManager.cpp
src/managers/CnsXmlLoader.cpp
)
source_group(Model FILES
src/views/ChannelTableView.cpp 
)               
source_group(View FILES
src/views/ChannelTableView.cpp 
)               

target_sources(cns PRIVATE
        ${HDRS}
        ${SRCS}    
        cns.qrc 
        cns_de_DE.qm
        cns_en_US.qm
)

target_include_directories(cns PUBLIC
        .
        src
        src/dialogs
        src/gui
        src/managers
        src/models
        src/views
        ../../include
        ../../../../library/include
        ../../../../applicationlibrary/include  
		    ${CMAKE_INSTALL_PREFIX}/include
        ${Boost_INCLUDE_DIRS}
        ${CARLSIM_INCLUDE_DIR}
)

target_link_directories(cns PUBLIC
    ${Boost_LIBRARY_DIRS}
	  ${CMAKE_INSTALL_PREFIX}/lib  # CARLSIM
	${CMAKE_INSTALL_PREFIX}/bin  # Cns
)

target_link_libraries(cns PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Gui
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
        #${CARLSIM_LIBRARY}
        carlsim$<$<CONFIG:Debug>:d>.lib
        ${Boost_LIBRARIES}
        CARLsimIO$<$<CONFIG:Debug>:d>
)

install(TARGETS cns
        EXPORT cns
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/simulation/carlsimdevices
            COMPONENT cns
)      

