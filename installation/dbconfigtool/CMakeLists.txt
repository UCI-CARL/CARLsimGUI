
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
            DESTINATION bin 
            COMPONENT dbconfigtool
        LIBRARY
            DESTINATION lib
            COMPONENT dbconfigtool
        ARCHIVE
            DESTINATION lib
            COMPONENT dbconfigtool
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT dbconfigtool
)      

install(DIRECTORY ../../database DESTINATION / )
