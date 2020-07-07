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

add_executable(evol
		src/main.c
		Application/App.h
		Application/App.c
		include/ev_log/rxi_logger/log.c
)

target_include_directories(evol PUBLIC
		include
        ${evol_SOURCE_DIR}
		${evol_BINARY_DIR}
)

