#ifndef _FILE_IO_H
#define _FILE_IO_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"

/**
 * @brief A safe read function that reads the entire contents of buf
 *
 * @param file_d file descriptor to read
 * @param buf buffer to read into
 * @param len size of the buffer to read
 * @return size_t number of bytes read
 */
size_t safe_read(int file_d, void * buf, size_t len);

/**
 * @brief a safe write function that writes the entire contents of buf
 *
 * @param file_d file descriptor to write to
 * @param buf buffer to write into
 * @param len size of the buffer to write
 * @return size_t number of bytes to write
 */
size_t safe_write(int file_d, void * buf, size_t len);

/**
 * @brief safely close a file
 *
 * @param file_d file descriptor to close
 * @return int 0 for success -1 for failure
 */
int safe_close(int file_d);

#endif