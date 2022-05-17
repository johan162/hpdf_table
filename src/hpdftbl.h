/**
 * @file
 * @brief    Header file for libhpdftbl
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

#include "config.h"
#ifndef hpdftbl_H
#define    hpdftbl_H

#ifdef    __cplusplus
// in case we have C++ code, we should use its' types and logic
#include <algorithm>
typedef std::_Bool _Bool;
#endif

#ifdef    __cplusplus
extern "C" {
#endif

#ifndef TRUE
/** @brief Boolean truth value */
#define TRUE 1
#endif

#ifndef FALSE
/** @brief Boolean false value */
#define FALSE 0
#endif

#ifndef max

/**
 * @brief Return the maximum value of numeric variables.
 */
#define max(a,b) (((a)>(b)) ? (a):(b))

/**
 * @brief Return the minimum value of numeric variables.
 */
#define min(a,b) (((a)<(b)) ? (a):(b))
#endif

/** Internal variable to record last error */
extern int hpdftbl_err_code ;

/** Internal variable to record last error */
extern int hpdftbl_err_row ;

/** Internal variable to record last error */
extern int hpdftbl_err_col ;

/** Internal variable to record last error */
extern int hpdftbl_err_lineno;

/** Internal variable to record last error */
extern char *hpdftbl_err_file;

/** Internal variable to record last error */
extern char hpdftbl_err_extrainfo[];

/** Data structure version for serialization of themes */
#define THEME_JSON_VERSION 1

/** Data structure version for serialization of tables */
#define TABLE_JSON_VERSION 1


/** Font family */
#define HPDF_FF_TIMES "Times-Roman"
/** Font family */
#define HPDF_FF_TIMES_ITALIC "Times-Italic"
/** Font family */
#define HPDF_FF_TIMES_BOLD "Times-Bold"
/** Font family */
#define HPDF_FF_TIMES_BOLDITALIC "Times-BoldItalic"
/** Font family */
#define HPDF_FF_HELVETICA "Helvetica"
/** Font family */
#define HPDF_FF_HELVETICA_ITALIC "Helvetica-Oblique"
/** Font family */
#define HPDF_FF_HELVETICA_BOLD "Helvetica-Bold"
/** Font family */
#define HPDF_FF_HELVETICA_BOLDITALIC "Helvetica-BoldOblique"
/** Font family */
#define HPDF_FF_COURIER "Courier"
/** Font family */
#define HPDF_FF_COURIER_BOLD "Courier-Bold"
/** Font family */
#define HPDF_FF_COURIER_IALIC "Courier-Oblique"
/** Font family */
#define HPDF_FF_COURIER_BOLDITALIC "Courier-BoldOblique"



#ifdef __cplusplus
/**
 * @brief Utility macro to create a HPDF color constant from integer RGB values
 */
#define _TO_HPDF_RGB(r, g, b) \
    { r / 255.0f, g / 255.0f, b / 255.0f }
#else
/**
 * @brief Utility macro to create a HPDF color constant from integer RGB values
 */
#define HPDF_RGB_CONVERT(r, g, b) \
    (HPDF_RGBColor) { r / 255.0f, g / 255.0f, b / 255.0f }
#endif

#ifdef __cplusplus

#define HPDF_COLOR_DARK_RED      { 0.6f, 0.0f, 0.0f }
#define HPDF_COLOR_RED           { 1.0f, 0.0f, 0.0f }
#define HPDF_COLOR_LIGHT_GREEN   { 0.9f, 1.0f, 0.9f }
#define HPDF_COLOR_GREEN         { 0.4f, 0.9f, 0.4f }
#define HPDF_COLOR_DARK_GREEN    { 0.05f, 0.37f, 0.02f }
#define HPDF_COLOR_DARK_GRAY     { 0.2f, 0.2f, 0.2f }
#define HPDF_COLOR_LIGHT_GRAY    { 0.9f, 0.9f, 0.9f }
#define HPDF_COLOR_XLIGHT_GRAY   { 0.95f, 0.95f, 0.95f }
#define HPDF_COLOR_GRAY          { 0.5f, 0.5f, 0.5f }
#define HPDF_COLOR_SILVER        { 0.75f, 0.75f, 0.75f }
#define HPDF_COLOR_LIGHT_BLUE    { 1.0f, 1.0f, 0.9f }
#define HPDF_COLOR_BLUE          { 0.0f, 0.0f, 1.0f }
#define HPDF_COLOR_DARK_BLUE     { 0.0f, 0.0f, 0.6f }
#define HPDF_COLOR_WHITE         { 1.0f, 1.0f, 1.0f }
#define HPDF_COLOR_BLACK         { 0.0f, 0.0f, 0.0f }

#else

#define HPDF_COLOR_DARK_RED      (HPDF_RGBColor) { 0.6f, 0.0f, 0.0f }
#define HPDF_COLOR_RED           (HPDF_RGBColor) { 1.0f, 0.0f, 0.0f }
#define HPDF_COLOR_LIGHT_GREEN   (HPDF_RGBColor) { 0.9f, 1.0f, 0.9f }
#define HPDF_COLOR_GREEN         (HPDF_RGBColor) { 0.4f, 0.9f, 0.4f }
#define HPDF_COLOR_DARK_GREEN    (HPDF_RGBColor) { 0.05f, 0.37f, 0.02f }
#define HPDF_COLOR_DARK_GRAY     (HPDF_RGBColor) { 0.2f, 0.2f, 0.2f }
#define HPDF_COLOR_LIGHT_GRAY    (HPDF_RGBColor) { 0.9f, 0.9f, 0.9f }
#define HPDF_COLOR_XLIGHT_GRAY   (HPDF_RGBColor) { 0.95f, 0.95f, 0.95f }
#define HPDF_COLOR_GRAY          (HPDF_RGBColor) { 0.5f, 0.5f, 0.5f }
#define HPDF_COLOR_SILVER        (HPDF_RGBColor) { 0.75f, 0.75f, 0.75f }
#define HPDF_COLOR_LIGHT_BLUE    (HPDF_RGBColor) { 1.0f, 1.0f, 0.9f }
#define HPDF_COLOR_BLUE          (HPDF_RGBColor) { 0.0f, 0.0f, 1.0f }
#define HPDF_COLOR_DARK_BLUE     (HPDF_RGBColor) { 0.0f, 0.0f, 0.6f }
#define HPDF_COLOR_WHITE         (HPDF_RGBColor) { 1.0f, 1.0f, 1.0f }
#define HPDF_COLOR_BLACK         (HPDF_RGBColor) { 0.0f, 0.0f, 0.0f }

#endif

#define HPDF_COLOR_ORANGE        HPDF_RGB_CONVERT(0xF5, 0xD0, 0x98);
#define HPDF_COLOR_ALMOST_BLACK  HPDF_RGB_CONVERT(0x14, 0x14, 0x14);

/**
 * @brief The margin from the bottom of the cell to the baseline of the text is calculated
 * as a fraction of the font size.
 *
 * The margin is calculated as:
 * @code bottom_margin = fontsize * AUTO_VBOTTOM_MARGIN_FACTOR @endcode
 * @see hpdftbl_set_bottom_vmargin_bottom()
 */
#define DEFAULT_AUTO_VBOTTOM_MARGIN_FACTOR 0.5

/**
 * @brief Default PDF text encodings
 */
#define HPDFTBL_DEFAULT_TARGET_ENCODING "ISO8859-4"

/**
 * @brief Default input source text encodings
 */
#define HPDFTBL_DEFAULT_SOURCE_ENCODING "UTF-8"

/**
 * @brief Standard A4 paper height in cm
 */
#define A4PAGE_HEIGHT_CM 29.7

/**
 * @brief Standard A4 paper width in cm
 */
#define A4PAGE_WIDTH_CM 21.0

/**
 * @brief Standard A3 paper height in cm
 */
#define A3PAGE_HEIGHT_CM 42.0

/**
 * @brief Standard A3 paper width in cm
 */
#define A3PAGE_WIDTH_CM 29.7

/**
 * @brief US Letter Height in cm
 */
#define LETTERRPAGE_HEIGHT_CM 27.9

/**
 * @brief US Letter width in cm
 */
#define LETTERRPAGE_WIDTH_CM 21.6

/**
 * @brief US Legal Height in cm
 */
#define LEGALPAGE_HEIGHT_CM 35.6

/**
 * @brief US Legal Width in cm
 */
#define LEGALPAGE_WIDTH_CM 21.6

/**
 * @brief Sentinel to mark the end of Cell Specifications for data driven table definition
 */
#define HPDFTBL_END_CELLSPECS {0, 0, 0, 0, 0, 0, 0, 0, 0}

/** 
 * @brief Utility macro to calculate a color constant from RGB integer values [0,255]
 */
#define HPDF_COLOR_FROMRGB(r, g, b) (HPDF_RGBColor){(r)/255.0,(g)/255.0,(b)/255.0}

/** 
 * @brief The smallest size in percent of table width allowed by automatic calculation before giving an error
 */
#define HPDFTBL_MIN_CALCULATED_PERCENT_CELL_WIDTH 2.0

/**
 * @brief Convert cm to dots using the default resolution (72 DPI)
 *
 * @param c Measure in cm
 * @return HPDF_REAL Converted value in dots
 */
#define hpdftbl_cm2dpi(c) (((HPDF_REAL)(c))/2.54*72)

/**
 * @brief Call the error handler with specified error code and table row, col where error occured.
 * @param t Table handler
 * @param err Error code
 * @param r Row where error occured
 * @param c Column where error occured
 */
#define _HPDFTBL_SET_ERR(t, err, r, c) do {hpdftbl_err_code=err;hpdftbl_err_row=r;hpdftbl_err_col=c;hpdftbl_err_lineno=__LINE__;hpdftbl_err_file=__FILE__; if(hpdftbl_err_handler){hpdftbl_err_handler(t,r,c,err);}} while(0)

/**
 * @brief Set optional extra info at error state. (Currently only used by the late binding setting
 * callback functions)
 * @param info Extra info that can be set by a function at a state of error
 * @see hpdftbl_set_label_dyncb(),hpdftbl_set_content_dyncb()
 */
#define _HPDFTBL_SET_ERR_EXTRA(info) do {strncpy(hpdftbl_err_extrainfo,info,1023);hpdftbl_err_extrainfo[1023]=0;} while(0)

/**
 * @brief NPE check before using a table handler
 */
#define _HPDFTBL_CHK_TABLE(t) do {if(NULL == t) {hpdftbl_err_code=-3;hpdftbl_err_row=-1;hpdftbl_err_col=-1;return -1;}} while(0)

/**
 * @brief Shortcut to calculate the index in an array from a row,column (table) position.
 */
#define _HPDFTBL_IDX(r, c) (r*t->cols+c)

/**
 * @brief Enumeration for horizontal text alignment
 *
 * @see hpdftbl_set_header_halign()
 * @see hpdftbl_set_title_halign()
 * @see hpdftbl_text_align
 */
typedef enum hpdftbl_text_align {
    LEFT = 0,       /**< Left test alignment */
    CENTER = 1,     /**< Center test alignment */
    RIGHT = 2       /**< Right test alignment */
} hpdftbl_text_align_t;

/**
 * @brief Specification of a text style
 *
 * This structure collects the basic properties for a text string (font, color, background, horizontal alignment)
 */
typedef struct text_style {
    char *font;                     /**< Font face name */
    HPDF_REAL fsize;                /**< Font size */
    HPDF_RGBColor color;            /**< Font color */
    HPDF_RGBColor background;       /**< Font background color */
    hpdftbl_text_align_t halign;    /**< Text horizontal alignment */
} hpdf_text_style_t;


/**
 * @brief Table handle is a pointer to the hpdftbl structure
 *
 * This is the basic table handle used in almost all API calls. A table reference is returned
 * when a table is created.
 *
 * @see hpdftbl_create()
 */
typedef struct hpdftbl *hpdftbl_t;

/**
 * @brief Type specification for the table content callback
 *
 * The content callback is used to specify the textual content in a cell and is an alternative
 * method to specifying the content to be displayed.
 *
 * @see hpdftbl_set_content_cb()
 */
typedef char *(*hpdftbl_content_callback_t)(void *, size_t, size_t);

/**
 * @brief Type specification for the table canvas callback
 *
 * A canvas callback, if specified, is called for each cell before the content is stroked. The
 * callback will be given the bounding box for the cell (x,y,width,height) in addition to the
 * row and column the cell has.
 *
 * @see hpdftbl_set_canvas_cb()
 */
typedef void (*hpdftbl_canvas_callback_t)(HPDF_Doc, HPDF_Page, void *, size_t, size_t, HPDF_REAL, HPDF_REAL, HPDF_REAL,
                                          HPDF_REAL);

/**
 * @brief Type specification for the content style.
 *
 * The content callback is used to specify the textual style in a cell and is an alternative
 * method to specifying the style of content to be displayed.
 *
 * @see hpdftbl_set_content_style_cb()
 *
 */
typedef _Bool (*hpdftbl_content_style_callback_t)(void *, size_t, size_t, char *content, hpdf_text_style_t *);


/**
 * @brief Callback type for optional post processing when constructing table from a data array
 *
 * Type for generic table callback used when constructing a table from data. This can be used
 * to perform any potential table manipulation. The callback happens after the table has been
 * fully constructed and just before it is stroked.
 *
 * @see hpdftbl_stroke_from_data()
 */
typedef void (*hpdftbl_callback_t)(hpdftbl_t);

/**
 * @brief Possible line dash styles for grid lines.
 *
 * In the illustration of the patterns "x"=solid and "_"=space.
 *
 * For each pattern we show
 * two full cycles which should give a good visual indication of the different patterns.
 */
typedef enum hpdftbl_dashstyle {
    LINE_SOLID ,              /**< Solid line */
    LINE_DOT1 ,               /**< Dotted line variant 1 "x_x_x_" */
    LINE_DOT2 ,               /**< Dotted line variant 2 "x__x__x__" */
    LINE_DOT3 ,               /**< Dotted line variant 3 "x___x___x___" */
    LINE_DOT4 ,               /**< Dotted line variant 3 "x_____x_____x_____" */
    LINE_DASH1 ,              /**< Dashed line variant 1 "xx__xx__xx__" */
    LINE_DASH2 ,              /**< Dashed line variant 2 "xx___xx___xx___" */
    LINE_DASH3 ,              /**< Dashed line variant 3 "xxxx__xxxx__xxxx__" */
    LINE_DASH4 ,              /**< Dashed line variant 4 "xxxx____xxxx____xxxx____" */
    LINE_DASH5 ,              /**< Dashed line variant 4 "xxxxxx___xxxxxx___xxxxxx___" */
    LINE_DASHDOT1 ,           /**< Dashed-dot line variant 1 "xxxxx__xx__xxxxx__xx__xxxxx__xx__" */
    LINE_DASHDOT2             /**< Dashed-dot line variant 1 "xxxxxxx___xxx___xxxxxxx___xxx___xxxxxxx___xxx___" */
} hpdftbl_line_dashstyle_t;

/**
 * @brief Specification for table grid lines
 *
 * Contains line properties used when stroking a grid line
 */
typedef struct grid_style {
    HPDF_REAL width; /**< Line width of grids */
    HPDF_RGBColor color; /**< Color of grids */
    hpdftbl_line_dashstyle_t line_dashstyle; /**< Line style for grid*/
} hpdftbl_grid_style_t;

/**
 * @brief Specification of individual cells in the table
 *
 * This structure contains all information pertaining to each cell in the
 * table. The position of the cell is given as relative position from the lower
 * left corner of the table.
 */
struct hpdftbl_cell {
    /** When serializing it makes it easier to have row,col in each cell */
    size_t row;
    /** When serializing it makes it easier to have row,col in each cell */
    size_t col;
    /** String reference for label text*/
    char *label;
    /** String reference for cell content */
    char *content;
    /** Number of column this cell spans*/
    size_t colspan;
    /** Number of rows this cell spans*/
    size_t rowspan;
    /** Height of cell */
    HPDF_REAL height;
    /** Width of cells */
    HPDF_REAL width;
    /** X-Position of cell from bottom left of table */
    HPDF_REAL delta_x;
    /** Y-Position of cell from bottom left of table */
    HPDF_REAL delta_y;
    /** Width of content string */
    HPDF_REAL textwidth;
    /** Content callback. If this is specified then this will override any content callback specified for the table */
    hpdftbl_content_callback_t content_cb;
    /** Cell content dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *content_dyncb;
    /** Label callback. If this is specified then this will override any content callback specified for the table */
    hpdftbl_content_callback_t label_cb;
    /** Cell label dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *label_dyncb;
    /** Style for content callback. If this is specified then this will override any style content callback specified for the table */
    hpdftbl_content_style_callback_t style_cb;
    /** Cell content style dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *content_style_dyncb;
    /** Canvas callback. If this is specified then this will override any canvas callback specified for the table  */
    hpdftbl_canvas_callback_t canvas_cb;
    /** Cell canvas dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *canvas_dyncb;
    /** The style of the text content. If a style callback is specified the callback will override this setting */
    hpdf_text_style_t content_style;
    /** Parent cell. If this cell is part of another cells row or column spanning this is a reference to this parent cell.
     * Normal cells without spanning has NULL as parent cell.
     */
    struct hpdftbl_cell *parent_cell;
};

/**
 * @brief Type definition for the cell structure
 *
 * This is an internal structure that represents an individual cell in the table.
 */
typedef struct hpdftbl_cell hpdftbl_cell_t;

/**
 * @brief Core table handle
 *
 * This is the main structure that contains all information for the table. The basic
 * structure is an array of cells.
 *
 * @see hpdftbl_cell_t
 */
struct hpdftbl {
    /** PDF document references */
    HPDF_Doc pdf_doc;
    /** PDF page reference */
    HPDF_Page pdf_page;
    /** Number of columns in table */
    size_t cols;
    /** Number of rows in table */
    size_t rows;
    /** X-position of table. Reference point defaults to lower left but can be changed by calling hpdftbl_set_anchor_top_left() */
    HPDF_REAL posx;
    /** Y-position of table. Reference point defaults to lower left but can be changed by calling hpdftbl_set_anchor_top_left() */
    HPDF_REAL posy;
    /** Table height. If specified as 0 then the height will be automatically calculated */
    HPDF_REAL height;
    /** Minimum table row height. If specified as 0 it has no effect */
    HPDF_REAL minrowheight;
    /** Is the table anchor to be upper top left or bottom left */
    _Bool anchor_is_top_left;
    /** The content text bottom margin as a factor of the fontsize */
    HPDF_REAL bottom_vmargin_factor;
    /** Table width */
    HPDF_REAL width;
    /** Optional tag used in callbacks. This can be used to identify the table or add any reference needed by a particular application  */
    void *tag;
    /** Title text  */
    char *title_txt;
    /** Title style */
    hpdf_text_style_t title_style;
    /** Header style  */
    hpdf_text_style_t header_style;
    /** Flag to determine if the first row in the table should be formatted as a header row */
    _Bool use_header_row;
    /** Label style settings */
    hpdf_text_style_t label_style;
    /** Flag to determine if cell labels should be used */
    _Bool use_cell_labels;
    /** Flag to determine of the short vertical label border should be used. Default is to use half grid. */
    _Bool use_label_grid_style;
    /** Content style */
    hpdf_text_style_t content_style;
    /** Table content callback. Will be called for each cell unless the cella has its own content callback */
    hpdftbl_content_callback_t label_cb;
    /** Table label dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *label_dyncb;
    /** Table content callback. Will be called for each cell unless the cell has its own content callback */
    hpdftbl_content_callback_t content_cb;
    /** Table content dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *content_dyncb;
    /** Style for content callback. Will be called for each cell unless the cell has its own content style callback */
    hpdftbl_content_style_callback_t content_style_cb;
    /** Table content style dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *content_style_dyncb;
    /** Table canvas callback. Will be called for each cell unless the cell has its own canvas callback  */
    hpdftbl_canvas_callback_t canvas_cb;
    /** Table canvas dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *canvas_dyncb;
    /** Post table creation callback. This is an opportunity for a client to do any special
     * table manipulation before the table is stroked to the page. A reference to the table
     * will be passed on in the callback.
     */
    hpdftbl_callback_t post_cb;
    /** Table post dynamic callback name. The name is created vi `strdup()` and must be freed on destruction */
    char *post_dyncb;
    /** Table outer border settings */
    hpdftbl_grid_style_t outer_grid;
    /** Table inner vertical border settings, if width>0 this takes precedence over the generic inner border */
    hpdftbl_grid_style_t inner_vgrid;
    /** Table inner horizontal border settings, if width>0 this takes precedence over the generic inner border */
    hpdftbl_grid_style_t inner_hgrid;
    /** Table inner horizontal top border settings, if width>0 this takes precedence over the generic horizontal and inner horizontal border */
    hpdftbl_grid_style_t inner_tgrid;
    /** Use alternating background color on every second line TRUE or FALSE. Defaults to FALSE.
     * @see hpdftbl_set_zebra()
     */
    _Bool use_zebra;
    /** Determine if we start with color1 (phase=0) or start with color2 (phase=1)
     * @see hpdftbl_set_zebra()
     */
    int zebra_phase;
    /** First zebra color. @see hpdftbl_set_zebra_color() */
    HPDF_RGBColor zebra_color1;
    /** Second zebra color. @see hpdftbl_set_zebra_color() */
    HPDF_RGBColor zebra_color2;
    /** User specified column width array as fraction of the table width. Defaults to equ-width */
    float *col_width_percent;
    /** Reference to all an array of cells in the table*/
    hpdftbl_cell_t *cells;
};

/**
 * @brief Used in data driven table creation
 *
 * A table can be specified by creating a array of this structure together
 * with the hpdftbl_spec_t structure. The array should have one entry for each
 * cell in the table.
 *
 * @see hpdftbl_stroke_from_data()
 */
typedef struct hpdftbl_cell_spec {
    /** Row for specified cell */
    size_t row;
    /** Row for specified cell*/
    size_t col;
    /** Number of rows the specified cell should span */
    unsigned rowspan;
    /** Number of columns the specified cell should span */
    unsigned colspan;
    /** The label for this cell */
    char *label;
    /** Content callback for this cell */
    hpdftbl_content_callback_t content_cb;
    /** Label callback for this cell */
    hpdftbl_content_callback_t label_cb;
    /**  Content style callback for this cell */
    hpdftbl_content_style_callback_t style_cb;
    /**  Canvas callback for this cell */
    hpdftbl_canvas_callback_t canvas_cb;
} hpdftbl_cell_spec_t;

/**
 * @brief Used in data driven table creation
 *
 * This is used together with an array of cell specification hpdftbl_cell_spec_t
 * to specify the layout of a table.
 */
typedef struct hpdftbl_spec {
    /** Table title */
    char *title;
    /** Use a header for the table */
    _Bool use_header;
    /** Use labels in table */
    _Bool use_labels;
    /** Use label grid in table */
    _Bool use_labelgrid;
    /** Number of rows in the table */
    size_t rows;
    /** Number of columns in the table */
    size_t cols;
    /** X-position for table */
    HPDF_REAL xpos;
    /** Y-position for table */
    HPDF_REAL ypos;
    /** Width of table */
    HPDF_REAL width;
    /** Height of table */
    HPDF_REAL height;
    /** Content callback for this table */
    hpdftbl_content_callback_t content_cb;
    /** Label callback for this table */
    hpdftbl_content_callback_t label_cb;
    /** Content style callback for table */
    hpdftbl_content_style_callback_t style_cb;
    /** Post table creation callback. */
    hpdftbl_callback_t post_cb;
    /** Array of cell specification */
    hpdftbl_cell_spec_t *cell_spec;
} hpdftbl_spec_t;

/**
 * @brief Define a set of styles into a table theme
 *
 * Contains all information about the styles of various elements in the table
 * that together make up the table style
 */
typedef struct hpdftbl_theme {
    /** Content text style */
    hpdf_text_style_t content_style;
    /** Label text style */
    hpdf_text_style_t label_style;
    /** Header text style */
    hpdf_text_style_t header_style;
    /** Table title text style */
    hpdf_text_style_t title_style;
    /** Table outer border style */
    hpdftbl_grid_style_t outer_border;
    /** Flag if cell labels should be used  */
    _Bool use_labels;
    /** Flag if the special short vertical grid style for labels should be used  */
    _Bool use_label_grid_style;
    /** Flag if header row should be used */
    _Bool use_header_row;
    /** Table inner vertical border settings, if width>0 this takes precedence over the generic inner border */
    hpdftbl_grid_style_t inner_vborder;
    /** Table inner horizontal border settings, if width>0 this takes precedence over the generic inner border */
    hpdftbl_grid_style_t inner_hborder;
    /** Table inner horizontal top border settings, if width>0 this takes precedence over the generic horizontal and inner horizontal border */
    hpdftbl_grid_style_t inner_tborder;
    /** Use alternating background color on every second line TRUE or FALSE. Defaults to FALSE. */
    _Bool use_zebra;
    /** Start with color1 or color2 */
    int zebra_phase;
    /** First zebra color. */
    HPDF_RGBColor zebra_color1;
    /** Second zebra color.  */
    HPDF_RGBColor zebra_color2;
    /** Specify the vertical margin factor */
    HPDF_REAL bottom_vmargin_factor;
} hpdftbl_theme_t;

/**
 * @brief TYpe for error handler function
 *
 * The error handler (of set) will be called if the table library descovers an error condition
 *
 * @see hpdftbl_set_errhandler()
 */
typedef void (*hpdftbl_error_handler_t)(hpdftbl_t, int, int, int);

extern hpdftbl_error_handler_t hpdftbl_err_handler ;

/*
 * Table creation and destruction function
 */
hpdftbl_t
hpdftbl_create(size_t rows, size_t cols);

hpdftbl_t
hpdftbl_create_title(size_t rows, size_t cols, char *title);

int
hpdftbl_stroke(HPDF_Doc pdf,
               HPDF_Page page, hpdftbl_t t,
               HPDF_REAL xpos, HPDF_REAL ypos,
               HPDF_REAL width, HPDF_REAL height);

int
hpdftbl_stroke_pos(HPDF_Doc pdf,
                   const HPDF_Page page, hpdftbl_t t);

int
hpdftbl_stroke_from_data(HPDF_Doc pdf_doc, HPDF_Page pdf_page, hpdftbl_spec_t *tbl_spec, hpdftbl_theme_t *theme);

int
hpdftbl_setpos(hpdftbl_t t,
               const HPDF_REAL xpos, const HPDF_REAL ypos,
               const HPDF_REAL width, HPDF_REAL height);

int
hpdftbl_destroy(hpdftbl_t t);

int
hpdftbl_get_last_auto_height(HPDF_REAL *height);

void
hpdftbl_set_anchor_top_left(hpdftbl_t tbl, _Bool anchor);

_Bool
hpdftbl_get_anchor_top_left(hpdftbl_t tbl);

/*
 * Table error handling functions
 */
hpdftbl_error_handler_t
hpdftbl_set_errhandler(hpdftbl_error_handler_t);

const char *
hpdftbl_get_errstr(int err);

const char *
hpdftbl_hpdf_get_errstr(HPDF_STATUS err_code);

int
hpdftbl_get_last_errcode(const char **errstr, int *row, int *col);

void
hpdftbl_get_last_err_file(int *lineno, char **file, char **extrainfo);

void
hpdftbl_default_table_error_handler(hpdftbl_t t, int r, int c, int err);

/*
 * Theme handling functions
 */
int
hpdftbl_apply_theme(hpdftbl_t t, hpdftbl_theme_t *theme);

hpdftbl_theme_t *
hpdftbl_get_default_theme(void);

int
hpdftbl_get_theme(hpdftbl_t tbl, hpdftbl_theme_t *theme);

int
hpdftbl_destroy_theme(hpdftbl_theme_t *theme);

/*
 * Table layout adjusting functions
 */

void
hpdftbl_set_bottom_vmargin_factor(hpdftbl_t t, HPDF_REAL f);

int
hpdftbl_set_min_rowheight(hpdftbl_t t, float h);

int
hpdftbl_set_colwidth_percent(hpdftbl_t t, size_t c, float w);

int
hpdftbl_clear_spanning(hpdftbl_t t);

int
hpdftbl_set_cellspan(hpdftbl_t t, size_t r, size_t c, size_t rowspan, size_t colspan);

/*
 * Table style handling functions
 */
int
hpdftbl_set_zebra(hpdftbl_t t, _Bool use, int phase);

int
hpdftbl_set_zebra_color(hpdftbl_t t, HPDF_RGBColor z1,  HPDF_RGBColor z2);

int
hpdftbl_use_labels(hpdftbl_t t, _Bool use);

int
hpdftbl_use_labelgrid(hpdftbl_t t, _Bool use);

int
hpdftbl_set_background(hpdftbl_t t, HPDF_RGBColor background);

int
hpdftbl_set_inner_tgrid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle);

int
hpdftbl_set_inner_vgrid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle);

int
hpdftbl_set_inner_hgrid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle);

int
hpdftbl_set_inner_grid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle);

int
hpdftbl_set_outer_grid_style(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color, hpdftbl_line_dashstyle_t dashstyle);

int
hpdftbl_set_header_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

int
hpdftbl_set_header_halign(hpdftbl_t t, hpdftbl_text_align_t align);

int
hpdftbl_use_header(hpdftbl_t t, _Bool use);

int
hpdftbl_set_label_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

int
hpdftbl_set_row_content_style(hpdftbl_t t, size_t r, char *font, HPDF_REAL fsize, HPDF_RGBColor color,
                              HPDF_RGBColor background);

int
hpdftbl_set_col_content_style(hpdftbl_t t, size_t c, char *font, HPDF_REAL fsize, HPDF_RGBColor color,
                              HPDF_RGBColor background);

int
hpdftbl_set_content_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

int
hpdftbl_set_cell_content_style(hpdftbl_t t, size_t r, size_t c, char *font, HPDF_REAL fsize, HPDF_RGBColor color,
                               HPDF_RGBColor background);

int
hpdftbl_set_title_style(hpdftbl_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

/*
 * Table content handling
 */
int
hpdftbl_set_cell(hpdftbl_t t, size_t r, size_t c, char *label, char *content);

int
hpdftbl_set_tag(hpdftbl_t t, void *tag);

int
hpdftbl_set_title(hpdftbl_t t, char *title);

int
hpdftbl_set_title_halign(hpdftbl_t t, hpdftbl_text_align_t align);

int
hpdftbl_set_labels(hpdftbl_t t, char **labels);

int
hpdftbl_set_content(hpdftbl_t t, char **content);

/*
 * Table callback functions
 */
int
hpdftbl_set_content_cb(hpdftbl_t t, hpdftbl_content_callback_t cb);

int
hpdftbl_set_cell_content_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_callback_t cb);

int
hpdftbl_set_label_cb(hpdftbl_t t, hpdftbl_content_callback_t cb);

int
hpdftbl_set_cell_label_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_callback_t cb);

int
hpdftbl_set_canvas_cb(hpdftbl_t t, hpdftbl_canvas_callback_t cb);

int
hpdftbl_set_cell_canvas_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_canvas_callback_t cb);

int
hpdftbl_set_content_style_cb(hpdftbl_t t, hpdftbl_content_style_callback_t cb);

int
hpdftbl_set_cell_content_style_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_style_callback_t cb);

int
hpdftbl_set_post_cb(hpdftbl_t t, hpdftbl_callback_t cb);

/*
 * Table dynamic callback functions
 */
void
hpdftbl_set_dlhandle(void *);

int
hpdftbl_set_content_dyncb(hpdftbl_t, const char *);

int
hpdftbl_set_canvas_dyncb(hpdftbl_t, const char *);

int
hpdftbl_set_cell_content_dyncb(hpdftbl_t, size_t, size_t, const char *);

int
hpdftbl_set_label_dyncb(hpdftbl_t, const char *);

int
hpdftbl_set_cell_label_dyncb(hpdftbl_t, size_t, size_t, const char *);

int
hpdftbl_set_content_style_dyncb(hpdftbl_t, const char *);

int
hpdftbl_set_cell_content_style_dyncb(hpdftbl_t, size_t, size_t, const char *);

int
hpdftbl_set_cell_canvas_dyncb(hpdftbl_t, size_t, size_t, const char *);

int
hpdftbl_set_post_dyncb(hpdftbl_t t, const char *cb_name);

/*
 * Text encoding
 */
void
hpdftbl_set_text_encoding(char *target, char *source);

int
hpdftbl_encoding_text_out(HPDF_Page page, HPDF_REAL xpos, HPDF_REAL ypos, char *text);

/*
 * Misc utility and widget functions
 */

void
HPDF_RoundedCornerRectangle(HPDF_Page page, HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                            HPDF_REAL rad);

void
hpdftbl_stroke_grid(HPDF_Doc pdf, HPDF_Page page);

void
hpdftbl_table_widget_letter_buttons(HPDF_Doc doc, HPDF_Page page,
                                    HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                                    HPDF_RGBColor on_color, HPDF_RGBColor off_color,
                                    HPDF_RGBColor on_background, HPDF_RGBColor off_background,
                                    HPDF_REAL fsize,
                                    const char *letters, _Bool *state);

void
hpdftbl_widget_slide_button(HPDF_Doc doc, HPDF_Page page,
                            HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, _Bool state);

void
hpdftbl_widget_hbar(HPDF_Doc doc, HPDF_Page page,
                    HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                    HPDF_RGBColor color, float val, _Bool hide_val);

void
hpdftbl_widget_segment_hbar(HPDF_Doc doc, HPDF_Page page,
                            HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                            size_t num_segments, HPDF_RGBColor on_color, double val_percent,
                            _Bool hide_val);

void
hpdftbl_widget_strength_meter(HPDF_Doc doc, HPDF_Page page,
                              HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                              size_t num_segments, HPDF_RGBColor on_color, size_t num_on_segments);

int
hpdftbl_stroke_pdfdoc(HPDF_Doc pdf_doc, char *file);

#ifdef HAVE_LIBJANSSON

int
hpdftbl_dump(hpdftbl_t tbl, char *filename);

int
hpdftbl_dumps(hpdftbl_t tbl, char *buff, size_t buffsize);

int
hpdftbl_load(hpdftbl_t tbl, char *filename);

int
hpdftbl_loads(hpdftbl_t tbl, char *buff);

int
hpdftbl_theme_dump(hpdftbl_theme_t *theme, char *filename);

int
hpdftbl_theme_dumps(hpdftbl_theme_t *theme, char *buff, size_t buffsize);

int
hpdftbl_theme_loads(hpdftbl_theme_t *tbl, char *buff);

int
hpdftbl_theme_load(hpdftbl_theme_t *tbl, char *filename);

#endif

size_t
xstrlcat(char *dst, const char *src, size_t siz);

size_t
xstrlcpy(char * __restrict dst, const char * __restrict src, size_t dsize);

int
hpdftbl_read_file(char *buff, size_t buffsize, char *filename);

/*
 * Internal functions
 */
_Bool
chktbl(hpdftbl_t, size_t, size_t);

#ifdef    __cplusplus
}
#endif

#endif    /* hpdftbl_H */
