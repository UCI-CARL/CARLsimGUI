
add_library(random1 SHARED)

set_property(TARGET random1 PROPERTY PROJECT_LABEL connectiongroups_random1)

set(HDRS
      src/gui/Random1Widget.h
			src/model/Random1BuilderThread.h
)

set(SRCS
      src/gui/Random1Widget.cpp
			src/model/Random1BuilderThread.cpp
)
             
qt_generate_moc(src/gui/Random1Widget.h moc/moc_Random1Widget.cpp TARGET random1)
qt_generate_moc(src/model/Random1BuilderThread.h moc/moc_Random1BuilderThread.cpp TARGET random1)

set(MOCS
        moc/moc_Random1Widget.cpp
        moc/moc_Random1BuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})


target_sources(random1 PRIVATE
        ${HDRS}
        ${SRCS}
        ${MOCS}        
        random1_resource.rc
)

target_include_directories(random1 PUBLIC
        src 
        src/model 
        src/gui 
        ../../library/include
        ../../applicationlibrary/include  
)

target_link_libraries(random1 PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
)


install(TARGETS random1
        EXPORT random1
        LIBRARY 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/connections
            COMPONENT random1
)      

