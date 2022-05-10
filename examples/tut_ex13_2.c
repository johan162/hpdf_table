/**
 * @file tut_ex13_2.c
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

//static char *
//cb_date(void *tag, size_t r, size_t c) {
//    static char buf[64];
//    time_t t = time(NULL);
//    ctime_r(&t, buf);
//    return buf;
//}

static char *
cb_content(void *tag, size_t r, size_t c) {
    static char *cell_content[] =
            {"Mark Ericsen",
             "12 Sep 2021",
             "123 Downer Mews",
             "London", 
             "NW2 HB3",
             "mark.p.ericsen@myfinemail.com",
             "+44734 354 184 56",
             "+44771 938 137 11"};

    if( 0==r && 0==c) return cell_content[0];
    else if (0==r && 3==c) return cell_content[1];
    else if (1==r && 0==c) return cell_content[2];
    else if (2==r && 0==c) return cell_content[3];
    else if (2==r && 3==c) return cell_content[4];
    else if (3==r && 0==c) return cell_content[5];
    else if (4==r && 0==c) return cell_content[6];
    else if (4==r && 2==c) return cell_content[7];
    else return NULL;
}


#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif


hpdftbl_cell_spec_t cell_specs[] = {
        {.row=0, .col=0, .rowspan=1, .colspan=3,
                .label="Name:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        {.row=0, .col=3, .rowspan=1, .colspan=1,
                .label="Date:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        {.row=1, .col=0, .rowspan=1, .colspan=4,
                .label="Address:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        {.row=2, .col=0, .rowspan=1, .colspan=3,
                .label="City:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        {.row=2, .col=3, .rowspan=1, .colspan=1,
                .label="Zip:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        {.row=3, .col=0, .rowspan=1, .colspan=4,
                .label="E-mail:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        {.row=4, .col=0, .rowspan=1, .colspan=2,
                .label="Work-phone:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        {.row=4, .col=2, .rowspan=1, .colspan=2,
                .label="Mobile:",
                .content_cb=NULL, .label_cb=NULL, .style_cb=NULL, .canvas_cb=NULL},
        HPDFTBL_END_CELLSPECS
};

hpdftbl_spec_t tbl_spec = {
        // Title and header flag
        .title=NULL, .use_header=FALSE,

        // Label and labelgrid flags
        .use_labels=TRUE, .use_labelgrid=TRUE,

        // Row and columns
        .rows=5, .cols=4,

        // xpos and ypos
        .xpos=hpdftbl_cm2dpi(1), .ypos=hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM-2),

        // width and height
        .width=hpdftbl_cm2dpi(15), .height=0,

        // Content and label callback
        .content_cb=cb_content, .label_cb=0,

        // Style and table post creation callback
        .style_cb=NULL, .post_cb=NULL,

        // Pointer to optional cell specifications
        .cell_spec=cell_specs
};


/**
 * Table 13_2 example - Table from data
 */
void
create_table_ex13_2(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
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

    create_table_ex13_2(pdf_doc, pdf_page);

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
