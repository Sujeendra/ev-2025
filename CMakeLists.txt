
cmake_minimum_required(VERSION 3.21.1)

option(LINK_INSIGHT "Link Qt Insight Tracker library" ON)
option(BUILD_QDS_COMPONENTS "Build design studio components" ON)

project(VehicleApplicationApp LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY}
    CACHE STRING "Import paths for Qt Creator's code model"
    FORCE
)
## please change this to the boot2qt directory
link_directories(C:/Qt/6.8.3/Boot2Qt/raspberrypi-armv8/toolchain/sysroots/cortexa53-poky-linux/usr/lib)

find_package(Qt6 6.7 REQUIRED COMPONENTS Core Gui Qml Quick VirtualKeyboard Sql Charts Graphs SerialBus)
qt_standard_project_setup()

# Add sysroot to the search paths for libraries


qt_add_executable(${CMAKE_PROJECT_NAME}
    gpiod.h
    WifiManager.h
    App/WifiManager.cpp
    SystemManager.h
    App/SystemManager.cpp
    LogManager.h
    App/LogManager.cpp
    Crc32Calculator.h
    App/Crc32Calculator.cpp
    DatabaseManager.h
    App/DatabaseManager.cpp
    CanWorker.h
    App/CanWorker.cpp
    SignalValue.h
    App/SignalValue.cpp
    SignalInfo.h
    SignalValueMap.h
    App/SignalValueMap.cpp
    utility.h
    App/utility.cpp
    TreeItem.h
    App/TreeItem.cpp
    TreeModel.h
    App/TreeModel.cpp
    TimeManager.h
    App/TimeManager.cpp

    fa.h


)
qt_add_resources(${CMAKE_PROJECT_NAME} "configuration"
    PREFIX "/"
    FILES
        qtquickcontrols2.conf)

include(qds)


if (BUILD_QDS_COMPONENTS)
    include(qmlcomponents OPTIONAL)
endif()

if (LINK_INSIGHT)
    include(insight OPTIONAL)
endif ()

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
## please change this to the boot2qt directory
## and copy the dll or .so from the dll folder of master thesis resources
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE C:/Qt/6.8.3/Boot2Qt/raspberrypi-armv8/toolchain/sysroots/cortexa53-poky-linux/usr/lib/libgpiod.so.3)
