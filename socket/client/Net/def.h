/*************************************************************************
	> File Name: def.h
	> Author: 
	> Mail: 
	> Created Time: Mon 05 Nov 2018 11:45:53 AM CST
 ************************************************************************/

#ifndef _DEF_H
#define _DEF_H

#define DEFAULT_PORT 8080
#define MAX_BUFFER_LENGH 1024
#define DEFAULT_BACKLOG 32

/**
 * \bref NetHeader.type
 * \{
 */
#define NET_SEND 0
#define NET_ACK 1
/**
 *}/
 */

/**
 * \bref NetHeader.success
 * \{
 */
#define NET_SUCCESS 1
#define NET_FAILED 0
/**
 *\}
 */

/**
 * \bref data type in net
 * \{
 */
#define NET_FLOAT 1
#define NET_DOUBLE 2
#define NET_INT 3
#define NET_UINT 4
#define NET_UINT8 6

template <int NETTYPE>
struct get_type {
    using type = float;
};
template <>
struct get_type<NET_FLOAT> {
    using type = float;
};
template <>
struct get_type<NET_DOUBLE> {
    using type = double;
};
template <>
struct get_type<NET_INT> {
    using type = int;
};
template <>
struct get_type<NET_UINT> {
    using type = unsigned int;
};
template <>
struct get_type<NET_UINT8> {
    using type = unsigned char;
};



/**
 *\}
 */

typedef struct NetHeader {
    char type; // type
    char success; //success or faild
    char flag1; // reserved
    char flag2; // reserved
    int size; //size of following data, excluding the header size 8
}NetHeader;

#endif
