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

/**
 * @brief Default style for table title
 */
#define HPDFTBL_DEFAULT_TITLE_STYLE {HPDF_FF_HELVETICA_BOLD,11,{0,0,0},{0.9f,0.9f,0.9f}, LEFT}

/**
 * @brief Default style for table header row
 * @see hpdftbl_set_header_style()
 */
#define HPDFTBL_DEFAULT_HEADER_STYLE {HPDF_FF_HELVETICA_BOLD,10,{0,0,0},{0.9f,0.9f,0.97f}, CENTER}

/**
 * @brief Default style for table header row
 * @see hpdftbl_set_label_style()
 */
#define HPDFTBL_DEFAULT_LABEL_STYLE {HPDF_FF_TIMES_ITALIC,9,{0.4f,0.4f,0.4f},{1,1,1}, LEFT}

/**
 * @brief Default style for table header row
 * @see hpdftbl_set_content_style()
 */
#define HPDFTBL_DEFAULT_CONTENT_STYLE {HPDF_FF_COURIER,10,{0.2f,0.2f,0.2f},{1,1,1}, LEFT}

/**
 * @brief Default style for table vertical inner grid
 * @see hpdftbl_set_inner_vgrid_style()
 */
#define HPDFTBL_DEFAULT_INNER_VGRID_STYLE {0.7, {0.5f,0.5f,0.5f},hpdftbl_line_dashstyle_t(0)}

/**
 * @brief Default style for table horizontal inner grid
 * @see hpdftbl_set_inner_hgrid_style()
 */
#define HPDFTBL_DEFAULT_INNER_HGRID_STYLE {0.7, {0.5f,0.5f,0.5f},hpdftbl_line_dashstyle_t(0)}

/**
 * @brief Default style for table outer grid (border)
 * @see hpdftbl_set_outer_grid_style()
 */
#define HPDFTBL_DEFAULT_OUTER_GRID_STYLE {1.0f, {0.2f,0.2f,0.2f},hpdftbl_line_dashstyle_t(0)}

/**
 * @brief Default style for alternating row backgrounds color 1
 *
 */
#define HPDFTBL_DEFAULT_ZEBRA_COLOR1 {1.0f,1.0f,1.0f}

/**
 * @brief Default style for alternating row backgrounds color 2
 *
 */
#define HPDFTBL_DEFAULT_ZEBRA_COLOR2 {0.95f,0.95f,0.95f}

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
 */
#define HPDFTBL_DEFAULT_ZEBRA_COLOR1    HPDF_COLOR_WHITE

/**
 * @brief Default style for alternating row backgrounds color 2
 */
#define HPDFTBL_DEFAULT_ZEBRA_COLOR2    HPDF_COLOR_XLIGHT_GRAY
#endif

#ifdef _MSC_VER
#define strdup _strdup
#endif


/**
 * @brief Apply a specified theme to a table
 *
 * Note however that a limitation (by design) of themes is that settings in individual
 * cells are not recorded in a theme since a theme can be applied to any table
 * despite the structure. This mean only settings that are generic to a table
 * is stored in a theme. Not individal cells.
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
        hpdftbl_set_zebra(t, theme->use_zebra, theme->zebra_phase);
        hpdftbl_set_zebra_color(t, theme->zebra_color1, theme->zebra_color2);
        hpdftbl_set_bottom_vmargin_factor(t, theme->bottom_vmargin_factor);
        return 0;
    }
    _HPDFTBL_SET_ERR(t, -9, -1, -1);
    return -1;
}

/**
 * @brief Extract theme from settings of a specific table.
 *
 * This is useful if a table has been specified with some specific look & feel
 * and another table should be given the same l&f.
 *
 * Note however that a limitation (by design) of themes is that settings in individual
 * cells are not recorded in a theme since a theme can be applied to any table
 * despite the structure. This mean only settings that are generic to a table
 * is stored in a theme. Not individal cells.
 *
 * @param tbl Table handle for table to have its settings extracted
 * @param theme Theme to be read out to.
 * @return 0 on success, -1 on failure
 */
int
hpdftbl_get_theme(hpdftbl_t tbl, hpdftbl_theme_t *theme) {
    _HPDFTBL_CHK_TABLE(tbl);
    if (theme) {
        theme->title_style = tbl->title_style;
        theme->use_header_row = tbl->use_header_row;
        theme->use_labels = tbl->use_cell_labels;
        theme->use_header_row = tbl->use_header_row;
        theme->use_label_grid_style = tbl->use_label_grid_style;
        theme->label_style = tbl->label_style;
        theme->header_style = tbl->header_style;
        theme->title_style = tbl->title_style;
        theme->content_style = tbl->content_style;
        theme->inner_vborder = tbl->inner_vgrid;
        theme->inner_tborder = tbl->inner_tgrid;
        theme->inner_hborder = tbl->inner_hgrid;
        theme->outer_border = tbl->outer_grid;
        theme->use_zebra = tbl->use_zebra;
        theme->zebra_phase = tbl->zebra_phase;
        theme->zebra_color1 = tbl->zebra_color1;
        theme->zebra_color2 = tbl->zebra_color2;
        theme->bottom_vmargin_factor = tbl->bottom_vmargin_factor;
        return 0;
    } else
        return -1;
}

/**
 * @brief Return the default theme
 *
 * Create and return a theme corresponding to the default table theme. It is the calling
 * functions responsibility to call hpdftbl_destroy_theme() to free the allocated
 * memory. The default theme is a good starting point to just make minor modifications
 * without having to define all elements.
 *
 * @return A new theme initialized to the default settings. It is the calling
 * routines responsibility to free memory used in the returned theme
 * with hpdftbl_destroy_theme()
 *
 * @see hpdftbl_apply_theme(), hpdftbl_destroy_theme()
 */
hpdftbl_theme_t *
hpdftbl_get_default_theme(void) {

#ifdef __cplusplus
    hpdftbl_theme_t *t = static_cast<hpdftbl_theme_t*>(calloc(1,sizeof(hpdftbl_theme_t)));
#else
    hpdftbl_theme_t *theme = calloc(1, sizeof(hpdftbl_theme_t));
#endif
    if (NULL == theme) {
        _HPDFTBL_SET_ERR(NULL, -5, -1, -1);
        return NULL;
    }

    // Disable labels and label short style grid by default
    theme->use_labels = FALSE;
    theme->use_label_grid_style = FALSE;
    theme->use_header_row = FALSE;

    theme->title_style = HPDFTBL_DEFAULT_TITLE_STYLE;
    theme->header_style = HPDFTBL_DEFAULT_HEADER_STYLE;
    theme->label_style = HPDFTBL_DEFAULT_LABEL_STYLE;
    theme->content_style = HPDFTBL_DEFAULT_CONTENT_STYLE;
    theme->outer_border = HPDFTBL_DEFAULT_OUTER_GRID_STYLE;
    theme->inner_vborder = HPDFTBL_DEFAULT_INNER_VGRID_STYLE;
    theme->inner_hborder = HPDFTBL_DEFAULT_INNER_HGRID_STYLE;
    theme->inner_tborder = HPDFTBL_DEFAULT_INNER_HGRID_STYLE;

    theme->use_zebra = FALSE;
    theme->zebra_color1 = HPDFTBL_DEFAULT_ZEBRA_COLOR1;
    theme->zebra_color2 = HPDFTBL_DEFAULT_ZEBRA_COLOR2;
    theme->zebra_phase = 0;
    theme->bottom_vmargin_factor = DEFAULT_AUTO_VBOTTOM_MARGIN_FACTOR;

    return theme;
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
