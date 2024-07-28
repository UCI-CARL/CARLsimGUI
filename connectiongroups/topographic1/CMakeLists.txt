
add_library(topographic1 SHARED)

set_property(TARGET topographic1 PROPERTY PROJECT_LABEL connectiongroups_topographic1)

set(HDRS
      src/gui/Topographic1Widget.h
			src/model/Topographic1BuilderThread.h
)

set(SRCS
      src/gui/Topographic1Widget.cpp
			src/model/Topographic1BuilderThread.cpp
)
             
qt_generate_moc(src/gui/Topographic1Widget.h moc/moc_Topographic1Widget.cpp TARGET topographic1)
qt_generate_moc(src/model/Topographic1BuilderThread.h moc/moc_Topographic1BuilderThread.cpp TARGET topographic1)

set(MOCS
        moc/moc_Topographic1Widget.cpp
        moc/moc_Topographic1BuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})


target_sources(topographic1 PRIVATE
        ${HDRS}
        ${SRCS}
        ${MOCS}        
        topographic1_resource.rc     
)


target_include_directories(topographic1 PUBLIC
        src 
        src/model 
        src/gui 
        ../../library/include
        ../../applicationlibrary/include  
)

target_link_libraries(topographic1 PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
)


install(TARGETS topographic1
        EXPORT topographic1
        LIBRARY 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/connections 
            COMPONENT topographic1
)      

