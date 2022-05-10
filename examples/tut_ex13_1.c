/**
 * @file
 */

#include "unit_test.inc.h"

#ifndef _MSC_VER
// Silent gcc about unused "arg" in the callback and error functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

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

TUTEX_MAIN(create_table_ex13_1, FALSE)

