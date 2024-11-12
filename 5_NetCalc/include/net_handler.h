#ifndef _NET_HANDLER_H
#define _NET_HANDLER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <endian.h>

#include "threadpool.h"
#include "signals.h"
#include "debug.h"
#include "calc_handle.h"
#include "calc_handle_thirty_two.h"
#include "file_io.h"
#include "math_funcs.h"
#include "math_funcs_thirty_two.h"

#define MAX_CONNECTIONS   20
#define MAX_FILENAME_LEN  32
#define NET_HEADER_SIZE   48
#define MAX_FILENAME_SIZE 24
#define EQUATION_SIZE     32
#define SOLUTION_SIZE     14
#define HEADER_SIZE       27

typedef struct con_args
{
    int8_t socket;
} con_args_t;

typedef struct net_header
{
    uint32_t header_size;
    uint32_t file_name_len;
    uint64_t file_size;
    char     filename[MAX_FILENAME_LEN];
} net_header_t;

typedef struct file_header     file_header_t;
typedef struct equation        equation_t;
typedef struct solved_equation solve_equation_t;

/**
 * @brief Sets up the thread connection
 *
 * @param args
 * @return void*
 */
void * handle_new_connection(void * args);

/**
 * @brief starts the listening server to the specified port
 *
 * @param port_number port number to listen on
 * @return int 0 for success non zero for failure
 */
int start_listener(char * port_number, threadpool_t * pool_p);

/**
 * @brief recieves and validates the network connection header
 *
 * @param socket socket to read from
 * @return int 0 on success non zero on error
 */
int read_net_header(int socket, net_header_t * header);

/**
 * @brief tries to guarantee receipt of all n bytes from socket
 *
 * @param socket socket to recieve from
 * @param n_bytes number of bytes 0 if read until done
 * @return int 0 for success non zero on error
 */
int receive_n(int socket, int n_bytes, char * buffer);

/**
 * @brief check that the file has a valid header
 *
 * @param file_stream file to check
 * @return int 0 if successful
 * @return int -1 if failed
 */
int check_header(int             file_des,
                 int64_t *       num_equations,
                 file_header_t * p_header);

/**
 * @brief logic for running through a file
 *
 * @param socket socket to read from
 * @return int 0 on success, -1 on failure
 */
int parse_stream(int socket, net_header_t * net_header_p);

/**
 * @brief Write results to a stream
 *
 * @param file_des file descriptor of the output file
 * @param calc_result calc_t for solution and operator
 * @param equation equation id
 * @return int 0 for success
 * @return int -1 for failure
 */
size_t write_stream(int file_des, calc_t * calc_result, equation_t * equation);

/**
 * @brief writes the network header to the stream(socket)
 *
 * @param stream socket or fd to write to
 * @param header input header with useful information
 * @return int 0 for success non zero for failure
 */
int write_net_header(int stream, net_header_t * header, int64_t num_equations);

/**
 * @brief writes the file header to the stream(socket)
 *
 * @param stream socket or fd to write to
 * @param file_header input file header with useful information
 * @return int 0 on success non zero for error
 */
int write_file_header(int stream, file_header_t * file_header);

/**
 * @brief outputs the appropriate netheader to the stream for a bad input header
 *
 * @param stream stream to write to
 * @return int 0 on success non zero for error
 */
int send_error_header(int stream);

#endif
