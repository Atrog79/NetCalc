#include "file_io.h"

// https://lynxbee.com/c-code-for-safe-read-and-write-from-a-file/#.ZCM6l47MJhE
size_t safe_read(int file_d, void * buf, size_t len)
{
    ssize_t bytes_read  = 0;
    size_t  total_bytes = 0;

    if (NULL == buf)
    {
        ERR_MSG("Buf is NULL\n");
        goto END;
    }

    DEBUG_PRINT("FILE DES IN SAFE READ %d\n", file_d);

    errno = 0;

    while (bytes_read < (int)len)
    {
        bytes_read = read(file_d, (char *)buf + total_bytes, len - total_bytes);
        if (0 == bytes_read)
        {
            DEBUG_PRINT("returning error: %d\n", errno);
            total_bytes = 0;
            goto END;
        }
        if (EISDIR == errno)
        {
            DEBUG_PRINT("Current file is a directory\n");
            total_bytes = 0;
            goto END;
        }
        if (0 > bytes_read)
        {
            DEBUG_PRINT("returning error 2: %d\n", errno);
            total_bytes = bytes_read;
            goto END;
        }

        total_bytes += bytes_read;
        DEBUG_PRINT("%zu bytes read\n", total_bytes);
    }

END:
    return total_bytes;
}

// 4.8.1 Safe reads and writes
size_t safe_write(int file_d, void * buf, size_t len)
{
    ssize_t bytes_read  = 0;
    size_t  total_bytes = 0;

    if (NULL == buf)
    {
        ERR_MSG("Buf is NULL\n");
        goto END;
    }

    errno = 0;

    while (total_bytes < len)
    {
        // 4.1.11 make use of pointer arithmetic
        bytes_read =
            write(file_d, (char *)buf + total_bytes, len - total_bytes);
        if (0 > bytes_read)
        {
            total_bytes = bytes_read;
            goto END;
        }
        if (total_bytes == len)
        {
            break;
        }

        total_bytes += bytes_read;
        DEBUG_PRINT("%zu bytes read\n", total_bytes);
    }

END:
    return total_bytes;
}

int safe_close(int file_d)
{
    int err = -1;
    if (2 < file_d)
    {
        err = close(file_d);
    }
    return err;
}
