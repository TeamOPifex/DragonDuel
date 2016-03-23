cmake_minimum_required(VERSION 2.8)

project(OscarPapa)

SET(OPIFEX_ENGINE_REPOSITORY CACHE STRING "../OPengine/")

# Sets up all of your application needs
# it then sets up all of the OPifex Engine links
include(CMake/application.cmake)




#########################################
# LAYER 9 - Application
#########################################

# Include Directories
include_directories(
	${PROJECT_SOURCE_DIR}
	.
	..
)

# Include directories for the engine itself
# Plus all of the external libraries needed
add_external_opifex_includes()


# Application Source Files
add_source_group(src "${PROJECT_SOURCE_DIR}/src" APPLICATION_SRC)

# Application BUILD
if("${OPIFEX_OS}" STREQUAL "OPIFEX_IOS")
    if(${OPIFEX_OPTION_SHARED})
    	add_library(9_Application SHARED ${APPLICATION_SRC})
    else()
    	add_library(9_Application STATIC ${APPLICATION_SRC})
    endif()
    output_library_from_app(9_Application "OscarPapa")
else()
    add_executable(9_Application ${APPLICATION_SRC})
endif()

add_opifex_definitions(9_Application "")
add_opifex_defines()
set_target_properties(9_Application PROPERTIES OUTPUT_NAME "OscarPapa")
add_opifex_libraries(9_Application)
add_opifex_assets(9_Application)


opifex_engine_status_messages()
