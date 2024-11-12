#include "thread_handle.h"

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

int check_in_directory(const char * directory_name_p, DIR ** p_dir_des)
{
    int err = -1;

    if (NULL == *p_dir_des)
    {
        ERR_MSG("NULL directory descriptor is intended\n");
    }

    char * p_full_dir = realpath(directory_name_p, NULL);

    if (NULL == p_full_dir)
    {
        ERR_MSG("Failed to get filepath\n");
        goto END;
    }

    *p_dir_des = opendir(directory_name_p);
    if (NULL == p_dir_des)
    {
        ERR_MSG("Failed to open directory\n");
        goto END;
    }

    free(p_full_dir);
    p_full_dir = NULL;

    err = 0;

END:
    return err;
}

int check_out_directory(const char * directory_name_p, DIR ** p_dir_des)
{
    int err = -1;

    DEBUG_PRINT("Checking out directory = %s\n", directory_name_p);
    char * p_full_dir = realpath(directory_name_p, NULL);
    DEBUG_PRINT("Full directory = %s\n", p_full_dir);
    if (NULL == p_full_dir)
    {
        ERR_MSG("Failed to get filepath\n");
        goto END;
    }

    *p_dir_des = opendir(directory_name_p);
    if (NULL == p_dir_des)
    {
        ERR_MSG("Failed to open directory\n");
        goto END;
    }

    free(p_full_dir);
    p_full_dir = NULL;

    err = 0;

END:
    return err;
}

int check_convert_to_long(int errno_, long tester, char ** endptr_p)
{
    int err = ERROR;
    if (0 != errno_)
    {
        ERR_MSG("Error converting to integer\n");
        goto END;
    }
    else if ((ERANGE == errno_) && (LONG_MIN == tester))
    {
        ERR_MSG("Underflow error\n");
        goto END;
    }
    else if ((ERANGE == errno_) && (LONG_MAX == tester))
    {
        ERR_MSG("Overflow error\n");
        goto END;
    }

    if ((INT32_MAX < tester) || (INT32_MIN > tester))
    {
        ERR_MSG("Number is out of bounds of 32bits\n");
        goto END;
    }

    if ((tester == **endptr_p) && (*endptr_p != 0))
    {
        ERR_MSG("No digits\n");
        goto END;
    }

    err = SUCCESS;
END:
    return err;
}

int directory_parse(DIR *          p_in_directory,
                    char *         in_dir_name,
                    char *         out_dir_name,
                    threadpool_t * threadpool_p)
{
    int             err            = -1;
    int64_t         num_equations  = 0;
    int             out_file_des   = -1;
    struct dirent * p_dirent       = NULL;
    char *          p_full_dir     = NULL;
    parse_args_t *  args           = { 0 };
    char *          full_file_path = NULL;

    if (NULL == p_in_directory)
    {
        ERR_MSG("directory parse failed on input\n");
        goto END;
    }
    if (NULL == in_dir_name)
    {
        ERR_MSG("in_dir is null\n");
        goto END;
    }
    if (NULL == out_dir_name)
    {
        ERR_MSG("out_dir is null\n");
        goto END;
    }

    p_dirent = readdir(p_in_directory);

    // Get the absolute path of the directory
    DEBUG_PRINT("outdirname = %s\n", out_dir_name);
    p_full_dir = realpath(out_dir_name, NULL);
    if (NULL == p_full_dir)
    {
        ERR_MSG("Failed to get realpath\n");
        goto END;
    }
    for (;;)
    {
        while (NULL != p_dirent)
        {
            int  file_d                   = ERROR;
            char directory_name[MAX_PATH] = { 0 };
            // 4.6.2 Input Validation
            if ((0 == strncmp(p_dirent->d_name, ".", 1)) ||
                (0 == strncmp(p_dirent->d_name,
                              "..",
                              1))) // Check for default directories in linux
            {
                p_dirent = readdir(p_in_directory);
                goto END_LOOP;
            }

            err = snprintf(directory_name,
                           MAX_PATH,
                           "%s/%s",
                           in_dir_name,
                           p_dirent->d_name); // Combine directory strings
            if (ERROR == err)
            {
                ERR_MSG("Failed to combine directory\n");
                goto END;
            }

            full_file_path = realpath(directory_name, NULL);

            // 4.1.8 Open existing file
            file_d = open(full_file_path, O_RDONLY | O_CLOEXEC);

            if (ERROR == file_d)
            {
                ERR_MSG("%d - %s is not a readable file\n",
                        file_d,
                        p_dirent->d_name);
            }

            file_header_t * p_header = calloc(1, sizeof(file_header_t));

            err = check_header(file_d, &num_equations, p_header);
            if (SUCCESS != err)
            {
                free(p_header);
                p_header = NULL;
                goto END;
            }

            out_file_des = create_file(p_full_dir, p_dirent->d_name, p_header);
            free(p_header);
            p_header = NULL;
            if (2 > out_file_des)
            {
                ERR_MSG("Failed to create file descriptor!\n");
                goto END;
            }

            args                  = calloc(1, sizeof(parse_args_t));
            args->in_file_stream  = file_d;
            args->out_file_stream = out_file_des;
            args->num_equations   = num_equations;

            err = threadpool_add_job(threadpool_p, parse_file, NULL, args);
            if (SUCCESS != err)
            {
                ERR_MSG("Failed to parse file %d\n", file_d);
            }

            // 4.6.2 zero out memory allocated
            free(full_file_path);
            full_file_path = NULL;

            p_dirent = readdir(p_in_directory);

        END_LOOP:
            if (ERROR == err)
            {
                ERR_MSG("loop skipped\n");
            }
        }

        if (SUCCESS != signal_flag_g)
        {
            break;
        }
    }
    err = SUCCESS;
END:
    if (NULL != full_file_path)
    {
        free(full_file_path);
        full_file_path = NULL;
    }
    if (NULL != p_full_dir)
    {
        free(p_full_dir);
        p_full_dir = NULL;
    }
    p_dirent = NULL;

    return err;
}

void * parse_file(void * args)
{
    if (NULL == args)
    {
        goto TAP;
    }

    int            err    = ERROR;
    parse_args_t * args_p = (parse_args_t *)args;

    int in_file_des   = args_p->in_file_stream;
    int out_file_des  = args_p->out_file_stream;
    int num_equations = args_p->num_equations;

    if (2 > in_file_des)
    {
        ERR_MSG("Invalid input file descriptor\n");
        goto END;
    }

    if (2 > out_file_des)
    {
        ERR_MSG("Invalid output file descriptor\n");
        goto END;
    }

    err = process_equations(in_file_des, out_file_des, num_equations);
    if (SUCCESS != err)
    {
        ERR_MSG("Failed to process equations\n");
        goto END;
    }

END:

    if (NULL != args)
    {
        free(args);
        args = NULL;
    }

TAP:
    return NULL;
}

int process_equations(int file_stream, int out_file_des, int64_t num_equations)
{
    int          err                = ERROR;
    int          equations_read     = 0;
    int          check              = 0;
    size_t       num_bytes          = 0;
    equation_t * p_current_equation = calloc(1, sizeof(equation_t));

    if (NULL == p_current_equation)
    {
        ERR_MSG("Failed to allocate space for equation\n");
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
        bytes_read = safe_read(file_stream, p_current_equation, EQUATION_SIZE);

        if (0 == bytes_read)
        {
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
        if (SUCCESS != err)
        {
            ERR_MSG("Failed to set calc");
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

        // 4.1.8 Write/Modify data to a file
        num_bytes =
            write_file(out_file_des,
                       p_new_calc,
                       p_current_equation); // Write solution to output file

        if (SUCCESS == num_bytes)
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
    err = safe_close(out_file_des);
    if (SUCCESS != err)
    {
        ERR_MSG("Failed to close fd\n");
    }
    free(p_current_equation);
    p_current_equation = NULL;
    return err;
}

// 4.8.2
size_t write_file(int file_des, calc_t * p_calc_result, equation_t * equation)
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

// 4.1.11 declare integer pointer
int check_header(int             file_des,
                 int64_t *       p_num_equations,
                 file_header_t * p_header)
{
    int    err       = ERROR;
    size_t byte_read = 0;

    if (NULL == p_header)
    {
        ERR_MSG("Failed to allocate header\n");
        goto END;
    }
    if (p_header == NULL)
    {
        ERR_MSG("Header not found\n");
        goto END;
    }

    DEBUG_PRINT("FILE DES IN CHECK HEADER %d\n", file_des);

    byte_read = safe_read(file_des, p_header, HEADER_SIZE);

    if (HEADER_SIZE != byte_read)
    {
        ERR_MSG(
            "DID NOT read full header\n %d : %zu\n", HEADER_SIZE, byte_read);
        goto END;
    }

    if (SUCCESS != p_header->is_solved)
    {
        ERR_MSG("File already solved\n");
        goto END;
    }

    if (SUCCESS >= p_header->num_equations)
    {
        ERR_MSG("Invalid number of equations\n");
        goto END;
    }

    DEBUG_PRINT("magic number: %d\n", p_header->magic);
    DEBUG_PRINT("num equations: %ld\n", p_header->num_equations);
    DEBUG_PRINT("offset: %d\n", p_header->offset);
    // 4.1.11 Assigning a value to a pointer
    *p_num_equations = p_header->num_equations;

    err = SUCCESS;

END:
    return err;
}

int create_file(const char *    p_out_directory_name,
                const char *    filename,
                file_header_t * p_header)
{
    int    file_des           = -1;
    char   path[MAX_PATH + 2] = { 0 };
    size_t bytes              = 0;

    if ((NULL == p_out_directory_name) || (NULL == filename) ||
        (NULL == p_header))
    {
        ERR_MSG("Failed to pass initial variables\n");
        goto END;
    }
    // 4.6.2 Formatting strings and safe buffer size
    file_des = snprintf(
        path, MAX_PATH + 1, "%s/solved_%s", p_out_directory_name, filename);
    if (ERROR == file_des)
    {
        ERR_MSG("Failed to create filepath\n");
        goto END;
    }

    // 4.1.8 Create and append data to a file
    file_des = open(path, O_RDWR | O_CREAT | O_APPEND | O_CLOEXEC, RWRR);
    if (ERROR == file_des)
    {
        ERR_MSG("Failed to open filepath: %s\n", path);
        goto END;
    }

    p_header->is_solved = 1;
    bytes               = safe_write(file_des, p_header, HEADER_SIZE);
    if (bytes == SUCCESS)
    {
        ERR_MSG("Failed to write header\n");
    }

END:
    return file_des;
}
