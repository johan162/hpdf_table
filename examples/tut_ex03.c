
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


/**
 * Table 1 example - The most basic table with all default settings
 */
void
create_table_ex03(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 2;
    const size_t num_cols = 2;
    //char *table_title = "tut_ex01: 2x2 table";
    hpdftbl_t tbl = hpdftbl_create(num_rows, num_cols);

    hpdftbl_set_cell(tbl, 0, 0, "Label 1", "Cell 0x0");
    hpdftbl_set_cell(tbl, 0, 1, "Label 2", "Cell 0x1");
    hpdftbl_set_cell(tbl, 1, 0, "Label 3", "Cell 1x0");
    hpdftbl_set_cell(tbl, 1, 1, "Label 4", "Cell 1x1");

    hpdftbl_use_labels(tbl, TRUE);
    hpdftbl_use_labelgrid(tbl, FALSE);

    // We have to specify the top left position on the PDF as well as the width.
    // We let the library automatically determine the height of the table based
    // on the font and number of rows.
    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(5);
    HPDF_REAL height = 0;  // Calculate height automatically

    // Stroke the table to the page
    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
}

// Setup a new PDF document with one age
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

    setup_hpdf(&pdf_doc, &pdf_page, FALSE);

    create_table_ex03(pdf_doc, pdf_page);

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
