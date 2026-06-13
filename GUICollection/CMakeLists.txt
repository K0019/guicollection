file(GLOB UI_SOURCES *.cpp *.h *.ipp)

add_executable(GUICollection ${UI_SOURCES})
source_group("Source Files" FILES ${UI_SOURCES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

target_link_libraries(GUICollection PRIVATE imgui glm glfw ${OPENGL_LIBRARIES})

set_target_properties(GUICollection PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")