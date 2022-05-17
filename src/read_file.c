/**
 * @file
 * @brief   Function for reading a file into a memory buffer
 * @author   Johan Persson (johan162@gmail.com)
 *
 * Copyright (C) 2022 Johan Persson
 *
 * @see LICENSE
 *
 * Released under the MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <hpdf.h>
#include "hpdftbl.h"
/**
 * @brief Read content of file into a specified buffer
 * @param buff Destination buffer
 * @param buffsize Size of buffer
 * @param filename Name of file to read from
 * @return -1 on failure, 0 on success
 */
int
hpdftbl_read_file(char *buff, size_t buffsize, char *filename) {
    FILE *fh = fopen(filename, "r");
    if (fh == NULL) {
        return -1;
    }

    const int linesize = 1024;
    char line[linesize];

    while (fgets(line, linesize, fh) != NULL) {
        if (xstrlcat(buff, line, buffsize) >= buffsize) {
            // Truncation error
            fclose(fh);
            return -2;
        }
    }

    fclose(fh);
    return 0;
}
