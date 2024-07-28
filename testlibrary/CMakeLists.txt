
add_library(carlsimguitest SHARED
        include/api.h
        include/TestDao.h
        src/TestDao.cpp
        moc/moc_TestDao.cpp
)

qt_generate_moc(include/TestDao.h moc/moc_TestDao.cpp TARGET carlsimguitest)

#add_compile_definitions(carlsimguitest CARLSIMGUI_BUILD_TEST_LIB )

target_include_directories(carlsimguitest PUBLIC
        include
        ../library/include
)

target_link_libraries(carlsimguitest PUBLIC 
        carlsimguilib
        Qt::Core
        Qt::Gui
        Qt::Sql
        Qt::Test
)


set_property(TARGET carlsimguitest PROPERTY PUBLIC_HEADER
        include/TestDao.h
)

install(TARGETS carlsimguitest
        EXPORT carlsimguitest
        RUNTIME 
            DESTINATION $<$<CONFIG:Debug>:debug/>bin 
            COMPONENT carlsimguitest
        LIBRARY
            DESTINATION $<$<CONFIG:Debug>:debug/>lib
            COMPONENT carlsimguitest
        ARCHIVE
            DESTINATION $<$<CONFIG:Debug>:debug/>lib
            COMPONENT carlsimguitest
        PUBLIC_HEADER
            DESTINATION $<$<CONFIG:Debug>:debug/>include
            COMPONENT carlsimguitest
)      

