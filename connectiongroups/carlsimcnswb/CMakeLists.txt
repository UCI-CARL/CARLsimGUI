
add_library(carlsimcnswb SHARED)

set_property(TARGET carlsimcnswb PROPERTY PROJECT_LABEL connectiongroups_carlsimcnswb)

set(HDRS
      src/gui/CARLsimCNSWbWidget.h
			src/model/CARLsimCNSWbBuilderThread.h
)

set(SRCS
      src/gui/CARLsimCNSWbWidget.cpp
			src/model/CARLsimCNSWbBuilderThread.cpp
)
        
qt_generate_moc(src/gui/CARLsimCNSWbWidget.h moc/moc_CARLsimCNSWbWidget.cpp TARGET carlsimcnswb)
qt_generate_moc(src/model/CARLsimCNSWbBuilderThread.h moc/moc_CARLsimCNSWbBuilderThread.cpp TARGET carlsimcnswb)

set(MOCS
        moc/moc_CARLsimCNSWbWidget.cpp
        moc/moc_CARLsimCNSWbBuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})


target_sources(carlsimcnswb PRIVATE
        ${HDRS}
        ${SRCS}
        ${MOCS}        
        carlsimcnswb_resource.rc
)


target_include_directories(carlsimcnswb PUBLIC
        src 
        src/model 
        src/gui 
        ../../library/include
        ../../applicationlibrary/include  
)

target_link_libraries(carlsimcnswb PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
)


install(TARGETS carlsimcnswb
        EXPORT carlsimcnswb
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/connections
            COMPONENT carlsimcnswb
)      

