/* =========================================================================
 * File:        hpdf_table.h
 * Description: Utility module for flexible table drawing with HPDF library
 * Author:      Johan Persson (johan162@gmail.com)
 * SVN:         $Id$
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


#ifndef HPDF_TABLE_H
#define	HPDF_TABLE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define TIMES "Times-Roman"
#define TIMES_ITALIC "Times-Italic"
#define TIMES_BOLD "Times-Bold"
#define TIMES_BOLDITALIC "Times-BoldItalic"

#define HELVETICA "Helvetica"
#define HELVETICA_ITALIC "Helvetica-Oblique"
#define HELVETICA_BOLD "Helvetica-Bold"
#define HELVETICA_BOLDITALIC "Helvetica-BoldOblique"

#define COURIER "Courier"
#define COURIER_BOLD "Courier-Bold"
#define COURIER_IALIC "Courier-Oblique"
#define COURIER_BOLDITALIC "Courier-BoldOblique"

#define DEFAULT_TARGET_ENCODING "ISO8859-4"
#define DEFAULT_SOURCE_ENCODING "UTF-8"

#define HTABLE_TEXT_HALIGN_LEFT 0
#define HTABLE_TEXT_HALIGN_CENTER 1
#define HTABLE_TEXT_HALIGN_RIGHT 2

#define HPDF_COLOR_FROMRGB(r,g,b) (HPDF_RGBColor){r/255.0,g/255.0,b/255.0}    

    typedef enum hpdf_table_text_align {
        LEFT = 0, CENTER = 1, RIGHT = 2
    } hpdf_table_text_align_t;


    typedef struct text_style {
        char *font;
        HPDF_REAL fsize;
        HPDF_RGBColor color;
        HPDF_RGBColor background;
        hpdf_table_text_align_t halign;
    } hpdf_text_style_t;

    typedef char * (*hpdf_table_content_callback_t)(void *, size_t, size_t);
    typedef void (*hpdf_table_cell_callback)(HPDF_Page, void *, size_t, size_t, HPDF_REAL, HPDF_REAL, HPDF_REAL, HPDF_REAL);    
    typedef _Bool (*hpdf_table_content_style_callback_t)(void *, size_t, size_t, hpdf_text_style_t *);
        
    typedef enum hpdf_table_dash_style {
        SOLID = 0,
        DOT1 = 1, DOT2 = 2, DOT3 = 3,
        DASH1 = 4, DASH2 = 5, DASH3 = 6,
        DASHDOT = 7
    } hpdf_table_line_style_t;

    typedef struct border_style {
        HPDF_REAL width;
        HPDF_RGBColor color;
        hpdf_table_line_style_t line_style; /* reserved for future use */
    } hpdf_border_style_t;

    struct hpdf_table_cell {
        char *label;
        char *content;
        int colspan;
        int rowspan;
        HPDF_REAL height;
        HPDF_REAL width;
        HPDF_REAL delta_x; // Delta x in table from bottom left
        HPDF_REAL delta_y; // Delta y in table from bottom left
        HPDF_REAL textwidth;
        hpdf_table_content_callback_t content_cb;
        hpdf_table_content_style_callback_t style_cb;
        hpdf_text_style_t content_style;
        struct hpdf_table_cell *parent_cell;
    };
    typedef struct hpdf_table_cell haru_table_cell_t;

    struct hpdf_table {
        /** HPDF references */
        HPDF_Doc pdf_doc;
        HPDF_Page pdf_page;

        /** Basic table layout*/
        int cols;
        int rows;
        HPDF_REAL posx;
        HPDF_REAL posy;
        HPDF_REAL height;
        HPDF_REAL width;

        /** Optional tag used in callbacks */
        void *tag;

        /** Title settings */
        char *title_txt;
        hpdf_text_style_t title_style;

        /** Header setings */
        hpdf_text_style_t header_style;
        _Bool use_header_row;

        /** Label settings */
        hpdf_text_style_t label_style;
        _Bool use_cell_labels;
        _Bool use_label_grid_style;
        hpdf_table_content_callback_t label_cb;

        /** Content settings */
        hpdf_text_style_t content_style;
        hpdf_table_content_callback_t content_cb;

        /** Generic cell callback */
        hpdf_table_cell_callback cell_callback;
        haru_table_cell_t *cells;

        /** Table border settings */
        hpdf_border_style_t outer_border;
        hpdf_border_style_t inner_border;
    };

    typedef struct hpdf_table *hpdf_table_t;

    /** Used in data driven table creation */
    typedef struct hpdf_table_data_spec {
        size_t r, c;
        unsigned rspan, cspan;
        char *label;
        hpdf_table_content_callback_t cb;
        hpdf_table_content_style_callback_t style_cb;
    } hpdf_table_data_spec_t;

    /** Used in data driven table creation */
    typedef struct hpdf_table_spec {
        char *title;
        size_t rows, cols;
        HPDF_REAL xpos, ypos;
        HPDF_REAL width, height;
        hpdf_table_data_spec_t *cell_spec;
    } hpdf_table_spec_t;

    /** Define a set of styles into a table theme */
    typedef struct hpdf_table_theme {
        hpdf_text_style_t *content_style;
        hpdf_text_style_t *label_style;
        hpdf_text_style_t *header_style;
        hpdf_text_style_t *title_style;

        hpdf_border_style_t *inner_border;
        hpdf_border_style_t *outer_border;

        _Bool use_labels;
        _Bool use_label_grid_style;
        _Bool use_header_row;

    } hpdf_table_theme_t;


    hpdf_table_t
    hpdf_table_create(int rows, int cols, char *title);

    int
    hpdf_table_get_last_errcode(char **errstr, int *row, int *col);

    int
    hpdf_table_stroke_from_data(HPDF_Doc pdf_doc, HPDF_Page pdf_page, hpdf_table_spec_t tbl_spec, hpdf_table_theme_t *theme);

    int
    hpdf_table_destroy(hpdf_table_t t);

    int
    hpdf_table_apply_theme(hpdf_table_t t, hpdf_table_theme_t *theme);

    hpdf_table_theme_t *
    hpdf_table_get_default_theme(void);

    int
    hpdf_table_destroy_theme(hpdf_table_theme_t *theme);

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
    hpdf_table_set_cell(const hpdf_table_t t, int r, int c, char *label, char *content);

    int
    hpdf_table_clear_spanning(const hpdf_table_t t);

    int
    hpdf_table_set_cellspan(const hpdf_table_t t, int r, int c, int rowspan, int colspan);

    int
    hpdf_table_use_labels(hpdf_table_t t, _Bool use);

    int
    hpdf_table_use_labelgrid(hpdf_table_t t, _Bool use);

    int
    hpdf_table_set_tag(hpdf_table_t t, void *tag);

    int
    hpdf_table_set_label_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_content_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_cell_content_style(hpdf_table_t t, size_t r, size_t c, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_title_style(hpdf_table_t t, char *font, HPDF_REAL fsize, HPDF_RGBColor color, HPDF_RGBColor background);

    int
    hpdf_table_set_title(hpdf_table_t t, char *title);

    int
    hpdf_table_set_title_halign(hpdf_table_t t, hpdf_table_text_align_t align);

    int
    hpdf_table_set_labels(hpdf_table_t t, char **labels);

    int
    hpdf_table_set_content(hpdf_table_t t, char **content);

    int
    hpdf_table_stroke(const HPDF_Doc pdf, const HPDF_Page page, hpdf_table_t t,
            const HPDF_REAL x, const HPDF_REAL y,
            const HPDF_REAL width, const HPDF_REAL height);

    int
    hpdf_table_set_content_callback(hpdf_table_t t, hpdf_table_content_callback_t cb);

    int
    hpdf_table_set_label_callback(hpdf_table_t t, hpdf_table_content_callback_t cb);

    int
    hpdf_table_set_cell_callback(hpdf_table_t t, hpdf_table_cell_callback cb);

    int
    hpdf_table_set_cell_content_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_content_callback_t cb);

    void
    hpdf_table_set_text_encoding(char *target, char *source);

    int
    hpdf_table_set_line_dash(hpdf_table_t t, hpdf_table_line_style_t style );

    int
    hpdf_table_get_last_auto_height(HPDF_REAL *height);
    
    void
    hpdf_table_set_origin_top_left(const _Bool origin);
    
    int 
    hpdf_table_set_cell_content_style_callback(hpdf_table_t t, size_t r, size_t c, hpdf_table_content_style_callback_t cb);

#ifdef	__cplusplus
}
#endif

#endif	/* HPDF_TABLE_H */
