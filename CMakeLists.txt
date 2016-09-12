cmake_minimum_required(VERSION 3.0)
project(openwrt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES1 Server.cpp Net2Serial.cpp Net2Serial.h Messages.cpp Messages.h)

set(SOURCE_FILES2 Client.cpp Net2Serial.cpp Net2Serial.h Messages.cpp Messages.h)

add_executable(Server ${SOURCE_FILES1})

add_executable(Client ${SOURCE_FILES2})