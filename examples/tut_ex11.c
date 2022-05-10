/**
 * @file
 */

#include "unit_test.inc.h"

/**
 * Table 10 example - Error handling
 */
void
create_table_ex11(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 4;
    const size_t num_cols = 4;

    hpdftbl_set_errhandler(hpdftbl_default_table_error_handler);

    hpdftbl_t tbl = hpdftbl_create(num_rows, num_cols);

    hpdftbl_use_header(tbl, TRUE);

    hpdftbl_set_colwidth_percent(tbl, 0, 40);

    content_t content;
    setup_dummy_content(&content, num_rows, num_cols);
    hpdftbl_set_content(tbl, content);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(A4PAGE_WIDTH_CM - 5);
    HPDF_REAL height = 0;  // Calculate height automatically

    // Stroke the table to the page
    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
}

TUTEX_MAIN(create_table_ex11, FALSE)
