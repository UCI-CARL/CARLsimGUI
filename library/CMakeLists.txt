

#####################################################################
## carlsimguilib Generic Library:
#####################################################################

add_library(carlsimguilib SHARED)

#set_property(TARGET carlsimguilib PROPERTY PROJECT_LABEL carlsimgui_dll)

#    EXCEPTIONS
#    OUTPUT_DIRECTORY "$${CARLSIMGUI_ROOT_DIR}/lib"
#    SOURCES

    set(HDRS 
        include/api.h
        include/Box.h
        include/GlobalVariables.h      
        include/PerformanceTimer.h   
        include/RGBColor.h
        include/SpikeStreamThread.h
        include/SpikeStreamTypes.h  
        include/Util.h
        include/XMLParameterParser.h
    )

    set(model_HDRS
        include/AnalysisInfo.h
        include/Archive.h
        include/ArchiveInfo.h  
        include/Connection.h
        include/ConnectionGroup.h
        include/ConnectionGroupInfo.h
        include/Network.h
        include/NetworkInfo.h
        include/Neuron.h
        include/NeuronGroup.h
        include/NeuronGroupInfo.h
        include/NeuronType.h
        include/ParameterInfo.h
        include/Point3D.h
        include/SynapseType.h        
        include/WeightlessNeuron.h
    )

    set(file_HDRS
        include/ConfigEditor.h
        include/ConfigLoader.h
    )

    set(database_HDRS
        include/AbstractDao.h
        include/AnalysisDao.h
        include/ArchiveDao.h
        include/DBInfo.h
        include/DatabaseDao.h
        include/NetworkDao.h
        include/NetworkDaoThread.h
    )
    set(exceptions_HDRS     
        include/NumberConversionException.h
        include/SpikeStreamAnalysisException.h
        include/SpikeStreamDBException.h
        include/SpikeStreamException.h
        include/SpikeStreamIOException.h
        include/SpikeStreamSimulationException.h
        include/SpikeStreamXMLException.h
    )


    set(SRCS     
        src/Box.cpp
        src/PerformanceTimer.cpp
        src/RGBColor.cpp
        src/SpikeStreamThread.cpp
        src/Util.cpp
        src/xml/XMLParameterParser.cpp
    )
    set(database_SRCS     
        src/database/AbstractDao.cpp
        src/database/AnalysisDao.cpp
        src/database/ArchiveDao.cpp
        src/database/DBInfo.cpp
        src/database/DatabaseDao.cpp
        src/database/NetworkDao.cpp
        src/database/NetworkDaoThread.cpp
    )
    set(exceptions_SRCS     
        src/exceptions/NumberConversionException.cpp
        src/exceptions/SpikeStreamAnalysisException.cpp
        src/exceptions/SpikeStreamDBException.cpp
        src/exceptions/SpikeStreamException.cpp
        src/exceptions/SpikeStreamIOException.cpp
        src/exceptions/SpikeStreamSimulationException.cpp
        src/exceptions/SpikeStreamXMLException.cpp
    )
    set(file_SRCS     
        src/file/ConfigEditor.cpp
        src/file/ConfigLoader.cpp
    )
    set(model_SRCS     
        src/model/AnalysisInfo.cpp
        src/model/Archive.cpp
        src/model/ArchiveInfo.cpp
        src/model/Connection.cpp
        src/model/ConnectionGroup.cpp
        src/model/ConnectionGroupInfo.cpp
        src/model/Network.cpp
        src/model/NetworkInfo.cpp
        src/model/Neuron.cpp
        src/model/NeuronGroup.cpp
        src/model/NeuronGroupInfo.cpp
        src/model/NeuronType.cpp
        src/model/ParameterInfo.cpp
        src/model/Point3D.cpp
        src/model/SynapseType.cpp
        src/model/WeightlessNeuron.cpp
    )

    source_group("Header Files\\Exceptions" FILES ${exceptions_HDRS})
    source_group("Header Files\\Database" FILES ${database_HDRS})    
    source_group("Header Files\\File" FILES ${file_HDRS})
    source_group("Header Files\\Model" FILES ${model_HDRS})


target_sources(carlsimguilib PRIVATE
        ${HDRS}
        ${database_HDRS}
        ${exceptions_HDRS}
        ${file_HDRS}
        ${model_HDRS}
        ${SRCS}
        ${database_SRCS}
        ${exceptions_SRCS}
        ${file_SRCS}
        ${model_SRCS}
        generated/moc_SpikeStreamThread.cpp
        generated/moc_NetworkDaoThread.cpp
        generated/moc_Network.cpp
)

qt_generate_moc(include/SpikeStreamThread.h generated/moc_SpikeStreamThread.cpp TARGET carlsimguilib)
qt_generate_moc(include/NetworkDaoThread.h generated/moc_NetworkDaoThread.cpp TARGET carlsimguilib)
qt_generate_moc(include/Network.h generated/moc_Network.cpp TARGET carlsimguilib)
                                                                                                     
#    INCLUDE_DIRECTORIES
#        include
#    PUBLIC_LIBRARIES

#add_compile_definitions(carlsimguilib CARLSIMGUI_BUILD_LIB )

target_include_directories(carlsimguilib PRIVATE
		.
    include
)

# Since Qt 5.15, the CMake targets are also available as Qt::Core, Qt::Gui, and so on. 
# This eases writing CMake code that can work with both Qt 5 and Qt 6.
# https://doc.qt.io/qt-5/cmake-get-started.html#imported-library-targets
#
target_link_libraries(carlsimguilib
        Qt::Core
        Qt::Gui
        Qt::Sql
        Qt::Xml

# patch https://stackoverflow.com/questions/15459191/libgl-so-libglu-so-and-libglut-so-are-not-found-in-the-standard-path
#
	gmp
)



set_property(TARGET carlsimguilib PROPERTY PUBLIC_HEADER
        ${HDRS}
        ${database_HDRS}
        ${exceptions_HDRS}
        ${file_HDRS}
        ${model_HDRS} 
)

install(TARGETS carlsimguilib
        EXPORT carlsimguilib
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>bin 
            COMPONENT carlsimguilib
        LIBRARY
            DESTINATION $<$<CONFIG:Debug>:debug/>lib
            COMPONENT carlsimguilib
        ARCHIVE
            DESTINATION $<$<CONFIG:Debug>:debug/>lib
            COMPONENT carlsimguilib
        PUBLIC_HEADER
            DESTINATION $<$<CONFIG:Debug>:debug/>include
            COMPONENT carlsimguilib
)      
    
    
    
