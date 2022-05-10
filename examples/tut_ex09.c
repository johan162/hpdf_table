/**
 * @file tut_ex09.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !(defined _WIN32 || defined __WIN32__)
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#endif
#include <hpdf.h>
#include <math.h>
#include <setjmp.h>
#include <time.h>
#if !(defined _WIN32 || defined __WIN32__)
#include <sys/utsname.h>
#include <libgen.h>
#include <sys/stat.h>

#endif

// This include should always be used
#include "../src/hpdftbl.h"

// For the case when we use this example as a unit/integration test
_Bool static_date = FALSE;

// For simulated exception handling
jmp_buf env;

#ifndef _MSC_VER
// Silent gcc about unused "arg" in the callback and error functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

// A standard hpdf error handler which also translates the hpdf error code to a
// human readable string
static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no,
                          void *user_data) {
    fprintf(stderr, "*** PDF ERROR: \"%s\", [0x%04X : %d]\n",
            hpdftbl_hpdf_get_errstr(error_no), (unsigned int)error_no, (int)detail_no);
    longjmp(env, 1);
}

_Bool
cb_style(void *tag, size_t r, size_t c, char *content, hpdf_text_style_t *style) {
    // Format the header row/column with a grey background and Helvetica font while the rest of the
    // table uses "Times Roman"
    if( 0==r || 0==c ) { // Headers
        style->font = HPDF_FF_HELVETICA_BOLD;
        style->fsize = 12;
        style->color = HPDF_COLOR_BLACK;
        style->background = HPDF_COLOR_LIGHT_GRAY;
        if ( c > 0 )
            style->halign = CENTER;
        else
            style->halign = LEFT;
    } else { // Content
        style->font = HPDF_FF_TIMES;
        style->fsize = 11;
        style->color = HPDF_COLOR_BLACK;
        style->background = HPDF_COLOR_WHITE;
        style->halign = CENTER;
    }
    return TRUE;
}

static char *
cb_content(void *tag, size_t r, size_t c) {
    static char buf[32];
    if( 0==r && 0==c ) return NULL;

    if( 0==c ) {
#if (defined _WIN32 || defined __WIN32__)
        snprintf(buf, sizeof buf, "Extra long Header %2ix%2i", r, c);
#else
        snprintf(buf, sizeof buf, "Extra long Header %zux%zu", r, c);
#endif
    } else if( 0==r ) {
#if (defined _WIN32 || defined __WIN32__)
        snprintf(buf, sizeof buf, "Header %2ix%2i", r, c);
#else
        snprintf(buf, sizeof buf, "Header %zux%zu", r, c);
#endif
    } else {
#if (defined _WIN32 || defined __WIN32__)
        snprintf(buf, sizeof buf, "Content %2ix%2i", r, c);
#else
        snprintf(buf, sizeof buf, "Content %zux%zu", r, c);
#endif
    }
    return buf;
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

/**
 * Table 9 example - Table with styles
 */
void
create_table_ex09(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 4;
    const size_t num_cols = 4;

    hpdftbl_t tbl = hpdftbl_create(num_rows, num_cols);

    hpdftbl_set_content_cb(tbl, cb_content);
    hpdftbl_set_content_style_cb(tbl, cb_style);

    hpdftbl_set_colwidth_percent(tbl, 0, 40);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(A4PAGE_WIDTH_CM - 4);
    HPDF_REAL height = 0;  // Calculate height automatically

    // Stroke the table to the page
    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
}

// Setup a new PDF document with one page
void
setup_hpdf(HPDF_Doc* pdf_doc, HPDF_Page* pdf_page, _Bool addgrid) {
    // Setup the basic PDF document
    *pdf_doc = HPDF_New(error_handler, NULL);
    *pdf_page = HPDF_AddPage(*pdf_doc);
    HPDF_SetCompressionMode(*pdf_doc, HPDF_COMP_ALL);
    HPDF_Page_SetSize(*pdf_page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    if (addgrid) { 
        hpdftbl_stroke_grid(*pdf_doc, *pdf_page);
    }
}


#ifndef _MSC_VER
// Silent gcc about unused "arg" in the callback and error functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

char *
setup_filename(int argc, char **argv) {
    static char file[1024];
    if ( 2==argc ) {
        strncpy(file, argv[1], sizeof file);
        file[sizeof(file)-1] = 0;
    } else if ( 1==argc ) {
        char fbuff[255];
        strncpy(fbuff, argv[0], sizeof fbuff);
        fbuff[sizeof(fbuff) - 1] = 0;
        char *bname = basename(fbuff);
        snprintf(file, sizeof file, "out/%s.pdf", bname);
    } else {
        return NULL;
    }
    return file;
}


int
main(int argc, char **argv) {
    
    HPDF_Doc pdf_doc;
    HPDF_Page pdf_page;

    if (setjmp(env)) {
        HPDF_Free(pdf_doc);
        return EXIT_FAILURE;
    }

    // For the case when we use this example as a unit/integration test we need to
    // look down a static date since we cannot compare otherwise since the date
    // strings will be different.
    static_date = 2==argc ;

    setup_hpdf(&pdf_doc, &pdf_page, FALSE);

    create_table_ex09(pdf_doc, pdf_page);

    char *file;
    if( NULL == (file=setup_filename(argc, argv)) ) {
        fprintf(stderr,"ERROR: Unknown arguments!\n");
        return EXIT_FAILURE;
    }

    printf("Sending to file \"%s\" ...\n", file);
    if ( -1 == hpdftbl_stroke_pdfdoc(pdf_doc, file) ) {
        fprintf(stderr,"ERROR: Cannot save to file. Does the full directory path exist?\n");
        return EXIT_FAILURE;
    }
    printf("Done.\n");
    return EXIT_SUCCESS;
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
