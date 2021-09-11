cmake_minimum_required(VERSION 3.21.1)

project(qmacs VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_policy(SET CMP0100 NEW)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(qmacs
	buffer.cc
	buffer-model.cc
	buffer-picker.cc
	frame.cc
	main.cc
	window.cc
)

target_link_libraries(qmacs PRIVATE Qt6::Widgets)
