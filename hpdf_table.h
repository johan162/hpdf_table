/* =========================================================================
 * File:        hpdf_table.h
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
 * 
 * @brief Necessary header file for HPDF table usage
 * 
 */

#ifndef HPDF_TABLE_H
#define	HPDF_TABLE_H

#ifdef	__cplusplus
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
 * Text encodings
 */
#define HPDF_TABLE_DEFAULT_TARGET_ENCODING "ISO8859-4"
#define HPDF_TABLE_DEFAULT_SOURCE_ENCODING "UTF-8"

#define HPDF_TABLE_TEXT_HALIGN_LEFT 0
#define HPDF_TABLE_TEXT_HALIGN_CENTER 1
#define HPDF_TABLE_TEXT_HALIGN_RIGHT 2

/** 
 * Utility macro to calculate a color constant from RGB integer values [0,255] 
 */    
#define HPDF_COLOR_FROMRGB(r,g,b) (HPDF_RGBColor){r/255.0,g/255.0,b/255.0}   
    
/** 
 * The smallest size in percent of table width allowed by automatic calculation before giving an error 
 */
#define MIN_CALCULATED_PERCENT_CELL_WIDTH 2.0



    /**
     * @brief Oprions for horizontal text alignment
     * 
     * The horizontal alignment for a text
     */
    typedef enum hpdf_table_text_align {
        LEFT = 0, CENTER = 1, RIGHT = 2
    } hpdf_table_text_align_t;


    /**
     * @brief Specification of a text style
     * 
     * This structure collects the basic properties for a text string (font, color, background, horizontal alignment)
     */
    typedef struct text_style {
        char *font;                     /**< Font face name */
        HPDF_REAL fsize;                /**< Font size */
        HPDF_RGBColor color;            /**< Font color */
        HPDF_RGBColor background;       /**< Font background colot */
        hpdf_table_text_align_t halign; /**< Text horizontal alignment */
    } hpdf_text_style_t;

    /**
     * @brief Type specification for the table content callback
     * 
     * The content callback is used to specify the textual content in a cell and is an alternative
     * method to specifying the content to be displayed.
     * 
     * @see hpdf_table_set_content_callback()
     */
    typedef char * (*hpdf_table_content_callback_t)(void *, size_t, size_t);

    /**
     * @brief Type specification for the table canvas callback
     * 
     * A canvas callback, if specified, is called for each cell before the content is stroked. The
     * callback will be given the bounding box for the cell (x,y,width,height) in addition to the 
     * row and column the cell has.
     * 
     * @see hpdf_table_set_canvas_callback()
     */    
    typedef void (*hpdf_table_canvas_callback_t)(HPDF_Doc, HPDF_Page, void *, size_t, size_t, HPDF_REAL, HPDF_REAL, HPDF_REAL, HPDF_REAL);    
    
    /**
     * @brief Type specification for the content style
     * 
     * The content callback is used to specify the textual style in a cell and is an alternative
     * method to specifying the style of content to be displayed.
     * 
     * @see hpdf_table_set_content_style_callback()
     * 
     */        
    typedef _Bool (*hpdf_table_content_style_callback_t)(void *, size_t, size_t, hpdf_text_style_t *);
    
    /**
     * @brief Possible line dash styles
     */
    typedef enum hpdf_table_dash_style {
        SOLID = 0,
        DOT1 = 1, DOT2 = 2, DOT3 = 3,
        DASH1 = 4, DASH2 = 5, DASH3 = 6,
        DASHDOT = 7
    } hpdf_table_line_style_t;

    /**
     * @brief Specification for table borders
     * 
     * Contains line properties used when stroking a border line
     */
    typedef struct border_style {        
        HPDF_REAL width; /**< Line width of border */
        HPDF_RGBColor color; /**< Color of line */        
        hpdf_table_line_style_t line_style; /**< Line style (currently not used, preparation for future extensions) */
    } hpdf_border_style_t;

    /**
     * @brief Specification of individual cells in the table
     * 
     * This structure contains all information pertaining to each cell in the 
     * table. The position of the cell is given as relative position from the lower
     * left corner of the table.
     */
    struct hpdf_table_cell {
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
        hpdf_table_content_callback_t content_cb;
        /** Style for content callback. If this is specified then this will override any style content callback specified for the table */
        hpdf_table_content_style_callback_t style_cb;
        /** Canvas callback. If this is specified then this will override any canvas callback specified for the table  */
        hpdf_table_canvas_callback_t canvas_cb;
        /** The style of the text content. If a style callback is specified the callback will override this setting */
        hpdf_text_style_t content_style;
        /** Parent cell. If this cell is part of another cells row or column spanning this is a reference to this parent cell. 
         * Normal cells without spanning has NULL as parent cell.
         */
        struct hpdf_table_cell *parent_cell;
    };
    
    /**
     * @brief Type definition for the cell structure
     * 
     * This is an internal structure that represents an individual cell in the table.
     */
    typedef struct hpdf_table_cell hpdf_table_cell_t;
    
    /**
     * @brief Core table handle
     * 
     * This is the main structure that contains all information for the table. The basic
     * structure is an array of cells.
     * 
     * @see hpdf_table_cell_t
     */
    struct hpdf_table {
        /** PDF document references */
        HPDF_Doc pdf_doc;
        /** PDF page reference */
        HPDF_Page pdf_page;
        /** Number of columns in table */
        size_t cols;
        /** Number of rows in table */
        size_t rows;
        /** X-position of table. Reference point defaults to lower left but can be changed by calling hpdf_table_set_origin_top_left() */
        HPDF_REAL posx;
        /** Y-position of table. Reference point defaults to lower left but can be changed by calling hpdf_table_set_origin_top_left() */
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
        hpdf_table_content_callback_t label_cb;
        /** Content style */
        hpdf_text_style_t content_style;
        /** Table content callback. Will be called for each cell unless the cell has its own content callback */
        hpdf_table_content_callback_t content_cb;
        /** Style for content callback. Will be called for each cell unless the cell has its own content style callback */
        hpdf_table_content_style_callback_t content_style_cb;        
        /** Table canvas callback. Will be called for each cell unless the cell has its own canvas callback  */
        hpdf_table_canvas_callback_t canvas_cb;
        /** Reference to all an array of cells in the table*/
        hpdf_table_cell_t *cells;
        /** Table outer border settings */
        hpdf_border_style_t outer_border;
        /** Table inner border settings */
        hpdf_border_style_t inner_border;
        /** User specified column width as fraction of the table width. Defaults to equ-width */
        float *col_width_percent;
    };

    /**
     * @brief Table handle is a pointer to the hpdf_table structure
     * 
     * This is the basic table handle used in almost all API calls. A table reference is returned
     * when a table is created.
     * 
     * @see hpdf_table_create()
     */
    typedef struct hpdf_table *hpdf_table_t;
    
    /**
     * @brief Callback type for optional post processing when constructing table from a data array 
     * 
     * Type for generic table callback used when constructing a table from data. This can be used
     * to perform any potential table manipulation. The callback happens after the table has been
     * fully constructed and just before it is stroked. 
     * 
     * @see hpdf_table_stroke_from_data()
     */
    typedef void (*hpdf_table_callback_t)(hpdf_table_t);

    /** 
     * @brief Used in data driven table creation 
     * 
     * A table can be specified by creating a array of this structure together
     * with the hpdf_table_spec_t structure. The array should have one entry for each
     * cell in the table.
     * 
     * @see hpdf_table_stroke_from_data()
     */
    typedef struct hpdf_table_data_spec {
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
        hpdf_table_content_callback_t cb;
        /**  Content style callback for this cell */
        hpdf_table_content_style_callback_t style_cb;
        /**  Canvas callback for this cell */
        hpdf_table_canvas_callback_t canvas_cb;
        
    } hpdf_table_data_spec_t;

    /** 
     * @brief Used in data driven table creation 
     * 
     * This is used together with an array of cell specification hpdf_table_data_spec_t
     * to specify the layout of a table.
     */
    typedef struct hpdf_table_spec {
        /** Table title */
        char *title;
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
        /** Array of cell specification */
        hpdf_table_data_spec_t *cell_spec;
        /** Post table creation callback. This is an opportunity for a client to do any special 
         * table manipulation before the table is stroked to the page. A reference to the table
         * will be passed on in the callback.
         */
        hpdf_table_callback_t table_post_cb;
    } hpdf_table_spec_t;

    /** 
     * @brief Define a set of styles into a table theme 
     * 
     * Contains all information about the styles of various elements in the table
     * that together make up the table style
     */
    typedef struct hpdf_table_theme {
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
    } hpdf_table_theme_t;


    /** 
     * Table creation and destruction function 
     */
    hpdf_table_t
    hpdf_table_create(size_t rows, size_t cols, char *title);
    
    int
    hpdf_table_stroke(const HPDF_Doc pdf, const HPDF_Page page, hpdf_table_t t,
            const HPDF_REAL x, const HPDF_REAL y,
            const HPDF_REAL width, const HPDF_REAL height);
    
    int
    hpdf_table_stroke_from_data(HPDF_Doc pdf_doc, HPDF_Page pdf_page, hpdf_table_spec_t tbl_spec, hpdf_table_theme_t *theme);

    int
    hpdf_table_destroy(hpdf_table_t t);
    
    int
    hpdf_table_get_last_auto_height(HPDF_REAL *height);
    
    void
    hpdf_table_set_origin_top_left(const _Bool origin);
      
    /** 
     * Table error handling functions 
     */
    int
    hpdf_table_get_last_errcode(char **errstr, int *row, int *col);

    /** 
     * Theme handling functions 
     */    
    int
    hpdf_table_apply_theme(hpdf_table_t t, hpdf_table_theme_t *theme);

    hpdf_table_theme_t *
    hpdf_table_get_default_theme(void);

    int
    hpdf_table_destroy_theme(hpdf_table_theme_t *theme);
    
    /** 
     * Table layout adjusting functions 
     */
    int
    hpdf_table_set_colwidth_percent(hpdf_table_t t, size_t c, float w);

    int
    hpdf_table_clear_spanning(const hpdf_table_t t);

    int
    hpdf_table_set_cellspan(const hpdf_table_t t, size_t r, size_t c, size_t rowspan, size_t colspan);

    /** 
     * Table style handling functions 
     */
    int
    hpdf_table_use_labels(hpdf_table_t t, _Bool use);

    int
    hpdf_table_use_labelgrid(hpdf_table_t t, _Bool use);
    
    int
    hpdf_table_set_background(hpdf_table_t t, HPDF_RGBColor background);

    int
    hpdf_table_set_outer_border(hpdf_table_t t, HPDF_REAL width, HPDF_RGBColor color);

    int
    hpdf_table_set_inner_border(hpdf_table_t t, HPDF_REAL width, HPDF_RGBColor color);

    int
    hpdf_table_set_header_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_header_halign(hpdf_table_t t, hpdf_table_text_align_t align);

    int
    hpdf_table_use_header(hpdf_table_t t, _Bool use);

    int
    hpdf_table_set_label_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_content_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_cell_content_style(hpdf_table_t t, size_t r, size_t c, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_title_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    /** 
     * Table content handling 
     */
    int
    hpdf_table_set_cell(const hpdf_table_t t, int r, int c, char *label, char *content);

    int
    hpdf_table_set_tag(hpdf_table_t t, void *tag);

    int
    hpdf_table_set_title(hpdf_table_t t, char *title);

    int
    hpdf_table_set_title_halign(hpdf_table_t t, hpdf_table_text_align_t align);

    int
    hpdf_table_set_labels(hpdf_table_t t, char **labels);

    int
    hpdf_table_set_content(hpdf_table_t t, char **content);


    /** 
     * Table callback functions 
     */
    int
    hpdf_table_set_content_callback(hpdf_table_t t, hpdf_table_content_callback_t cb);
    
    int
    hpdf_table_set_cell_content_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_content_callback_t cb);

    int 
    hpdf_table_set_cell_content_style_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_content_style_callback_t cb);

    int
    hpdf_table_set_content_style_callback(hpdf_table_t t, hpdf_table_content_style_callback_t cb);    

    int
    hpdf_table_set_label_callback(hpdf_table_t t, hpdf_table_content_callback_t cb);

    int
    hpdf_table_set_canvas_callback(hpdf_table_t t, hpdf_table_canvas_callback_t cb);
    
    int
    hpdf_table_set_cell_canvas_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_canvas_callback_t cb);

    
    /** 
     * Text encoding 
     */
    void
    hpdf_table_set_text_encoding(char *target, char *source);
    
    int
    hpdf_table_encoding_text_out(HPDF_Page page, HPDF_REAL xpos, HPDF_REAL ypos, char *text);

    
    /**
     * Misc utility and widget functions
     */
    void
    HPDF_RoundedCornerRectangle(HPDF_Page page,HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, HPDF_REAL rad);

#ifdef	__cplusplus
}
#endif

#endif	/* HPDF_TABLE_H */
