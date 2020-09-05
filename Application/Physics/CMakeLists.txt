CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(evphys)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(evphys
  src/Physics.cpp
  src/BulletState.cpp
)

add_subdirectory(bullet3)
list(APPEND EXTRA_INCLUDES "bullet3/src")
list(APPEND EXTRA_LIBS 
  BulletDynamics
  BulletCollision
  LinearMath
)


IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_subdirectory(PhysicsDebugWindow)
  list(APPEND EXTRA_INCLUDES "${evphys_SOURCE_DIR}/PhysicsDebugWindow/include")
  list(APPEND EXTRA_LIBS "PhysicsDebug")
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

IF(DEFINED GLOBAL_INCLUDES)
  list(APPEND EXTRA_INCLUDES ${GLOBAL_INCLUDES})
ENDIF(DEFINED GLOBAL_INCLUDES)

target_include_directories(evphys PUBLIC
  ${evphys_SOURCE_DIR}
  ${EXTRA_INCLUDES}
)

target_link_libraries(evphys
  ${EXTRA_LIBS}
)
