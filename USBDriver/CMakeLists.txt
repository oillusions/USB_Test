cmake_minimum_required(VERSION 3.22)


project("USBDriver" VERSION 1.0 LANGUAGES C CXX)


add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
	./inc
)

target_sources(${PROJECT_NAME} INTERFACE
	usb_driver.cpp
)
