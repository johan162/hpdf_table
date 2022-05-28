/**
 * @file
 */

#include <yaml.h>
#include "unit_test.inc.h"

#ifndef _MSC_VER
// Silent gcc about unused "arg" in the callback and error functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define FROM_JSON 0
/**
 * Table 41 example - dump/load theme and table
 */
void
create_table_ex41(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {

#if FROM_JSON == 0
    hpdftbl_t tbl = calloc(1, sizeof (struct hpdftbl));
    hpdftbl_theme_t theme;

    if(0 == hpdftbl_load(tbl, mkfullpath("tut_ex41.json"))  ) {

        if(0 == hpdftbl_theme_load(&theme, mkfullpath("tut41_theme.json"))) {
            hpdftbl_apply_theme(tbl, &theme);
            hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
        } else {
            fprintf(stderr,"%s\n","Failed to load 'tests/default_theme.json'\n");
            exit(1);
        }
    } else {
        fprintf(stderr,"%s\n","Failed to load 'tests/tut_ex41.json'\n");
        exit(1);
    }

#else

    const size_t num_rows = 4;
    const size_t num_cols = 2;
    //char *table_title = "tut_ex01: 2x2 table";
    hpdftbl_t tbl = hpdftbl_create(num_rows, num_cols);
    content_t content, labels;

    setup_dummy_content_label(&content, &labels, num_rows, num_cols);
    hpdftbl_set_content(tbl, content);
    hpdftbl_set_labels(tbl, labels);

    hpdftbl_use_labels(tbl, TRUE);
    hpdftbl_set_zebra(tbl, TRUE, 0);

    hpdftbl_set_cellspan(tbl,0,0,1,2);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(8);
    HPDF_REAL height = 0;  // Calculate height automatically


    hpdftbl_theme_t *theme = hpdftbl_get_default_theme();
    hpdftbl_get_theme(tbl,theme);

    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
    hpdftbl_dump(tbl, "out/tut_ex41.json");
    hpdftbl_theme_dump(theme,"out/tut41_theme.json");

#endif
}


#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

TUTEX_MAIN(create_table_ex41, FALSE)

