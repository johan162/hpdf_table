/* =========================================================================
 * File:        hpdftbl.h
 * Description: Utility module for flexible table drawing with HPDF library
 * Author:      Johan Persson (johan162@gmail.com)
 *
 * Copyright (C) 2022 Johan Persson
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
 * @file hpdftbl.h
 * 
 * @brief Necessary header file for HPDF table usage
 * 
 */

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

/**
 * Definition of built-in HPDF font families
 */
#define HPDF_FF_TIMES "Times-Roman"
#define HPDF_FF_TIMES_ITALIC "Times-Italic"
#define HPDF_FF_TIMES_BOLD "Times-Bold"
#define HPDF_FF_TIMES_BOLDITALIC "Times-BoldItalic"

#define HPDF_FF_HELVETICA "Helvetica"
#define HPDF_FF_HELVETICA_ITALIC "Helvetica-Oblique"
#define HPDF_FF_HELVETICA_BOLD "Helvetica-Bold"
#define HPDF_FF_HELVETICA_BOLDITALIC "Helvetica-BoldOblique"

#define HPDF_FF_COURIER "Courier"
#define HPDF_FF_COURIER_BOLD "Courier-Bold"
#define HPDF_FF_COURIER_IALIC "Courier-Oblique"
#define HPDF_FF_COURIER_BOLDITALIC "Courier-BoldOblique"

/**
 * Basic color definitions
 */
#define COLOR_DARK_RED      (HPDF_RGBColor) { 0.6f, 0.0f, 0.0f }
#define COLOR_RED           (HPDF_RGBColor) { 1.0f, 0.0f, 0.0f }
#define COLOR_LIGHT_GREEN   (HPDF_RGBColor) { 0.9f, 1.0f, 0.9f }
#define COLOR_GREEN         (HPDF_RGBColor) { 0.4f, 0.9f, 0.4f }
#define COLOR_DARK_GREEN    (HPDF_RGBColor) { 0.05f, 0.37f, 0.02f }
#define COLOR_DARK_GRAY     (HPDF_RGBColor) { 0.2f, 0.2f, 0.2f }
#define COLOR_LIGHT_GRAY    (HPDF_RGBColor) { 0.9f, 0.9f, 0.9f }
#define COLOR_GRAY          (HPDF_RGBColor) { 0.5f, 0.5f, 0.5f }
#define COLOR_SILVER        (HPDF_RGBColor) { 0.75f, 0.75f, 0.75f }
#define COLOR_LIGHT_BLUE    (HPDF_RGBColor) { 1.0f, 1.0f, 0.9f }
#define COLOR_BLUE          (HPDF_RGBColor) { 0.0f, 0.0f, 1.0f }
#define COLOR_DARK_BLUE     (HPDF_RGBColor) { 0.0f, 0.0f, 0.6f }
#define COLOR_WHITE         (HPDF_RGBColor) { 1.0f, 1.0f, 1.0f }
#define COLOR_BLACK         (HPDF_RGBColor) { 0.0f, 0.0f, 0.0f }

/**
 * Text encodings
 */
#define HPDFTBL_DEFAULT_TARGET_ENCODING "ISO8859-4"
#define HPDFTBL_DEFAULT_SOURCE_ENCODING "UTF-8"

#define HPDFTBL_TEXT_HALIGN_LEFT 0
#define HPDFTBL_TEXT_HALIGN_CENTER 1
#define HPDFTBL_TEXT_HALIGN_RIGHT 2

/*
 * Standard paper heights
 */
#define A4PAGE_HEIGHT_CM 29.7       /**< A4 Height in CM */
#define A4PAGE_WIDTH_CM 21.0        /**< A4 Width in CM */
#define A3PAGE_HEIGHT_CM 42.0       /**< A3 Height in CM */
#define A3PAGE_WIDTH_CM 29.7        /**< A3 Width in CM */
#define LETTERRPAGE_HEIGHT_CM 27.9  /**< US Letter Height in CM */
#define LETTERRPAGE_WIDTH_CM 21.6   /**< US Letter Width in CM */
#define LEGALPAGE_HEIGHT_CM 35.6    /**< US Legal Height in CM */
#define LEGALPAGE_WIDTH_CM 21.6     /**< US Legal Width in CM */

/**
 * Sentinel to mark the end of Cell Specifications for data driven table definition
 */
#define HPDFTBL_END_CELLSPECS {0, 0, 0, 0, 0, 0, 0, 0, 0}

/** 
 * Utility macro to calculate a color constant from RGB integer values [0,255] 
 */
#define HPDF_COLOR_FROMRGB(r, g, b) (HPDF_RGBColor){(r)/255.0,(g)/255.0,(b)/255.0}

/** 
 * The smallest size in percent of table width allowed by automatic calculation before giving an error 
 */
#define MIN_CALCULATED_PERCENT_CELL_WIDTH 2.0

/**
 * @brief Convert cm to dots using the default resolution (72 DPI)
 *
 * @param cm Measure in cm
 * @return HPDF_REAL Converted value in dots
 */
#define hpdftbl_cm2dpi(c) (((HPDF_REAL)(c))/2.54*72)

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
    hpdftbl_text_align_t halign; /**< Text horizontal alignment */
} hpdf_text_style_t;

/**
 * @brief Type specification for the table content callback
 *
 * The content callback is used to specify the textual content in a cell and is an alternative
 * method to specifying the content to be displayed.
 *
 * @see hpdftbl_set_content_callback()
 */
typedef char *(*hpdftbl_content_callback_t)(void *, size_t, size_t);

/**
 * @brief Type specification for the table canvas callback
 *
 * A canvas callback, if specified, is called for each cell before the content is stroked. The
 * callback will be given the bounding box for the cell (x,y,width,height) in addition to the
 * row and column the cell has.
 *
 * @see hpdftbl_set_canvas_callback()
 */
typedef void (*hpdftbl_canvas_callback_t)(HPDF_Doc, HPDF_Page, void *, size_t, size_t, HPDF_REAL, HPDF_REAL, HPDF_REAL,
                                          HPDF_REAL);

/**
 * @brief Type specification for the content style.
 *
 * The content callback is used to specify the textual style in a cell and is an alternative
 * method to specifying the style of content to be displayed.
 *
 * @see hpdftbl_set_content_style_callback()
 *
 */
typedef _Bool (*hpdftbl_content_style_callback_t)(void *, size_t, size_t, char *content, hpdf_text_style_t *);

/**
 * @brief Possible line dash styles in table frames.
 */
typedef enum hpdftbl_dash_style {
    SOLID = 0,              /**< Solid line */
    DOT1 = 1,               /**< Dotted line variant 1 */
    DOT2 = 2,               /**< Dotted line variant 2 */
    DOT3 = 3,               /**< Dotted line variant 3 */
    DASH1 = 4,              /**< Dashed line variant 1 */
    DASH2 = 5,              /**< Dashed line variant 2 */
    DASH3 = 6,              /**< Dashed line variant 3 */
    DASHDOT = 7             /**< Dashed-dot line variant 1 */
} hpdftbl_line_style_t;

/**
 * @brief Specification for table borders
 *
 * Contains line properties used when stroking a border line
 */
typedef struct border_style {
    HPDF_REAL width; /**< Line width of border */
    HPDF_RGBColor color; /**< Color of line */
    hpdftbl_line_style_t line_style; /**< Line style (currently not used, preparation for future extensions) */
} hpdf_border_style_t;

/**
 * @brief Specification of individual cells in the table
 *
 * This structure contains all information pertaining to each cell in the
 * table. The position of the cell is given as relative position from the lower
 * left corner of the table.
 */
struct hpdftbl_cell {
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
    /** Label callback. If this is specified then this will override any content callback specified for the table */
    hpdftbl_content_callback_t label_cb;
    /** Style for content callback. If this is specified then this will override any style content callback specified for the table */
    hpdftbl_content_style_callback_t style_cb;
    /** Canvas callback. If this is specified then this will override any canvas callback specified for the table  */
    hpdftbl_canvas_callback_t canvas_cb;
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
    /** Table content callback. Will be called for each cell unless the cella has its own content callback */
    hpdftbl_content_callback_t label_cb;
    /** Content style */
    hpdf_text_style_t content_style;
    /** Table content callback. Will be called for each cell unless the cell has its own content callback */
    hpdftbl_content_callback_t content_cb;
    /** Style for content callback. Will be called for each cell unless the cell has its own content style callback */
    hpdftbl_content_style_callback_t content_style_cb;
    /** Table canvas callback. Will be called for each cell unless the cell has its own canvas callback  */
    hpdftbl_canvas_callback_t canvas_cb;
    /** Reference to all an array of cells in the table*/
    hpdftbl_cell_t *cells;
    /** Table outer border settings */
    hpdf_border_style_t outer_border;
    /** Table inner border settings */
    hpdf_border_style_t inner_border;
    /** User specified column width as fraction of the table width. Defaults to equ-width */
    float *col_width_percent;
};

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
    /** Post table creation callback. This is an opportunity for a client to do any special
     * table manipulation before the table is stroked to the page. A reference to the table
     * will be passed on in the callback.
     */
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
    hpdf_text_style_t *content_style;
    /** Label text style */
    hpdf_text_style_t *label_style;
    /** Header text style */
    hpdf_text_style_t *header_style;
    /** Table title text style */
    hpdf_text_style_t *title_style;
    /** Table inner border style */
    hpdf_border_style_t *inner_border;
    /** Table outer border style */
    hpdf_border_style_t *outer_border;
    /** Flag if cell labels should be used  */
    _Bool use_labels;
    /** Flag if the special short vertical grid style for labels should be used  */
    _Bool use_label_grid_style;
    /** Flag if header row should be used */
    _Bool use_header_row;
} hpdftbl_theme_t;

/**
 * @brief TYpe for error handler function
 *
 * The error handler (of set) will be called if the table library descovers an error condition
 *
 * @see hpdftbl_set_errhandler()
 */
typedef void (*hpdftbl_error_handler_t)(hpdftbl_t, int, int, int);

static hpdftbl_error_handler_t hpdftbl_err_handler = NULL;

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
hpdftbl_stroke_from_data(HPDF_Doc pdf_doc, HPDF_Page pdf_page, hpdftbl_spec_t *tbl_spec, hpdftbl_theme_t *theme);

int
hpdftbl_destroy(hpdftbl_t t);

int
hpdftbl_get_last_auto_height(HPDF_REAL *height);

void
hpdftbl_set_anchor_top_left(_Bool anchor);

_Bool
hpdftbl_get_anchor_top_left(void);

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
hpdftbl_default_table_error_handler(hpdftbl_t t, int r, int c, int err);

/*
 * Theme handling functions
 */
int
hpdftbl_apply_theme(hpdftbl_t t, hpdftbl_theme_t *theme);

hpdftbl_theme_t *
hpdftbl_get_default_theme(void);

int
hpdftbl_destroy_theme(hpdftbl_theme_t *theme);

/*
 * Table layout adjusting functions
 */
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
hpdftbl_use_labels(hpdftbl_t t, _Bool use);

int
hpdftbl_use_labelgrid(hpdftbl_t t, _Bool use);

int
hpdftbl_set_background(hpdftbl_t t, HPDF_RGBColor background);

int
hpdftbl_set_outer_border(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color);

int
hpdftbl_set_inner_border(hpdftbl_t t, HPDF_REAL width, HPDF_RGBColor color);

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
hpdftbl_set_cell(hpdftbl_t t, int r, int c, char *label, char *content);

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
hpdftbl_set_cell_content_style_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_style_callback_t cb);

int
hpdftbl_set_content_style_cb(hpdftbl_t t, hpdftbl_content_style_callback_t cb);

int
hpdftbl_set_label_cb(hpdftbl_t t, hpdftbl_content_callback_t cb);

int
hpdftbl_set_cell_label_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_callback_t cb);

int
hpdftbl_set_canvas_cb(hpdftbl_t t, hpdftbl_canvas_callback_t cb);

int
hpdftbl_set_cell_canvas_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_canvas_callback_t cb);

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

#ifdef    __cplusplus
}
#endif

#endif    /* hpdftbl_H */
