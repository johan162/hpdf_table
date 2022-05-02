/**
 * @file
 * @brief   Functions for theme handling
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

#if !(defined _WIN32 || defined __WIN32__)

#include <unistd.h>

#endif

#include <hpdf.h>

#include "hpdftbl.h"

/* Default styles */
#ifdef __cplusplus
#define HPDFTBL_DEFAULT_TITLE_STYLE			{HPDF_FF_HELVETICA_BOLD,11,{0,0,0},{0.9,0.9,0.9}, LEFT}
#define HPDFTBL_DEFAULT_HEADER_STYLE		{HPDF_FF_HELVETICA_BOLD,10,{0.2,0,0},{0.9,0.9,0.97}, CENTER}
#define HPDFTBL_DEFAULT_LABEL_STYLE			{HPDF_FF_TIMES_ITALIC,9,{0.4,0.4,0.4},{1,1,1}, LEFT}
#define HPDFTBL_DEFAULT_CONTENT_STYLE		{HPDF_FF_COURIER,10,{0.2,0.2,0.2},{1,1,1}, LEFT}
#define HPDFTBL_DEFAULT_INNER_GRID_STYLE	{0.7, {0.5,0.5,0.5}, hpdftbl_line_dashstyle_t(0)}
#define HPDFTBL_DEFAULT_INNER_VGRID_STYLE {0,   {0.5f,0.5f,0.5f}, hpdftbl_line_dashstyle_t(0)}
#define HPDFTBL_DEFAULT_INNER_HGRID_STYLE {0,   {0.5f,0.5f,0.5f}, hpdftbl_line_dashstyle_t(0)}
#define hpdftbl_DEFAULT_OUTER_BORDER_STYLE	{1.0, {0.2,0.2,0.2}, hpdftbl_line_dashstyle_t(0)}
#else
/**
 * @brief Default style for table title
 */
#define HPDFTBL_DEFAULT_TITLE_STYLE (hpdf_text_style_t){HPDF_FF_HELVETICA_BOLD,11,(HPDF_RGBColor){0,0,0},(HPDF_RGBColor){0.9f,0.9f,0.9f}, LEFT}

/**
 * @brief Default style for table header row
 * @see hpdftbl_set_header_style()
 */
#define HPDFTBL_DEFAULT_HEADER_STYLE (hpdf_text_style_t){HPDF_FF_HELVETICA_BOLD,10,(HPDF_RGBColor){0,0,0},(HPDF_RGBColor){0.9f,0.9f,0.97f}, CENTER}

/**
 * @brief Default style for table header row
 * @see hpdftbl_set_label_style()
 */
#define HPDFTBL_DEFAULT_LABEL_STYLE (hpdf_text_style_t){HPDF_FF_TIMES_ITALIC,9,(HPDF_RGBColor){0.4f,0.4f,0.4f},(HPDF_RGBColor){1,1,1}, LEFT}

/**
 * @brief Default style for table header row
 * @see hpdftbl_set_content_style()
 */
#define HPDFTBL_DEFAULT_CONTENT_STYLE (hpdf_text_style_t){HPDF_FF_COURIER,10,(HPDF_RGBColor){0.2f,0.2f,0.2f},(HPDF_RGBColor){1,1,1}, LEFT}

/**
 * @brief Default style for table vertical inner grid
 * @see hpdftbl_set_inner_vgrid_style()
 */
#define HPDFTBL_DEFAULT_INNER_VGRID_STYLE (hpdftbl_grid_style_t){0.7, (HPDF_RGBColor){0.5f,0.5f,0.5f},0}

/**
 * @brief Default style for table horizontal inner grid
 * @see hpdftbl_set_inner_hgrid_style()
 */
#define HPDFTBL_DEFAULT_INNER_HGRID_STYLE (hpdftbl_grid_style_t){0.7, (HPDF_RGBColor){0.5f,0.5f,0.5f},0}

/**
 * @brief Default style for table outer grid (border)
 * @see hpdftbl_set_outer_grid_style()
 */
#define HPDFTBL_DEFAULT_OUTER_GRID_STYLE (hpdftbl_grid_style_t){1.0f, (HPDF_RGBColor){0.2f,0.2f,0.2f},0}

/**
 * @brief Default style for alternating row backgrounds color 1
 * @todo Implement zebra table coloring
 */
#define HPDFTBL_DEFAULT_ZEBRA1_COLOR (HPDF_RGBColor){1.0f,1.0f,1.0f}

/**
 * @brief Default style for alternating row backgrounds color 2
 * @todo Implement zebra table coloring
 */
#define HPDFTBL_DEFAULT_ZEBRA2_COLOR (HPDF_RGBColor){0.95f,0.95f,0.95f}
#endif

#ifdef _MSC_VER
#define strdup _strdup
#endif


/**
 * @brief Apply a specified theme to a table
 *
 * The default table theme can be retrieved with
 * hpdftbl_get_default_theme()
 * @param t Table handle
 * @param theme Theme reference
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_get_default_theme()
 */
int
hpdftbl_apply_theme(hpdftbl_t t, hpdftbl_theme_t *theme) {
    _HPDFTBL_CHK_TABLE(t);
    if (theme) {
        t->use_header_row = theme->use_header_row;
        t->use_cell_labels = theme->use_labels;
        t->use_label_grid_style = theme->use_label_grid_style;
        hpdftbl_set_label_style(t, theme->label_style.font, theme->label_style.fsize, theme->label_style.color, theme->label_style.background);
        hpdftbl_set_header_style(t, theme->header_style.font, theme->header_style.fsize, theme->header_style.color, theme->header_style.background);
        hpdftbl_set_header_halign(t, theme->header_style.halign);
        hpdftbl_set_title_style(t, theme->title_style.font, theme->title_style.fsize, theme->title_style.color, theme->title_style.background);
        hpdftbl_set_title_halign(t, theme->title_style.halign);
        hpdftbl_set_content_style(t, theme->content_style.font, theme->content_style.fsize,theme->content_style.color, theme->content_style.background);
        hpdftbl_set_inner_vgrid_style(t, theme->inner_vborder.width, theme->inner_vborder.color, theme->inner_vborder.line_dashstyle);
        hpdftbl_set_inner_hgrid_style(t, theme->inner_hborder.width, theme->inner_hborder.color, theme->inner_hborder.line_dashstyle);
        hpdftbl_set_inner_tgrid_style(t, theme->inner_tborder.width, theme->inner_tborder.color, theme->inner_tborder.line_dashstyle);
        hpdftbl_set_outer_grid_style(t, theme->outer_border.width, theme->outer_border.color, theme->outer_border.line_dashstyle);
        return 0;
    }
    _HPDFTBL_SET_ERR(t, -9, -1, -1);
    return -1;
}


/**
 * @brief Return the default theme
 *
 * Create and return a theme corresponding to the default table theme. It is the calling
 * functions responsibility to call hpdftbl_destroy_theme() to free the allocated
 * memory. The default theme is a good starting point to just make minor modifications
 * without having to define all elements.
 * @return A new theme initialized to the default settings
 *
 * @see hpdftbl_apply_theme()
 */
hpdftbl_theme_t *
hpdftbl_get_default_theme(void) {

#ifdef __cplusplus
    hpdftbl_theme_t *t = static_cast<hpdftbl_theme_t*>(calloc(1,sizeof(hpdftbl_theme_t)));
#else
    hpdftbl_theme_t *t = calloc(1, sizeof(hpdftbl_theme_t));
#endif
    if (NULL == t) {
        _HPDFTBL_SET_ERR(NULL, -5, -1, -1);
        return NULL;
    }

    // Disable labels and label short style grid by default
    t->use_labels = FALSE;
    t->use_label_grid_style = FALSE;
    t->use_header_row = FALSE;

    t->title_style = HPDFTBL_DEFAULT_TITLE_STYLE;
    t->header_style = HPDFTBL_DEFAULT_HEADER_STYLE;
    t->label_style = HPDFTBL_DEFAULT_LABEL_STYLE;
    t->content_style = HPDFTBL_DEFAULT_CONTENT_STYLE;
    t->outer_border = HPDFTBL_DEFAULT_OUTER_GRID_STYLE;
    t->inner_vborder = HPDFTBL_DEFAULT_INNER_VGRID_STYLE;
    t->inner_hborder = HPDFTBL_DEFAULT_INNER_HGRID_STYLE;
    t->inner_tborder = HPDFTBL_DEFAULT_INNER_HGRID_STYLE;

    t->use_zebra = FALSE;
    t->zebra1_color = HPDFTBL_DEFAULT_ZEBRA1_COLOR;
    t->zebra2_color = HPDFTBL_DEFAULT_ZEBRA1_COLOR;

    return t;
}

/**
 * @brief Destroy existing theme structure and free memory
 *
 * Free all memory allocated by a theme
 * @param theme The theme to free
 * @return -1 for error , 0 for success
 */
int
hpdftbl_destroy_theme(hpdftbl_theme_t *theme) {
    if (NULL == theme) {
        _HPDFTBL_SET_ERR(NULL, -9, -1, -1);
        return -1;
    }
    free(theme);
    return 0;
}
