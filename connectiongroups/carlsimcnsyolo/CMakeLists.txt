
add_library(carlsimcnsyolo SHARED)

set_property(TARGET carlsimcnsyolo PROPERTY PROJECT_LABEL connectiongroups_carlsimcnsyolo)

set(HDRS
      src/gui/CARLsimCNSYoloWidget.h
			src/model/CARLsimCNSYoloBuilderThread.h
)

set(SRCS
      src/gui/CARLsimCNSYoloWidget.cpp
			src/model/CARLsimCNSYoloBuilderThread.cpp
)
        
qt_generate_moc(src/gui/CARLsimCNSYoloWidget.h moc/moc_CARLsimCNSYoloWidget.cpp TARGET carlsimcnsyolo)
qt_generate_moc(src/model/CARLsimCNSYoloBuilderThread.h moc/moc_CARLsimCNSYoloBuilderThread.cpp TARGET carlsimcnsyolo)

set(MOCS
        moc/moc_CARLsimCNSYoloWidget.cpp
        moc/moc_CARLsimCNSYoloBuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})


target_sources(carlsimcnsyolo PRIVATE
        ${HDRS}
        ${SRCS}
        ${MOCS}        
        carlsimcnsyolo_resource.rc
)


target_include_directories(carlsimcnsyolo PUBLIC
        src 
        src/model 
        src/gui 
        ../../library/include
        ../../applicationlibrary/include  
)

target_link_libraries(carlsimcnsyolo PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
)


install(TARGETS carlsimcnsyolo
        EXPORT carlsimcnsyolo
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/connections
            COMPONENT carlsimcnsyolo
)      

