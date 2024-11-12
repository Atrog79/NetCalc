#include "net_handler.h"

union convert
{
    int64_t  result;
    uint64_t u_result;
};

struct __attribute__((__packed__)) file_header // total of 27 bytes
{
    int32_t magic;
    int64_t file_id;
    int64_t num_equations;
    int8_t  is_solved;
    int32_t offset;
    int16_t reserved;
};

struct __attribute__((__packed__)) equation // total of 27 bytes
{
    int32_t equation_id;
    int8_t  reserved;
    int64_t num1;
    int8_t  operator;
    int64_t num2;
    // 10 bytes of padding
    int64_t padding1;
    int16_t padding2;
};

struct __attribute__((__packed__)) solved_equation // total of 27 bytes
{
    int32_t equation_id;
    int8_t  is_solved;
    int8_t  solution_type;
    int64_t solution;
};

struct calc
{
    union convert num1;
    union convert num2;
    union convert result;
    int           oper;
    math_func     func_ptr;
};

/**
 * @brief accepts the connection and adds it to the jobpool
 * 
 * @param parentfd file descriptor of the connection
 * @param their_addr necessary info for the accept 
 * @param addrlen length of the value
 * @param pool_p threadpool to add the job into
 * @param c_args command arguements, basically just a socket id 
 */
static void accept_connection(int parentfd, struct sockaddr_storage * their_addr, socklen_t addrlen, threadpool_t * pool_p, con_args_t * c_args);

int start_listener(char * port_number, threadpool_t * pool_p)
{
    int parentfd   = 0;
    int err        = ERROR;
    int optval     = 1; // for use with setsockopt()
    
    struct addrinfo         hints      = { 0 };
    struct addrinfo *       result     = { 0 };
    struct sockaddr_storage their_addr = { 0 };

    socklen_t addrlen = sizeof(int);

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if (SUCCESS != (err = getaddrinfo(NULL, port_number, &hints, &result)))
    {
        ERR_MSG("getaddrinfo() failed %s\n", gai_strerror(err));
        goto END;
    }
    con_args_t * c_args = calloc(1, sizeof(con_args_t));
    if (NULL == c_args)
    {
        goto END;
    }

    parentfd =
        socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (0 > parentfd)
    {
        ERR_MSG("ERROR opening socket\n");
    }

    // Code adapted from carnegie mellon's tcpserver:
    // Allows for fast debugging of sockets that allow for restarting the server
    // Immediately after killing it.
    
    err = setsockopt(parentfd,
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     (const void *)&optval,
                     sizeof(optval));
    if (SUCCESS != err)
    {
        ERR_MSG("setsockopt() failed\n");
    }
    err = getsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, &addrlen);
    if (SUCCESS != err)
    {
        ERR_MSG("getsockopt() failed\n");
    }

    err = bind(parentfd, result->ai_addr, result->ai_addrlen);
    if (SUCCESS != err)
    {
        ERR_MSG("bind() failed\n");
        free(c_args);
        c_args = NULL;
        goto END;
    }

    err = listen(parentfd, MAX_CONNECTIONS);
    if (SUCCESS != err)
    {
        ERR_MSG("listen() failed\n");
        free(c_args);
        c_args = NULL;
        goto END;
    }

    addrlen = sizeof(their_addr);

    do
    {
        accept_connection(parentfd, &their_addr, addrlen, pool_p, c_args);
    } while (0 == signal_flag_g);

    free(c_args);
    c_args = NULL;
    err    = SUCCESS;
END:
    if (NULL != result)
    {
        freeaddrinfo(result);
    }
    return err;
}

static void accept_connection(int parentfd, struct sockaddr_storage * their_addr, socklen_t addrlen, threadpool_t * pool_p, con_args_t * c_args)
{
    if((NULL == their_addr) || (NULL == pool_p) || (NULL == c_args))
    {
        ERR_MSG("Passed NULL pointer\n");
        goto LOOP;
    }
    int con_socket = accept(parentfd, (struct sockaddr *)their_addr, &addrlen);
    if (ERROR == con_socket)
    {
        goto LOOP;
    }
    struct sockaddr_in * ipv4addr = (struct sockaddr_in *)&their_addr;
    struct in_addr       ipaddr  = ipv4addr->sin_addr;
    char                 str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipaddr, str, INET_ADDRSTRLEN);

    if (ERROR != con_socket)
    {
        DEBUG_PRINT("Connected to %s\n", str);
        c_args->socket = con_socket;
        threadpool_add_job(pool_p, handle_new_connection, NULL, c_args);
    }
LOOP:
    DEBUG_PRINT("NEXT CLIENT\n");
    
}

void * handle_new_connection(void * args)
{
    if (NULL == args)
    {
        goto END;
    }
    int8_t         socket = *(int8_t *)args;
    int            err    = SUCCESS;
    net_header_t * header = NULL;

    header = calloc(1, sizeof(net_header_t));
    if (NULL == header)
    {
        goto END;
    }
    err = read_net_header(socket, header);
    if (SUCCESS != err)
    {
        ERR_MSG("BAD Read on NET HEADER\n");
        DEBUG_PRINT("SEND ERROR HEADER IN RETURN\n");
        send_error_header(socket);
    }
    if (SUCCESS == err)
    {
        err = parse_stream(socket, header);
        if (SUCCESS != err)
        {
            ERR_MSG("BAD PARSE\n");
        }
    }

    if (NULL != header)
    {
        free(header);
        header = NULL;
    }
    DEBUG_PRINT("socket: %d\n", socket);
    close(socket);

END:
    return NULL;
}

int read_net_header(int socket, net_header_t * header)
{
    int            amount_recv = 0;
    int            total       = 0;
    net_header_t * buf         = header;
    int            err         = ERROR;

    for (;;)
    {
        amount_recv = recv(socket, buf, NET_HEADER_SIZE - total, 0);
        if ((0 == amount_recv) || (NET_HEADER_SIZE == amount_recv) ||
            (NET_HEADER_SIZE == total))
        {
            break;
        }
        if (0 > amount_recv)
        {
            break;
        }
        else
        {
            total += amount_recv;
            buf += amount_recv;
        }
    }
    if (NET_HEADER_SIZE != ntohl(buf->header_size))
    {
        ERR_MSG("Invalid header size %d\n", ntohl(buf->header_size));
        goto END;
    }

    if (MAX_FILENAME_SIZE < ntohl(buf->file_name_len))
    {
        ERR_MSG("Invalid file name size: %d\n", ntohl(buf->file_name_len));
        goto END;
    }

    if (MAX_FILENAME_SIZE < strlen(buf->filename))
    {
        ERR_MSG("Invalid filename\n");
        goto END;
    }

    DEBUG_PRINT(
        " FILE: %s\n NAME_LEN: %d\n", buf->filename, ntohl(buf->file_name_len));

    err = SUCCESS;
END:
    return err;
}

int parse_stream(int socket, net_header_t * net_header_p)
{
    int             err                = ERROR;
    int             equations_read     = 0;
    int             out_file_des       = ERROR;
    int             check              = 0;
    size_t          num_bytes          = 0;
    int64_t         num_equations      = 0;
    equation_t *    p_current_equation = NULL;
    file_header_t * p_header = NULL;
    if(NULL == net_header_p)
    {
        ERR_MSG("Invalid Netheader\n");
        goto END;
    }

    p_current_equation = calloc(1, sizeof(equation_t));
    p_header           = calloc(1, sizeof(file_header_t));

    if ((NULL == p_current_equation) || (NULL == p_header))
    {
        ERR_MSG("Failed to allocate space for equation or header\n");
        free(p_header);
        p_header = NULL;
        goto END;
    }

    out_file_des = socket;

    err = check_header(out_file_des, &num_equations, p_header);
    if (SUCCESS != err)
    {
        ERR_MSG("Sending ERROR header: %d\n", err);
        send_error_header(out_file_des);
        goto END;
    }

    err = write_net_header(out_file_des, net_header_p, num_equations);
    if (SUCCESS != err)
    {
        ERR_MSG("Failed to write net_header\n");
        goto END;
    }
    err = write_file_header(out_file_des, p_header);
    if (SUCCESS != err)
    {
        ERR_MSG("Failed to write file_header\n");
        goto END;
    }
    while (equations_read <
           num_equations) // Loop through the equations in the file.
    {
        size_t bytes_read = 0;

        calc_t * p_new_calc = NULL;
        p_new_calc          = calc_init();

        if (NULL == p_new_calc)
        {
            ERR_MSG("Failed to allocate space for new_calc\n");
            goto END;
        }
        // 4.1.8 Read data from a file
        bytes_read = safe_read(socket, p_current_equation, EQUATION_SIZE);
        DEBUG_PRINT("Bytes read: %ld | %d | %ld | %ld current equation %d\n",
                    bytes_read,
                    p_current_equation->equation_id,
                    p_current_equation->num1,
                    p_current_equation->num2,
                    equations_read);

        if (0 == bytes_read)
        {
            ERR_MSG("EOF\n");
            check = 1;
            goto FREE_NEWCALC;
        }
        if (EQUATION_SIZE != bytes_read)
        {
            ERR_MSG("Failed to read equation\n");
            check = 1;
            goto FREE_NEWCALC;
        }
        err = set_calc_file(p_new_calc,
                            p_current_equation->num1,
                            p_current_equation->num2,
                            &p_current_equation->operator);
        if (0 != err)
        {
            ERR_MSG("Failed to set calc\n");
            check = 1;
            goto FREE_NEWCALC;
        }
        err = solve_func(p_new_calc);
        if (SUCCESS != err)
        {
            ERR_MSG("Failed to solve\n");
            check = 1;
            goto FREE_NEWCALC;
        }
        equations_read++;
        // 4.1.8 Write/Modify data to a file
        num_bytes =
            write_stream(out_file_des,
                         p_new_calc,
                         p_current_equation); // Write solution to output file
        if (0 == num_bytes)
        {
            ERR_MSG("Failed to write\n");
            check = 1;
            goto FREE_NEWCALC;
        }
    FREE_NEWCALC:
        free(p_new_calc);
        p_new_calc = NULL;
        if (1 == check)
        {
            goto END;
        }
    }

END:

    if (NULL != p_header)
    {
        free(p_header);
        p_header = NULL;
    }
    if(NULL != p_current_equation)
    {
        free(p_current_equation);
        p_current_equation = NULL;
    }
    return err;
}

int check_header(int             file_des,
                 int64_t *       p_num_equations,
                 file_header_t * p_header)
{
    int    err       = -1;
    size_t byte_read = 0;

    if (NULL == p_header)
    {
        ERR_MSG("Failed to allocate header\n");
        goto END;
    }

    byte_read = safe_read(file_des, p_header, HEADER_SIZE);

    if (HEADER_SIZE != byte_read)
    {
        ERR_MSG(
            "DID NOT read full header\n %d : %zu\n", HEADER_SIZE, byte_read);
        goto END;
    }

    if (0 != p_header->is_solved)
    {
        ERR_MSG("File already solved\n");
        goto END;
    }

    if (0 >= p_header->num_equations)
    {
        ERR_MSG("Invalid number of equations\n");
        goto END;
    }

    DEBUG_PRINT("magic number: %d\n", p_header->magic);
    DEBUG_PRINT("num equations: %ld\n", p_header->num_equations);
    DEBUG_PRINT("offset: %d\n", p_header->offset);
    // 4.1.11 Assigning a value to a pointer
    *p_num_equations = p_header->num_equations;

    err = 0;

END:
    return err;
}

size_t write_stream(int file_des, calc_t * p_calc_result, equation_t * equation)
{
    size_t err = ERROR;
    if ((NULL == p_calc_result) || (NULL == equation))
    {
        ERR_MSG("Failed to pass allocated writes\n");
        goto END;
    }

    solve_equation_t * p_solution = calloc(1, sizeof(solve_equation_t));
    if (NULL == p_solution)
    {
        ERR_MSG("Failed to allocate solution\n");
        goto END;
    }

    p_solution->equation_id = equation->equation_id;
    p_solution->is_solved   = 1;
    p_solution->solution    = p_calc_result->result.result;
    // Check for correct p_solution type, MOD is enum that is the last numerical
    // value of the signed operations

    if (MOD64 >= p_calc_result->oper)
    {
        p_solution->solution_type = INT;
    }

    else
    {
        p_solution->solution_type = UINT;
    }

    err = safe_write(file_des, p_solution, SOLUTION_SIZE);
    if (0 == err)
    {
        ERR_MSG("Failed to write p_solution to file\n");
    }

    free(p_solution);
    p_solution = NULL;

END:
    return err;
}

int write_net_header(int stream, net_header_t * header, int64_t num_equations)
{
    int            err        = ERROR;
    net_header_t * new_header = calloc(1, sizeof(net_header_t));
    if (NULL == new_header)
    {
        goto END;
    }
    header->file_name_len     = ntohl(header->file_name_len);
    new_header->file_name_len = htobe32(header->file_name_len);
    new_header->file_size     = htobe64(NET_HEADER_SIZE + HEADER_SIZE +
                                    (SOLUTION_SIZE * num_equations));
    strncpy(new_header->filename, header->filename, MAX_FILENAME_LEN);
    new_header->header_size = htonl(NET_HEADER_SIZE);
    safe_write(stream, new_header, ntohl(new_header->header_size));

    free(new_header);
    new_header = NULL;

    err = SUCCESS;
END:
    return err;
}

int send_error_header(int stream)
{
    int            err    = ERROR;
    net_header_t * header = calloc(1, sizeof(net_header_t));
    if (NULL == header)
    {
        goto END;
    }

    header->file_name_len = htonl(0);
    header->file_size     = htobe64(48);
    strncpy(header->filename, "", MAX_FILENAME_LEN);
    header->header_size = htonl(NET_HEADER_SIZE);
    safe_write(stream, header, ntohl(header->header_size));

    free(header);
    header = NULL;

    err = SUCCESS;
END:
    return err;
}

int write_file_header(int stream, file_header_t * file_header)
{
    int             err        = ERROR;
    if(NULL == file_header)
    {
        ERR_MSG("Invalid file header passed to write\n");
        goto END;
    }
    file_header_t * new_header = calloc(1, sizeof(file_header_t));
    if (NULL == new_header)
    {
        goto END;
    }

    new_header->file_id       = file_header->file_id;
    new_header->is_solved     = 1;
    new_header->magic         = file_header->magic;
    new_header->offset        = file_header->offset;
    new_header->num_equations = file_header->num_equations;
    new_header->reserved      = file_header->reserved;
    safe_write(stream, new_header, HEADER_SIZE);

    free(new_header);
    new_header = NULL;

    err = SUCCESS;
END:
    return err;
}
