cmake_minimum_required(VERSION 3.1)

project(evol)

set(CMAKE_BUILD_TYPE Debug)
option(DebugMode "Enable debugging features." ON)
option(ColoredLogs "Use color coding for different logging levels. (DebugMode Needed)" ON)

set(EVENT_DIRECTORY ${evol_SOURCE_DIR}/include/events)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(DebugMode)
  add_compile_definitions(DEBUG)

  if(ColoredLogs)
    add_compile_definitions(LOG_USE_COLOR)
  endif()
endif()

add_subdirectory(Application/Window/glfw)
add_subdirectory(Application/EventSystem)
add_subdirectory(Application/World/flecs)

add_executable(evol
    src/main.c
    Application/App.c
    include/ev_log/rxi_logger/log.c
    Application/Window/Window.c
    Application/Input/Input.c
    Application/EventDebug/EventDebug.c
    Application/World/World.c
    )

target_include_directories(evol PUBLIC
    include
    Application
    ${evol_SOURCE_DIR}
    ${evol_BINARY_DIR}
    Application/Window/glfw/include
    Application/EventSystem/include
    Application/World/flecs/include
)

target_include_directories(event_system PUBLIC
    Application
)

target_link_libraries(evol
  glfw
  event_system
  flecs
  )


configure_file(${event_system_SOURCE_DIR}/extern/pthreads4w/dll/x64/pthreadVC2.dll
    ${evol_BINARY_DIR}/pthreadVC2.dll COPYONLY)
