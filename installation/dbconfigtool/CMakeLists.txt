
 set(CMAKE_AUTOMOC ON)

  add_executable(dbconfigtool 

      src/DBConfigMainWindow.h 
			src/DBDetailsWidget.h
			src/SuccessWidget.h

      src/Main.cpp 
			src/DBConfigMainWindow.cpp 
			src/DBDetailsWidget.cpp 
			src/SuccessWidget.cpp


    ) 
    
            
    target_include_directories(dbconfigtool PRIVATE
        src
        ../../library/include
    )        

    target_link_libraries(dbconfigtool PUBLIC
        carlsimguilib     
        Qt::Gui
        Qt::Sql
        Qt::Xml
        Qt::Core
        Qt::Widgets
    )
        
         
set_property(TARGET carlsimguilib PROPERTY PUBLIC_HEADER
        src/DBConfigMainWindow.h 
		src/DBDetailsWidget.h
		src/SuccessWidget.h
)

install(TARGETS dbconfigtool
        EXPORT dbconfigtool
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>bin 
            COMPONENT dbconfigtool
        LIBRARY
            DESTINATION $<$<CONFIG:Debug>:debug/>lib
            COMPONENT dbconfigtool
        ARCHIVE
            DESTINATION $<$<CONFIG:Debug>:debug/>lib
            COMPONENT dbconfigtool
        PUBLIC_HEADER
            DESTINATION $<$<CONFIG:Debug>:debug/>include
            COMPONENT dbconfigtool
)      

install(DIRECTORY ../../database DESTINATION ${CMAKE_INSTALL_PREFIX} )
