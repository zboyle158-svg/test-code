# ESP-IDF工程的顶层CMake配置文件。
# 官方构建系统说明：
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html

# 指定所需的最低CMake版本。
cmake_minimum_required(VERSION 3.16)

# IDF_PATH是ESP-IDF环境变量；该文件负责载入ESP-IDF的构建规则。
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# 定义工程名称，构建产物通常也会使用该名称。
project(01_led)
