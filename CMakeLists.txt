cmake_minimum_required(VERSION 3.20)

project(CARLsimGUI
    VERSION 1.0.0
    LANGUAGES CXX
)

if(MSVC)
  # enforce parallel build as default
  get_property(_USER_CMAKE_CXX_FLAGS CACHE CMAKE_CXX_FLAGS PROPERTY VALUE)
  string(FIND ${_USER_CMAKE_CXX_FLAGS} "/MP" _MSVC_MP_IDX)
  if(_MSVC_MP_IDX EQUAL -1)
    message("enforce parallel build")
    set(CMAKE_CXX_FLAGS "${_USER_CMAKE_CXX_FLAGS} /MP" CACHE STRING "enforce parallel build" FORCE)    
  endif()
endif()

option(CARLSIMGUI_USES_CARLSIM "x" TRUE)
#option(CARLSIMGUI_USES_CARLSIMIO "x" TRUE)
option(CARLSIMGUI_USES_CARLSIMIO "x" FALSE)

# Publish for all projects
if(CARLSIMGUI_USES_CARLSIM) 
	add_compile_definitions(CARLSIMGUI_USES_CARLSIM)
	add_compile_definitions(LN_FIX_PROTOTYPE_IDS)	
	add_compile_definitions(LN_FIX_POSITION_KEY)
	add_compile_definitions(CARLSIMGUI$<$<CONFIG:Debug>:_DEBUG>_PLUGINS)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(Qt5 5.15.13 COMPONENTS Core Xml Gui Sql Test Widgets OpenGL PrintSupport REQUIRED)
find_package(Qt5 5.15 COMPONENTS Core Xml Gui Sql Test Widgets OpenGL PrintSupport REQUIRED)

add_compile_definitions(WIN32_SPIKESTREAM)


# core system 
add_subdirectory(library)

add_subdirectory(testlibrary)

#add_subdirectory(library/test)
  
add_subdirectory(applicationlibrary)
  
add_subdirectory(application)
  
add_subdirectory(installation/dbconfigtool)
  
# basic plugins
add_subdirectory(neurongroups/cuboid)
add_subdirectory(connectiongroups/random1)
add_subdirectory(connectiongroups/topographic1)
if(CARLSIMGUI_USES_CARLSIMIO)
  add_subdirectory(neurongroups/carlsimcnswb) 
  add_subdirectory(connectiongroups/carlsimcnswb)
endif()   
add_subdirectory(networks/izhikevichnetworks)

  
if(CARLSIMGUI_USES_CARLSIM)  
   add_subdirectory(simulators/carlsim) 
   add_subdirectory(simulators/carlsim/test)
   add_subdirectory(simulators/carlsim/injectors/noise)
   add_subdirectory(simulators/carlsim/monitors/oat)
   if(CARLSIMGUI_USES_CARLSIMIO)
     add_subdirectory(simulators/carlsim/devices/cns) 
   endif()  
endif()
  



                                                                  
