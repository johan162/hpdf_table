/**
 * @file tut_ex14.c
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

static char *
cb_labels(void *tag, size_t r, size_t c) {
    static char buf[32];

    if (0==r && 0==c) {
        snprintf(buf, sizeof buf, "Device name:");
    } else if (0==r && 1==c) {
        snprintf(buf, sizeof buf, "Date:");
    } else if (1==r && 0==c) {
        snprintf(buf, sizeof buf, "Battery strength:");
    } else if (1==r && 1==c) {
        snprintf(buf, sizeof buf, "Signal:");
    } else {
        return NULL;
    }
    return buf;
}


static char *
cb_date(void *tag, size_t r, size_t c) {
    static char buf[64];
    if ( ! static_date ) {
        time_t t = time(NULL);
        ctime_r(&t, buf);
        return buf;
    } else {
        return "Wed May 4 19:01:01 2022";
    }
}

static char *
cb_device_name(void *tag, size_t r, size_t c) {
    static char buf[32];
    snprintf(buf, sizeof buf, "IoT Device ABC123");
    return buf;
}

void
cb_draw_battery_widget(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r,
                       size_t c, HPDF_REAL xpos, HPDF_REAL ypos,
                       HPDF_REAL width, HPDF_REAL height) {
    const HPDF_REAL segment_tot_width = width * 0.5;
    const HPDF_REAL segment_height = height / 3;
    const HPDF_REAL segment_xpos = xpos + 40;
    const HPDF_REAL segment_ypos = ypos + 4;
    const size_t num_segments = 10;

    const HPDF_RGBColor on_color = HPDF_COLOR_DARK_GREEN;
    const double val_percent = 0.4;
    const _Bool val_text_hide = FALSE;

    hpdftbl_widget_segment_hbar(
            doc, page, segment_xpos, segment_ypos, segment_tot_width,
            segment_height, num_segments, on_color, val_percent, val_text_hide);
}

void
cb_draw_signal_widget(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r,
                      size_t c, HPDF_REAL xpos, HPDF_REAL ypos,
                      HPDF_REAL width, HPDF_REAL height) {
    const HPDF_REAL wwidth = 35;
    const HPDF_REAL wheight = 20;
    const HPDF_REAL wxpos = xpos + 70;
    const HPDF_REAL wypos = ypos + 4;
    const size_t num_segments = 5;
    const HPDF_RGBColor on_color = HPDF_COLOR_DARK_RED;

    // This should be the real data retrieved from a DB (for example)
    const size_t num_on_segments = 3;

    hpdftbl_widget_strength_meter(doc, page, wxpos, wypos, wwidth, wheight,
                                  num_segments, on_color, num_on_segments);
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

/**
 * Table 14 example - Table with widget callbacks
 */
void
create_table_ex14(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 2;
    const size_t num_cols = 2;

    char *table_title = "tut_ex14: 2x2 table widget callbacks";
    hpdftbl_t tbl = hpdftbl_create_title(num_rows, num_cols, table_title);
    hpdftbl_use_labels(tbl, TRUE);
    hpdftbl_use_labelgrid(tbl, TRUE);

    // Use one label callback for the entire table
    hpdftbl_set_label_cb(tbl, cb_labels);

    // Name in top left corner
    hpdftbl_set_cell_content_cb(tbl, 0, 0, cb_device_name);

    // Date in top right corner
    hpdftbl_set_cell_content_cb(tbl, 0, 1, cb_date);

    // Draw battery strength
    hpdftbl_set_cell_canvas_cb(tbl, 1, 0, cb_draw_battery_widget);

    // Draw signal strength
    hpdftbl_set_cell_canvas_cb(tbl, 1, 1, cb_draw_signal_widget);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(12);
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

    create_table_ex14(pdf_doc, pdf_page);

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
