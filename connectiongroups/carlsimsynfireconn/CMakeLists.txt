
add_library(carlsimsynfireconn SHARED)

set_property(TARGET carlsimsynfireconn PROPERTY PROJECT_LABEL connectiongroups_carlsimsynfireconn)

set(HDRS
	src/gui/CARLsimSynfireConnWidget.h
	src/model/CARLsimSynfireConnBuilderThread.h
)

set(SRCS
	src/gui/CARLsimSynfireConnWidget.cpp
	src/model/CARLsimSynfireConnBuilderThread.cpp
)
        
qt_generate_moc(src/gui/CARLsimSynfireConnWidget.h moc/moc_CARLsimSynfireConnWidget.cpp TARGET carlsimsynfireconn)
qt_generate_moc(src/model/CARLsimSynfireConnBuilderThread.h moc/moc_CARLsimSynfireConnBuilderThread.cpp TARGET carlsimsynfireconn)

set(MOCS
	moc/moc_CARLsimSynfireConnWidget.cpp
	moc/moc_CARLsimSynfireConnBuilderThread.cpp
)

source_group("Generated Files" FILES ${MOCS})


target_sources(carlsimsynfireconn PRIVATE
	${HDRS}
	${SRCS}
	${MOCS}        
	carlsimsynfireconn_resource.rc
)


target_include_directories(carlsimsynfireconn PUBLIC
	src 
	src/model 
	src/gui 
	../../library/include
	../../applicationlibrary/include  
)

target_link_libraries(carlsimsynfireconn PUBLIC 
	carlsimguilib
	carlsimguiapplication
	Qt::Xml
	Qt::OpenGL
	Qt::Sql
)


install(TARGETS carlsimsynfireconn
	EXPORT carlsimsynfireconn
	RUNTIME 
		DESTINATION $<$<CONFIG:Debug>:debug/>plugins/connections
		COMPONENT carlsimsynfireconn
)      

