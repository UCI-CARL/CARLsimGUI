
add_library(izhikevichetworks SHARED)

set(HDRS
      src/gui/IzhikevichNetworksWidget.h
			src/model/IzhikevichNetworkBuilder.h
)

set(SRCS
      src/gui/IzhikevichNetworksWidget.cpp
			src/model/IzhikevichNetworkBuilder.cpp
)
        
      
qt_generate_moc(src/gui/IzhikevichNetworksWidget.h moc/moc_IzhikevichNetworksWidget.cpp TARGET izhikevichetworks)
qt_generate_moc(src/model/IzhikevichNetworkBuilder.h moc/moc_IzhikevichNetworkBuilder.cpp TARGET izhikevichetworks)

set(MOCS
        moc/moc_IzhikevichNetworksWidget.cpp
        moc/moc_IzhikevichNetworkBuilder.cpp
)

source_group("Generated Files" FILES ${MOCS})


target_sources(izhikevichetworks PRIVATE
        ${HDRS}
        ${SRCS}
        ${MOCS}        
        izhikevichetworks_resource.rc     
)


target_include_directories(izhikevichetworks PUBLIC
        src 
        src/model 
        src/gui 
        ../../library/include
        ../../applicationlibrary/include
)

target_link_libraries(izhikevichetworks PUBLIC 
        carlsimguilib
        carlsimguiapplication
        Qt::Xml
        Qt::OpenGL
        Qt::Sql
)


install(TARGETS izhikevichetworks
        EXPORT izhikevichetworks
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/networks
            COMPONENT izhikevichetworks
)      

