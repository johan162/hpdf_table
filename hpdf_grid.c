#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hpdf.h>

/**
 * Stroke a point grid on specified page to make it easier to
 * position text.
 * @param pdf Document handle
 * @param page Page handle
 */
void
hpdf_stroke_grid(HPDF_Doc pdf,HPDF_Page page) {

    HPDF_REAL height = HPDF_Page_GetHeight(page);
    HPDF_REAL width = HPDF_Page_GetWidth(page);
    HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
    HPDF_UINT x, y;

    HPDF_Page_SetFontAndSize(page, font, 5);
    HPDF_Page_SetGrayFill(page, 0.5);
    HPDF_Page_SetGrayStroke(page, 0.8);

    /* Draw horizontal lines */
    y = 0;
    while (y < height) {
        if (y % 10 == 0)
            HPDF_Page_SetLineWidth(page, 0.5);
        else {
            if (HPDF_Page_GetLineWidth(page) != 0.25)
                HPDF_Page_SetLineWidth(page, 0.25);
        }

        HPDF_Page_MoveTo(page, 0, y);
        HPDF_Page_LineTo(page, width, y);
        HPDF_Page_Stroke(page);

        if (y % 10 == 0 && y > 0) {
            HPDF_Page_SetGrayStroke(page, 0.5);

            HPDF_Page_MoveTo(page, 0, y);
            HPDF_Page_LineTo(page, 5, y);
            HPDF_Page_Stroke(page);

            HPDF_Page_SetGrayStroke(page, 0.8);
        }

        y += 5;
    }


    /* Draw vertical lines */
    x = 0;
    while (x < width) {
        if (x % 10 == 0)
            HPDF_Page_SetLineWidth(page, 0.5);
        else {
            if (HPDF_Page_GetLineWidth(page) != 0.25)
                HPDF_Page_SetLineWidth(page, 0.25);
        }

        HPDF_Page_MoveTo(page, x, 0);
        HPDF_Page_LineTo(page, x, height);
        HPDF_Page_Stroke(page);

        if (x % 50 == 0 && x > 0) {
            HPDF_Page_SetGrayStroke(page, 0.5);

            HPDF_Page_MoveTo(page, x, 0);
            HPDF_Page_LineTo(page, x, 5);
            HPDF_Page_Stroke(page);

            HPDF_Page_MoveTo(page, x, height);
            HPDF_Page_LineTo(page, x, height - 5);
            HPDF_Page_Stroke(page);

            HPDF_Page_SetGrayStroke(page, 0.8);
        }

        x += 5;
    }

    /* Draw horizontal text */
    y = 0;
    while (y < height) {
        if (y % 10 == 0 && y > 0) {
            char buf[12];

            HPDF_Page_BeginText(page);
            HPDF_Page_MoveTextPos(page, 5, y - 2);
            snprintf(buf, sizeof(buf), "%u", y);
            HPDF_Page_ShowText(page, buf);
            HPDF_Page_EndText(page);
        }

        y += 5;
    }


    /* Draw vertical text */
    x = 0;
    while (x < width) {
        if (x % 50 == 0 && x > 0) {
            char buf[12];

            HPDF_Page_BeginText(page);
            HPDF_Page_MoveTextPos(page, x, 5);
            snprintf(buf, sizeof(buf), "%u", x);
            HPDF_Page_ShowText(page, buf);
            HPDF_Page_EndText(page);

            HPDF_Page_BeginText(page);
            HPDF_Page_MoveTextPos(page, x, height - 10);
            HPDF_Page_ShowText(page, buf);
            HPDF_Page_EndText(page);
        }

        x += 5;
    }

    HPDF_Page_SetGrayFill(page, 0);
    HPDF_Page_SetGrayStroke(page, 0);
}


