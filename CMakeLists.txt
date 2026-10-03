cmake_minimum_required(VERSION 3.16)

project(rtos_system_health_monitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(
    rtos_core
    src/TaskScheduler.cpp
    src/HealthMonitor.cpp
    src/FaultInjector.cpp
    src/Watchdog.cpp
)

target_include_directories(
    rtos_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    rtos_core
    PUBLIC
    Threads::Threads
)

add_executable(
    rtos_health_monitor
    src/main.cpp
)

target_link_libraries(
    rtos_health_monitor
    PRIVATE
    rtos_core
)

enable_testing()

add_executable(
    test_health_monitor
    tests/test_health_monitor.cpp
)

target_link_libraries(
    test_health_monitor
    PRIVATE
    rtos_core
)

add_test(
    NAME HealthMonitorTests
    COMMAND test_health_monitor
)

add_executable(
    test_watchdog
    tests/test_watchdog.cpp
)

target_link_libraries(
    test_watchdog
    PRIVATE
    rtos_core
)

add_test(
    NAME WatchdogTests
    COMMAND test_watchdog
)