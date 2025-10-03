cmake_minimum_required(VERSION 3.22)

string(TIMESTAMP BUILD_DATE "%Y.%m.%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")
string(TIMESTAMP BUILD_DATE_TIME "%Y.%m.%d-%H:%M:%S")

configure_file(
	project_info_in.h
	inc/project_info.h
)


project("project_info" VERSION 1.0 LANGUAGES C CXX)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
	/inc
)

