cmake_minimum_required(VERSION 3.22)


project("USB" VERSION 1.0 LANGUAGES C CXX)


add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC
	./inc
)

target_sources(${PROJECT_NAME} PRIVATE
	./usb.cpp
)

