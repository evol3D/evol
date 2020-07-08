cmake_minimum_required(VERSION 3.1)

project(evol)

option(DebugMode "Enable debugging features." ON)
option(ColoredLogs "Use color coding for different logging levels. (DebugMode Needed)" ON)

if(DebugMode)
	add_compile_definitions(DEBUG)

	if(ColoredLogs)
		add_compile_definitions(LOG_USE_COLOR)
	endif()
endif()

add_subdirectory(Application/Window/glfw)

add_executable(evol
		src/main.c
		Application/App.h
		Application/App.c
		include/ev_log/rxi_logger/log.c
		include/globals.h Application/Window/Window.c Application/Window/Window.h)

target_include_directories(evol PUBLIC
		include
		Application
        ${evol_SOURCE_DIR}
		${evol_BINARY_DIR}
		Application/Window/glfw/include
)

target_link_libraries(evol glfw)
