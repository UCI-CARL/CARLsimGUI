
add_library(carlsimcnswb0 SHARED)

set_property(TARGET carlsimcnswb0 PROPERTY PROJECT_LABEL neurongroups_carlsimcnswb0)

find_package(boost 1.76)

set(HDRS
      src/gui/CARLsimCNSWbWidget.h
			src/model/CARLsimCNSWbBuilderThread.h
)

set(SRCS
      src/gui/CARLsimCNSWbWidget.cpp
			src/model/CARLsimCNSWbBuilderThread.cpp
)
        
qt_generate_moc(src/gui/CARLsimCNSWbWidget.h moc/moc_CARLsimCNSWbWidget.cpp TARGET carlsimcnswb0)
qt_generate_moc(src/model/CARLsimCNSWbBuilderThread.h moc/moc_CARLsimCNSWbBuilderThread.cpp TARGET carlsimcnswb0)

set(MOCS
        moc/moc_CARLsimCNSWbWidget.cpp
        moc/moc_CARLsimCNSWbBuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})

target_sources(carlsimcnswb0 PRIVATE
        ${HDRS}
        ${SRCS}
        ${MOCS}
        carlsimcnswb0_resource.rc   
)

target_include_directories(carlsimcnswb0 PUBLIC
        src 
        src/model 
        src/gui 
        ../../library/include
        ../../applicationlibrary/include  
		    ${CMAKE_INSTALL_PREFIX}/include
        ${Boost_INCLUDE_DIR}
)

target_link_directories(carlsimcnswb0 PUBLIC
	${CMAKE_INSTALL_PREFIX}/bin  
)

target_link_libraries(carlsimcnswb0 PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
)


install(TARGETS carlsimcnswb0
        EXPORT carlsimcnswb0
        LIBRARY 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/neurons
            COMPONENT carlsimcnswb0
)      

