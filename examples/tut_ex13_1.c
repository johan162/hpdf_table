/**
 * @file tut_ex13_1.c
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

static char *
cb_content(void *tag, size_t r, size_t c) {
    static char buf[32];
#if (defined _WIN32 || defined __WIN32__)
    if( 0==r )
        snprintf(buf, sizeof buf, "Header %02ix%02i", r, c);
    else
        snprintf(buf, sizeof buf, "Content %02ix%02i", r, c);
#else
    if( 0==r )
        snprintf(buf, sizeof buf, "Header %02zux%02zu", r, c);
    else
        snprintf(buf, sizeof buf, "Content %02zux%02zu", r, c);
#endif
    return buf;
}

static char *
cb_label(void *tag, size_t r, size_t c) {
    static char buf[32];
#if (defined _WIN32 || defined __WIN32__)
    if (0==r && 0==c) {
        snprintf(buf, sizeof buf, "Date:");
    } else {
        snprintf(buf, sizeof buf, "Label %ix%i:", r, c);
    }
#else
    if (0==r && 0==c) {
        snprintf(buf, sizeof buf, "Date:");
    } else {
        snprintf(buf, sizeof buf, "Label %zux%zu:", r, c);
    }
#endif
    return buf;
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

hpdftbl_spec_t tbl_spec = {
        // Title and header flag
        .title=NULL, .use_header=TRUE,

        // Label and labelgrid flags
        .use_labels=FALSE, .use_labelgrid=FALSE,

        // Row and columns
        .rows=4, .cols=3,

        // xpos and ypos
        .xpos=hpdftbl_cm2dpi(1), .ypos=hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM-2),

        // width and height
        .width=hpdftbl_cm2dpi(15), .height=0,

        // Content and label callback
        .content_cb=cb_content, .label_cb=cb_label,

        // Style and table post creation callback
        .style_cb=NULL, .post_cb=NULL,

        // Pointer to optional cell specifications
        .cell_spec=NULL
};


/**
 * Table 13_1 example - Talbe from data
 */
void
create_table_ex13_1(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
   hpdftbl_stroke_from_data(pdf_doc, pdf_page, &tbl_spec, NULL);
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

    setup_hpdf(&pdf_doc, &pdf_page, FALSE);

    create_table_ex13_1(pdf_doc, pdf_page);

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
