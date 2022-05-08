/**
 * @file tut_ex20.c
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

// For simulated exception handling
jmp_buf env;

#ifndef _MSC_VER
// Silent gcc about unused "arg" in the callback and error functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// A standard hpdf error handler which also translates the hpdf error code to a
// human readable string
static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no,
                          void *user_data) {
    fprintf(stderr, "*** PDF ERROR: \"%s\", [0x%04X : %d]\n",
            hpdftbl_hpdf_get_errstr(error_no), (unsigned int)error_no, (int)detail_no);
    longjmp(env, 1);
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

typedef char **content_t;

void setup_dummy_data(content_t *content, content_t *labels, size_t rows, size_t cols) {
    char buff[255];
    *content = calloc(rows*cols, sizeof(char*));
    *labels = calloc(rows*cols, sizeof(char*));
    size_t cnt = 0;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            snprintf(buff, sizeof(buff), "Content %zu", cnt);
            (*content)[cnt] = strdup(buff);
            snprintf(buff, sizeof(buff), "Label %zu", cnt);
            (*labels)[cnt] = strdup(buff);
            cnt++;
        }
    }
}

/**
 * Table example - Style on vertical/horizontal grid lines
 */
void
create_table_ex20(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 5;
    const size_t num_cols = 4;

    hpdftbl_t tbl = hpdftbl_create(num_rows, num_cols);
    content_t content, labels;

    setup_dummy_data(&content, &labels, num_rows, num_cols);
    hpdftbl_set_content(tbl, content);
    hpdftbl_set_labels(tbl, labels);
    
    hpdftbl_use_labels(tbl, FALSE);
    hpdftbl_use_labelgrid(tbl, TRUE);
    hpdftbl_use_header(tbl, FALSE);

    hpdftbl_set_inner_vgrid_style(tbl, 0.7, HPDF_COLOR_DARK_GRAY, LINE_SOLID);
    hpdftbl_set_inner_hgrid_style(tbl, 0.8, HPDF_COLOR_GRAY, LINE_DOT1);
    hpdftbl_set_inner_tgrid_style(tbl, 1.5, HPDF_COLOR_BLACK, LINE_SOLID);
    hpdftbl_set_outer_grid_style(tbl, 1.5, HPDF_COLOR_BLACK, LINE_SOLID);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(10);
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


void
stroke_pdfdoc(HPDF_Doc pdf_doc, char *file) {
    printf("Sending to file \"%s\" ...\n", file);
    if (HPDF_OK != HPDF_SaveToFile(pdf_doc, file)) {
        fprintf(stderr, "ERROR: Cannot save to file!");
    }
    HPDF_Free(pdf_doc);
    printf("Done.\n");
}

#ifndef _MSC_VER
// Silent gcc about unused "arg"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

int
main(int argc, char **argv) {
    
    HPDF_Doc pdf_doc;
    HPDF_Page pdf_page;

    if (setjmp(env)) {
        HPDF_Free(pdf_doc);
        return EXIT_FAILURE;
    }

    setup_hpdf(&pdf_doc, &pdf_page, FALSE);

    create_table_ex20(pdf_doc, pdf_page);

    if ( 2==argc ) {
        struct stat sb;
        if (stat(dirname(argv[1]), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            stroke_pdfdoc(pdf_doc, argv[1]);
            return EXIT_SUCCESS;
        }
    }

    char fname[255];
    snprintf(fname, sizeof fname, "out/%s.pdf", basename(argv[0]));
    stroke_pdfdoc(pdf_doc, fname);

    return EXIT_SUCCESS;
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif