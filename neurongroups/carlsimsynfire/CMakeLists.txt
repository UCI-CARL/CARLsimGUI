
add_library(carlsimsynfire SHARED)

set_property(TARGET carlsimsynfire PROPERTY PROJECT_LABEL neurongroups_carlsimsynfire)

find_package(boost 1.76)

set(HDRS
    src/gui/CARLsimSynfireWidget.h
	src/model/CARLsimSynfireBuilderThread.h
)

set(SRCS
    src/gui/CARLsimSynfireWidget.cpp
	src/model/CARLsimSynfireBuilderThread.cpp
)
        
qt_generate_moc(src/gui/CARLsimSynfireWidget.h moc/moc_CARLsimSynfireWidget.cpp TARGET carlsimsynfire)
qt_generate_moc(src/model/CARLsimSynfireBuilderThread.h moc/moc_CARLsimSynfireBuilderThread.cpp TARGET carlsimsynfire)

set(MOCS
    moc/moc_CARLsimSynfireWidget.cpp
    moc/moc_CARLsimSynfireBuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})

target_sources(carlsimsynfire PRIVATE
    ${HDRS}
    ${SRCS}
    ${MOCS}
    carlsimsynfire_resource.rc   
)

target_include_directories(carlsimsynfire PUBLIC
    src 
    src/model 
    src/gui 
    ../../library/include
    ../../applicationlibrary/include  
	${CMAKE_INSTALL_PREFIX}/include
    ${Boost_INCLUDE_DIR}
)

target_link_directories(carlsimsynfire PUBLIC
	${CMAKE_INSTALL_PREFIX}/bin  
)

target_link_libraries(carlsimsynfire PUBLIC 
    carlsimguilib
    carlsimguiapplication
    Qt::Xml
    Qt::OpenGL
    Qt::Sql
)


install(TARGETS carlsimsynfire
    EXPORT carlsimsynfire
    RUNTIME 
		DESTINATION $<$<CONFIG:Debug>:debug/>plugins/neurons
        COMPONENT carlsimsynfire
)      

