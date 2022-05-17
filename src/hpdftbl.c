/**
 * @file
 * @brief    Main module for flexible table drawing with HPDF library
 * @author   Johan Persson (johan162@gmail.com)
 *
 * Copyright (C) 2022 Johan Persson
 *
 * @see LICENSE
 *
 * Released under the MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>

#if !(defined _WIN32 || defined __WIN32__)

#endif

#include <string.h>
#include <iconv.h>
#include <hpdf.h>
#include <libgen.h>
#include <sys/stat.h>
#include "hpdftbl.h"


/**
 * @brief Last automatically calculated total height
 */
static HPDF_REAL last_auto_height;

/**
 * @brief Internal state variable to keep track of necessary encodings
 */
static char *target_encoding = HPDFTBL_DEFAULT_TARGET_ENCODING;

/**
 * @brief Internal state variable to keep track of necessary encodings
 */
static char *source_encoding = HPDFTBL_DEFAULT_SOURCE_ENCODING;

/** @brief This stores a pointer to the function acting as the error handler callback */
hpdftbl_error_handler_t hpdftbl_err_handler = NULL;


/**
 * @brief Definition of a dashed line style
 */
typedef struct line_dash_style {
    HPDF_UINT16 dash_ptn[8]; /**< HPDF dash line definition */
    size_t num;              /**< Number of segments in the dashed line */
} line_dash_style_t;

/**
 * @brief Vector with defined line styles.
 *
 * Each row defines a line segment in points and spaces and how many segments are used.
 * For example: A sgement defined as
 * (1,1) Means 1pt solid, 1pt space, which are repeated
 *
 * @see line_dash_style_t
 */
static line_dash_style_t dash_styles[] = {
        {{1, 0, 0, 0, 0, 0, 0, 0}, 0},  /**< Solid line */
        {{1, 1, 0, 0, 0, 0, 0, 0}, 2},  /**< Dotted line variant 1 */
        {{1, 2, 0, 0, 0, 0, 0, 0}, 2},  /**< Dotted line variant 2 */
        {{1, 3, 0, 0, 0, 0, 0, 0}, 2},  /**< Dotted line variant 3 */
        {{1, 5, 0, 0, 0, 0, 0, 0}, 2},  /**< Dotted line variant 4 */
        {{2, 2, 0, 0, 0, 0, 0, 0}, 2},  /**< Dashed line variant 1 */
        {{2, 4, 0, 0, 0, 0, 0, 0}, 2},  /**< Dashed line variant 2 */
        {{4, 2, 0, 0, 0, 0, 0, 0}, 2},  /**< Dashed line variant 3 */
        {{4, 4, 0, 0, 0, 0, 0, 0}, 2},  /**< Dashed line variant 4 */
        {{6, 3, 0, 0, 0, 0, 0, 0}, 2},  /**< Dashed line variant 5 */
        {{5, 2, 2, 2, 0, 0, 0, 0}, 4},  /**< Dashed-dot line variant 1 */
        {{7, 3, 3, 3, 0, 0, 0, 0}, 4},  /**< Dashed-dot line variant 2 */
};

/**
 * @brief Internal helper to set the line style.
 *
 * The drawing of a dashed line uses the underlying HPDF function HPDF_Page_SetDash()
 *
 * @param t Table handle
 * @param style
 * @return -1 on error, 0 on success
 *
 * @see line_dash_style
 *
 */
int
hpdftbl_set_line_dash(hpdftbl_t t, hpdftbl_line_dashstyle_t style) {
    _HPDFTBL_CHK_TABLE(t);
    if (style > LINE_DASHDOT2) {
        _HPDFTBL_SET_ERR(t, -8, -1, -1);
        return -1;
    }
    HPDF_Page_SetDash(t->pdf_page, dash_styles[style].dash_ptn, dash_styles[style].num, 0);
    return 0;
}

/**
 * @brief Switch stroking anchor point
 *
 * Set anchor point for table positioning. By default the top left is used as anchor.
 * Calling this function with FALSE can sets the anchor to bottom left instead.
 *
 * @param tbl Table handle
 * @param anchor Set to TRUE to use top left as anchor, FALSE for bottom left
 */
void
hpdftbl_set_anchor_top_left(hpdftbl_t tbl, const _Bool anchor) {
    tbl->anchor_is_top_left = anchor;
}

/**
 * @brief Get stroking anchor point
 *
 * Get anchor point for table positioning. By default the top left is used.
 *
 * @param tbl Table handle
 *
 * @see hpdftbl_set_anchor_top_left
 * @return TRUE if anchor is top left, FALSE otherwise
 */
_Bool
hpdftbl_get_anchor_top_left(hpdftbl_t tbl) {
    return tbl->anchor_is_top_left;
}

/**
 * @brief Determine text source encoding
 *
 * The default HPDF encoding is a standard PDF encoding. The problem
 * with that is that now  almost 100% of all code is written in
 * UTF-8 encoding and trying to print text strings with accented
 * characters will simply not work.
 * For example the default encoding assumes that strings are
 * given in UTF-8 and sets the target to ISO8859-4 which includes
 * northern europe accented characters.
 * The conversion is internally handled by the standard iconv()
 * routines.
 * @param target The target encoding. See HPDF documentation for
 * supported encodings.
 * @param source The source encodings, i.e. what encodings are sth
 * strings in the source specified in.
 */
void
hpdftbl_set_text_encoding(char *target, char *source) {
    target_encoding = target;
    source_encoding = source;
}

/**
 * @brief Internal function to do text encoding
 *
 * Utility function to do encoding from UTF-8 to the default
 * target encoding which must match the encoding specified in
 * the HPDF_GetFont()
 * @param input Input string
 * @param output Output buffer
 * @param out_len Number of bytes available in the output buffer
 * @return 0 on success, -1 otherwise
 */
static int
do_encoding(char *input, char *output, const size_t out_len) {
    char *out_buf = &output[0];
    char *in_buf = &input[0];
    size_t out_left = out_len - 1;
    size_t in_left = strlen(input);
    iconv_t cd = iconv_open(target_encoding, source_encoding);

    do {
        if (iconv(cd, &in_buf, &in_left, &out_buf, &out_left) == (size_t) -1) {
            iconv_close(cd);
            return -1;
        }
    } while (in_left > 0 && out_left > 0);
    *out_buf = 0;

    iconv_close(cd);
    return 0;
}

/**
 * @brief Strke text with current encoding
 *
 * Utility function to stroke text with character encoding. It is the calling routines
 * responsibility to enclose text in a HPDF_Page_BeginText() / HPDF_Page_EndText()
 * @param page Page handle
 * @param xpos X coordinate
 * @param ypos Y coordinate
 * @param text Text to print
 * @return -1 on error, 0 on success
 */
int
hpdftbl_encoding_text_out(HPDF_Page page, HPDF_REAL xpos, HPDF_REAL ypos, char *text) {
    // Assume that the encoding we are converting to never exceeds three times the
    // original string

    if (NULL == text)
        return 0;

    const size_t out_len = 3 * strlen(text);
#ifdef __cplusplus
    char *output = static_cast<char*>(calloc(1, out_len));
#else
    char *output = calloc(1, out_len);
#endif
    if (-1 == do_encoding(text, output, out_len)) {
        _HPDFTBL_SET_ERR(NULL, -4, (int) xpos, (int) ypos);
        HPDF_Page_TextOut(page, xpos, ypos, "???");
        return -1;
    } else {
        HPDF_Page_TextOut(page, xpos, ypos, output);
    }
    free(output);
    return 0;
}

/**
 * @brief Draw rectangle with rounded corner
 *
 * Draw a rectangle with rounded corner with the current line width, color. The rectangle will not be stroked.
 *
 * @param page Page handle
 * @param xpos Lower left x-position of rectangle
 * @param ypos Lower left y-position of rectangle
 * @param width Width of rectangle
 * @param height Height of rectangle
 * @param rad Radius of corners
 */
void
HPDF_RoundedCornerRectangle(HPDF_Page page, HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                            HPDF_REAL rad) {

    // Left vertical line
    HPDF_Page_MoveTo(page, xpos, ypos + rad);
    HPDF_Page_LineTo(page, xpos, ypos + height - rad);

    // Upper left
    HPDF_Page_CurveTo2(page, xpos, ypos + height, xpos + rad, ypos + height);

    // Top line
    HPDF_Page_LineTo(page, xpos + width - rad, ypos + height);

    // Upper right
    HPDF_Page_CurveTo2(page, xpos + width, ypos + height, xpos + width, ypos + height - rad);

    // Right vertical
    HPDF_Page_LineTo(page, xpos + width, ypos + rad);

    // Lower right
    HPDF_Page_CurveTo2(page, xpos + width, ypos, xpos + width - rad, ypos);

    // Bottom line
    HPDF_Page_LineTo(page, xpos + rad, ypos);

    // Lower left corner
    HPDF_Page_CurveTo2(page, xpos, ypos, xpos, ypos + rad);
}

/**
 * @brief The margin from the bottom of the cell to the baseline of the text is calculated
 * as a fraction of the font size.
 * The margin is calculated as:
 * @code bottom_margin = fontsize * f @endcode
 * The default margin is specified by the define @ref DEFAULT_AUTO_VBOTTOM_MARGIN_FACTOR "DEFAULT_AUTO_VBOTTOM_MARGIN_FACTOR"
 *
 * @param t Table handle
 * @param f Bottom margin factor
 */
void
hpdftbl_set_bottom_vmargin_factor(hpdftbl_t t, HPDF_REAL f) {
    t->bottom_vmargin_factor = f;
}

/**
 * @brief Create a new table with no title
 *
 * Create a new table structure. This is the basic handler needed for most other API
 * functions.
 * @param rows Number of rows
 * @param cols Number of columns
 * @return A handle to a table, NULL in case of OOM
 */
hpdftbl_t
hpdftbl_create(size_t rows, size_t cols) {
    return hpdftbl_create_title(rows, cols, NULL);
}

/**
 * @brief Create a new table with title top row
 *
 * Create a new table structure. This is the basic handler needed for most other API
 * functions.
 * @param rows Number of rows
 * @param cols Number of columns
 * @param title Title of table
 * @return A handle to a table, NULL in case of OOM
 */
hpdftbl_t
hpdftbl_create_title(size_t rows, size_t cols, char *title) {

    // Initializing to zero means default color is black
#ifdef __cplusplus
    hpdftbl_t t = static_cast<hpdftbl_t>(calloc(1, sizeof(struct hpdftbl)));
#else
    hpdftbl_t t = calloc(1, sizeof(struct hpdftbl));
#endif
    if (t == NULL) {
        _HPDFTBL_SET_ERR(t, -5, -1, -1);
        return NULL;
    }

    t->anchor_is_top_left = TRUE;

#ifdef __cplusplus
    t->cells = static_cast<hpdftbl_cell_t*>(calloc(cols*rows, sizeof(hpdftbl_cell_t)));
#else
    t->cells = calloc(cols * rows, sizeof(hpdftbl_cell_t));
#endif
    if (t->cells == NULL) {
        free(t);
        _HPDFTBL_SET_ERR(t, -5, -1, -1);
        return NULL;
    }

    hpdftbl_cell_t *cell = t->cells;
    for(size_t r=0; r < rows; r++) {
        for(size_t c=0; c < cols; c++) {
            cell->row = r;
            cell->col = c;
            cell++;
        }
    }

    t->cols = cols;
    t->rows = rows;

    // Setup common column widths
#ifdef __cplusplus
    t->col_width_percent = static_cast<float*>(calloc(cols, sizeof(float)));
#else
    t->col_width_percent = calloc(cols, sizeof(float));
#endif
    if (t->col_width_percent == NULL) {
        free(t->cells);
        free(t);
        _HPDFTBL_SET_ERR(t, -5, -1, -1);
        return NULL;
    }

    // Initialize column width. Setting it to zero will set the column to the default
    // width.
    for (size_t i = 0; i < cols; i++) {
        t->col_width_percent[i] = 0.0f;
    }

    if (title) {
        t->title_txt = strdup(title);
        if (t->title_txt == NULL) {
            free(t->col_width_percent);
            free(t->cells);
            free(t);
            _HPDFTBL_SET_ERR(t, -5, -1, -1);
            return NULL;
        }
    }

    hpdftbl_theme_t *theme = hpdftbl_get_default_theme();
    hpdftbl_apply_theme(t, theme);
    hpdftbl_destroy_theme(theme);

    return t;
}

/**
 * @brief Set the minimum row height in the table.
 *
 * The row height is normally calculated based on the font size and if labels are
 * displayed or not. However, it is not possible for the table to know the height
 * of specific widgets (for example) without a two-pass table drawing algorithm.
 *
 * To handle thos odd cases when the calculated height is not sufficient a manual
 * minimum height can be specified.
 *
 * @param t Table handler
 * @param h The minimum height (in points). If specified as 0 the min height will have no effect.
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_set_min_rowheight(hpdftbl_t t, float h) {
    t->minrowheight=h;
    return 0;
}


/**
 * @brief Set column width as percentage of overall table width
 *
 * Specify column width as percentage of total column width. Note that this will only take effect
 * if the table has an overall width specified when stroked. Too avoid errors one column should
 * be left unspecified to let the library use whatever space is left for that column.
 *
 * @param t Table handle
 * @param c Column to set width of first column has index 0
 * @param w Width as percentage in range [0.0, 100.0]
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_set_colwidth_percent(hpdftbl_t t, size_t c, float w) {
    _HPDFTBL_CHK_TABLE(t);
    if (c >= t->cols) {
        _HPDFTBL_SET_ERR(t, -2, -1, c);
        return -1;
    }
    if (w < 0.0 || w > 100.0) {
        _HPDFTBL_SET_ERR(t, -12, -1, c);
        return -1;
    }
    t->col_width_percent[c] = w;
    return 0;
}

/**
 * @brief Set outer border grid style.
 *
 * @param t Table handle
 * @param width Line width (in pt)
 * @param color Line color
 * @param dashstyle Line dash style
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_inner_grid_style()
 */
int
hpdftbl_set_outer_grid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle) {
    _HPDFTBL_CHK_TABLE(t);
    t->outer_grid.width = width;
    t->outer_grid.color = color;
    t->outer_grid.line_dashstyle = dashstyle;
    return 0;
}

/**
 * @brief Set inner border grid style.
 *
 * This is a shortform to set both the vertical and horizontal gridline style with one call.
 *
 * @param t Table handle
 * @param width Line width (in pt)
 * @param color Line color
 * @param dashstyle Line dash style
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_inner_hgrid_style(), hpdftbl_set_inner_vgrid_style(), hpdftbl_set_outer_grid_style()
 */
int
hpdftbl_set_inner_grid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle) {
    _HPDFTBL_CHK_TABLE(t);
    hpdftbl_set_inner_hgrid_style(t, width, color, dashstyle);
    hpdftbl_set_inner_vgrid_style(t, width, color, dashstyle);
    return 0;
}

/**
 * @brief Set inner horizontal border grid style.
 *
 * @param t Table handle
 * @param width Line width (in pt)
 * @param color Line color
 * @param dashstyle Line dash style
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_inner_grid_style(), hpdftbl_set_inner_vgrid_style()
 */
int
hpdftbl_set_inner_hgrid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle) {
    _HPDFTBL_CHK_TABLE(t);
    t->inner_hgrid.width = width;
    t->inner_hgrid.color = color;
    t->inner_hgrid.line_dashstyle = dashstyle;
    return 0;
}

/**
 * @brief Set inner vertical border grid style.
 *
 * @param t Table handle
 * @param width Line width (in pt)
 * @param color Line color
 * @param dashstyle Line dash style
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_inner_grid_style(), hpdftbl_set_inner_hgrid_style()
 */
int
hpdftbl_set_inner_vgrid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle) {
    _HPDFTBL_CHK_TABLE(t);
    t->inner_vgrid.width = width;
    t->inner_vgrid.color = color;
    t->inner_vgrid.line_dashstyle = dashstyle;
    return 0;
}

/**
 * @brief Set inner horizontal top border grid style.
 *
 * This would be the gridline just below the
 * header row.
 *
 * @param t Table handle
 * @param width Line width (in pt)
 * @param color Line color
 * @param dashstyle Line dash style
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_inner_hgrid_style()
 */
int
hpdftbl_set_inner_tgrid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle) {
    _HPDFTBL_CHK_TABLE(t);
    t->inner_tgrid.width = width;
    t->inner_tgrid.color = color;
    t->inner_tgrid.line_dashstyle = dashstyle;
    return 0;
}

/**
 *
 * @param t     Table handle
 * @param use   TRUE=Use Zebra, FALSE=Don't use zebra
 * @param phase 0=Start with color 1, 1=Start with color 1
 * @return 0 on successes -1 on failure
 */
int
hpdftbl_set_zebra(hpdftbl_t t, _Bool use, int phase) {
    t->use_zebra = use;
    t->zebra_phase = phase;
    return 0;
}

/**
 * @brief Specify first and second color for a zebra grid table.
 *
 * By default the colors start with `z1` color. To have the top row
 * (below any potential header row) instead start with z2 specify
 * `phase=1` in the hpdftbl_set_zebra() function.
 * @param t   Table handle
 * @param z1  Color 1
 * @param z2  Color 2
 * @return 0 on successes -1 on failure
 */
int
hpdftbl_set_zebra_color(hpdftbl_t t, HPDF_RGBColor z1,  HPDF_RGBColor z2) {
    t->zebra_color1 = z1;
    t->zebra_color2 = z2;
    return 0;
}


/**
 * @brief Specify style for table header row
 *
 * Set the font properties and background for the header row which is the top
 * row if enabled. The header row will be automatically enabled after calling
 * this function. The header can be enabled/disabled separately with hpdftbl_use_header()
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Font color
 * @param background Cell background color
 * @return 0 on success, -1 on failure
 * @see hpdftbl_use_header()
 */

int
hpdftbl_set_header_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _HPDFTBL_CHK_TABLE(t);
    t->header_style.font = font;
    t->header_style.fsize = fsize;
    t->header_style.color = color;
    t->header_style.background = background;
    return 0;
}

/**
 * @brief Set table background color
 *
 * @param t Table handle
 * @param background Background color
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_set_background(hpdftbl_t t, HPDF_RGBColor background) {
    _HPDFTBL_CHK_TABLE(t);
    t->content_style.background = background;
    return 0;
}

/**
 * @brief Set table header horizontal text align
 *
 * @param t Table handle
 * @param align Alignment
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_set_header_halign(hpdftbl_t t, hpdftbl_text_align_t align) {
    _HPDFTBL_CHK_TABLE(t);
    t->header_style.halign = align;
    return 0;
}

/**
 * @brief Enable/disable the interpretation of the top row as a header row
 *
 * A header row will have a different style and labels will be disabled on this row.
 * In addition the text will be centered vertically and horizontal in the cell.
 *
 * @param t Table handle
 * @param use TRUE to enable, FALSE to disable
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_header_style()
 */
int
hpdftbl_use_header(hpdftbl_t t, _Bool use) {
    _HPDFTBL_CHK_TABLE(t);
    t->use_header_row = use;
    return 0;
}

/**
 * @brief Enable/Disable the use of cell labels.
 *
 * By default a newly created
 * table will not use cell labels. Enabling labels will also by default
 * enable the special label grid style. To adjust the grid style separately
 * us the @ref hpdftbl_use_labelgrid() method.
 *
 * @param t Table handle
 * @param use Set to TRUE for cell labels
 * @return 0 on success, -1 on failure
 * @see hpdftbl_use_labelgrid()
 */
int
hpdftbl_use_labels(hpdftbl_t t, _Bool use) {
    _HPDFTBL_CHK_TABLE(t);
    t->use_cell_labels = use;
    t->use_label_grid_style = use;
    return 0;
}

/**
 * @brief Shorter vertical line to mark labels.
 *
 * Set the usage of special grid style where the vertical grid
 * only covers the label text and a gap to the next line. Horizontal
 * lines are drawn as usual.
 * The label grid style gives the table a "lighter" look.
 *
 * @param t Table handle
 * @param use TRUE to use label grid, FALSE o disable it
 * @return 0 on success, -1 on failure
 * @see hpdftbl_use_labels()
 */
int
hpdftbl_use_labelgrid(hpdftbl_t t, _Bool use) {
    _HPDFTBL_CHK_TABLE(t);
    t->use_label_grid_style = use;
    return 0;
}

/**
 * @brief Set an optional tag for the table
 *
 * Set an optional tag in the table. The tag can be a pointer
 * to anything. The tag is passed as the first argument in the
 * various callbacks and can be used to supply table specific
 * information or identify a specific table in the case the
 * same callback is used for multiple tables.
 * @param t The table handle
 * @param tag The tag (pointer to any object)
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_set_tag(hpdftbl_t t, void *tag) {
    _HPDFTBL_CHK_TABLE(t);
    t->tag = tag;
    return 0;
}

/**
 * @brief Internal function to destroy an individual cell
 *
 * Internal function. Destroy content in a cell
 * @param t Table handle
 * @param r Row
 * @param c Column
 * @return 0 on success, -1 on failure
 */
static int
cell_destroy(hpdftbl_t t, size_t r, size_t c) {
    _HPDFTBL_CHK_TABLE(t);
    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];
    if (cell->label)
        free(cell->label);
    if (cell->content)
        free(cell->content);
    if (cell->content_dyncb)
        free(cell->content_dyncb);
    if (cell->content_style_dyncb)
        free(cell->content_style_dyncb);
    if (cell->label_dyncb)
        free(cell->label_dyncb);
    if (cell->canvas_dyncb)
        free(cell->canvas_dyncb);
    cell->parent_cell = NULL;
    return 0;
}

/**
 * @brief Destroy a table and free all memory
 *
 * Destroy a table previous created with table_create(), It is the calling routines
 * responsibility not to acceess \c t again.
 * @param t Handle to table
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_destroy(hpdftbl_t t) {
    _HPDFTBL_CHK_TABLE(t);
    if (t->title_txt)
        free(t->title_txt);
    if (t->label_dyncb)
        free(t->label_dyncb);
    if (t->content_dyncb)
        free(t->content_dyncb);
    if (t->content_style_dyncb)
        free(t->content_style_dyncb);
    if (t->post_dyncb)
        free(t->post_dyncb);
    if (t->canvas_dyncb)
        free(t->canvas_dyncb);
    free(t->col_width_percent);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            cell_destroy(t, r, c);
        }
    }
    free(t->cells);
    free(t);
    return 0;
}

/**
 * @brief Internal function. Check that a row and column are within the table
 *
 * Internal function. Check that a row and column are within the table
 * @param t Table handle
 * @param r Row
 * @param c Column
 * @return TRUE if within bounds, FALSE otherwise
 */
_Bool
chktbl(hpdftbl_t t, size_t r, size_t c) {
    if (r < t->rows && c < t->cols)
        return TRUE;
    _HPDFTBL_SET_ERR(t, -2, r, c);
    return FALSE;
}

/**
 * @brief Set content for specific cell
 *
 * Set label and content for a specific cell. If the specified cell is part of
 * another cells spanning an error occurs (returns -1),
 * @param t Table handle
 * @param r Row
 * @param c Column
 * @param label Label
 * @param content Text content
 * @return -1 on error, 0 if successful
 */
int
hpdftbl_set_cell(hpdftbl_t t, size_t r, size_t c, char *label, char *content) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c)) return -1;
    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If we are trying to specify a cell that is part of another cell that spans we
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    cell->colspan = 1;
    cell->rowspan = 1;
    cell->label = label ? strdup(label) : NULL;
    cell->content = content ? strdup(content) : NULL;
    return 0;
}

/**
 * @brief Set cell spanning
 *
 * Set row and column spanning for a cell, an expanded cell is referenced via the position of it's top-left
 * cell
 * @param t Table handle
 * @param r Row
 * @param c Column
 * @param rowspan Row span
 * @param colspan Column span
 * @return -1 on error, 0 if successful
 *
 * @see hpdftbl_clear_spanning()
 */
int
hpdftbl_set_cellspan(hpdftbl_t t, size_t r, size_t c, size_t rowspan, size_t colspan) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c))
        return -1;

    if (r + rowspan - 1 >= t->rows || c + colspan - 1 >= t->cols) {
        _HPDFTBL_SET_ERR(t, -7, r, c);
        return -1;
    }
    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    // Clear any potential previous spanning setting for this cell
    if (cell->colspan > 1 || cell->rowspan > 1) {
        for (size_t rr = r; rr < cell->rowspan + r; rr++) {
            for (size_t cc = c; cc < cell->colspan + c; cc++) {
                t->cells[_HPDFTBL_IDX(rr, cc)].parent_cell = NULL;
            }
        }
    }

    cell->colspan = colspan;
    cell->rowspan = rowspan;
    for (size_t rr = r; rr < rowspan + r; rr++) {
        for (size_t cc = c; cc < colspan + c; cc++) {
            if (rr != r || cc != c) {
                // Point back to the parent cell
                t->cells[_HPDFTBL_IDX(rr, cc)].parent_cell = cell;

                // Reset the row and col-span in covered cells
                t->cells[_HPDFTBL_IDX(rr, cc)].colspan = 1;
                t->cells[_HPDFTBL_IDX(rr, cc)].rowspan = 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Clear all cell spanning
 *
 * Reset all spanning cells to no spanning
 * @param t Table handle
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_cellspan()
 */
int
hpdftbl_clear_spanning(hpdftbl_t t) {
    _HPDFTBL_CHK_TABLE(t);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            t->cells[_HPDFTBL_IDX(r, c)].colspan = 1;
            t->cells[_HPDFTBL_IDX(r, c)].rowspan = 1;
            t->cells[_HPDFTBL_IDX(r, c)].parent_cell = NULL;
        }
    }
    return 0;
}

/**
 * @brief Internal function.
 *
 * Internal function. Set the current HPDF font, size and color
 * @param t Table handle
 * @param fontname Font name
 * @param fsize Font size
 * @param color font color
 */
static void
set_fontc(hpdftbl_t t, char *fontname, HPDF_REAL fsize, HPDF_RGBColor color) {
    HPDF_Page_SetFontAndSize(t->pdf_page, HPDF_GetFont(t->pdf_doc, fontname, HPDFTBL_DEFAULT_TARGET_ENCODING), fsize);
    HPDF_Page_SetRGBFill(t->pdf_page, color.r, color.g, color.b);
    HPDF_Page_SetTextRenderingMode(t->pdf_page, HPDF_FILL);
}

/*static void
_stroke_haligned_text(hpdftbl_t t, char *txt, hpdftbl_text_align_t align, HPDF_REAL x, HPDF_REAL y, HPDF_REAL width) {
    HPDF_REAL xpos = x;
    HPDF_REAL tw = HPDF_Page_TextWidth(t->pdf_page, txt);
    if (align == CENTER) {
        xpos = x + (width - tw) / 2.0;
    } else if (align == RIGHT) {
        xpos = x + (width - tw);
    }
    HPDF_Page_BeginText(t->pdf_page);
    HPDF_Page_TextOut(t->pdf_page, xpos, y, txt);
    HPDF_Page_EndText(t->pdf_page);
}
*/

/**
 * @brief Internal function t stroke the title.
 *
 * Internal function.Stroke the optional table title
 * @param t Table handle
 * @return -1 on failure, 0 otherwise
 */
static HPDF_REAL
table_title_stroke(hpdftbl_t t) {
    if (t->title_txt == NULL)
        return 0;

    HPDF_REAL x = t->posx;
    HPDF_REAL y = t->posy;

    //const HPDF_REAL page_height = HPDF_Page_GetHeight(page);
    if (t->anchor_is_top_left) {
        y -= t->height;
        y -= 1.5f * t->title_style.fsize;
    }

    const HPDF_REAL height = 1.5f * t->title_style.fsize;

    // Stoke outer border and fill
    HPDF_Page_SetRGBStroke(t->pdf_page, t->outer_grid.color.r, t->outer_grid.color.g, t->outer_grid.color.b);
    HPDF_Page_SetRGBFill(t->pdf_page, t->title_style.background.r, t->title_style.background.g,
                         t->title_style.background.b);
    HPDF_Page_SetLineWidth(t->pdf_page, t->outer_grid.width);
    HPDF_Page_Rectangle(t->pdf_page, x, y + t->height, t->width, height);
    HPDF_Page_FillStroke(t->pdf_page);

    set_fontc(t, t->title_style.font, t->title_style.fsize, t->title_style.color);

    HPDF_REAL left_right_padding = t->outer_grid.width + 3;
    HPDF_REAL xpos = x + left_right_padding;
    const HPDF_REAL ypos = y + t->height + t->outer_grid.width * 2 + t->title_style.fsize * 0.28f;

    if (t->title_style.halign == CENTER) {
        xpos = x + (t->width - HPDF_Page_TextWidth(t->pdf_page, t->title_txt)) / 2.0f;
    } else if (t->title_style.halign == RIGHT) {
        xpos = x + (t->width - HPDF_Page_TextWidth(t->pdf_page, t->title_txt)) - left_right_padding;
    }

    HPDF_Page_BeginText(t->pdf_page);
    hpdftbl_encoding_text_out(t->pdf_page, xpos, ypos, t->title_txt);
    HPDF_Page_EndText(t->pdf_page);

    // Return height of the stroked bounding box
    return height;
}

/**
 * @brief Set the text for the cell labels
 *
 * Set labels for all the cell. It is the calling functions responsibility that
 * the labels array is big enough to cover the entire table. The string array
 * corresponds to a flattened 2-d array and the label for cell (r,c) is
 * calculated as (r * num_cols + c) where num_cols is the number of columns
 * in the table.
 *
 * It is allowed to specify NULL as placeholder for empty labels.
 * The actual text in the table will be allocated with strdup() so it is safe
 * to free the memory for the labels after the call to this function.
 * Please note that even if the table contains spanning cells the content
 * data must include empty data for covered cells. For a N x M table the data
 * must have (N*M) entries.
 *
 * @param t Table handle
 * @param labels A one dimensional string array of labels
 * @return -1 on error, 0 if successful
 * @see hpdftbl_set_cell_label_cb()
 * @see hpdftbl_set_label_cb()
 */
int
hpdftbl_set_labels(hpdftbl_t t, char **labels) {
    _HPDFTBL_CHK_TABLE(t);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            size_t idx = r * t->cols + c;
            hpdftbl_cell_t *cell = &t->cells[idx];
            cell->label = labels[idx] ? strdup(labels[idx]) : NULL;
        }
    }
    return 0;
}

/**
 * @brief Set the content for the table
 *
 * Set content for all cells. It is the calling functions responsibility that
 * the content array is big enough to cover the entire table. The string array
 * corresponds to a flattened 2-d array and the label for cell (r,c) is
 * calculated as (r * num_cols + c) where num_cols is the number of columns
 * in the table.
 *
 * It is allowed to specify NULL as placeholder for empty labels.
 *  The actual text in the table will be allocated with strdup() so it is safe
 * to free the memory for the labels after the call to this function.
 * Please note that even if the table contains spanning cells the content
 * data must include empty data for covered cells. For a N x M table the data
 * must have (N*M) entries.
 *
 * Another way to specify the content is to use the callback mechanism. By setting
 * up a content callback function that returns the content for a cell.
 *
 * @param t Table handle
 * @param content A one dimensional string array of content string
 * @return -1 on error, 0 if successful
 * @see hpdftbl_set_content_callback()
 * @see hpdftbl_set_cell_content_callback()
 */
int
hpdftbl_set_content(hpdftbl_t t, char **content) {
    _HPDFTBL_CHK_TABLE(t);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            size_t idx = r * t->cols + c;
            hpdftbl_cell_t *cell = &t->cells[idx];
            cell->content = content[idx] ? strdup(content[idx]) : NULL;
        }
    }
    return 0;
}

/**
 * @brief Set the text style for labels in the entire table.
 *
 * Set font, color and background options for cell labels. If a style callback have been
 * specified for either the table or a cell that style take precedence.
 *
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return -1 on error, 0 if successful
 */
int
hpdftbl_set_label_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _HPDFTBL_CHK_TABLE(t);
    t->label_style.font = font;
    t->label_style.fsize = fsize;
    t->label_style.color = color;
    t->label_style.background = background;
    return 0;
}

/**
 * @brief Set text style for text content.
 *
 * Set style options for cell content (font, color, background). This will be applied for all cells in the table.
 * If a style callback have been
 * specified for either the table or a cell that style take precedence.
 *
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return -1 on error, 0 if successful
 * @see hpdftbl_set_cell_content_style()
 * @see hpdftbl_set_cell_content_style_cb()
 */
int
hpdftbl_set_content_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _HPDFTBL_CHK_TABLE(t);
    t->content_style.font = font;
    t->content_style.fsize = fsize;
    t->content_style.color = color;
    t->content_style.background = background;
    return 0;
}

/**
 * @brief Set the text style for an entire row of cells.
 *
 * Set font options for the specified row of cells. This will override the global cell content.
 * @param t Table handle
 * @param r Row to affect
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_content_style()
 * @see hpdftbl_set_cell_content_style_cb()
 */
int
hpdftbl_set_row_content_style(hpdftbl_t t, size_t r, char *font, HPDF_REAL fsize, HPDF_RGBColor color,
                              HPDF_RGBColor background) {
    for (size_t c = 0; c < t->cols; c++) {
        hpdftbl_set_cell_content_style(t, r, c, font, fsize, color, background);
    }
    return 0;
}

/**
 * @brief Set the text style for an entire column of cells.
 *
 * Set font options for the specified column of cells. This will override the global cell content setting.
 * @param t Table handle
 * @param c Column to affect
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_content_style()
 * @see hpdftbl_set_cell_content_style_cb()
 */
int
hpdftbl_set_col_content_style(hpdftbl_t t, size_t c, char *font, HPDF_REAL fsize, HPDF_RGBColor color,
                              HPDF_RGBColor background) {
    for (size_t r = 0; r < t->rows; r++) {
        hpdftbl_set_cell_content_style(t, r, c, font, fsize, color, background);
    }
    return 0;
}

/**
 * @brief Set the text style for content of specified cell.
 *
 * SSet the font style for content of specified cell. This will override the global cell content setting.
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return 0 on success, -1 on failure
 * @see hpdftbl_set_content_style()
 * @see hpdftbl_set_cell_content_style_cb()
 */
int
hpdftbl_set_cell_content_style(hpdftbl_t t, size_t r, size_t c,
                               char *font, HPDF_REAL fsize, HPDF_RGBColor color,
                               HPDF_RGBColor background) {
    _HPDFTBL_CHK_TABLE(t);
    chktbl(t, r, c);
    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];
    cell->content_style.font = font;
    cell->content_style.fsize = fsize;
    cell->content_style.color = color;
    cell->content_style.background = background;
    return 0;
}


/**
 * @brief Set the table title text style
 *
 * Set font options for title
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_title()
 * @see hpdftbl_set_title_halign()
 */
int
hpdftbl_set_title_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _HPDFTBL_CHK_TABLE(t);
    t->title_style.font = font;
    t->title_style.fsize = fsize;
    t->title_style.color = color;
    t->title_style.background = background;
    return 0;
}

/**
 * @brief Set table title
 *
 * Set table title. A title will occupy a separate row above the table that is not included in the
 * row count. A table is enabled when the table text is <> NULL and disabled when the title text is == NULL.
 *
 * @param t Table handle
 * @param title Title string
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_title_style()
 * @see hpdftbl_set_title_halign()
 */
int
hpdftbl_set_title(hpdftbl_t t, char *title) {
    _HPDFTBL_CHK_TABLE(t);
    if (t->title_txt)
        free(t->title_txt);
    t->title_txt = strdup(title);
    return 0;
}

/**
 * @brief Set horizontal alignment for table title
 *
 * @param t Table handle
 * @param align Alignment
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_title()
 * @see hpdftbl_set_title_style()
 */
int
hpdftbl_set_title_halign(hpdftbl_t t, hpdftbl_text_align_t align) {
    _HPDFTBL_CHK_TABLE(t);
    t->title_style.halign = align;
    return 0;
}

/**
 * @brief Construct the table from a array specification
 *
 * Create and stroke a table specified by a data structure. This makes it easier to separate
 * the view of the data from the model which provides the data. The intended use case is that
 * the data structure specifies the core layout of the table together with the labels and
 * callback functions to handle the content in each cell.
 * Using this method to create a table also makes it much more maintainable.
 * @param pdf_doc The PDF overall document
 * @param pdf_page The pageto stroke to
 * @param tbl_spec The table specification
 * @param theme Table theme to be applied
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_stroke()
 */
int
hpdftbl_stroke_from_data(HPDF_Doc pdf_doc, HPDF_Page pdf_page, hpdftbl_spec_t *tbl_spec, hpdftbl_theme_t *theme) {

    hpdftbl_t t = hpdftbl_create_title(tbl_spec->rows, tbl_spec->cols, tbl_spec->title);
    if (NULL == t) {
        _HPDFTBL_SET_ERR(NULL, -5, -1, -1);
        return -1;
    }

    hpdftbl_use_header(t, tbl_spec->use_header);
    hpdftbl_set_content_cb(t, tbl_spec->content_cb);
    hpdftbl_set_label_cb(t, tbl_spec->label_cb);
    hpdftbl_set_content_style_cb(t, tbl_spec->style_cb);

    if (theme)
        hpdftbl_apply_theme(t, theme);

    hpdftbl_use_labels(t, tbl_spec->use_labels);
    hpdftbl_use_labelgrid(t, tbl_spec->use_labelgrid);

    if (tbl_spec->cell_spec) {
        size_t i = 0;
        while (TRUE) {
            struct hpdftbl_cell_spec *spec = &tbl_spec->cell_spec[i];

            if (spec->rowspan == 0 && spec->colspan == 0)
                break;

            if (-1 == hpdftbl_set_cell(t, spec->row, spec->col, spec->label, NULL) ||
                -1 == hpdftbl_set_cellspan(t, spec->row, spec->col, spec->rowspan, spec->colspan) ||
                -1 == hpdftbl_set_cell_content_cb(t, spec->row, spec->col, spec->content_cb) ||
                -1 == hpdftbl_set_cell_label_cb(t, spec->row, spec->col, spec->label_cb) ||
                -1 == hpdftbl_set_cell_content_style_cb(t, spec->row, spec->col, spec->style_cb) ||
                -1 == hpdftbl_set_cell_canvas_cb(t, spec->row, spec->col, spec->canvas_cb)) {
                hpdftbl_destroy(t);
                return -1;
            }

            i++;
        }
    }

    // Final chance for the client to do any specific table modifications
    if (tbl_spec->post_cb) {
        tbl_spec->post_cb(t);
    }

    int ret = hpdftbl_stroke(pdf_doc, pdf_page, t, tbl_spec->xpos, tbl_spec->ypos, tbl_spec->width, tbl_spec->height);
    //hpdftbl_destroy(t);
    return ret;
}

/**
 * @brief Internal function.
 *
 * Internal function. Calculate the relative position of each cell in the
 * table taking row and column spanning into account. This function loops
 * through each cell and calculates its delta_X and delta_y positions which
 * are hen stored in each cell data structure.
 *
 * The calculation is done at the time of stroking and is not available
 * prior.
 *
 * @param t Table handle
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_stroke()
 */
static int
calc_cell_pos(hpdftbl_t t) {
    // Calculate relative position for all cells in relation
    // to bottom left table corner
    HPDF_REAL base_cell_height = (float)(t->height) / (float)(t->rows);
    //HPDF_REAL base_cell_width = t->width / t->cols;
    HPDF_REAL base_cell_width_percent = 100.0f / (float)t->cols;
    HPDF_REAL delta_x = 0;
    HPDF_REAL delta_y = 0;

    // Recalculate column widths
    // Pass 1. Determine how many columns have been manually specified
    float tot_specified_width_percent = 0;
    size_t num_specified_cols = 0;
    for (size_t c = 0; c < t->cols; c++) {
        if (t->col_width_percent[c] > 0) {
            num_specified_cols++;
            tot_specified_width_percent += t->col_width_percent[c];
        }
    }

    if (tot_specified_width_percent > 100.0) {
        _HPDFTBL_SET_ERR(t, -12, -1, -1);
        return -1;
    }

    // Recalculate column widths
    // Pass 2. Divide the remaining width along the unspecified columns
    const float remaining_width_percent = 100.0f - tot_specified_width_percent;
    const float num_unspecified_cols = (float) (t->cols - num_specified_cols);
    if (num_unspecified_cols > 0) {
        base_cell_width_percent = remaining_width_percent / num_unspecified_cols;
    }

    // Sanity check
    if (base_cell_width_percent < HPDFTBL_MIN_CALCULATED_PERCENT_CELL_WIDTH) {
        _HPDFTBL_SET_ERR(t, -13, -1, -1);
        return -1;
    }

    for (size_t c = 0; c < t->cols; c++) {
        if (t->col_width_percent[c] == 0.0) {
            t->col_width_percent[c] = base_cell_width_percent;
        }
    }

    // Calculate the position for all cells.
    //
    // Pass 1. Give the basic position for all cells without
    // taking spanning in consideration
    for (int r = (int)t->rows - 1; r >= 0; r--) {
        for (size_t c = 0; c < t->cols; c++) {
            hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];
            cell->delta_x = delta_x;
            cell->delta_y = delta_y;
            cell->width = (t->col_width_percent[c] / 100.0f) * t->width; //base_cell_width;
            cell->height = base_cell_height;
            delta_x += cell->width; //base_cell_width;
        }
        delta_x = 0;
        delta_y += base_cell_height;
    }

    // Adjust for row and column spanning
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];
            if (cell->rowspan > 1) {
                cell->delta_y = t->cells[(r + cell->rowspan - 1) * t->cols + c].delta_y;
                cell->height = (float)cell->rowspan * base_cell_height;
            }
            if (cell->colspan > 1) {
                HPDF_REAL col_span_with = 0.0f;
                for (size_t cc = 0; cc < cell->colspan; cc++) {
                    col_span_with += t->cells[_HPDFTBL_IDX(r, cc + c)].width;
                }
                cell->width = col_span_with;
            }
        }
    }

#ifdef ENABLE_DEBUG_TRACE_PRINT
    for (size_t c = 0; c < t->cols; c++) {
        printf("%.1f (%.1f) @ %.1f, \n",t->col_width_percent[c],t->cells[_HPDFTBL_IDX(0, c)].width,t->cells[_HPDFTBL_IDX(0, c)].delta_x);
    }
    printf("\n");
#endif

    return 0;
}


/**
 * @brief Get the height calculated for the last constructed table
 *
 * Get the last automatically calculated height when stroking a table.
 * (The height will be automatically calculated if it was specified as 0)
 * @param height Returned height
 * @return -1 on error, 0 if successful
 */
int
hpdftbl_get_last_auto_height(HPDF_REAL *height) {
    if (last_auto_height > 0) {
        *height = last_auto_height;
        return 0;
    }
    _HPDFTBL_SET_ERR(NULL, -10, -1, -1);
    return -1;
}

//static void
//_draw_cross(hpdftbl_t t, HPDF_REAL x, HPDF_REAL y, HPDF_REAL l) {
//    HPDF_Page_SetRGBStroke(t->pdf_page, 1, 0, 0);
//    HPDF_Page_SetLineWidth(t->pdf_page, 1.2);
//    HPDF_Page_MoveTo(t->pdf_page, x, y-l);
//    HPDF_Page_LineTo(t->pdf_page, x, y+l);
//    HPDF_Page_MoveTo(t->pdf_page, x-l, y);
//    HPDF_Page_LineTo(t->pdf_page, x+l, y);
//    HPDF_Page_Stroke(t->pdf_page);
//}


/**
 * @brief Internal function.
 *
 * Stroke each cell content.
 * @param t Table handle
 * @param r Row
 * @param c Column
 * @see hpdftbl_stroke()
 *
 */
static void
table_cell_stroke(hpdftbl_t t, const size_t r, const size_t c) {
    hpdftbl_cell_t *cell = &t->cells[r * t->cols + c];

    if (cell->parent_cell != NULL) {
        return;
    }

    HPDF_REAL x = t->posx;
    HPDF_REAL y = t->posy;

    if (t->anchor_is_top_left) {
        y -= t->height;
        if (t->title_txt) {
            y -= 1.5f * t->title_style.fsize;
        }
    }

    HPDF_REAL left_right_padding = c == 0 ? t->outer_grid.width + 2 : t->inner_vgrid.width + 2;

    // Check if this is the first row, and we should format it as a header row.
    // In case this is a header row we also ignore the label
    if (t->use_header_row && r == 0) {
        HPDF_Page_SetRGBFill(t->pdf_page, t->header_style.background.r, t->header_style.background.g,
                             t->header_style.background.b);
        HPDF_Page_Rectangle(t->pdf_page,
                            x + cell->delta_x, y + cell->delta_y,
                            cell->width, cell->height);
        HPDF_Page_Fill(t->pdf_page);
    }

    if (!(t->use_header_row && r == 0)) {

        // Stroke label if those are used
        if (t->use_cell_labels) {
            set_fontc(t, t->label_style.font, t->label_style.fsize, t->label_style.color);
            char *label = cell->label;

            if (cell->label_cb) {
                char *_label = cell->label_cb(t->tag, r, c);
                if (_label)
                    label = strdup(_label);
            } else if (t->label_cb) {
                char *_label = t->label_cb(t->tag, r, c);
                if (_label)
                    label = strdup(_label);
            }

            HPDF_Page_BeginText(t->pdf_page);
            hpdftbl_encoding_text_out(t->pdf_page,x + cell->delta_x + left_right_padding,
                                      y + cell->delta_y + cell->height - t->label_style.fsize * 1.05f, label);

            HPDF_Page_EndText(t->pdf_page);
        }
    }

    // Stroke content
    char *content = cell->content;

    // If the cell has its own callback this will override the tables global cell callback
    if (cell->content_cb) {
        char *_content = cell->content_cb(t->tag, r, c);
        if (_content)
            content = _content;
    } else if (t->content_cb) {
        char *_content = t->content_cb(t->tag, r, c);
        if (_content)
            content = _content;
    }

    hpdftbl_text_align_t halign = t->content_style.halign;
    if (t->use_header_row && r == 0) {
        set_fontc(t, t->header_style.font, t->header_style.fsize, t->header_style.color);
    } else {
        set_fontc(t, t->content_style.font, t->content_style.fsize, t->content_style.color);
        // Check if cell has its own style which should override global setting but a defined
        // callback will override both
#ifdef __cplusplus
        hpdf_text_style_t cb_val = {	t->content_style.font, t->content_style.fsize,
                                        t->content_style.color, t->content_style.background,
                                        t->content_style.halign };
#else
        hpdf_text_style_t cb_val = (hpdf_text_style_t) {t->content_style.font, t->content_style.fsize,
                                                        t->content_style.color, t->content_style.background,
                                                        t->content_style.halign};
#endif
        if (cell->style_cb && cell->style_cb(t->tag, r, c, content, &cb_val)) {
            set_fontc(t, cb_val.font, cb_val.fsize, cb_val.color);
            halign = cb_val.halign;
        } else if (t->content_style_cb && t->content_style_cb(t->tag, r, c, content, &cb_val)) {
            set_fontc(t, cb_val.font, cb_val.fsize, cb_val.color);
            halign = cb_val.halign;
        } else if (cell->content_style.font) {
            set_fontc(t, cell->content_style.font, cell->content_style.fsize, cell->content_style.color);
        } else {
            set_fontc(t, t->content_style.font, t->content_style.fsize, t->content_style.color);
        }
    }

    HPDF_REAL xpos = x + cell->delta_x + left_right_padding;
    if (halign == RIGHT) {
        xpos = x + cell->delta_x + (cell->width - HPDF_Page_TextWidth(t->pdf_page, content)) - left_right_padding;
    } else if (halign == CENTER) { // Center text
        xpos = x + cell->delta_x + (cell->width - HPDF_Page_TextWidth(t->pdf_page, content)) / 2.0f;
    }

    HPDF_REAL ypos = y + cell->delta_y + t->content_style.fsize * t->bottom_vmargin_factor; //AUTO_VBOTTOM_MARGIN_FACTOR;

    if (t->use_header_row && r == 0) {
        // Roughly center the text vertical
        ypos = y + cell->delta_y + (cell->height / 2 - t->header_style.fsize / 2) + t->header_style.fsize / 5;

        // Center the header
        if (t->header_style.halign == CENTER)
            xpos = x + cell->delta_x + (cell->width - HPDF_Page_TextWidth(t->pdf_page, content)) / 2.0f;
        else if (t->header_style.halign == RIGHT)
            xpos = x + cell->delta_x + (cell->width - HPDF_Page_TextWidth(t->pdf_page, content)) -
                   left_right_padding;
    }

    if (content && *content) {
        HPDF_Page_BeginText(t->pdf_page);
        hpdftbl_encoding_text_out(t->pdf_page, xpos, ypos, content);
        HPDF_Page_EndText(t->pdf_page);
    }

}

/**
 * @brief Set size and position for table.
 *
 * The position is by default specified
 * as the upper left corner of the table. Use the hpdftbl_set_origin_top_left() to use
 * the bottom left of the table as reference point.
 *
 * Ths standard stroke function hpdftbl_stroke() also take the size and position as
 * argument for ease of use but the hpdftbl_stroke_pos() do assume that the table
 * has it's size set.
 *
 * @param t Table handle
 * @param xpos x position for table
 * @param ypos y position for table
 * @param width width of table
 * @param height  height of table. If the height is specified as 0 it will be automatically
 * calculated. The calculated height can be retrieved after the table has been stroked by a
 * call to hpdftbl_get_last_auto_height()
 * @return -1 on error, 0 if successful
 * @see hpdftbl_get_last_auto_height(), hpdftbl_set_origin_top_left()
 */
int
hpdftbl_setpos(hpdftbl_t t,
               const HPDF_REAL xpos, const HPDF_REAL ypos,
               const HPDF_REAL width, HPDF_REAL height) {
    if (NULL == t) {
        _HPDFTBL_SET_ERR(t, -6, -1, -1);
        return -1;
    }
    t->posx = xpos;
    t->posy = ypos;
    t->width = width;
    t->height = height;
    return 0;
}

/**
 * @brief Stroke the table using the already specified size and position within the table.
 *
 * Stroke the table at the specified position and size. The position is by default specified
 * as the upper left corner of the table. Use the hpdftbl_set_origin_top_left(FALSE) to use
 * the bottom left of the table as reference point.
 *
 * This is a convenient method to use when stroking a serialized table
 * as the table already holds the size and position. Stroking a table
 * read back ccan be done with just two lines of code
 *
 * ```c
 *  hpdftbl_t tbl = calloc(1, sizeof(struct hpdftbl));
 *  if( 0 == hpdftbl_load(tbl, filename)  ) {
 *       hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
 *  }
 * ```
 *
 * @param pdf The HPDF document handle
 * @param page The HPDF page handle
 * @param t Table handle
 * @return -1 on error, 0 if successful
 * @see hpdftbl_get_last_auto_height()
 * @see hpdftbl_setpos(), hpdftbl_stroke()
 */
int
hpdftbl_stroke_pos(HPDF_Doc pdf,
                   const HPDF_Page page, hpdftbl_t t) {
    return hpdftbl_stroke(pdf,page,t,
                   t->posx,t->posy,
                   t->width, t->height);
}

/**
 * @brief Stroke the table
 *
 * Stroke the table at the specified position and size. The position is by default specified
 * as the upper left corner of the table. Use the hpdftbl_set_origin_top_left(FALSE) to use
 * the bottom left of the table as reference point.
 *
 * @param pdf The HPDF document handle
 * @param page The HPDF page handle
 * @param t Table handle
 * @param xpos x position for table
 * @param ypos y position for table
 * @param width width of table
 * @param height height of table. If the height is specified as 0 it will be automatically
 * calculated. The calculated height can be retrieved after the table has been stroked by a
 * call to hpdftbl_get_last_auto_height()
 * @return -1 on error, 0 if successful
 * @see hpdftbl_get_last_auto_height()
 * @see hpdftbl_stroke_from_data()
 */
int
hpdftbl_stroke(HPDF_Doc pdf,
               const HPDF_Page page, hpdftbl_t t,
               const HPDF_REAL xpos, const HPDF_REAL ypos,
               const HPDF_REAL width, HPDF_REAL height) {

    if (NULL == pdf || NULL == page || NULL == t) {
        _HPDFTBL_SET_ERR(t, -6, -1, -1);
        return -1;
    }

    // Local positions to enable position adjustment
    HPDF_REAL y = ypos;
    HPDF_REAL x = xpos;

    last_auto_height = 0;
    if (height <= 0) {
        // Calculate height automagically based on number of rows and font sizes
        height = t->content_style.fsize;
        if (t->use_cell_labels) {
            height += t->label_style.fsize;
            height = max(t->minrowheight, height);
            height *= 1.5f * (float)t->rows;
        } else {
            height = max(t->minrowheight, height);
            height *= 1.6f * (float)t->rows;
        }
        last_auto_height = height;
    }

    t->posx = x;
    t->posy = y;

    //const HPDF_REAL page_height = HPDF_Page_GetHeight(page);
    if (t->anchor_is_top_left) {
        y = ypos - height;
        if (t->title_txt) {
            y -= 1.5f * t->title_style.fsize;
        }
    }

    t->pdf_doc = pdf;
    t->pdf_page = page;
    t->height = height;
    t->width = width;

    if (-1 == calc_cell_pos(t)) {
        return -1;
    }

    // Stroke table background
    HPDF_Page_SetRGBFill(page, t->content_style.background.r, t->content_style.background.g,
                         t->content_style.background.b);
    HPDF_Page_Rectangle(page, x, y, width, height);
    HPDF_Page_Fill(page);

    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

            // Only cells which are not covered by a parent spanning cell will be stroked
            if (cell->parent_cell == NULL) {
#ifdef __cplusplus
                hpdf_text_style_t style = {	t->content_style.font, t->content_style.fsize,
                                                t->content_style.color, t->content_style.background, t->content_style.halign};
#else
                hpdf_text_style_t style = (hpdf_text_style_t) {t->content_style.font, t->content_style.fsize,
                                                               t->content_style.color, t->content_style.background,
                                                               t->content_style.halign};
#endif
                if (cell->style_cb) {
                    if (cell->style_cb(t->tag, r, c, NULL, &style)) {
                        HPDF_Page_SetRGBFill(page, style.background.r, style.background.g, style.background.b);
                        HPDF_Page_Rectangle(page, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                        HPDF_Page_Fill(page);
                    }
                } else if (t->content_style_cb && t->content_style_cb(t->tag, r, c, NULL, &style)) {
                    HPDF_Page_SetRGBFill(page, style.background.r, style.background.g, style.background.b);
                    HPDF_Page_Rectangle(page, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                    HPDF_Page_Fill(page);
                } else if (cell->content_style.font) {
                    // If cell has its own style set this will override, and we have to stroke the background here
                    HPDF_Page_SetRGBFill(page, cell->content_style.background.r, cell->content_style.background.g,
                                         cell->content_style.background.b);
                    HPDF_Page_Rectangle(page, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                    HPDF_Page_Fill(page);
                }

                // If we are to use zebra coloring of rows
                if( t->use_zebra ) {
                    if ( r % 2 == 0 ) {
                        if ( 0 == t->zebra_phase )
                            HPDF_Page_SetRGBFill(page, t->zebra_color1.r, t->zebra_color1.g, t->zebra_color1.b);
                        else
                            HPDF_Page_SetRGBFill(page, t->zebra_color2.r, t->zebra_color2.g, t->zebra_color2.b);
                    } else {
                        if ( 0 == t->zebra_phase )
                            HPDF_Page_SetRGBFill(page, t->zebra_color2.r, t->zebra_color2.g, t->zebra_color2.b);
                        else
                            HPDF_Page_SetRGBFill(page, t->zebra_color1.r, t->zebra_color1.g, t->zebra_color1.b);
                    }
                    HPDF_Page_Rectangle(page, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                    HPDF_Page_Fill(page);
                }

                if (cell->canvas_cb) {
                    cell->canvas_cb(pdf, page, t->tag, r, c, x + cell->delta_x, y + cell->delta_y, cell->width,
                                    cell->height);
                } else if (t->canvas_cb) {
                    t->canvas_cb(pdf, page, t->tag, r, c, x + cell->delta_x, y + cell->delta_y, cell->width,
                                 cell->height);
                }

                table_cell_stroke(t, r, c);

                // Vertical grid. This is either a full cell height grid or a shorter depending
                // on if cell labels are used and the user setting for `use_label_grid_style`.
                // In case a header row should be used we don't use the shorter grids in the header.
                if (t->use_label_grid_style && t->use_cell_labels && !(t->use_header_row && 0==r)) {
                    HPDF_Page_SetRGBStroke(page, t->inner_vgrid.color.r, t->inner_vgrid.color.g,
                                           t->inner_vgrid.color.b);

                    HPDF_Page_SetLineWidth(page, t->inner_vgrid.width);
                    hpdftbl_set_line_dash(t, t->inner_vgrid.line_dashstyle);

                    // If this cell spans multiple rows we draw the left line full and not just the short
                    // label lead since the visual appearance will just be bad otherwise
                    if (cell->rowspan > 1) {
                        HPDF_Page_MoveTo(page, x + cell->delta_x, y + cell->delta_y);
                    } else {
                        HPDF_Page_MoveTo(page, x + cell->delta_x,y + cell->delta_y + cell->height - t->label_style.fsize * 1.2f);
                    }
                    HPDF_Page_LineTo(page, x + cell->delta_x, y + cell->delta_y + cell->height);
                    HPDF_Page_Stroke(page);
                } else {
                    HPDF_Page_SetRGBStroke(page, t->inner_vgrid.color.r, t->inner_vgrid.color.g,
                                           t->inner_vgrid.color.b);
                    HPDF_Page_SetLineWidth(page, t->inner_vgrid.width);
                    hpdftbl_set_line_dash(t, t->inner_vgrid.line_dashstyle);
                    HPDF_Page_MoveTo(page, x + cell->delta_x, y + cell->delta_y);
                    HPDF_Page_LineTo(page, x + cell->delta_x, y + cell->delta_y + cell->height);
                    HPDF_Page_Stroke(page);
                }

                // Horizontal grid
                if (r > 0 || 0 == t->inner_tgrid.width) {
                    // Not special top inner gridline
                    HPDF_Page_SetRGBStroke(page, t->inner_hgrid.color.r, t->inner_hgrid.color.g,t->inner_hgrid.color.b);
                    HPDF_Page_SetLineWidth(page, t->inner_hgrid.width);
                    hpdftbl_set_line_dash(t, t->inner_hgrid.line_dashstyle);
                    HPDF_Page_MoveTo(page, x + cell->delta_x, y + cell->delta_y);
                    HPDF_Page_LineTo(page, x + cell->delta_x + cell->width, y + cell->delta_y);
                    HPDF_Page_Stroke(page);
                } else {
                    // Draw the top-inner horizontal grid line
                    HPDF_Page_SetRGBStroke(page, t->inner_tgrid.color.r, t->inner_tgrid.color.g,
                                           t->inner_tgrid.color.b);
                    HPDF_Page_SetLineWidth(page, t->inner_tgrid.width);
                    hpdftbl_set_line_dash(t, t->inner_tgrid.line_dashstyle);
                    HPDF_Page_MoveTo(page, x + cell->delta_x, y + cell->delta_y);
                    HPDF_Page_LineTo(page, x + cell->delta_x + cell->width, y + cell->delta_y);
                    HPDF_Page_Stroke(page);
                }
            }
        }
    }

    // Stoke outer border
    HPDF_Page_SetRGBStroke(page, t->outer_grid.color.r, t->outer_grid.color.g, t->outer_grid.color.b);
    HPDF_Page_SetLineWidth(page, t->outer_grid.width);
    hpdftbl_set_line_dash(t, t->outer_grid.line_dashstyle);
    HPDF_Page_Rectangle(page, x, y, width, height);
    HPDF_Page_Stroke(page);

    // If header row is enabled we add a thicker (same as outer border) line under the top row
    if (t->use_header_row) {
        //hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(0, 0)];
        //HPDF_Page_MoveTo(page, x + cell->delta_x, y + cell->delta_y);
        //HPDF_Page_LineTo(page, x + width, y + cell->delta_y);
        //HPDF_Page_Stroke(page);
    }

    // Stroke title
    const HPDF_REAL title_height = table_title_stroke(t);
    if (last_auto_height > 0) {
        last_auto_height += title_height;
    }

    return 0;
}


/**
 * @brief Stroke PDF document to file with check that the directory in path exists.
 *
 * Note: It is a checked error if the full path is longer than 1014 characters
 *
 * @param pdf_doc Haru PDF document handle
 * @param file Full pathname of file to write to
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_stroke_pdfdoc(HPDF_Doc pdf_doc, char *file) {
    if( strnlen(file, 1024) >= 1024 )
        return -1;
    struct stat sb;
    char dbuff[1024];
    strncpy(dbuff, file, sizeof dbuff);
    dbuff[sizeof(dbuff)-1] = 0;
    char *dir = dirname(dbuff);
    if (stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        if (HPDF_OK != HPDF_SaveToFile(pdf_doc, file)) {
            return -1;
        }
        return 0;
    } else {
        return -1;
    }
}


//  Some Doxygen magic to get all the examples forced to be included in the documentation
/**
 * @example example01.c
 * A collection of more and less advanced examples in one file. For learning the library it is better
 * to start with the organized tutorial examples like @ref tut_ex01.c "tut_ex01.c"
 * and @ref tut_ex02.c "tut_ex02.c"
 *
 * @example tut_ex00.c
 * The very most basic table with a header
 * @image html screenshots/tut_ex00.png
 *
 * @example tut_ex01.c
 * The very most basic table with API call to set content in each cell.
 * @image html screenshots/tut_ex01.png
 *
 * @example tut_ex02.c
 * Basic table with content data specified as an array.
 * @image html screenshots/tut_ex02.png
 *
 * @example tut_ex02_1.c
 * Basic table with content data specified as an array.
 * @image html screenshots/tut_ex02_1.png
 *
 * @example tut_ex03.c
 * First example with API call to set content in each cell with added labels and shortened grid.
 * @image html screenshots/tut_ex03.png
 *
 * @example tut_ex04.c
 * Specifying labels as data array.
 * @image html screenshots/tut_ex04.png
 *
 * @example tut_ex05.c
 * Set content data specified as an array with added labels and shortened grid.
 * @image html screenshots/tut_ex05.png
 *
 * @example tut_ex06.c
 * Use content to set content and labels.
 * @image html screenshots/tut_ex06.png
 *
 * @example tut_ex07.c
 * Expand cells over multiple columns and rows.
 * @image html screenshots/tut_ex07.png
 *
 * @example tut_ex08.c
 * Adjust column width and expand cells over multiple columns and rows.
 * @image html screenshots/tut_ex08.png
 *
 * @example tut_ex09.c
 * Adjusting font style with a callback.
 * @image html screenshots/tut_ex09.png
 *
 * @example tut_ex10.c
 * Adjust column widths and add error handler.
 * @image html screenshots/tut_ex09.png
 *
 * @example tut_ex11.c
 * Table with header row and error handler.
 * @image html screenshots/tut_ex11.png
 *
 * @example tut_ex12.c
 * Table with header row and error handler.
 * @image html screenshots/tut_ex12.png
 *
 * @example tut_ex13_1.c
 * Defining a table with a data structure for the table.
 * @image html screenshots/tut_ex13_1.png
 *
 * @example tut_ex13_2.c
 * Defining a table with a data structure for table and cells.
 * @image html screenshots/tut_ex13_2.png
 *
 * @example tut_ex14.c
 * Defining a table with widgets.
 * @image html screenshots/tut_ex14.png
 *
 * @example tut_ex15.c
 * Defining a table with zebra lines.
 * @image html screenshots/tut_ex15.png
 *
 * @example tut_ex15_1.c
 * Defining a table with zebra lines and different phase.
 * @image html screenshots/tut_ex15_1.png
 *
 * @example tut_ex20.c
 * Defining a table and adjusting the gridlines.
 * @image html screenshots/tut_ex20.png
 *
 * @example tut_ex30.c
 * Defining a table using dynamic callbacks
 * @image html screenshots/tut_ex30.png
 *
 * @example tut_ex40.c
 * Example of importing a table from a serialized json file.
 * @see hpdftbl_dump()
 * @image html screenshots/tut_ex40.png
 *
 * @example tut_ex41.c
 * Example of importing a table and theme from a serialized representation.
 * @see hpdftbl_load(), hpdftbl_theme_load()
 * @image html screenshots/tut_ex41.png
 *
 * @example tests/tut_ex40.json
 * An output example from hpdftbl_dump() that shows a
 * serialized table. This can later be import to a table structure
 * with the use of hpdftbl_load()
 * @image html screenshots/tut_ex40.png
 *
 * @example tests/tut_ex41.json
 * An output example from hpdftbl_dump() as well as
 * hpdftbl_theme_dump() that shows a
 * serialized table and theme.
 * This can later be import to a table structure
 * with the use of hpdftbl_load() and hpdftbl_theme_import()
 * @image html screenshots/tut_ex41.png
 */

/* EOF */
