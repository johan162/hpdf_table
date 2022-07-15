/**
 * @file
 */

#include "unit_test.inc.h"

#ifndef _MSC_VER
// Silent gcc about unused "arg" in the callback and error functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define FROM_JSON 1
/**
 * Table 40 example - Load serialized tablecd
 */
void
create_table_ex40(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    hpdftbl_t tbl=calloc(1, sizeof(struct hpdftbl));

#if FROM_JSON == 1
    if(0 == hpdftbl_load(tbl, mkfullpath("tut_ex40.json"))  ) {
        hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
    }
#else
    const size_t num_rows = 2;
    const size_t num_cols = 2;
    //char *table_title = "tut_ex01: 2x2 table";
    tbl = hpdftbl_create(num_rows, num_cols);
    content_t content, labels;

    setup_dummy_content_label(&content, &labels, num_rows, num_cols);
    hpdftbl_set_content(tbl, content);
    hpdftbl_set_labels(tbl, labels);

    hpdftbl_use_labels(tbl, TRUE);
    hpdftbl_use_labelgrid(tbl, TRUE);

    hpdftbl_set_cellspan(tbl,0,0,1,2);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(5);
    HPDF_REAL height = 0;  // Calculate height automatically

    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
    hpdftbl_dump(tbl, "out/tut_ex40.json");
#endif
}


#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

TUTEX_MAIN(create_table_ex40, FALSE)

