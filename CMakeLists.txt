
cmake_minimum_required(VERSION 3.0.0)

cmake_policy(VERSION 3.0.0)

set(PRJ_NAME "glslScene")
set(PRJ_EXENAME "glslscene")

set(PRJ_USE_GLEW true)

project(${PRJ_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")

set(CMAKE_CXX_STANDARD 11)

if (WIN32)
	set(SDL2_PATH "lib/SDL2-2.0.10")
endif()

find_package(SDL2 REQUIRED)
if (PRJ_USE_GLEW)
	if (WIN32)
		set(CMAKE_PREFIX_PATH "lib/glew-2.1.0")
		set(CMAKE_LIBRARY_PATH "lib/glew-2.1.0/lib/Release/Win32/")
	endif()
	find_package(GLEW REQUIRED)
endif()
find_package(OpenGL REQUIRED)

if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Winit-self -Wstrict-aliasing -Wfloat-equal -Wno-comment -Wno-missing-field-initializers -Wno-unused -fPIC -felide-constructors -Wreorder")
	set (CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0")
	set (CMAKE_CXX_FLAGS_RELEASE  "-O3")
endif ()

file(GLOB_RECURSE PRJ_HEADERFILES include/*.h external/tml/include/*.h
		external/imgui/imconfig.h
		external/imgui/imgui.h
		external/imgui/examples/imgui_impl_opengl2.h
		external/imgui/examples/imgui_impl_opengl3.h
		external/imgui/examples/imgui_impl_sdl.h
		external/imgui/imgui_internal.h
		external/imgui/imstb_rectpack.h
		external/imgui/imstb_textedit.h
		external/imgui/imstb_truetype.h
		)

file(GLOB_RECURSE PRJ_SOURCEFILES src/*.cpp external/tml/src/cfg/*.cpp external/tml/src/tml/*.cpp
		external/imgui/imgui.cpp
		external/imgui/imgui_demo.cpp
		external/imgui/imgui_draw.cpp
		external/imgui/examples/imgui_impl_opengl2.cpp
		external/imgui/examples/imgui_impl_opengl3.cpp
		external/imgui/examples/imgui_impl_sdl.cpp
		external/imgui/imgui_widgets.cpp
		)


file(GLOB PRJ_PICOC_ROOT_SOURCEFILES external/picoc/*.c)
file(GLOB PRJ_PICOC_CSTDLIB_SOURCEFILES external/picoc/cstdlib/*.c)
set(PRJ_PICOC_SOURCEFILES ${PRJ_PICOC_ROOT_SOURCEFILES} ${PRJ_PICOC_CSTDLIB_SOURCEFILES})

file(GLOB PRJ_PICOC_HEADERFILES external/picoc/*.h)

if (WIN32)
	set(PRJ_PICOC_SOURCEFILES
			${PRJ_PICOC_SOURCEFILES}
			"external/picoc/platform/platform_msvc.c"
			"external/picoc/platform/library_msvc.c")
else()
	set(PRJ_PICOC_SOURCEFILES
			${PRJ_PICOC_SOURCEFILES}
			"external/picoc/platform/platform_unix.c"
			"external/picoc/platform/library_unix.c")
endif()
add_definitions( -DPICOC_SKIP_MAIN )
add_definitions( -DIMGUI_IMPL_OPENGL_LOADER_GLEW )

set(PRJ_C_SOURCEFILES
		${PRJ_PICOC_SOURCEFILES}
		"src/gs/logic/capi_example_logic.c"
		)

set(PRJ_C_HEADERFILES ${PRJ_PICOC_HEADERFILES})

add_executable(${PRJ_EXENAME} ${PRJ_HEADERFILES} ${PRJ_C_HEADERFILES} ${PRJ_SOURCEFILES} ${PRJ_C_SOURCEFILES})

target_include_directories(${PRJ_EXENAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_include_directories(${PRJ_EXENAME} PRIVATE "${CMAKE_SOURCE_DIR}/external/tml/include")
target_include_directories(${PRJ_EXENAME} PRIVATE "${CMAKE_SOURCE_DIR}/external/picoc")
target_include_directories(${PRJ_EXENAME} PRIVATE "${CMAKE_SOURCE_DIR}/external/imgui")

target_include_directories(${PRJ_EXENAME} PRIVATE
	${SDL2_INCLUDE_DIR}
	)

if (PRJ_USE_GLEW)
	target_include_directories(${PRJ_EXENAME} PRIVATE
		${GLEW_INCLUDE_DIRS}
		)
endif()

if (WIN32)
target_include_directories(${PRJ_EXENAME} PRIVATE
	"${CMAKE_SOURCE_DIR}/lib/glm"
	)
endif()

target_link_libraries(${PRJ_EXENAME}
	${SDL2_LIBRARY}
	${OPENGL_LIBRARIES}
	)

if (PRJ_USE_GLEW)
	target_link_libraries(${PRJ_EXENAME}
		${GLEW_LIBRARIES}
		)
endif()

if (WIN32)
target_link_libraries(${PRJ_EXENAME} Ws2_32)
endif()

if (UNIX)
# dl lib is necessary for glad/opengl under linux
# readline is for picoc
target_link_libraries(${PRJ_EXENAME} dl readline)
endif()

if (WIN32)
set(_src_root_path "${CMAKE_SOURCE_DIR}")

foreach(_source IN ITEMS ${PRJ_SOURCEFILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${PRJ_HEADERFILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${PRJ_PICOC_ROOT_SOURCEFILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    #set(_source_path_rel "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${PRJ_PICOC_CSTDLIB_SOURCEFILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${PRJ_C_HEADERFILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
endif()
