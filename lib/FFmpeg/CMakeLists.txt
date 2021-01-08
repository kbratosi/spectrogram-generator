cmake_minimum_required(VERSION 3.1)

project(FFmpeg)

find_package(PkgConfig REQUIRED QUIET)
pkg_check_modules(avcodec    REQUIRED IMPORTED_TARGET libavcodec   )
pkg_check_modules(avformat   REQUIRED IMPORTED_TARGET libavformat  )
pkg_check_modules(avutil     REQUIRED IMPORTED_TARGET libavutil    )
pkg_check_modules(swresample REQUIRED IMPORTED_TARGET libswresample)