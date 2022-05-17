/**
 * @file
 * @brief   Functions for load (internalizing) serialized data structure
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !(defined _WIN32 || defined __WIN32__)

#include <unistd.h>

#endif

#include <hpdf.h>
#include "hpdftbl.h"

#ifdef HAVE_LIBJANSSON

#include <jansson.h>

#ifndef _MSC_VER
// Silent gcc about unused "arg" in the callback and error functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


/*-----------------------------------------------------------------------
 * Load section.
 * Functions to de-serialize structures read from buffer or file.
 *
 */

#define GETJSON_STRING(table, k, var) do  { \
    json_t *_elem=json_object_get(table,k); \
    if(!_elem) {                            \
        json_not_found_str=k;               \
        goto json_raise_notfound_error;     \
    }                                       \
    if( strlen(json_string_value(_elem)) == 0 ) \
        var=NULL;                           \
    else                                    \
        var=strdup(json_string_value(_elem)); \
} while(0)

#define GETJSON_UINT(table, k, var) do { \
    json_t *_elem=json_object_get(table,k); \
    if(!_elem) {                         \
        json_not_found_str=k;            \
        goto json_raise_notfound_error;  \
    }                                    \
    var=(size_t)json_integer_value(_elem);  \
} while(0)

#define GETJSON_REAL(table, k, var) do { \
    json_t *_elem=json_object_get(table,k); \
    if(!_elem) {                         \
        json_not_found_str=k;            \
        goto json_raise_notfound_error;  \
    }                                    \
    var=json_real_value(_elem);          \
} while(0)

#define GETJSON_BOOLEAN(table, k, var) do { \
    json_t *_elem=json_object_get(table,k); \
    if(!_elem) {                            \
        json_not_found_str=k;               \
        goto json_raise_notfound_error;     \
    }                                       \
    var=json_boolean_value(_elem);          \
} while(0)

#define GETJSON_RGB(table, k, var) do { \
    json_t *_elem; \
    json_t *_val; \
    size_t _idx; \
    double tmpcol[3]; \
    _elem = json_object_get(table, k);\
    if(!_elem) {                        \
        json_not_found_str=k;           \
        goto json_raise_notfound_error; \
    } \
    json_array_foreach(_elem, _idx, _val) { \
        tmpcol[_idx] = json_real_value(_val); \
    } \
    var.r = tmpcol[0]; \
    var.g = tmpcol[1]; \
    var.b = tmpcol[2]; \
} while(0)

#define GETJSON_GRIDSTYLE(table, k, var) do { \
    json_t *_grid=json_object_get(table,k); \
    if(!_grid) {                              \
        json_not_found_str=k;                 \
        goto json_raise_notfound_error;       \
    } \
    if(json_is_object(_grid)) { \
        GETJSON_REAL(_grid,"width",var.width); \
        GETJSON_UINT(_grid,"dashstyle",var.line_dashstyle); \
        GETJSON_RGB(_grid,"color",var.color); \
    } \
} while(0)

#define GETJSON_TXTSTYLE(table, k, var) do { \
     json_t *_txtstyle=json_object_get(table,k); \
     if(!_txtstyle) {                        \
        json_not_found_str=k;                \
        goto json_raise_notfound_error;      \
     } \
     if(json_is_object(_txtstyle)) { \
        GETJSON_STRING(_txtstyle,"font",var.font); \
        GETJSON_REAL(_txtstyle,"fsize",var.fsize); \
        GETJSON_RGB(_txtstyle,"color",var.color); \
        GETJSON_RGB(_txtstyle,"background",var.background); \
        GETJSON_UINT(_txtstyle,"halign",var.halign); \
     } \
} while(0)

#define GETJSON_REALARRAY(table, k, var) do { \
    json_t *_array = json_object_get(table, k); \
    if(!_array){ \
        json_not_found_str=k; \
        goto json_raise_notfound_error; \
    } \
    size_t _idx; \
    json_t *_val; \
    json_array_foreach(_array, _idx, _val) { \
        var[_idx] = json_real_value(_val); \
    } \
} while(0)

#define GETJSON_DYNCB(table, key) do { \
    json_t *_elem=json_object_get(table, #key); \
    if(!_elem) { \
        json_not_found_str= #key; \
        goto json_raise_notfound_error; \
    } if( strlen(json_string_value(_elem)) != 0 ) \
        hpdftbl_set_ ## key(t, json_string_value(_elem)); \
} while(0);

#define GETJSON_CELLDYNCB(table, key, r, c) do { \
    json_t *_elem=json_object_get(table,#key); \
    if(!_elem) { \
        json_not_found_str=#key; \
        goto json_raise_notfound_error; \
    } if( strlen(json_string_value(_elem)) != 0 ) \
        hpdftbl_set_cell_ ## key(t, r, c, json_string_value(_elem)); \
} while(0);

#define GETJSON_CELLTXTSTYLE(table, key, r, c) do { \
    json_t *__elem=json_object_get(table, #key); \
    if(!__elem) { \
        json_not_found_str= #key; \
        goto json_raise_notfound_error; \
    } \
    if( json_is_object(__elem) ) {  \
        GETJSON_STRING(__elem,"font",t->cells[t->cols*r+c].key.font); \
        GETJSON_REAL(__elem,"fsize",t->cells[t->cols*r+c].key.fsize); \
        GETJSON_RGB(__elem,"color",t->cells[t->cols*r+c].key.color);  \
        GETJSON_RGB(__elem,"background",t->cells[t->cols*r+c].key.background); \
        GETJSON_UINT(__elem,"halign",t->cells[t->cols*r+c].key.halign); \
    } else {                                        \
        json_not_found_str= #key;                   \
        goto json_raise_notfound_error;             \
    } \
} while(0);

/**
 * @brief Load theme from a serialized string. This is the invert function
 * of hpdftbl_theme_dumps().
 *
 * @param theme Theme to load to.
 * @param buff Buffer which holds the previous serialized theme
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_theme_dumps(), hpdftbl_theme_load(), hpdftbl_apply_theme()
 */
int
hpdftbl_theme_loads(hpdftbl_theme_t *theme, char *buff) {

    hpdftbl_theme_t *t = theme;
    char *json_not_found_str = NULL;

    json_error_t json_error;
    json_t *root = json_loads(buff, 0, &json_error);
    if (!root) {
        goto json_raise_parse_error;
    }

    if (json_is_object(root)) {
        json_t *version = json_object_get(root, "version");

        int v = json_integer_value(version);
        if (v != THEME_JSON_VERSION)
            goto json_raise_notfound_error;

        json_t *theme = json_object_get(root, "hpdftbl_theme");
        if (!theme)
            goto json_raise_notfound_error;

        GETJSON_TXTSTYLE(theme, "content_style", t->content_style);
        GETJSON_TXTSTYLE(theme, "label_style", t->label_style);
        GETJSON_TXTSTYLE(theme, "header_style", t->header_style);
        GETJSON_TXTSTYLE(theme, "title_style", t->title_style);
        GETJSON_GRIDSTYLE(theme, "outer_border", t->outer_border);
        GETJSON_GRIDSTYLE(theme, "inner_vborder", t->inner_vborder);
        GETJSON_GRIDSTYLE(theme, "inner_hborder", t->inner_hborder);
        GETJSON_GRIDSTYLE(theme, "inner_tborder", t->inner_tborder);

        GETJSON_BOOLEAN(theme, "use_labels", t->use_labels);
        GETJSON_BOOLEAN(theme, "use_label_grid_style", t->use_label_grid_style);
        GETJSON_BOOLEAN(theme, "use_header_row", t->use_header_row);
        GETJSON_BOOLEAN(theme, "use_zebra", t->use_zebra);
        GETJSON_UINT(theme, "zebra_phase", t->zebra_phase);
        GETJSON_RGB(theme, "zebra_color1", t->zebra_color1);
        GETJSON_RGB(theme, "zebra_color2", t->zebra_color2);
        GETJSON_REAL(theme, "bottom_vmargin_factor", t->bottom_vmargin_factor);
    }

    return 0;

    json_raise_notfound_error:
    fprintf(stderr, "JSON Not Found: '%s'\n", json_not_found_str);
    hpdftbl_destroy_theme(t);
    return -2;
    json_raise_parse_error:
    fprintf(stderr, "JSON Err: '%s' at line %d\n", json_error.text, json_error.line);
    hpdftbl_destroy_theme(t);
    return -1;
}

/**
 * @brief Read a theme from a previous serialized theme from a named file.
 *
 * *Example:*
 * ```c
 * hpdftbl_t tbl = calloc(1, sizeof (struct hpdftbl));
 * hpdftbl_theme_t theme;
 *  if( 0 == hpdftbl_load(tbl, "tests/tut_ex41.json") ) {
 *      if( 0 == hpdftbl_theme_load(&theme, "mytheme.json") ) {
 *          hpdftbl_apply_theme(tbl, &theme);
 *          hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
 *      }
 *  }
 * ```
 *
 * @param theme Theme to read into
 * @param filename File to read from
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_theme_load(hpdftbl_theme_t *theme, char *filename) {
    const size_t buffsize = 2 * 1024; // 2k buffer
    char *buff = calloc(buffsize, sizeof(char));

    if (NULL == buff)
        return -1;

    if (0 == hpdftbl_read_file(buff, buffsize, filename)) {
        if (0 == hpdftbl_theme_loads(theme, buff)) {
            free(buff);
            return 0;
        }
    }
    free(buff);
    return -1;
}

/**
 * @brief Import a table structure from a serialized table on file.
 *
 * The json file make it possible to adjust the table directly. However
 * it is easy to get it wrong. Some things to keep in mind while doing
 * manual changes.
 *
 *  - A real number must always be written as a decimal number with at least
 *  one decimal point (even if it .0)
 *  - Remember that the width of the table is specified manually and not
 *  automatically recalculated based on the text width.
 *
 * After reading a serialized table it can asily be be stroked with only
 * two lines of code as the following code-snippet shows
 *
 * ```c
 *  hpdftbl_t tbl = calloc(1, sizeof (struct hpdftbl));
 *  if(0 == hpdftbl_load(tbl, "mytablefile.json") ) {
 *       hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
 *  }
 * ```
 *
 * @note The `hpdftbl_t` is a pointer type to `struct hpdftbl` and hence must
 * must be either dynamically allocated as the example here shows or an instance
 * of the struct must be created whose address is given to this functions.
 *
 * @param tbl Table to read into
 * @param filename File to read from
 * @return  0 on success, -1 on file parse error, -2 on nay other error
 */
int
hpdftbl_load(hpdftbl_t tbl, char *filename) {
    const size_t buffsize = 100 * 1024; // 100k buffer
    char *buff = calloc(buffsize, sizeof(char));

    if (NULL == buff)
        return -1;

    if (0 == hpdftbl_read_file(buff, buffsize, filename)) {
        if (0 == hpdftbl_loads(tbl, buff)) {
            free(buff);
            return 0;
        }
    }
    free(buff);
    return -1;
}

/**
 * @brief Import a table structure from a serialized json buffert.
 *
 * This is the preferred way on how to store a table structure in
 * for example a database.
 *
 * *Example:*
 *
 * ```c
 *  char *mybuffer = ....
 *  hpdftbl_t tbl = calloc(1, sizeof (struct hpdftbl));
 *  if(0 == hpdftbl_load(tbl, mybuffer) ) {
 *       hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
 *  }
 * ```
 *
 * @param tbl Reference to table handle to be populated
 * @param buff Buffer with serialized data to read back
 * @return 0 on success, -1 on file parse error, -2 on nay other error
 * @see hpdftbl_dump(), hpdftbl_load(), hpdftbl_stroke_pos()
 */
int
hpdftbl_loads(hpdftbl_t tbl, char *buff) {

    hpdftbl_t t = tbl;
    char *json_not_found_str = NULL;

    json_error_t json_error;
    json_t *root = json_loads(buff, 0, &json_error);
    if (!root) {
        goto json_raise_parse_error;
    }

    if (json_is_object(root)) {
        json_t *version = json_object_get(root, "version");

        int v = json_integer_value(version);
        if (v != TABLE_JSON_VERSION)
            goto json_raise_notfound_error;

        json_t *table = json_object_get(root, "table");
        if (!table)
            goto json_raise_notfound_error;

        if (json_is_object(table)) {
            GETJSON_STRING(table, "tag", t->tag);
            GETJSON_UINT(table, "rows", t->rows);
            GETJSON_UINT(table, "cols", t->cols);
            GETJSON_REAL(table, "posx", t->posx);
            GETJSON_REAL(table, "posy", t->posy);
            GETJSON_REAL(table, "width", t->width);
            GETJSON_REAL(table, "height", t->height);
            GETJSON_REAL(table, "minrowheight", t->minrowheight);
            GETJSON_REAL(table, "bottom_vmargin_factor", t->bottom_vmargin_factor);
            GETJSON_STRING(table, "title_txt", t->title_txt);
            GETJSON_BOOLEAN(table, "use_header_row", t->use_header_row);
            GETJSON_BOOLEAN(table, "use_cell_labels", t->use_cell_labels);
            GETJSON_BOOLEAN(table, "use_label_grid_style", t->use_label_grid_style);
            GETJSON_BOOLEAN(table, "use_zebra", t->use_zebra);
            GETJSON_BOOLEAN(table, "anchor_is_top_left", t->anchor_is_top_left);
            GETJSON_RGB(table, "zebra_color1", t->zebra_color1);
            GETJSON_RGB(table, "zebra_color2", t->zebra_color2);

            GETJSON_GRIDSTYLE(table, "outer_grid", t->outer_grid);
            GETJSON_GRIDSTYLE(table, "inner_vgrid", t->inner_vgrid);
            GETJSON_GRIDSTYLE(table, "inner_hgrid", t->inner_hgrid);
            GETJSON_GRIDSTYLE(table, "inner_tgrid", t->inner_tgrid);

            GETJSON_TXTSTYLE(table, "content_style", t->content_style);
            GETJSON_TXTSTYLE(table, "title_style", t->title_style);
            GETJSON_TXTSTYLE(table, "header_style", t->header_style);
            GETJSON_TXTSTYLE(table, "label_style", t->label_style);
            GETJSON_TXTSTYLE(table, "title_style", t->title_style);

            t->col_width_percent = calloc(t->cols, sizeof(float));
            GETJSON_REALARRAY(table, "col_width_percent", t->col_width_percent);

            GETJSON_DYNCB(table, label_dyncb);
            GETJSON_DYNCB(table, content_dyncb);
            GETJSON_DYNCB(table, post_dyncb);
            GETJSON_DYNCB(table, canvas_dyncb);
            GETJSON_DYNCB(table, content_style_dyncb);

            t->cells = calloc(t->cols * t->rows, sizeof(hpdftbl_cell_t));
            size_t idx;
            json_t *obj;
            json_t *array = json_object_get(table, "cells");
            if (json_is_array(array)) {
                json_array_foreach(array, idx, obj) {
                    GETJSON_UINT(obj, "row", t->cells[idx].row);
                    GETJSON_UINT(obj, "col", t->cells[idx].col);
                    GETJSON_STRING(obj, "label", t->cells[idx].label);
                    GETJSON_STRING(obj, "content", t->cells[idx].content);
                    GETJSON_UINT(obj, "rowspan", t->cells[idx].rowspan);
                    GETJSON_UINT(obj, "colspan", t->cells[idx].colspan);
                    GETJSON_REAL(obj, "height", t->cells[idx].height);
                    GETJSON_REAL(obj, "width", t->cells[idx].width);
                    GETJSON_REAL(obj, "delta_x", t->cells[idx].delta_x);
                    GETJSON_REAL(obj, "delta_y", t->cells[idx].delta_y);
                    GETJSON_REAL(obj, "textwidth", t->cells[idx].textwidth);

                    GETJSON_CELLDYNCB(obj, content_dyncb, t->cells[idx].row, t->cells[idx].col);
                    GETJSON_CELLDYNCB(obj, label_dyncb, t->cells[idx].row, t->cells[idx].col);
                    GETJSON_CELLDYNCB(obj, content_style_dyncb, t->cells[idx].row, t->cells[idx].col);
                    GETJSON_CELLDYNCB(obj, canvas_dyncb, t->cells[idx].row, t->cells[idx].col);

                    GETJSON_CELLTXTSTYLE(obj, content_style, t->cells[idx].row, t->cells[idx].col);

                    do {
                        json_t *_parent = json_object_get(obj, "parent");
                        if (_parent && json_is_object(_parent)) {
                            size_t par_row = -1, par_col = -1;
                            GETJSON_UINT(_parent, "row", par_row);
                            GETJSON_UINT(_parent, "col", par_col);
                            t->cells[t->cols * t->cells[idx].row + t->cells[idx].col].parent_cell = &t->cells[
                                    t->cols * par_row + par_col];
                        }
                    } while (0);
                }
                return 0;
            }
        } else {
            goto json_raise_notfound_error;
        }
    } else {
        goto json_raise_notfound_error;
    }

    return 0;

    json_raise_notfound_error:
    fprintf(stderr, "JSON Not Found: '%s'\n", json_not_found_str);
    hpdftbl_destroy(t);
    return -2;
    json_raise_parse_error:
    fprintf(stderr, "JSON Err: '%s' at line %d\n", json_error.text, json_error.line);
    hpdftbl_destroy(t);
    return -1;
}

#endif

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
