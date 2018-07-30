cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(LSS)
set( EXE_NAME ${PROJECT_NAME} )
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" ABSOLUTE )
get_filename_component( INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" ABSOLUTE )
get_filename_component( SMLINC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/sml/include" ABSOLUTE )
get_filename_component( JNJ_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/Jinja2CppLight/src" ABSOLUTE )
get_filename_component( BUS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/EventBus/src/event" ABSOLUTE )
get_filename_component( MP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/micropather" ABSOLUTE )

# Write demo-config.h
message("Generating header file: ${CMAKE_BINARY_DIR}/config.hpp")
set(DEMO_ENABLE_MULTISAMPLE 0 CACHE BOOL "Enable multisample anti-aliasing")
configure_file(demo-config.h.in "${CMAKE_BINARY_DIR}/config.hpp")

# Find SDL2 and OpenGL
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

find_package( HarfBuzz REQUIRED )
find_package( Cairo REQUIRED )
find_package( Pango REQUIRED )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(NOT WIN32)
    find_package(OpenGL REQUIRED)
endif()

add_subdirectory(backward-cpp)

set( SRC_FILES
    ${BACKWARD_ENABLE}
    ${BUS_DIR}/EventBus.cpp
    ${BUS_DIR}/EventRegisttration.cpp
    
    "${JNJ_DIR}/stringhelper.cpp"
    "${JNJ_DIR}/Jinja2CppLight.cpp"

    ${MP_DIR}/micropather.cpp
    
	${SRC_DIR}/keyEvent.cpp

	${SRC_DIR}/fragment.cpp
	${SRC_DIR}/palette.cpp
	${SRC_DIR}/state.cpp

	${SRC_DIR}/modes.cpp
	${SRC_DIR}/commands.cpp

	${SRC_DIR}/game/cell.cpp
	${SRC_DIR}/game/fov.cpp
	${SRC_DIR}/game/location.cpp
	${SRC_DIR}/game/object.cpp
	${SRC_DIR}/game/effect.cpp
	${SRC_DIR}/game/overtime.cpp
	${SRC_DIR}/game/randomTools.cpp
	${SRC_DIR}/game/item.cpp
	${SRC_DIR}/game/creature.cpp
	${SRC_DIR}/game/enemy.cpp
	${SRC_DIR}/game/slot.cpp
	${SRC_DIR}/game/equipment.cpp
	${SRC_DIR}/game/player.cpp
	${SRC_DIR}/game/magic.cpp
	${SRC_DIR}/game/door.cpp

	${SRC_DIR}/generator/generator.cpp

	${SRC_DIR}/statusLine.cpp
	${SRC_DIR}/heroLine.cpp
	${SRC_DIR}/logPanel.cpp
	${SRC_DIR}/eventReactor.cpp
	${SRC_DIR}/LSSApp.cpp

	${SRC_DIR}/SDLPango.cpp
    ${INC_DIR}/gl2/Context.cpp

    )

# Define executable target
include_directories(
    PUBLIC ${INC_DIR} 
    PUBLIC ${SMLINC_DIR}
    PUBLIC ${JNJ_DIR}
    PUBLIC ${FOV_DIR}
    PUBLIC ${BUS_DIR}
    ${PANGO_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR})
add_executable(${EXE_NAME} ${SRC_FILES})
add_backward(${PROJECT_NAME})
target_link_libraries(${EXE_NAME}
  ${PANGO_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${CONAN_LIBS}
  Threads::Threads
  sndio
  )

# Copy SDL2 DLLs to output folder on Windows
if(WIN32)
    foreach(DLL ${SDL2_DLLS})
        add_custom_command(TARGET CMakeDemo POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:CMakeDemo>)
    endforeach()
endif()
