/* =========================================================================
 * File:        hpdf_table.c
 * Description: Utility module for flexible table drawing with HPDF library
 * Author:      Johan Persson (johan162@gmail.com)
 *
 * Copyright (C) 2015 Johan Persson
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
 * =========================================================================
 */

/**
 * @file
 * @brief Main source module for HPDF_TABLE
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iconv.h>
#include <hpdf.h>

#include "hpdf_table.h"

#define TRUE 1
#define FALSE 0

#define _IDX(r,c) (r*t->cols+c)

/* Default styles */
#define HPDF_TABLE_DEFAULT_TITLE_STYLE (hpdf_text_style_t){HPDF_FF_HELVETICA_BOLD,11,(HPDF_RGBColor){0.2,0,0},(HPDF_RGBColor){0.9,0.9,0.9}, LEFT}
#define HPDF_TABLE_DEFAULT_HEADER_STYLE (hpdf_text_style_t){HPDF_FF_HELVETICA_BOLD,10,(HPDF_RGBColor){0.2,0,0},(HPDF_RGBColor){0.9,0.9,0.97}, CENTER}
#define HPDF_TABLE_DEFAULT_LABEL_STYLE (hpdf_text_style_t){HPDF_FF_TIMES_ITALIC,9,(HPDF_RGBColor){0.4,0.4,0.4},(HPDF_RGBColor){1,1,1}, LEFT}
#define HPDF_TABLE_DEFAULT_CONTENT_STYLE (hpdf_text_style_t){HPDF_FF_COURIER,10,(HPDF_RGBColor){0.2,0.2,0.2},(HPDF_RGBColor){1,1,1}, LEFT}
#define HPDF_TABLE_DEFAULT_INNER_BORDER_STYLE (hpdf_border_style_t){0.7, (HPDF_RGBColor){0.5,0.5,0.5},0}
#define HPDF_TABLE_DEFAULT_OUTER_BORDER_STYLE (hpdf_border_style_t){1.0, (HPDF_RGBColor){0.2,0.2,0.2},0}

#define _SET_ERR(err,r,c) do {err_code=err;err_row=r;err_col=c;} while(0)
#define _CHK_TABLE(t) do {if(NULL == t) {err_code=-3;err_row=-1;err_col=-1;return -1;}} while(0)

/**
 * @brief Last automatically calculated total height
 */
static HPDF_REAL last_auto_height;

/**
 * @brief Specify position of tables as top left. By default bottom left is used as base point,
 */
static _Bool origin_as_top_left=FALSE;

/**
 * @brief Internal state variable to keep track of necessary encodings
 */
static char *target_encoding = HPDF_TABLE_DEFAULT_TARGET_ENCODING;

/**
 * @brief Internal state variable to keep track of necessary encodings
 */
static char *source_encoding = HPDF_TABLE_DEFAULT_SOURCE_ENCODING;


#define _ERR_UNKNOWN 11

static int err_code=0; /**< Stores the last generated error code */
static int err_row=-1; /**< The row where the last error was generated  */
static int err_col=-1; /**< The column where the last error was generated  */

/**
 * @brief Human readable error strings corresponding to error code
 */
static char *error_descriptions[] = {
    "No error",                                     /* 0 */
    "Cell is part of another spanning cell",        /* 1 */
    "Cell row and column exceeds table dimension",  /* 2 */
    "Table handle is NULL",                         /* 3 */
    "Text encoding error",                          /* 4 */
    "Out of memory",                                /* 5 */
    "Invalid table handle",                         /* 6 */
    "Cell spanning will exceed table dimension",    /* 7 */
    "Use of undefined line style",                  /* 8 */
    "Invalid theme handler",                        /* 9 */
    "No auto height available",                     /* 10  */
    "Internal error. Unknown error code",           /* 11  */
    "Total column width exceeds 100%",              /* 12  */
    "Calculated width of columns too small"         /* 13  */
};

/**
 * @brief Number of error messages
 */
static const size_t NUM_ERR_MSG = sizeof(error_descriptions)/sizeof(char *);

/**
 * @brief Definition of a dashed line style
 */
typedef struct line_dash_style {
    HPDF_UINT16 dash_ptn[8]; /**< HPDF ash line definition */
    size_t num;              /**< Number of segments in the dashed line */
} line_dash_style_t;

/**
 * @brief Vector with defined line styles
 */
static line_dash_style_t dash_styles[] = {
    { {1,0,0,0,0,0,0,0}, 0 },
    { {1,1,0,0,0,0,0,0}, 2 },
    { {2,3,0,0,0,0,0,0}, 2 },
    { {2,5,0,0,0,0,0,0}, 2 },
    { {3,3,0,0,0,0,0,0}, 2 },
    { {6,3,0,0,0,0,0,0}, 2 },
    { {7,5,0,0,0,0,0,0}, 2 },
    { {7,3,3,3,0,0,0,0}, 4 },
}  ;

/**
 * @brief Internal helper to set the line style
 * @param t Table handle
 * @param style
 * @return -1 on error, 0 on success
 */
int
hpdf_table_set_line_dash(hpdf_table_t t, hpdf_table_line_style_t style ) {
    _CHK_TABLE(t);
    if( style > DASHDOT ) {
        _SET_ERR(-8,-1,-1);
        return -1;
    }
    HPDF_Page_SetDash(t->pdf_page, dash_styles[style].dash_ptn, dash_styles[style].num, 0);
    return 0;
}

/**
 * @brief Switch stroking reference point
 *
 * Set base point for table positioning. By default the bottom left is used.
 * Calling this function can set the basepoint to top left instead.
 * @param origin Set to TRUE to use top left as origin
 */
void
hpdf_table_set_origin_top_left(const _Bool origin) {
    origin_as_top_left = origin;
}

/**
 * @brief Return last error code
 *
 * Return last error code. if errstr is not NULL a human
 * readable string describing the error will be copied to
 * the string.
 * The error code will be reset after call
 * @param errstr A string buffer where the error string
 * is written to
 * @param row The row where the error was found
 * @param col The col where the error was found
 * @return The last error code
 */
int
hpdf_table_get_last_errcode(char **errstr, int *row, int *col) {
    int old_err_code;
    if( errstr && err_code < 0 && ( (size_t)(-err_code) < NUM_ERR_MSG) )
        *errstr =  error_descriptions[-err_code];
    else if( errstr) {
        *errstr = error_descriptions[_ERR_UNKNOWN];
        err_code = _ERR_UNKNOWN;
        err_row = -1;
        err_col = -1;
    } else {
        return 0;
    }
    *row=err_row ;
    *col=err_col ;
    old_err_code = err_code;
    err_code=0;
    err_row=-1;
    err_col=-1;
    return old_err_code;
}

/**
 * @brief Determine text source encoding
 *
 * The default HPDF encoding is a standard PDF encoding. The problem
 * with that is that now  almost 100% of all code is written in
 * UTF-8 encoding and trying to print text strings with accented
 * charactes will simply not work.
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
hpdf_table_set_text_encoding(char *target, char *source) {
    target_encoding = target;
    source_encoding = source;
}

/**
 * @brief Internal fucntion to do text encoding
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
_do_encoding(char *input, char *output, const size_t out_len) {
    char *out_buf = &output[0];
    char *in_buf = &input[0];
    size_t out_left = out_len - 1;
    size_t in_left = strlen(input) ;
    iconv_t cd = iconv_open(target_encoding, source_encoding);

    do {
        if (iconv(cd, &in_buf, &in_left, &out_buf, &out_left) == (size_t) - 1) {
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
hpdf_table_encoding_text_out(HPDF_Page page, HPDF_REAL xpos, HPDF_REAL ypos, char *text) {
    // Assume that the encoding we are converting to never exceeds three times the
    // original string

    if( NULL==text )
        return 0;

    const size_t out_len = 3*strlen(text);
    char *output = calloc(1,out_len);

    if( -1 == _do_encoding(text,output,out_len) ) {
        _SET_ERR(-4,xpos,ypos);
        HPDF_Page_TextOut(page, xpos, ypos, "???");
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
HPDF_RoundedCornerRectangle(HPDF_Page page,HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, HPDF_REAL rad) {

    // Left vertical line
    HPDF_Page_MoveTo(page,xpos,ypos+rad);
    HPDF_Page_LineTo(page,xpos,ypos+height-rad);

    // Upper left
    HPDF_Page_CurveTo2(page,xpos,ypos+height,xpos+rad,ypos+height);

    // Top line
    HPDF_Page_LineTo(page,xpos+width-rad,ypos+height);

    // Upper right
    HPDF_Page_CurveTo2(page,xpos+width,ypos+height,xpos+width,ypos+height-rad);

    // Right vertical
    HPDF_Page_LineTo(page,xpos+width,ypos+rad);

    // Lower right
    HPDF_Page_CurveTo2(page,xpos+width,ypos,xpos+width-rad,ypos);

    // Bottom line
    HPDF_Page_LineTo(page,xpos+rad,ypos);

    // Lower left corner
    HPDF_Page_CurveTo2(page,xpos,ypos,xpos,ypos+rad);
}

/**
 * @brief Return the default theme
 *
 * Create and return a theme corresponding to the default table theme. It is the calling
 * functions responsibility to call hpdf_table_destroy_theme() to free the allocated
 * memory. The default theme is a good starting point to just make minor modifications
 * without having to define all elements.
 * @return A new theme initialized to the default settings
 */
hpdf_table_theme_t *
hpdf_table_get_default_theme(void) {
    hpdf_table_theme_t *t = calloc(1,sizeof(hpdf_table_theme_t));

    if( NULL == t ) {
        _SET_ERR(-5,-1,-1);
        return NULL;
    }

    // Enable labels and label short style grid by default
    t->use_labels = TRUE;
    t->use_label_grid_style = TRUE;
    t->use_header_row = FALSE;

    // Set default title font
    t->title_style = calloc(1,sizeof(hpdf_text_style_t));
    if( NULL == t->title_style ) {
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }
    *t->title_style = HPDF_TABLE_DEFAULT_TITLE_STYLE;

    // Set default header font
    t->header_style = calloc(1,sizeof(hpdf_text_style_t));
    if( NULL == t->header_style ) {
        free(t->title_style);
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }
    *t->header_style = HPDF_TABLE_DEFAULT_HEADER_STYLE;

    // Set default label font
    t->label_style = calloc(1,sizeof(hpdf_text_style_t));
    if( NULL == t->label_style ) {
        free(t->header_style);
        free(t->title_style);
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }
    *t->label_style = HPDF_TABLE_DEFAULT_LABEL_STYLE;

    // Set default content font
    t->content_style = calloc(1,sizeof(hpdf_text_style_t));
    if( NULL == t->content_style ) {
        free(t->label_style);
        free(t->header_style);
        free(t->title_style);
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }
    *t->content_style = HPDF_TABLE_DEFAULT_CONTENT_STYLE;

    // Set default borders and colors
    t->outer_border = calloc(1,sizeof(hpdf_border_style_t));
    if( NULL == t->outer_border ) {
        free(t->content_style);
        free(t->label_style);
        free(t->header_style);
        free(t->title_style);
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }
    *t->outer_border = HPDF_TABLE_DEFAULT_OUTER_BORDER_STYLE;

    t->inner_border = calloc(1,sizeof(hpdf_border_style_t));
    if( NULL == t->inner_border ) {
        free(t->outer_border);
        free(t->content_style);
        free(t->label_style);
        free(t->header_style);
        free(t->title_style);
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }
    *t->inner_border = HPDF_TABLE_DEFAULT_INNER_BORDER_STYLE;

    return t;
}

/**
 * @brief Destroy existing theme structure and free memory
 *
 * Free all memory allocated by a theme
 * @param theme The theme to free
 */
int
hpdf_table_destroy_theme(hpdf_table_theme_t *theme) {
    if( NULL == theme ) {
        _SET_ERR(-9,-1,-1);
        return -1;
    }
    free(theme->title_style);
    free(theme->header_style);
    free(theme->label_style);
    free(theme->content_style);
    free(theme->outer_border);
    free(theme->inner_border);
    free(theme);
    return 0;
}

/**
 * @brief Create a new table
 *
 * Create a new table structure. This is the basic handler needed for most other API
 * functions.
 * @param rows Number of rows
 * @param cols Number of columns
 * @param title Title of table
 * @return A handle to a table, NULL in case of OOM
 */
hpdf_table_t
hpdf_table_create(size_t rows, size_t cols, char *title) {

    // Initializing to zero means default color is black
    hpdf_table_t t = calloc(1, sizeof (struct hpdf_table));
    if (t == NULL) {
        _SET_ERR(-5,-1,-1);
        return NULL;
    }

    t->cells = calloc(cols*rows, sizeof (hpdf_table_cell_t));
    if (t->cells == NULL) {
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }

    t->cols = cols;
    t->rows = rows;

    // Setup common column widths
    t->col_width_percent = calloc(cols,sizeof(float));
    if( t->col_width_percent == NULL ) {
        free(t->cells);
        free(t);
        _SET_ERR(-5,-1,-1);
        return NULL;
    }

    // Initialize column width. Setting it to zero will set the column to the default
    // width.
    for(size_t i=0; i < cols; i++) {
        t->col_width_percent[i] = 0.0;
    }

    if (title) {
        t->title_txt = strdup(title);
        if( t->title_txt == NULL ) {
            free(t->col_width_percent);
            free(t->cells);
            free(t);
            _SET_ERR(-5,-1,-1);
            return NULL;
        }
    }

    hpdf_table_theme_t *theme = hpdf_table_get_default_theme();
    hpdf_table_apply_theme(t, theme);
    hpdf_table_destroy_theme(theme);

    return t;
}

/**
 * @brief Set column width as percentage of overall table width
 *
 * Specify column width as percentage of total column width. Note that this will only take effect
 * if the table has an overall width specified when stroked.
 * @param t Table handle
 * @param c Column to set width of first column has index 0
 * @param w Width as percentage in range [0.0, 100.0]
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_colwidth_percent(hpdf_table_t t, size_t c, float w) {
    _CHK_TABLE(t);
    if( c >= t->cols ) {
        _SET_ERR(-2,-1,c);
        return -1;
    }
    if( w < 0.0 || w > 100.0 ) {
        _SET_ERR(-12,-1,c);
        return -1;
    }
    t->col_width_percent[c] = w;
    return 0;
}

/**
 * @brief Specify style for table outer border
 *
 * Set outer border properties
 * @param t Table handle
 * @param width Line width
 * @param color Line color
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_outer_border(hpdf_table_t t,HPDF_REAL width, HPDF_RGBColor color) {
    _CHK_TABLE(t);
    t->outer_border.width = width;
    t->outer_border.color = color;
    return 0;
}

/**
 * @brief Specify style for table inner border
 *
 * Set inner border properties
 * @param t Table handle
 * @param width Line width
 * @param color Line color
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_inner_border(hpdf_table_t t,HPDF_REAL width, HPDF_RGBColor color) {
    _CHK_TABLE(t);
    t->inner_border.width = width;
    t->inner_border.color = color;
    return 0;
}


/**
 * @brief Specify style for table heder row
 *
 * Set the font properties and background for the header row which is the top
 * row if enabled. The header row will be automatically enabled after calling
 * this function. The header can be enabled/disabled separately with hpdf_table_use_header()
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Font color
 * @param background Cell background color
 * @return 0 on success, -1 on failure
 * @ref hpdf_table_use_header()
 */

int
hpdf_table_set_header_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _CHK_TABLE(t);
    t->header_style.font = font;
    t->header_style.fsize = fsize;
    t->header_style.color = color;
    t->header_style.background = background;
    return 0;
}

/**
 * @brief Set table background color
 *
 * Set table background
 * @param t Table handle
 * @param background Background color
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_background(hpdf_table_t t, HPDF_RGBColor background) {
    _CHK_TABLE(t);
    t->content_style.background = background;
    return 0;
}

/**
 * @brief Set table header text align
 *
 * Set horizontal text alignment for header row
 * @param t Table handle
 * @param align Alignment
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_header_halign(hpdf_table_t t, hpdf_table_text_align_t align) {
    _CHK_TABLE(t);
    t->header_style.halign = align;
    return 0;
}

/**
 * Enable/disable the interpretation of the top row as a header row
 * @param t Table handle
 * @param use TRUE to enable, FALSE to disable
 * @return 0 on success, -1 on failure
 * @see hpdf_table_set_header_style()
 */
int
hpdf_table_use_header(hpdf_table_t t, _Bool use) {
    _CHK_TABLE(t);
    t->use_header_row = use;
    return 0;
}

/**
 * Enable/Disable the use of cell labels. By default a newly created
 * table will use cell labels. Enabling labels will also by default
 * enable the special label grid style. To adjust the grid style separately
 * us the @ref hpdf_table_use_labelgrid() method.
 * @param t Table handle
 * @param use Set to TRUE for cell labels
 * @return 0 on success, -1 on failure
 * @see hpdf_table_use_labelgrid()
 */
int
hpdf_table_use_labels(hpdf_table_t t, _Bool use) {
    _CHK_TABLE(t);
    t->use_cell_labels = use;
    t->use_label_grid_style = use;
    return 0;
}

/**
 * Set the usage of special grid style where the vertical grid
 * only covers the label text and a gap to the next line. Horizontal
 * lines are drawn as usual.
 * The label grid style gives the table a "lighter" look.
 * @param t Table handle
 * @param use TRUE to use label grid, FALSE o disable it
 * @return 0 on success, -1 on failure
 * @see hpdf_table_use_labels
 */
int
hpdf_table_use_labelgrid(hpdf_table_t t, _Bool use) {
    _CHK_TABLE(t);
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
hpdf_table_set_tag(hpdf_table_t t, void *tag) {
    _CHK_TABLE(t);
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
_cell_destroy(hpdf_table_t t, int r, int c) {
    _CHK_TABLE(t);
    hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];
    if (cell->label)
        free(cell->label);
    if (cell->content)
        free(cell->content);
    cell->parent_cell = NULL;
    return 0;
}

/**
 * @brief Destroy a table and free all memory
 *
 * Destroy a table previous created with table_create()
 * @param t Handle to table
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_destroy(hpdf_table_t t) {
    _CHK_TABLE(t);
    if (t->title_txt)
        free(t->title_txt);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            _cell_destroy(t, r, c);
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
static _Bool
_chk(const hpdf_table_t t, size_t r, size_t c) {
    if (r < t->rows && c < t->cols)
        return TRUE;
    _SET_ERR(-2,r,c);
    return FALSE;
}

/**
 * @brief Set content for specific cell
 *
 * Set label and content for a specific cell. If the specified cell is part of
 * another cells spanning then error is given (returns -1),
 * @param t Table handle
 * @param r Row
 * @param c Column
 * @param label Label
 * @param content Text content
 * @return -1 on error, 0 if successful
 */
int
hpdf_table_set_cell(const hpdf_table_t t, int r, int c, char *label, char *content) {
    _CHK_TABLE(t);
    if (!_chk(t, r, c)) return -1;
    hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];

    // If we are trying to specify a cell that is part of another cell that spans we
    // indicate this as an error
    if( cell->parent_cell ) {
        _SET_ERR(-1,r,c);
        return -1;
    }

    cell->colspan = 1;
    cell->rowspan = 1;
    cell->label = label ? strdup(label) : NULL;;
    cell->content = content ? strdup(content) : NULL;
    return 0;
}

/**
 * @brief Set cell spanning
 *
 * Set row and column spanning for a cell
 * @param t Table handle
 * @param r Row
 * @param c Column
 * @param rowspan Row span
 * @param colspan Column span
 * @return -1 on error, 0 if successful
 */
int
hpdf_table_set_cellspan(const hpdf_table_t t, size_t r, size_t c, size_t rowspan, size_t colspan) {
    _CHK_TABLE(t);
    if (!_chk(t, r, c))
        return -1;

    if (r + rowspan - 1 >= t->rows || c + colspan - 1 >= t->cols) {
        _SET_ERR(-7,r,c);
        return -1;
    }
    hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if( cell->parent_cell ) {
        _SET_ERR(-1,r,c);
        return -1;
    }

    // Clear any potential previous spanning setting for this cell
    if( cell->colspan > 1 || cell->rowspan > 1 ) {
        for (size_t rr = r; rr < cell->rowspan + r; rr++) {
            for (size_t cc = c; cc < cell->colspan + c; cc++) {
                t->cells[_IDX(rr, cc)].parent_cell = NULL;
            }
        }
    }

    cell->colspan = colspan;
    cell->rowspan = rowspan;
    for (size_t rr = r; rr < rowspan + r; rr++) {
        for (size_t cc = c; cc < colspan + c; cc++) {
            if (rr != r || cc != c) {
                // Point back to the parent cell
                t->cells[_IDX(rr, cc)].parent_cell = cell;

                // Reset the row and col-span in covered cells
                t->cells[_IDX(rr, cc)].colspan = 1;
                t->cells[_IDX(rr, cc)].rowspan = 1;
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
 */
int
hpdf_table_clear_spanning(const hpdf_table_t t) {
    _CHK_TABLE(t);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            t->cells[_IDX(r, c)].colspan = 1;
            t->cells[_IDX(r, c)].rowspan = 1;
            t->cells[_IDX(r, c)].parent_cell = NULL;
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
_set_fontc(const hpdf_table_t t, char *fontname, HPDF_REAL fsize, HPDF_RGBColor color) {
    HPDF_Page_SetFontAndSize(t->pdf_page, HPDF_GetFont(t->pdf_doc, fontname, HPDF_TABLE_DEFAULT_TARGET_ENCODING), fsize);
    HPDF_Page_SetRGBFill(t->pdf_page, color.r, color.g, color.b);
    HPDF_Page_SetTextRenderingMode(t->pdf_page, HPDF_FILL);
}

/**
 * @brief Set table content callback
 *
 * Set content callback. This callback gets called for each cell in the
 * table and the returned string will be used as the content. The string
 * will be duplicated so it is safe for a client to reuse the string space.
 * If NULL is returned from the callback then the content will be set to the
 * content specified with the direct content setting.
 * The callback function will receive the Table tag and the row and column
 * @param t Table handle
 * @param cb Callback function
 */
int
hpdf_table_set_content_callback(hpdf_table_t t, hpdf_table_content_callback_t cb) {
    _CHK_TABLE(t);
    t->content_cb = cb;
    return 0;
}

/**
 * @brief Set cell content callback
 *
 * Set a content callback for an individual cell. This will override the table content
 * callback.
 * @param t Table handle
 * @param cb Callback function
 * @param r Cell row
 * @param c Cell column
 * @return -1 on failure, 0 otherwise
 */
int
hpdf_table_set_cell_content_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_content_callback_t cb) {
    _CHK_TABLE(t);
    if( !_chk(t,r,c) )
        return -1;

    hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if( cell->parent_cell ) {
        _SET_ERR(-1,r,c);
        return -1;
    }

    cell->content_cb = cb;
    return 0;
}


/**
 * @brief Set cell canvas callback
 *
 * Set a canvas callback for an individual cell. This will override the table canvas
 * callback.
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdf_table_canvas_callback_t
 * @see hpdf_table_set_canvas_callback
 */
int
hpdf_table_set_cell_canvas_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_canvas_callback_t cb) {
    _CHK_TABLE(t);
    if( !_chk(t,r,c) )
        return -1;

    hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if( cell->parent_cell ) {
        _SET_ERR(-1,r,c);
        return -1;
    }

    cell->canvas_cb = cb;
    return 0;
}

/**
 * @brief Set table label callback
 *
 * Set label callback. This callback gets called for each cell in the
 * table and the returned string will be used as the label. The string
 * will be duplicated so it is safe for a client to reuse the string space.
 * If NULL is returned from the callback then the label will be set to the
 * content specified with the direct label setting.
 * The callback function will receive the Table tag and the row and column
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdf_table_content_callback_t
 */
int
hpdf_table_set_label_callback(hpdf_table_t t, hpdf_table_content_callback_t cb) {
    _CHK_TABLE(t);
    t->label_cb = cb;
    return 0;
}

/**
 * @brief Set cell canvas callback
 *
 * Set cell callback. This callback gets called for each cell in the
 * table. The purpose is to allow the client to add dynamic content to the
 * specified cell.
 * The callback is made before the cell border and content is drawn making
 * it possible to for example add a background color to individual cells.
 * The callback function will receive the Table tag, the row and column,
 * the x, y position of the lower left corner of the table and the width
 * and height of the cell.
 * To set the canvas callback only for a sepcific cell use the
 * hpdf_table_set_cell_canvas_callback() function
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 */
int
hpdf_table_set_canvas_callback(hpdf_table_t t, hpdf_table_canvas_callback_t cb) {
    _CHK_TABLE(t);
    t->canvas_cb = cb;
    return 0;
}


/*static void
_stroke_haligned_text(hpdf_table_t t, char *txt, hpdf_table_text_align_t align, HPDF_REAL x, HPDF_REAL y, HPDF_REAL width) {
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
 * @brief Internal function
 *
 * Internal function.Stroke the optional table title
 * @param t Table handle
 * @return -1 on failure, 0 otherwise
 */
static HPDF_REAL
_table_title_stroke(const hpdf_table_t t) {
    if (t->title_txt == NULL)
        return 0;

    const HPDF_REAL height = 1.5 * t->title_style.fsize;

    // Stoke outer border and fill
    HPDF_Page_SetRGBStroke(t->pdf_page, t->outer_border.color.r, t->outer_border.color.g, t->outer_border.color.b);
    HPDF_Page_SetRGBFill(t->pdf_page, t->title_style.background.r, t->title_style.background.g, t->title_style.background.b);
    HPDF_Page_SetLineWidth(t->pdf_page, t->outer_border.width);
    HPDF_Page_Rectangle(t->pdf_page, t->posx, t->posy + t->height, t->width, height);
    HPDF_Page_FillStroke(t->pdf_page);

    _set_fontc(t, t->title_style.font, t->title_style.fsize, t->title_style.color);

    HPDF_REAL left_right_padding = t->outer_border.width + 3;
    HPDF_REAL xpos = t->posx + left_right_padding;
    const HPDF_REAL ypos = t->posy + t->height + t->outer_border.width * 2 + t->title_style.fsize * 0.28;

    if (t->title_style.halign == CENTER) {
        xpos = t->posx + (t->width - HPDF_Page_TextWidth(t->pdf_page, t->title_txt)) / 2.0;
    } else if (t->title_style.halign == RIGHT) {
        xpos = t->posx + (t->width - HPDF_Page_TextWidth(t->pdf_page, t->title_txt)) - left_right_padding;
    }

    HPDF_Page_BeginText(t->pdf_page);
    hpdf_table_encoding_text_out(t->pdf_page, xpos, ypos, t->title_txt);
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
 * It is allowed to specify NULL as placeholder for empty labels.
 * The actual text in the table will be allocated with strdup() so it is safe
 * to free the memory for the labels after the call to this function.
 * Please note that even if the table contains spanning cells the content
 * data must include empty data for covered cells. For a N x M table the data
 * must have (N*M) entries.
 * @param t Table handle
 * @param labels A one dimensional string array of labels
 * @return -1 on error, 0 if successful
 * @see set_label_callback
 */
int
hpdf_table_set_labels(hpdf_table_t t, char **labels) {
    _CHK_TABLE(t);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            size_t idx = r * t->cols + c;
            hpdf_table_cell_t *cell = &t->cells[idx];
            cell->label = labels[idx] ? strdup(labels[idx]) : NULL ;
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
 * It is allowed to specify NULL as placeholder for empty labels.
 *  The actual text in the table will be allocated with strdup() so it is safe
 * to free the memory for the labels after the call to this function.
 * Please note that even if the table contains spanning cells the content
 * data must include empty data for covered cells. For a N x M table the data
 * must have (N*M) entries.
 * Another way to specify the content is to use the callback mechanism. By setting
 * up a content callback function that returns the content for a cell
 * @param t Table handle
 * @param content A one dimensional string array of content string
 * @return -1 on error, 0 if successful
 * @see hpdf_table_set_content_callback()
 * @see hpdf_table_set_cell_content_callback()
 */
int
hpdf_table_set_content(hpdf_table_t t, char **content) {
    _CHK_TABLE(t);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            size_t idx = r * t->cols + c;
            hpdf_table_cell_t *cell = &t->cells[idx];
            cell->content = content[idx] ? strdup(content[idx]) : NULL ;
        }
    }
    return 0;
}

/**
 * @brief Set the font style for labels
 *
 * Set font, color and backgroundt options for cell labels
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return -1 on error, 0 if successful
 */
int
hpdf_table_set_label_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _CHK_TABLE(t);
    t->label_style.font = font;
    t->label_style.fsize = fsize;
    t->label_style.color = color;
    t->label_style.background = background;
    return 0;
}

/**
 * @brief Set font style for text content
 *
 * Set font options for cell content. This will be applied for all cells in the table. To
 * override the style for individual cells use the hpdf_table_set_cell_content_style()
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return -1 on error, 0 if successful
 * @see hpdf_table_set_cell_content_style()
 * @see hpdf_table_set_cell_content_style_callback()
 */
int
hpdf_table_set_content_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _CHK_TABLE(t);
    t->content_style.font = font;
    t->content_style.fsize = fsize;
    t->content_style.color = color;
    t->content_style.background = background;
    return 0;
}

/**
 * @brief Set the font style for a specific cell
 *
 * Set font options for a specific cell. This will override the global cell content setting
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return 0 on success, -1 on failure
 * @see hpdf_table_set_content_style()
 * @see hpdf_table_set_cell_content_style_callback()
 */
int
hpdf_table_set_cell_content_style(hpdf_table_t t, size_t r, size_t c, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _CHK_TABLE(t);
    _chk(t,r,c);
    hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];
    cell->content_style.font = font;
    cell->content_style.fsize = fsize;
    cell->content_style.color = color;
    cell->content_style.background = background;
    return 0;
}

/**
 * @brief Set cell specific callback to specify cell content style
 *
 * Set callback to format the style for the specified cell
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb Callback function
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_cell_content_style_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_content_style_callback_t cb) {
    _CHK_TABLE(t);
    _chk(t,r,c);
    // If this cell is part of another cells spanning then
    // indicate this as an error
    hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];
    if( cell->parent_cell ) {
        _SET_ERR(-1,r,c);
        return -1;
    }
    cell->style_cb = cb;
    return 0;
}


/**
 * @brief Set  callback to specify cell content style
 *
 * Set callback to format the style for cells in the table. If a cell has its own content style
 * callback that callback will override the generic table callback.
 * @param t Table handle
 * @param cb Callback function
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_content_style_callback(hpdf_table_t t, hpdf_table_content_style_callback_t cb) {
    _CHK_TABLE(t);
    t->content_style_cb = cb;
    return 0;
}


/**
 * @brief Set the table title style
 *
 * Set font options for title
 * @param t Table handle
 * @param font Font name
 * @param fsize Font size
 * @param color Color
 * @param background Background color
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_title_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background) {
    _CHK_TABLE(t);
    t->title_style.font = font;
    t->title_style.fsize = fsize;
    t->title_style.color = color;
    t->title_style.background = background;
    return 0;
}

/**
 * @brief Set table title
 *
 * Set table title
 * @param t Table handle
 * @param title Title string
 * @return 0 on success, -1 on failure
 * @see hpdf_table_set_title_style()
 */
int
hpdf_table_set_title(hpdf_table_t t, char *title) {
    _CHK_TABLE(t);
    if (t->title_txt)
        free(t->title_txt);
    t->title_txt = strdup(title);
    return 0;
}

/**
 * @brief Set horizontal alignment for table title
 *
 * Set horizontal text alignment for title
 * @param t Table handle
 * @param align Alignment
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_set_title_halign(hpdf_table_t t, hpdf_table_text_align_t align) {
    _CHK_TABLE(t);
    t->title_style.halign = align;
    return 0;
}

/**
 * @brief Apply a specified theme to a table
 *
 * Apply a specified theme to a table. The default table can be retrieved with
 * hpdf_table_get_default_theme()
 * @param t Table handle
 * @param theme Theme reference
 * @return 0 on success, -1 on failure
 */
int
hpdf_table_apply_theme(hpdf_table_t t, hpdf_table_theme_t *theme) {
    _CHK_TABLE(t);
    if( theme ) {

        t->use_header_row = theme->use_header_row;
        t->use_cell_labels = theme->use_labels;
        t->use_label_grid_style = theme->use_label_grid_style;

        if( theme->label_style ) {
            hpdf_text_style_t *f = theme->label_style;
            hpdf_table_set_label_style(t,f->font, f->fsize, f->color, f->background );
        }
        if( theme->content_style ) {
            hpdf_text_style_t *f = theme->content_style;
            hpdf_table_set_content_style(t, f->font, f->fsize, f->color, f->background);
        }
        if( theme->header_style ) {
            hpdf_text_style_t *f = theme->header_style;
            hpdf_table_set_header_style(t, f->font, f->fsize, f->color, f->background);
            hpdf_table_set_header_halign(t, f->halign);
        }
        if( theme->title_style ) {
            hpdf_text_style_t *f = theme->title_style;
            hpdf_table_set_title_style(t, f->font, f->fsize, f->color, f->background);
            hpdf_table_set_title_halign(t, f->halign);
        }
        if( theme->inner_border ) {
            hpdf_border_style_t *b = theme->inner_border;
            hpdf_table_set_inner_border(t,b->width,b->color);
        }
        if( theme->outer_border ) {
            hpdf_border_style_t *b = theme->outer_border;
            hpdf_table_set_outer_border(t,b->width,b->color);
        }
    }
    _SET_ERR(-9,-1,-1);
    return -1;
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
 * @see hpdf_table_stroke()
 */
int
hpdf_table_stroke_from_data(HPDF_Doc pdf_doc, HPDF_Page pdf_page, hpdf_table_spec_t tbl_spec, hpdf_table_theme_t *theme) {

    hpdf_table_t t = hpdf_table_create(tbl_spec.rows, tbl_spec.cols, tbl_spec.title);
    if( NULL==t ) {
        _SET_ERR(-5,-1,-1);
        return -1;
    }

    if( theme )
        hpdf_table_apply_theme(t, theme);

    size_t i=0;
    while( TRUE ) {
        struct hpdf_table_data_spec *spec = &tbl_spec.cell_spec[i];

        if( spec->rowspan==0 && spec->colspan==0 )
            break;

        if( -1 == hpdf_table_set_cell(t, spec->row, spec->col, spec->label, NULL) ||
            -1 == hpdf_table_set_cellspan(t,spec->row, spec->col,spec->rowspan,spec->colspan) ||
            -1 == hpdf_table_set_cell_content_callback(t,spec->row,spec->col,spec->cb) ||
            -1 == hpdf_table_set_cell_content_style_callback(t,spec->row,spec->col,spec->style_cb) ||
            -1 == hpdf_table_set_cell_canvas_callback(t,spec->row,spec->col,spec->canvas_cb) ) {
            
            hpdf_table_destroy(t);
            return -1;
        }

        i++;
    }

    // Final chance for the client to do any specific table modifications
    if( tbl_spec.table_post_cb ) {
        tbl_spec.table_post_cb(t);
    }

    int ret = hpdf_table_stroke(pdf_doc,pdf_page,t,tbl_spec.xpos,tbl_spec.ypos,tbl_spec.width,tbl_spec.height);
    hpdf_table_destroy(t);
    return ret;
}

/**
 * @brief Internal function.
 *
 * Internal function. Calculate the relative position of each cell in the
 * table taking row and column spanning into account
 * @param t Table handle
 * @return 0 on success, -1 on failure
 */
int
_calc_cell_pos(const hpdf_table_t t) {
    // Calculate relative position for all cells in relation
    // to bottom left table corner
    HPDF_REAL base_cell_height = t->height / t->rows;
    //HPDF_REAL base_cell_width = t->width / t->cols;
    HPDF_REAL base_cell_width_percent = 100.0 / t->cols;
    HPDF_REAL delta_x = 0;
    HPDF_REAL delta_y = 0;

    // Recalculate column widths
    // Pass 1. Determine how many columns have been manually specified
    float tot_specified_width_percent=0;
    size_t num_specified_cols=0;
    for (size_t c = 0; c < t->cols; c++) {
        if( t->col_width_percent[c] > 0 ) {
            num_specified_cols++;
            tot_specified_width_percent += t->col_width_percent[c];
        }
    }

    if( tot_specified_width_percent > 100.0 ) {
        _SET_ERR(-12,-1,-1);
        return -1;
    }

    // Recalculate column widths
    // Pass 2. Divide the remaining width along the unspecified columns
    const float remaining_width_percent = 100.0 - tot_specified_width_percent;
    const float num_unspecified_cols = t->cols - num_specified_cols;
    if( num_unspecified_cols > 0 ) {
        base_cell_width_percent = remaining_width_percent / num_unspecified_cols;
    }

    // Sanity check
    if( base_cell_width_percent < MIN_CALCULATED_PERCENT_CELL_WIDTH ) {
        _SET_ERR(-13,-1,-1);
        return -1;
    }

    for (size_t c = 0; c < t->cols; c++) {
        if( t->col_width_percent[c] == 0.0 ) {
            t->col_width_percent[c] = base_cell_width_percent;
        }
    }

    // Calculate the position for all cells.
    //
    // Pass 1. Give the basic position for all cells without
    // taking spanning in consideration
    for (int r = t->rows - 1; r >= 0; r--) {
        for (size_t c = 0; c < t->cols; c++) {
            hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];
            cell->delta_x = delta_x;
            cell->delta_y = delta_y;
            cell->width = (t->col_width_percent[c]/100.0) * t->width ;//base_cell_width;
            cell->height = base_cell_height;
            delta_x += cell->width; //base_cell_width;
        }
        delta_x = 0;
        delta_y += base_cell_height;
    }

    // Adjust for row and column spanning
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];
            if (cell->rowspan > 1) {
                cell->delta_y = t->cells[(r + cell->rowspan - 1) * t->cols + c].delta_y;
                cell->height = cell->rowspan*base_cell_height;
            }
            if (cell->colspan > 1) {
                HPDF_REAL col_span_with=0.0;
                for (size_t cc = 0; cc < cell->colspan; cc++) {
                    col_span_with += t->cells[_IDX(r, cc+c)].width;
                }
                cell->width = col_span_with;
            }
        }
    }


    //for (size_t c = 0; c < t->cols; c++) {
    //    printf("%.1f (%.1f) @ %.1f, \n",t->col_width_percent[c],t->cells[_IDX(0, c)].width,t->cells[_IDX(0, c)].delta_x);
    //}
    //printf("\n");

    return 0;
}

/**
 * @brief Internal function.
 *
 * Internal function. Stroke each cell content.
 * @param t Table handle
 * @param r Row
 * @param c Column
 */
static void
_table_cell_stroke(const hpdf_table_t t, const size_t r, const size_t c) {
    hpdf_table_cell_t *cell = &t->cells[r * t->cols + c];

    if (cell->parent_cell != NULL) {
        return;
    }

    HPDF_REAL left_right_padding = c == 0 ? t->outer_border.width + 2 : t->inner_border.width + 2;

    // Check if this is the first row and we should format it as a header row.
    // In case this is a header row we also ignore the label
    if (t->use_header_row && r == 0) {
        HPDF_Page_SetRGBFill(t->pdf_page, t->header_style.background.r, t->header_style.background.g, t->header_style.background.b);
        HPDF_Page_Rectangle(t->pdf_page,
                t->posx + cell->delta_x, t->posy + cell->delta_y,
                cell->width, cell->height);
        HPDF_Page_FillStroke(t->pdf_page);
    }

    if (!(t->use_header_row && r == 0) && cell->label) {
        // Stroke label
        if (t->use_cell_labels) {
            _set_fontc(t, t->label_style.font, t->label_style.fsize, t->label_style.color);
            char *label = cell->label;
            if (t->label_cb) {
                char *_label = t->label_cb(t->tag, r, c);
                if (_label)
                    label = _label;
            }
            HPDF_Page_BeginText(t->pdf_page);

            hpdf_table_encoding_text_out(t->pdf_page,
                    t->posx + cell->delta_x + left_right_padding,
                    t->posy + cell->delta_y + cell->height - t->label_style.fsize * 1.05, label);

            HPDF_Page_EndText(t->pdf_page);
        }
    }

    // Stroke content
    char *content = cell->content;

    // If the cell has its own callback this will override the tables
    // global cell callback
    if( cell->content_cb ) {
        char *_content = cell->content_cb(t->tag, r, c);
        if (_content)
            content = _content;
    }
    else if ( t->content_cb ) {
        char *_content = t->content_cb(t->tag, r, c);
        if (_content)
            content = _content;
    }


    if (t->use_header_row && r == 0) {
        _set_fontc(t, t->header_style.font, t->header_style.fsize, t->header_style.color);
    } else {
        _set_fontc(t, t->content_style.font, t->content_style.fsize, t->content_style.color);
        // Check if cell has its own stye which should override global setting but a defined
        // callback will override both
        hpdf_text_style_t cb_val = (hpdf_text_style_t){t->content_style.font, t->content_style.fsize,
                                                       t->content_style.color, t->content_style.background,
                                                       t->content_style.halign };
        if( cell->style_cb && cell->style_cb(t->tag, r, c, &cb_val) ) {
            _set_fontc(t, cb_val.font, cb_val.fsize, cb_val.color);
        } else if(t->content_style_cb && t->content_style_cb(t->tag, r, c, &cb_val) ) {
            _set_fontc(t, cb_val.font, cb_val.fsize, cb_val.color);
        } else if( cell->content_style.font ) {
            _set_fontc(t, cell->content_style.font, cell->content_style.fsize, cell->content_style.color);
        }
    }

    HPDF_REAL xpos = t->posx + cell->delta_x + left_right_padding;
    HPDF_REAL ypos = t->posy + cell->delta_y + cell->height - t->content_style.fsize * 1.25;

    if (t->use_header_row && r == 0) {
        // Roughly center the text vertically
        ypos = t->posy + cell->delta_y + (cell->height / 2 - t->header_style.fsize / 2) + t->header_style.fsize / 5;

        // Center the header
        if (t->header_style.halign == CENTER)
            xpos = t->posx + cell->delta_x + (cell->width - HPDF_Page_TextWidth(t->pdf_page, content)) / 2.0;
        else if (t->header_style.halign == RIGHT)
            xpos = t->posx + cell->delta_x + (cell->width - HPDF_Page_TextWidth(t->pdf_page, content)) - left_right_padding;
    } else {
        if (t->use_cell_labels)
            ypos -= (t->label_style.fsize * 1.15);
    }

    if( content ) {
        HPDF_Page_BeginText(t->pdf_page);
        hpdf_table_encoding_text_out(t->pdf_page, xpos, ypos, content);
        HPDF_Page_EndText(t->pdf_page);
    }

}

/**
 * @brief Get the height calculated for the last constructed table
 *
 * Get the last automatically calculated heigh when stroking a table.
 * (The height will be automatically calculated if it was specified as 0)
 * @param height Returned height
 * @return -1 on error, 0 if successful
 */
int
hpdf_table_get_last_auto_height(HPDF_REAL *height) {
    if( last_auto_height > 0 ) {
      *height=last_auto_height ;
      return 0;
    }
    _SET_ERR(-10,-1,-1);
    return -1;
}


/**
 * @brief Stroke the table
 *
 * Stroke the table at the specified position and size. The position is by default specified
 * as the lower left corner of the table. Use the hpdf_table_set_origin_top_left() to use
 * the top left of the table as reference point.
 *
 * @param pdf The HPDF document handle
 * @param page The HPDF page handle
 * @param t Table handle
 * @param xpos x position for table
 * @param ypos y position for table
 * @param width width of table
 * @param height height of table. If the height is specified as 0 it will be automatically
 * calculated. The calculated height can be retrived after the table has been stroked by a
 * call to hpdf_table_get_last_auto_height()
 * @return -1 on error, 0 if successful
 * @see hpdf_table_get_last_auto_height()
 * @see hpdf_table_stroke_from_data()
 */
int
hpdf_table_stroke(const HPDF_Doc pdf, const HPDF_Page page, hpdf_table_t t,
        const HPDF_REAL xpos, const HPDF_REAL ypos,
        const HPDF_REAL width, HPDF_REAL height) {

    if (NULL == pdf || NULL == page || NULL == t) {
        _SET_ERR(-6,-1,-1);
        return -1;
    }

    // Local positions to enable position adjustment
    HPDF_REAL y=ypos;
    HPDF_REAL x=xpos;

    last_auto_height = 0;
    if (height <= 0) {
        // Calculate height automagically based on number of rows
        // and font sizes
        height = t->content_style.fsize;
        if (t->use_cell_labels)
            height += t->label_style.fsize;
        height *= 1.5 * t->rows;
        last_auto_height = height;
    }

    const HPDF_REAL page_height = HPDF_Page_GetHeight(page);
    if( origin_as_top_left ) {
        y = page_height - ypos - height;
        if( t->title_txt ) {
            y -= 1.5 * t->title_style.fsize;
        }
    }

    t->pdf_doc = pdf;
    t->pdf_page = page;
    t->height = height;
    t->width = width;
    t->posx = x;
    t->posy = y;

    if( -1 == _calc_cell_pos(t) ) {
        return -1;
    }

    // Stroke table background
    HPDF_Page_SetRGBFill(page, t->content_style.background.r, t->content_style.background.g, t->content_style.background.b);
    HPDF_Page_Rectangle(page, x, y, width, height);
    HPDF_Page_Fill(page);

    // Stroke inner border
    HPDF_Page_SetRGBStroke(page, t->inner_border.color.r, t->inner_border.color.g, t->inner_border.color.b);
    HPDF_Page_SetLineWidth(page, t->inner_border.width);
    for (size_t r = 0; r < t->rows; r++) {
        for (size_t c = 0; c < t->cols; c++) {
            hpdf_table_cell_t *cell = &t->cells[_IDX(r, c)];

            // Only cells which are not covered by a parent spanning cell will be stroked
            if (cell->parent_cell == NULL) {

                if( cell->style_cb ) {
                    hpdf_text_style_t style = (hpdf_text_style_t){t->content_style.font, t->content_style.fsize,
                                                                  t->content_style.color, t->content_style.background, t->content_style.halign};
                    if( cell->style_cb(t->tag,r,c,&style) ) {
                        HPDF_Page_SetRGBFill(page, style.background.r, style.background.g, style.background.b);
                        HPDF_Page_Rectangle(page, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                        HPDF_Page_Fill(page);
                    }
                }
                else if( cell->content_style.font ) {
                    // If cell has its own style set this will override and we have to stroke the background here
                    HPDF_Page_SetRGBFill(page, cell->content_style.background.r, cell->content_style.background.g, cell->content_style.background.b);
                    HPDF_Page_Rectangle(page, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                    HPDF_Page_Fill(page);
                }

                if( cell->canvas_cb ) {
                    cell->canvas_cb(pdf, page, t->tag, r, c, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                }
                else if (t->canvas_cb) {
                    t->canvas_cb(pdf, page, t->tag, r, c, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);
                }

                _table_cell_stroke(t, r, c);

                HPDF_Page_SetLineWidth(page, t->inner_border.width);

                if( t->use_label_grid_style ) {

                    // If this cell spans multiple rows we draw the left line full and not just the short
                    // label lead since the visual appearance will just be bad otherwise
                    if( cell->rowspan > 1 ) {
                        HPDF_Page_MoveTo(page,x + cell->delta_x, y + cell->delta_y );
                    }
                    else {
                        HPDF_Page_MoveTo(page,x + cell->delta_x, y + cell->delta_y + cell->height - t->label_style.fsize*1.2);
                    }
                    HPDF_Page_LineTo(page,x + cell->delta_x, y + cell->delta_y + cell->height);
                    HPDF_Page_MoveTo(page,x + cell->delta_x, y + cell->delta_y + cell->height);
                    HPDF_Page_LineTo(page,x + cell->delta_x+cell->width, y + cell->delta_y + cell->height);

                } else {

                    HPDF_Page_Rectangle(page, x + cell->delta_x, y + cell->delta_y, cell->width, cell->height);

                }

                HPDF_Page_Stroke(page);
            }
        }
    }

    // Stoke outer border
    HPDF_Page_SetRGBStroke(page, t->outer_border.color.r, t->outer_border.color.g, t->outer_border.color.b);
    HPDF_Page_SetLineWidth(page, t->outer_border.width);
    HPDF_Page_Rectangle(page, x, y, width, height);
    HPDF_Page_Stroke(page);

    // If header row is enabled we add a thicker (same as outer border) line under the top row
    if (t->use_header_row) {
        hpdf_table_cell_t *cell = &t->cells[_IDX(0, 0)];
        HPDF_Page_MoveTo(page, x + cell->delta_x, y + cell->delta_y);
        HPDF_Page_LineTo(page, x + width, y + cell->delta_y);
        HPDF_Page_Stroke(page);
    }

    // Stroke title
    const HPDF_REAL title_height = _table_title_stroke(t);
    if( last_auto_height > 0 ) {
        last_auto_height += title_height;
    }

    return 0;
}


/* EOF */
