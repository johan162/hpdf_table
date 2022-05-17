/**
 * @file
 */
#include "unit_test.inc.h"

/**
 * @brief Table 0 example - The most basic table with all default settings
 */
void
create_table_ex00(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    int num_rows = 4;
    int num_cols = 3;
    char *table_title = "Example 1: Basic table with default theme";
    hpdftbl_t tbl = hpdftbl_create_title(num_rows, num_cols, table_title);

    content_t content, labels;
    setup_dummy_content_label(&content, &labels, num_rows, num_cols);
    hpdftbl_set_content(tbl, content);
    hpdftbl_set_labels(tbl, labels);

    hpdftbl_use_labels(tbl, TRUE);
    // hpdftbl_use_labelgrid(tbl, TRUE);

    // We have to specify the top left position on the PDF as well as the width.
    // We let the library automatically determine the height of the table based
    // on the font and number of rows.
    HPDF_REAL xpos = hpdftbl_cm2dpi(2);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 8);
    HPDF_REAL width = hpdftbl_cm2dpi(15);
    HPDF_REAL height = 0;  // Calculate height automatically

    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
}


TUTEX_MAIN(create_table_ex00, FALSE)
