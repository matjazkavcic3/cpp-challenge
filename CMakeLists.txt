cmake_minimum_required (VERSION 2.6)
project (cpp-challenge)

set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)


find_package( OpenCV REQUIRED )
find_library ( ZMQ_LIBRARY NAMES zmq )

add_executable(cpp-challenge main.cpp)
target_link_libraries(cpp-challenge ${OpenCV_LIBS} )
target_link_libraries(cpp-challenge ${ZMQ_LIBRARY})

install (TARGETS cpp-challenge DESTINATION bin)
