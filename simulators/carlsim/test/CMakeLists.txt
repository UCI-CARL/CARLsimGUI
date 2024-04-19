

project(testcarlsim LANGUAGES CXX)

find_package(carlsim REQUIRED)  

# https://doc.qt.io/qt-6/qtest-overview.html#building-with-cmake-and-ctest
#find_package(Qt6 REQUIRED COMPONENTS Test)
#find_package(Qt5 REQUIRED COMPONENTS Test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

    
enable_testing(true)

add_executable(testcarlsim 		
    src/TestRunner.h
	src/TestCarlsimLibrary.h
	src/TestCarlsimWrapper.h
	src/Main.cpp
	src/TestRunner.cpp
	src/TestCarlsimLibrary.cpp
	src/TestCarlsimWrapper.cpp			
	
) 
           
	
add_test(NAME mytest COMMAND testCarlsimLibrary)


    target_include_directories(testcarlsim PRIVATE
        src
#        ${CARLSIM_INCLUDE_DIR}
		C:\\Qwt-6.2.0-dev\\include 
    )        
    
    target_link_directories(testcarlsim PUBLIC
        ${CARLSIM_LIB_DIR}
        C:/Qwt-6.2.0-dev/lib
)
    target_link_libraries(testcarlsim PUBLIC
        carlsimguilib     
        carlsimguiapplication
		    Qt::Core
        Qt::Gui
        Qt::Sql
        Qt::Test
        carlsimwrapper
        carlsim$<$<CONFIG:Debug>:d>.lib
        carlsim-stopwatch$<$<CONFIG:Debug>:d>.lib
        carlsim-spike-generators$<$<CONFIG:Debug>:d>.lib
        carlsim-stopwatch$<$<CONFIG:Debug>:d>.lib
        qwt$<$<CONFIG:Debug>:d>.lib
    )

install(TARGETS testcarlsim
        EXPORT testcarlsim
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>plugins/simulation
            COMPONENT testcarlsim
)     



   