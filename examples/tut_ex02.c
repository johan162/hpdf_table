/**
 * @file
 */

#include "unit_test.inc.h"

/**
 * Table 2 example - The most basic table with all default settings
 */
void
create_table_ex02(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 2;
    const size_t num_cols = 2;

    hpdftbl_t tbl = hpdftbl_create(num_rows, num_cols);

    content_t content;
    setup_dummy_content(&content, 2, 2);
    hpdftbl_set_content(tbl, content);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(5);
    HPDF_REAL height = 0;  // Calculate height automatically

    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
}

TUTEX_MAIN(create_table_ex02, FALSE)
