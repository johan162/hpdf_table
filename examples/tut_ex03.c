/**
 * @file
 */

#include "unit_test.inc.h"

/**
 * Table 3 example - A table with cell labels
 */
void
create_table_ex03(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 2;
    const size_t num_cols = 2;
    //char *table_title = "tut_ex01: 2x2 table";
    hpdftbl_t tbl = hpdftbl_create(num_rows, num_cols);

    hpdftbl_set_cell(tbl, 0, 0, "Label 1:", "Cell 0x0");
    hpdftbl_set_cell(tbl, 0, 1, "Label 2:", "Cell 0x1");
    hpdftbl_set_cell(tbl, 1, 0, "Label 3:", "Cell 1x0");
    hpdftbl_set_cell(tbl, 1, 1, "Label 4:", "Cell 1x1");

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

TUTEX_MAIN(create_table_ex03, FALSE)

