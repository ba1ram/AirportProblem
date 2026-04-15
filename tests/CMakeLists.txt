include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(tests
    test_airport.cpp
    test_airport_database.cpp
    test_route_calculator.cpp
)

target_link_libraries(tests
    PRIVATE
    airport_lib
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)