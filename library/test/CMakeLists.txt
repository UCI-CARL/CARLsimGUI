
 set(CMAKE_AUTOMOC ON)

  add_executable(testcarlsimguilib 
        src/Main.cpp
        src/TestAnalysisDao.cpp src/TestAnalysisDao.h
        src/TestArchiveDao.cpp src/TestArchiveDao.h
        src/TestConnection.cpp src/TestConnection.h
        src/TestDatabaseDao.cpp src/TestDatabaseDao.h
        src/TestMemory.cpp src/TestMemory.h
        src/TestNetwork.cpp src/TestNetwork.h
        src/TestNetworkDao.cpp src/TestNetworkDao.h
        src/TestNetworkDaoThread.cpp src/TestNetworkDaoThread.h
        src/TestNeuronGroup.cpp src/TestNeuronGroup.h
        src/TestRunner.cpp src/TestRunner.h
        src/TestUtil.cpp src/TestUtil.h
        src/TestWeightlessNeuron.cpp src/TestWeightlessNeuron.h
        src/TestXMLParameterParser.cpp src/TestXMLParameterParser.h
    ) 
    
            
    target_include_directories(testcarlsimguilib PRIVATE
        ../../testlibrary/include
        ../include
        src
    )        

    target_link_libraries(testcarlsimguilib PUBLIC
        carlsimguilib     
        carlsimguitest
        Qt::Gui
        Qt::Sql
        Qt::Xml
        Qt::Test
    )
        

install(TARGETS testcarlsimguilib
        EXPORT testcarlsimguilib
        RUNTIME 
            DESTINATION bin 
            COMPONENT testcarlsimguilib
        LIBRARY
            DESTINATION lib
            COMPONENT testcarlsimguilib
        ARCHIVE
            DESTINATION lib
            COMPONENT testcarlsimguilib
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT testcarlsimguilib
)     

install(DIRECTORY test_files 
        DESTINATION $<$<CONFIG:Debug>:debug/>library/test )

   