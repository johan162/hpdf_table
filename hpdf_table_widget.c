/* =========================================================================
 * File:        hpdf_table_widget.c
 * Description: Utility module for  creating widgets in tables
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
 * @brief Support for drawing widgets
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hpdf.h>

#include "hpdf_table.h"
#include "hpdf_table_widget.h"

#define TRUE 1
#define FALSE 0

// Silent gcc about unused "arg"in the widget functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * @brief Table widget that draws a sliding on/off switch. Meant to be used in a canvas
 * callback to display a boolean value.
 *
 * This function can not be used directly as a canvas callback since it needs the state
 * of the button as an argument. Instead create a simple canvas callback that determines
 * the wanted state and then just passes on all argument to this widget function.
 *
 * @param doc HPDF document handle
 * @param page HPDF page handle
 * @param xpos X-öosition of cell
 * @param ypos Y-Position of cell
 * @param width Width of cell
 * @param height Height of cell
 * @param state State of button On/Off
 */
void
hpdf_table_widget_slide_button(HPDF_Doc doc, HPDF_Page page,
                     HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, _Bool state) {

    const HPDF_RGBColor red = HPDF_COLOR_FROMRGB(210,42,0);
    const HPDF_RGBColor green = HPDF_COLOR_FROMRGB(60,179,113);
    const HPDF_RGBColor smoke = HPDF_COLOR_FROMRGB(240,240,240);

    // Text colors
    const HPDF_RGBColor white = HPDF_COLOR_FROMRGB(255,255,255);
    const HPDF_RGBColor gray = HPDF_COLOR_FROMRGB(220,220,220);

    const HPDF_RGBColor border_color = HPDF_COLOR_FROMRGB(110,110,110);
    const HPDF_REAL line_width=0.8;

    // Bounding box for slide button
    const HPDF_REAL button_width = 38;
    const HPDF_REAL button_height = 11;
    const HPDF_REAL button_xpos = xpos+(width-button_width)/2;
    const HPDF_REAL button_ypos = ypos+4;
    const HPDF_REAL button_rad = 4;

    // Default to on setting of "push-button"
    const HPDF_REAL center_x = state ? button_xpos+button_width-button_height/2-0.3 : 0.2+button_xpos+button_height/2;
    const HPDF_REAL center_y = button_ypos+button_height/2 ;
    const HPDF_REAL radius = button_height/2+1;

    HPDF_Page_SetLineWidth(page,line_width);
    HPDF_Page_SetRGBStroke(page, border_color.r, border_color.g, border_color.b);

    if( state ) {

        HPDF_Page_SetRGBFill(page, green.r, green.g, green.b);
        HPDF_RoundedCornerRectangle(page,button_xpos, button_ypos, button_width, button_height, button_rad);
        //HPDF_Page_Rectangle(page,button_xpos, button_ypos, button_width, button_height);
        HPDF_Page_FillStroke(page);

        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, white.r, white.g, white.b);
        HPDF_Page_SetTextRenderingMode(page, HPDF_FILL);

        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(doc, HPDF_FF_HELVETICA, HPDF_TABLE_DEFAULT_TARGET_ENCODING), 8);
        HPDF_Page_TextOut(page, button_xpos+8, button_ypos+button_height/2-3, "ON");
        HPDF_Page_EndText(page);

    } else {

        HPDF_Page_SetRGBFill(page, red.r, red.g, red.b);
        HPDF_RoundedCornerRectangle(page,button_xpos, button_ypos, button_width, button_height, button_rad);
        //HPDF_Page_Rectangle(page,button_xpos, button_ypos, button_width, button_height);        
        HPDF_Page_FillStroke(page);

        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, gray.r, gray.g, gray.b);
        HPDF_Page_SetTextRenderingMode(page, HPDF_FILL);

        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(doc, HPDF_FF_HELVETICA, HPDF_TABLE_DEFAULT_TARGET_ENCODING), 8);
        HPDF_Page_TextOut(page, button_xpos+button_height+4, button_ypos+button_height/2-3, "OFF");
        HPDF_Page_EndText(page);

    }

    // Draw circle button with three friction lines
    HPDF_Page_SetLineWidth(page,1.2);
    HPDF_Page_SetRGBFill(page, smoke.r, smoke.g, smoke.b);
    HPDF_Page_Circle(page,center_x,center_y,radius);
    HPDF_Page_FillStroke(page);
    
    HPDF_Page_SetLineWidth(page,line_width);
    HPDF_Page_MoveTo(page,center_x-1.5,center_y-radius*1/3);
    HPDF_Page_LineTo(page,center_x-1.5,center_y+radius*1/3);
    HPDF_Page_Stroke(page);
    
    HPDF_Page_MoveTo(page,center_x,center_y-radius*1/3);
    HPDF_Page_LineTo(page,center_x,center_y+radius*1/3);
    HPDF_Page_Stroke(page);
    
    HPDF_Page_MoveTo(page,center_x+1.5,center_y-radius*1/3);
    HPDF_Page_LineTo(page,center_x+1.5,center_y+radius*1/3);
    HPDF_Page_Stroke(page);

}

/**
 * @brief Draw a horizontal partially filled bar to indicate an analog (percentage) value
 *
 * This function can not be used directly as a canvas callback since it needs additional
 * parameters. Instead create a simple canvas callback that gives the additional parameters.
 *
 * @param doc HPDF Document handle
 * @param page HPDF Page handle
 * @param xpos Lower left x
 * @param ypos Lower left y
 * @param width Width of meter
 * @param height Height of meter
 * @param color Fill color for bar
 * @param val Percentage fill in range [0.0, 100.0]
 * @param show_val TRUE to show the value (in percent) at the top of the filled bar
 */
void
hpdf_table_widget_hbar(const HPDF_Doc doc, const HPDF_Page page,
                       const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                       const HPDF_RGBColor color, const float val, const _Bool show_val) {

    const HPDF_RGBColor graph_border_color = HPDF_COLOR_FROMRGB(128,128,128);
    const HPDF_RGBColor graph_text_color = HPDF_COLOR_FROMRGB(90,90,90);

    const HPDF_REAL graph_fill_width = val*width;
    const HPDF_REAL line_width=0.8;
    HPDF_Page_SetLineWidth(page,line_width);
    HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);

    HPDF_Page_Rectangle(page,xpos,ypos,graph_fill_width,height);
    HPDF_Page_Fill(page);

    HPDF_Page_SetRGBStroke(page,graph_border_color.r,graph_border_color.g,graph_border_color.b);
    HPDF_Page_Rectangle(page,xpos,ypos,width,height);
    HPDF_Page_Stroke(page);

    HPDF_Page_BeginText(page);
    HPDF_Page_SetRGBFill(page, graph_text_color.r, graph_text_color.g, graph_text_color.b);
    HPDF_Page_SetTextRenderingMode(page, HPDF_FILL);

    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(doc, HPDF_FF_HELVETICA, HPDF_TABLE_DEFAULT_TARGET_ENCODING), 8);
    HPDF_Page_TextOut(page, xpos-2, ypos-9, "0");
    HPDF_Page_TextOut(page, xpos+width-8, ypos-9, "100%");

    if( show_val ) {
        char buf[16];
        snprintf(buf,sizeof(buf),"%.0f%%",val*100);
        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(doc, HPDF_FF_HELVETICA_ITALIC, HPDF_TABLE_DEFAULT_TARGET_ENCODING), 8);
        HPDF_Page_TextOut(page, xpos+graph_fill_width+2, ypos+2, buf);
    }

    HPDF_Page_EndText(page);

}

/**
 * @brief Draw a horizontal segment meter that can be used to visualize a discrete value
 *
 * This function can not be used directly as a canvas callback since it needs additional
 * parameters. Instead create a simple canvas callback that gives the additional parameters.
 *
 * @param doc HPDF Document handle
 * @param page HPDF Page handle
 * @param xpos Lower left x
 * @param ypos Lower left y
 * @param width Width of meter
 * @param height Height of meter
 * @param num_segments Total number of segments
 * @param on_color Color for "on" segment
 * @param num_on_segments Number of on segments
 */
void
hpdf_table_widget_segment_hbar(const HPDF_Doc doc, const HPDF_Page page,
                                const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                                const size_t num_segments, const HPDF_RGBColor on_color, const size_t num_on_segments) {

    const HPDF_RGBColor segment_border_color = HPDF_COLOR_FROMRGB(128,128,128);
    const HPDF_RGBColor segment_off_color = HPDF_COLOR_FROMRGB(240,240,240);
    const HPDF_RGBColor segment_text_color = HPDF_COLOR_FROMRGB(90,90,90);

    const HPDF_REAL inter_segment_space = 1.5;
    const HPDF_REAL segment_width = (width - inter_segment_space*(num_segments-1)) / num_segments;
    const HPDF_REAL line_width=0.8;

    HPDF_Page_SetLineWidth(page,line_width);
    HPDF_Page_SetRGBStroke(page,segment_border_color.r,segment_border_color.g,segment_border_color.b);
    HPDF_Page_SetRGBFill(page, on_color.r, on_color.g, on_color.b);

    HPDF_REAL x=xpos;
    HPDF_REAL y=ypos;

    // Draw "on" segments
    for(size_t i=0; i < num_on_segments; i++) {
        HPDF_Page_Rectangle(page, x, y, segment_width, height);
        HPDF_Page_FillStroke(page);
        x += inter_segment_space+segment_width;
    }

    // Draw "off" segments
    HPDF_Page_SetRGBFill(page,segment_off_color.r,segment_off_color.g,segment_off_color.b);
    for(size_t i=0; i < (num_segments- num_on_segments); i++) {
        HPDF_Page_Rectangle(page, x, y, segment_width, height);
        HPDF_Page_FillStroke(page);
        x += inter_segment_space+segment_width;
    }

    HPDF_Page_BeginText(page);
    HPDF_Page_SetRGBFill(page, segment_text_color.r, segment_text_color.g, segment_text_color.b);
    HPDF_Page_SetTextRenderingMode(page, HPDF_FILL);

    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(doc, HPDF_FF_HELVETICA, HPDF_TABLE_DEFAULT_TARGET_ENCODING), 8);
    HPDF_Page_TextOut(page, xpos-2, ypos-9, "0");
    HPDF_Page_TextOut(page, xpos+width-8, ypos-9, "100%");

    HPDF_Page_EndText(page);
}

/**
 * @brief Draw a phone strength meter
 *
 * This function can not be used directly as a canvas callback since it needs additional
 * parameters. Instead create a simple canvas callback that gives the additional parameters.
 *
 * @param doc HPDF Document handle
 * @param page HPDF Page handle
 * @param xpos Lower left x
 * @param ypos Lower left y
 * @param width Width of meter
 * @param height Height of meter
 * @param num_segments Total number of segments
 * @param on_color Color for "on" segment
 * @param num_on_segments Number of on segments
 */
void
hpdf_table_widget_strength_meter(const HPDF_Doc doc, const HPDF_Page page,
                                 const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                                 const size_t num_segments, const HPDF_RGBColor on_color, const size_t num_on_segments) {

    const HPDF_RGBColor segment_border_color = HPDF_COLOR_FROMRGB(128,128,128);
    const HPDF_RGBColor segment_off_color = HPDF_COLOR_FROMRGB(240,240,240);

    const HPDF_REAL inter_segment_space = 2.5;
    const HPDF_REAL segment_width = (width - inter_segment_space*(num_segments-1)) / num_segments;
    const HPDF_REAL smallest_segment_height = height/5.0;
    const HPDF_REAL line_width=0.8;

    HPDF_Page_SetLineWidth(page,line_width);
    HPDF_Page_SetRGBStroke(page,segment_border_color.r,segment_border_color.g,segment_border_color.b);
    HPDF_Page_SetRGBFill(page, on_color.r, on_color.g, on_color.b);

    HPDF_REAL current_height = smallest_segment_height;
    HPDF_REAL x=xpos;
    HPDF_REAL y=ypos;

    // Draw "on" segments
    for(size_t i=0; i < num_on_segments; i++) {
        HPDF_Page_Rectangle(page, x, y, segment_width, current_height);
        x += inter_segment_space+segment_width;
        current_height += (height-smallest_segment_height)/num_segments;
    }
    HPDF_Page_FillStroke(page);

    HPDF_Page_SetRGBFill(page, segment_off_color.r, segment_off_color.g, segment_off_color.b);
    for(size_t i=0; i < num_segments-num_on_segments; i++) {
        HPDF_Page_Rectangle(page, x, y, segment_width, current_height);
        x += inter_segment_space+segment_width;
        current_height += (height-smallest_segment_height)/num_segments;
    }
    HPDF_Page_FillStroke(page);
}

#pragma GCC diagnostic pop

/* EOF */
