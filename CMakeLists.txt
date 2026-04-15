cmake_minimum_required(VERSION 3.16)
project(AirportProblem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем тестирование
enable_testing()

# Флаг покрытия
option(ENABLE_COVERAGE "Enable coverage" OFF)
option(BUILD_TESTS "Build tests" OFF)

function(enable_coverage target)
    if(ENABLE_COVERAGE)
        target_compile_options(${target} PRIVATE --coverage -O0 -g)
        target_link_options(${target} PRIVATE --coverage)
        target_link_libraries(${target} PRIVATE gcov)
    endif()
endfunction()


add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

