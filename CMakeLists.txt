cmake_minimum_required(VERSION 2.8)

project(OscarPapa)

SET(OPIFEX_ENGINE_REPOSITORY CACHE STRING "../OPengine/")

# Sets up all of your application needs
# it then sets up all of the OPifex Engine links
include(CMake/application.cmake)




#########################################
# LAYER 9 - ApplicationGame
#########################################

# Include Directories
include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/src/game
	.
	..
)

# Include directories for the engine itself
# Plus all of the external libraries needed
add_external_opifex_includes()


# Application Source Files
add_source_group(src "${PROJECT_SOURCE_DIR}/src/game" APPLICATION_GAME_SRC)

add_library(9_Game STATIC ${APPLICATION_GAME_SRC})

add_opifex_definitions(9_Game "")
add_opifex_defines()
set_target_properties(9_Game PROPERTIES OUTPUT_NAME "OscarPapaGame")
add_opifex_libraries(9_Game)
add_opifex_assets(9_Game)



#########################################
# LAYER 9 - Application
#########################################

# Include Directories
include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/src/app
	${PROJECT_SOURCE_DIR}/src/game
	.
	..
)

# Include directories for the engine itself
# Plus all of the external libraries needed
add_external_opifex_includes()


# Application Source Files
add_source_group(src "${PROJECT_SOURCE_DIR}/src/app" APPLICATION_SRC)

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

    target_link_libraries(9_Application 9_Game)
endif()

add_opifex_definitions(9_Application "")
add_opifex_defines()
set_target_properties(9_Application PROPERTIES OUTPUT_NAME "OscarPapa")
add_opifex_libraries(9_Application)
add_opifex_assets(9_Application)



#########################################
# LAYER 9 - Application Server
#########################################

# Include Directories
include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/src/appServer
	${PROJECT_SOURCE_DIR}/src/game
	.
	..
)

# Include directories for the engine itself
# Plus all of the external libraries needed
add_external_opifex_includes()


# Application Source Files
add_source_group(src "${PROJECT_SOURCE_DIR}/src/appServer" APPLICATION_SERVER_SRC)

# Application BUILD
if("${OPIFEX_OS}" STREQUAL "OPIFEX_IOS")
    if(${OPIFEX_OPTION_SHARED})
    	add_library(9_Application SHARED ${APPLICATION_SRC})
    else()
    	add_library(9_Application STATIC ${APPLICATION_SRC})
    endif()
    output_library_from_app(9_Application "OscarPapa")
else()
    add_executable(9_ApplicationServer ${APPLICATION_SERVER_SRC})
    target_link_libraries(9_ApplicationServer 9_Game)
endif()

add_opifex_definitions(9_ApplicationServer "")
add_opifex_defines()
set_target_properties(9_ApplicationServer PROPERTIES OUTPUT_NAME "OscarPapaServer")
add_opifex_libraries(9_ApplicationServer)
add_opifex_assets(9_ApplicationServer)


opifex_engine_status_messages()
