set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_library(airport_lib
    core/Airport.cpp
    core/AirportDatabase.cpp
    core/RouteCalculator.cpp
)

enable_coverage(airport_lib)

target_include_directories(airport_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(airport_lib
    PUBLIC Qt6::Core
)

add_executable(app
    ui/main.cpp
    ui/AppController.cpp
    ui/AirportTableModel.cpp
    ui/MainWindow.cpp
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(app
    PRIVATE airport_lib Qt6::Core Qt6::Widgets
)