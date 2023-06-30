cmake_minimum_required(VERSION 3.25)
project(lua C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(lua
        lua.c
        lua_arginfo.h
        php_lua.h)


set(PHP_SOURCE ../php-8.2.7)

include_directories(${PHP_SOURCE}/main)
include_directories(${PHP_SOURCE}/Zend)
include_directories(${PHP_SOURCE}/sapi)
include_directories(${PHP_SOURCE}/pear)
include_directories(${PHP_SOURCE})
