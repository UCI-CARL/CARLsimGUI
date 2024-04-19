
 set(CMAKE_AUTOMOC ON)
 set(CMAKE_AUTORSC ON)

  add_executable(carlsimgui
	  src/Main.cpp 
	  carlsimgui.rc
	) 
	


	target_include_directories(carlsimgui PRIVATE
		../applicationlibrary/include
		../library/include
	)        

	target_link_libraries(carlsimgui PUBLIC
		carlsimguilib 
		carlsimguiapplication    
		Qt::Gui
		Qt::OpenGL
		Qt::Sql
		Qt::Xml
	)
		

install(TARGETS carlsimgui
		EXPORT carlsimgui
		RUNTIME 
			DESTINATION $<$<CONFIG:Debug>:debug/>bin 
			COMPONENT carlsimgui
)      

install(FILES ../carlsimgui.config.template DESTINATION / )

install(DIRECTORY ../images DESTINATION $<$<CONFIG:Debug>:debug/>/ )

install(DIRECTORY ../plugins DESTINATION $<$<CONFIG:Debug>:debug/>/ )   
