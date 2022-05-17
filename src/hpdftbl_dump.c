/**
 * @file
 * @brief   Functions for json serializing of table data structure
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

#define jsonprint(...) if( _string_ ) { sprintf (_sbuff_, __VA_ARGS__);size_t _len_=xstrlcat(_jsonbuff_,_sbuff_,_jsonbuff_size_); if(_len_>=_jsonbuff_size_) return -1;} else {fprintf(fh,__VA_ARGS__);}

#define OUTJSON_NEWBLK() do { jsonprint("%*s{\n", tab,""); } while(0)
#define OUTJSON_ENDDOC() do { jsonprint( "}\n"); } while(0)
#define OUTJSON_INDENT() do { jsonprint( "%*s", tab,""); } while(0)
#define OUTJSON_NEWLINE() do { jsonprint( "\n"); } while(0)
#define OUTJSON_STRINT(k, v, e)  do { jsonprint( "%*s\"%s\": %d%c\n",tab,"",k,(int)v,e); } while(0)
#define OUTJSON_STRREAL(k, v, e)  do { jsonprint( "%*s\"%s\": %.8f%c\n",tab,"",k,v,e); } while(0)
#define OUTJSON_STRSTR(k, v)  do { if( v==NULL ) {jsonprint( "%*s\"%s\": \"\",\n",tab,"",k) } else { jsonprint( "%*s\"%s\": \"%s\",\n",tab,"",k,v);} } while(0)
#define OUTJSON_STRBLK(k)  do { jsonprint( "%*s\"%s\": {\n",tab,"",k); } while(0)
#define OUTJSON_STRLIST(k)  do { jsonprint( "%*s\"%s\": [\n",tab,"",k); } while(0)
#define OUTJSON_ENDLIST(e)  do { jsonprint( "\n%*s]%c\n",tab,"",e); } while(0)
#define OUTJSON_STARTBLK()  do { jsonprint( "%*s{\n",tab,""); } while(0)
#define OUTJSON_ENDBLK(c)  do { jsonprint( "%*s}%c\n",tab,"",c); } while(0)
#define OUTJSON_BOOL(k, v)  do { if(v) { jsonprint( "%*s\"%s\": true,\n",tab,"",k);} else { jsonprint( "%*s\"%s\": false,\n",tab,"",k);} } while(0)
#define OUTJSON_RGB(k, v)  do { jsonprint( "%*s\"%s\": [%.5f, %.5f, %.5f],\n",tab,"",k,v.r,v.g,v.b); } while(0)

#define OUTJSON_GRID(k, v)  do { \
    OUTJSON_STRBLK(k);              \
    tab += 2;                    \
    OUTJSON_STRREAL("width",v.width,',');      \
    OUTJSON_RGB("color",v.color); \
    OUTJSON_STRINT("dashstyle",v.line_dashstyle,' '); \
    tab -= 2;                       \
    OUTJSON_ENDBLK(',');            \
} while(0)

#define OUTJSON_TXTSTYLE(k, v, c) do { \
    jsonprint( "%*s\"%s\": {\n",tab,"",k); \
    tab += 2;\
    OUTJSON_STRSTR("font",v.font); \
    OUTJSON_STRREAL("fsize",v.fsize,',');\
    OUTJSON_RGB("color",v.color);       \
    OUTJSON_RGB("background",v.background); \
    OUTJSON_STRINT("halign",v.halign,' '); \
    tab -= 2;  \
    OUTJSON_ENDBLK(c); \
} while(0)


/**
 * @brief Serialize the specified theme structure to a named file
 *
 * The theme is serialized as JSON string array and have whitespaces and
 * newlines to make it more human readable.
 *
 * @param theme Pointer to theme structure to be serialized
 * @param filename Filename to write to
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_theme_dump(hpdftbl_theme_t *theme, char *filename) {
    FILE *fh = fopen(filename, "w");
    if (!fh)
        return -1;

    // 2k buffer is adequate for a theme
    const size_t buffsize = 2 * 1024;
    char *s = calloc(buffsize, sizeof(char));
    int ret = hpdftbl_theme_dumps(theme, s, buffsize);
    fprintf(fh, "%s\n", s);
    free(s);
    fclose(fh);
    return !ret ? 0 : -1;
}

/**
 * @brief Serialize theme structure to a string buffer.
 *
 * The theme is serialized as JSON string array and have whitespaces and
 * newlines to make it more human readable.
 *
 * @param theme Theme to serialize
 * @param buff Buffer to write serialized theme to. It should be a minimum of 2k chars.
 * @param buffsize Buffer size (including ending string NULL)
 * @return 0 on success, < 0 on failure
 */
int
hpdftbl_theme_dumps(hpdftbl_theme_t *theme, char *buff, const size_t buffsize) {
    char ob[1024];
    int tab = 0;
    char _sbuff_[1024];
    _Bool _string_ = TRUE;
    char *_jsonbuff_ = buff;
    const size_t _jsonbuff_size_ = buffsize;

    FILE *fh = NULL;

    OUTJSON_NEWBLK();
    tab += 2;
    OUTJSON_STRINT("version", THEME_JSON_VERSION, ',');
    OUTJSON_STRBLK("hpdftbl_theme");
    tab += 2;

    OUTJSON_TXTSTYLE("content_style", theme->content_style, ',');
    OUTJSON_TXTSTYLE("label_style", theme->label_style, ',');
    OUTJSON_TXTSTYLE("header_style", theme->header_style, ',');
    OUTJSON_TXTSTYLE("title_style", theme->title_style, ',');

    OUTJSON_GRID("outer_border", theme->outer_border);
    OUTJSON_GRID("inner_vborder", theme->inner_vborder);
    OUTJSON_GRID("inner_hborder", theme->inner_hborder);
    OUTJSON_GRID("inner_tborder", theme->inner_tborder);

    OUTJSON_BOOL("use_labels", theme->use_labels);
    OUTJSON_BOOL("use_label_grid_style", theme->use_label_grid_style);
    OUTJSON_BOOL("use_header_row", theme->use_header_row);
    OUTJSON_BOOL("use_zebra", theme->use_zebra);
    OUTJSON_STRINT("zebra_phase", theme->zebra_phase, ',');
    OUTJSON_RGB("zebra_color1", theme->zebra_color1);
    OUTJSON_RGB("zebra_color2", theme->zebra_color2);
    OUTJSON_STRREAL("bottom_vmargin_factor", theme->bottom_vmargin_factor, ' ');

    tab -= 2;
    OUTJSON_ENDBLK(' ');
    tab -= 2;
    OUTJSON_ENDDOC();

    return 0;
}

/**
 * @brief  Serialize a table structure as a JSON file.
 *
 * The table is serialized as JSON file and have whitespaces and newlines
 * to make it more human readable. The serialization is a complete representation
 * of a table.
 *
 * @param tbl Table handle
 * @param filename Filename to write to. Any path specified must exists
 * @return -1 on failure, 0 on success
 */
int
hpdftbl_dump(hpdftbl_t tbl, char *filename) {
    FILE *fh = fopen(filename, "w");
    if (!fh)
        return -1;

    const size_t buffsize = 100 * 1024;
    char *s = calloc(buffsize, sizeof(char));
    int ret = hpdftbl_dumps(tbl, s, buffsize);
    fprintf(fh, "%s\n", s);
    free(s);
    fclose(fh);
    return !ret ? 0 : -1;
}

/**
 * @brief Serialize a table structure to a string buffer
 *
 * The table is serialized as JSON and have whitespaces and newlines to make it more human readable.
 * Note is is the callers responsibility to make sure the buffer is large enough to hold the
 * serialized table.
 *
 * @param tbl Table handle of table to dump
 * @param buff Buffer to dump structure to
 * @param buffsize  Size of buffer
 * @return -1 on failure, 0 on success
 * @see hpdftbl_load(),hpdftbl_dump(), hpdftbl_stroke_pos(),
 */
int
hpdftbl_dumps(hpdftbl_t tbl, char *buff, size_t buffsize) {
    char ob[1024];
    int tab = 0;
    char _sbuff_[1024];
    _Bool _string_ = TRUE;
    char *_jsonbuff_ = buff;
    FILE *fh = NULL;
    const size_t _jsonbuff_size_ = buffsize;

    OUTJSON_NEWBLK();
    OUTJSON_STRINT("version", TABLE_JSON_VERSION, ',');
    OUTJSON_STRBLK("table");
    tab += 2;
    OUTJSON_STRSTR("tag", (char *) tbl->tag);
    OUTJSON_STRINT("rows", tbl->rows, ',');
    OUTJSON_STRINT("cols", tbl->cols, ',');
    OUTJSON_STRREAL("posx", tbl->posx, ',');
    OUTJSON_STRREAL("posy", tbl->posy, ',');
    OUTJSON_STRREAL("height", tbl->height, ',');
    OUTJSON_STRREAL("minrowheight", tbl->minrowheight, ',');
    OUTJSON_STRREAL("width", tbl->width, ',');
    OUTJSON_STRREAL("bottom_vmargin_factor", tbl->bottom_vmargin_factor, ',');
    OUTJSON_STRSTR("title_txt", tbl->title_txt);
    OUTJSON_BOOL("anchor_is_top_left", tbl->anchor_is_top_left);
    OUTJSON_BOOL("use_header_row", tbl->use_header_row);
    OUTJSON_BOOL("use_cell_labels", tbl->use_cell_labels);
    OUTJSON_BOOL("use_label_grid_style", tbl->use_label_grid_style);
    OUTJSON_BOOL("use_zebra", tbl->use_zebra);
    OUTJSON_STRINT("zebra_phase", tbl->zebra_phase, ',');
    OUTJSON_RGB("zebra_color1", tbl->zebra_color1);
    OUTJSON_RGB("zebra_color2", tbl->zebra_color2);

    OUTJSON_GRID("outer_grid", tbl->outer_grid);
    OUTJSON_GRID("inner_vgrid", tbl->inner_vgrid);
    OUTJSON_GRID("inner_hgrid", tbl->inner_hgrid);
    OUTJSON_GRID("inner_tgrid", tbl->inner_tgrid);

    OUTJSON_TXTSTYLE("content_style", tbl->content_style, ',');
    OUTJSON_TXTSTYLE("title_style", tbl->title_style, ',');
    OUTJSON_TXTSTYLE("header_style", tbl->header_style, ',');
    OUTJSON_TXTSTYLE("label_style", tbl->label_style, ',');

    OUTJSON_STRLIST("col_width_percent");
    tab += 2;
    OUTJSON_INDENT();
    for (size_t i = 0; i < tbl->cols - 1; i++) {
        jsonprint("%05f, ", tbl->col_width_percent[i]);
    }
    jsonprint("%05f", tbl->col_width_percent[tbl->cols - 1]);
    tab -= 2;
    OUTJSON_ENDLIST(',');

    OUTJSON_STRSTR("label_dyncb", tbl->label_dyncb);
    OUTJSON_STRSTR("content_dyncb", tbl->content_dyncb);
    OUTJSON_STRSTR("content_style_dyncb", tbl->content_style_dyncb);
    OUTJSON_STRSTR("canvas_dyncb", tbl->canvas_dyncb);
    OUTJSON_STRSTR("post_dyncb", tbl->post_dyncb);
    OUTJSON_STRLIST("cells");

    hpdftbl_cell_t *cell = tbl->cells;
    tab += 2;
    for (size_t r = 0; r < tbl->rows; r++) {
        for (size_t c = 0; c < tbl->cols; c++) {
            OUTJSON_STARTBLK();
            tab += 2;
            OUTJSON_STRINT("row", cell->row, ',');
            OUTJSON_STRINT("col", cell->col, ',');
            OUTJSON_STRSTR("label", cell->label);
            OUTJSON_STRSTR("content", cell->content);
            OUTJSON_STRINT("colspan", cell->colspan, ',');
            OUTJSON_STRINT("rowspan", cell->rowspan, ',');
            OUTJSON_STRREAL("height", cell->height, ',');
            OUTJSON_STRREAL("width", cell->width, ',');
            OUTJSON_STRREAL("delta_x", cell->delta_x, ',');
            OUTJSON_STRREAL("delta_y", cell->delta_y, ',');
            OUTJSON_STRREAL("textwidth", cell->textwidth, ',');
            OUTJSON_STRSTR("content_dyncb", cell->content_dyncb);
            OUTJSON_STRSTR("label_dyncb", cell->label_dyncb);
            OUTJSON_STRSTR("content_style_dyncb", cell->content_style_dyncb);
            OUTJSON_STRSTR("canvas_dyncb", cell->canvas_dyncb);
            if (cell->parent_cell != NULL) {
                OUTJSON_STRBLK("parent");
                tab += 2;
                OUTJSON_STRINT("row", cell->parent_cell->row, ',');
                OUTJSON_STRINT("col", cell->parent_cell->col, ' ');
                tab -= 2;
                OUTJSON_ENDBLK(',');
            }
            OUTJSON_TXTSTYLE("content_style", cell->content_style, ' ');
            tab -= 2;
            if (r == tbl->rows - 1 && c == tbl->cols - 1)
                OUTJSON_ENDBLK(' ');
            else
                OUTJSON_ENDBLK(',');
            cell++;
        }
    }
    OUTJSON_ENDLIST(' ');
    tab -= 2;
    OUTJSON_ENDBLK(' ');
    OUTJSON_ENDDOC();
    return 0;
}

#endif

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
