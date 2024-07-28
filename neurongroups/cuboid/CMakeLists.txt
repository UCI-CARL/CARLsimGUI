
add_library(cuboid SHARED)

set_property(TARGET cuboid PROPERTY PROJECT_LABEL neurongroups_cuboid)


set(HDRS
      src/gui/CuboidWidget.h
			src/model/CuboidBuilderThread.h
)

set(SRCS
      src/gui/CuboidWidget.cpp
			src/model/CuboidBuilderThread.cpp
)
                
qt_generate_moc(src/gui/CuboidWidget.h moc/moc_CuboidWidget.cpp TARGET cuboid)
qt_generate_moc(src/model/CuboidBuilderThread.h moc/moc_CuboidBuilderThread.cpp TARGET cuboid)

set(MOCS
        moc/moc_CuboidWidget.cpp
        moc/moc_CuboidBuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})


target_sources(cuboid PRIVATE
        ${HDRS}
        ${SRCS}
        ${MOCS}        
        cuboid_resource.rc    
)

target_include_directories(cuboid PUBLIC
        src 
        src/model 
        src/gui 
        ../../library/include
        ../../applicationlibrary/include  
)

target_link_libraries(cuboid PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
)


install(TARGETS cuboid
        EXPORT cuboid
        LIBRARY 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/neurons
            COMPONENT cuboid
)      

