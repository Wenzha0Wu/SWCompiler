/*************************************************************************
	> File Name: tools/tools.h
	> Author: cgn
	> Mail: 
	> Created Time: Thu 11 Oct 2018 06:31:41 PM CST
 ************************************************************************/

#ifndef _AT_TOOLS_H
#define _AT_TOOLS_H

#include <type_traits>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <memory>
#include <utility>


/**
 * \bref log data using printf
 */
#define LOG(format, ...) do { \
        printf("[%s, %s:%d %s] " format "\n", \
        __TIME__, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    } while(0)

/**
 * \bref log char* data
 */
#define LOGINFO(msg) do { \
        printf("[%s, %s:%d %s] %s\n", \
        __TIME__, __FILE__, __LINE__, __FUNCTION__, msg); \
    } while(0)

/**
 * \bref debug exec, execute if ATDEBUG is defined
 */
#ifdef ATDEBUG

#define EXECDEBUG(exec) do { \
    exec; \
    } while(0)

#else

#define EXECDEBUG(exec) do {} while(0)

#endif

/**
 * \bref logdebug, print if ATDEBUG is defined
 */
#ifdef ATDEBUG // if ATDEBUG is defined, print the msg

#define LOGDEBUG(format, ...) do { \
        printf("[%s, %s:%d %s] " format "\n", \
        __TIME__, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    } while(0)

#else // if ATDEBUG is not-defined, do nothing.

#define LOGDEBUG(format, ...) do { } while(0)

#endif

/**
 * \bref check flag
 */
#define CHECK(flag) do { \
            if(!(flag)) { \
                printf("[%s, %s:%d %s] check failure!!!\n", \
                    __TIME__, __FILE__, __LINE__, __FUNCTION__ );\
                exit(-1); \
            } \
        } while(0)

#endif
