cmake_minimum_required(VERSION 2.8) 

cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0003 OLD)

set (TARGET final)

set (Boost_USE_STATIC_LIBS    OFF)
set (Boost_USE_MULTITHREADED   ON)
set (Boost_USE_STATIC_RUNTIME OFF)

#add_definitions(-DBOOST_ASIO_ENABLE_HANDLER_TRACKING)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03")

#######################################
find_package(Boost REQUIRED COMPONENTS system date_time program_options thread)
if(NOT Boost_FOUND)
   message(SEND_ERROR "Failed to find boost")
   return()
else()
   include_directories(${Boost_INCLUDE_DIRS})
endif()
#######################################

add_executable(${TARGET} final.cpp HttpServer.cpp MimeTypes.cpp Reply.cpp RequestParser.cpp FileHandler.cpp)

target_link_libraries(${TARGET} ${Boost_LIBRARIES})


