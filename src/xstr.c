/**
 * @file
 * @brief   Safe version of strncat() and strncpy() taken from the BSD stdlib
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/**
 * @brief Safe string copy
 *
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 *
 * Taken from BSD library.
 *
 * @param dst Destination string
 * @param src Source string
 * @param dsize Maximum size of destination
 * @return strlen(src); if retval >= dsize, truncation occurred.
 */
size_t
xstrlcpy(char * __restrict dst, const char * __restrict src, size_t dsize)
{
    const char *osrc = src;
    size_t nleft = dsize;

    /* Copy as many bytes as will fit. */
    if (nleft != 0) {
        while (--nleft != 0) {
            if ((*dst++ = *src++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src. */
    if (nleft == 0) {
        if (dsize != 0)
            *dst = '\0';		/* NUL-terminate dst */
        while (*src++)
            ;
    }

    return(src - osrc - 1);	/* count does not include NUL */
}

/**
 * @brief Safe string concatenation
 *
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 *
 * Taken from BSD library.
 *
 * @param dst Destination buffer
 * @param src Source buffer
 * @param siz Max size of destination buffer including terminating NULL
 * @return The number of bytes needed to be copied. If this is > `siz`
 * then data truncation happened.
 */
size_t
xstrlcat(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;
    size_t dlen;

    /* Find the end of dst and adjust bytes left but don't go past end */
    while (n-- != 0 && *d != '\0')
        d++;
    dlen = d - dst;
    n = siz - dlen;

    if (n == 0)
        return(dlen + strlen(s));
    while (*s != '\0') {
        if (n != 1) {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';

    return(dlen + (s - src));	/* count does not include NUL */
}

