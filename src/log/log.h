#pragma once

#define _DEBUG 1

#if defined(_DEBUG)
#   define LOG_INFOF(format, ...)   fprintf(stdout, format, __VA_ARGS__)
#   define LOG_INFO(text)           fprintf(stdout, text)
#   define LOG_WARNF(format, ...)   fprintf(stderr, format, __VA_ARGS__)
#   define LOG_WARN(text)           fprintf(stderr, text)
#else
#   define LOG_INFOF(format, ...)   
#   define LOG_INFO(text)           
#   define LOG_WARNF(format, ...)   
#   define LOG_WARN(text)           
#endif
