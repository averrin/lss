# PangoBasic
cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set(CMAKE_CXX_STANDARD 17)
# set( CMAKE_VERBOSE_MAKEFILE on )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Cinder" ABSOLUTE )
# get_filename_component( CINDER_DIR "/home/averrin/projects/Cinder/lib/linux/x86_64/ogl/Debug/" ABSOLUTE )
#include( ${CINDER_DIR}/linux/cmake/Cinder.cmake )

project( LSS )

get_filename_component( PANGO_BLOCK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include/Cinder-Pango/src" ABSOLUTE )
get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )
get_filename_component( INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE )
get_filename_component( SMLINC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include/sml/include" ABSOLUTE )
get_filename_component( JNJ_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include/Jinja2CppLight/src" ABSOLUTE )
get_filename_component( BUS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include/EventBus/src/event" ABSOLUTE )

if( NOT TARGET cinder${CINDER_LIB_SUFFIX} )
    find_package( cinder REQUIRED
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../Cinder/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
        $ENV{Cinder_DIR}/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
    )
endif()

set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

# Find and include Pango and dependencies.
find_package( HarfBuzz REQUIRED )
find_package( Cairo REQUIRED )
find_package( Pango REQUIRED )

# Use PROJECT_NAME since CMAKE_PROJET_NAME returns the top-level project name.
set( EXE_NAME ${PROJECT_NAME} )

set( SRC_FILES
    ${BACKWARD_ENABLE}
    ${BUS_DIR}/EventBus.cpp
    ${BUS_DIR}/EventRegisttration.cpp
    
    "${JNJ_DIR}/stringhelper.cpp"
    "${JNJ_DIR}/Jinja2CppLight.cpp"

	${SRC_DIR}/format.cc

	${SRC_DIR}/fragment.cpp
	${SRC_DIR}/palette.cpp
	${SRC_DIR}/state.cpp

	${SRC_DIR}/modes.cpp
	${SRC_DIR}/commands.cpp

	${SRC_DIR}/game/cell.cpp
	${SRC_DIR}/game/location.cpp
	${SRC_DIR}/game/object.cpp
	${SRC_DIR}/game/item.cpp
	${SRC_DIR}/game/creature.cpp
	${SRC_DIR}/game/enemy.cpp
	${SRC_DIR}/game/player.cpp
	${SRC_DIR}/game/door.cpp

	${SRC_DIR}/statusLine.cpp
	${SRC_DIR}/LSSApp.cpp
    ${PANGO_BLOCK_SRC_DIR}/CinderPango.cpp
)

add_subdirectory(backward-cpp)

add_executable( "${EXE_NAME}" ${SRC_FILES} )

add_backward(LSS)

target_include_directories(
	"${EXE_NAME}"
    PUBLIC ${INC_DIR} 
    PUBLIC ${SMLINC_DIR}
    PUBLIC ${JNJ_DIR}
    PUBLIC ${FOV_DIR}
    PUBLIC ${BUS_DIR}
           ${PANGO_BLOCK_SRC_DIR}
           ${HARFBUZZ_INCLUDE_DIRS}
           ${CAIRO_INCLUDE_DIRS}
           ${PANGO_INCLUDE_DIRS}
           
)

target_link_libraries( "${EXE_NAME}" cinder${CINDER_LIB_SUFFIX} ${HARFBUZZ_LIBRARIES} ${CAIRO_LIBRARIES} ${PANGO_LIBRARIES} )
