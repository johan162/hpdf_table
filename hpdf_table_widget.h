/* =========================================================================
 * File:        hpdf_table_widget.h
 * Description: Utility module for creating widgets in tables
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

#ifndef HPDF_TABLE_WIDGET_H
#define	HPDF_TABLE_WIDGET_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    void
    hpdf_table_widget_letter_buttons(HPDF_Doc doc, HPDF_Page page,
                     HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, 
                     const HPDF_RGBColor on_color, const HPDF_RGBColor off_color, 
                     const HPDF_RGBColor on_background, const HPDF_RGBColor off_background,
                     const HPDF_REAL fsize,
                     const char *letters, _Bool *state );

    void
    hpdf_table_widget_slide_button(HPDF_Doc doc, HPDF_Page page,
                     HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, _Bool state); 
    
    void
    hpdf_table_widget_hbar(const HPDF_Doc doc, const HPDF_Page page,
                       const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                       const HPDF_RGBColor color, const float val, const _Bool show_val);
    
    void
    hpdf_table_widget_segment_hbar(const HPDF_Doc doc, const HPDF_Page page,
                                const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                                const size_t num_segments, const HPDF_RGBColor on_color, const size_t num_on_segments, const _Bool text_below);
    
    void
    hpdf_table_widget_strength_meter(const HPDF_Doc doc, const HPDF_Page page,
                                 const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                                 const size_t num_segments, const HPDF_RGBColor on_color, const size_t num_on_segments);
    
#ifdef	__cplusplus
}
#endif

#endif	/* HPDF_TABLE_WIDGET_H */

