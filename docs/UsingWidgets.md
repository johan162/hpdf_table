# Widgets

## Overview
A feature in the library is the possibility to add widgets in table cell.
A widget is used to visualize da ata value in a cell instead of a numeric value. 
For example a percentage value can instead be represented by a horizontal bar.

As of this writing the library supports the following five widgets.

### 1. Segmented horizontal bar example
Horizontal discrete (segmented) bar. Number of segment is user defined.

@image html screenshots/segmented_hbar_small.png
@image latex screenshots/segmented_hbar.png width=4cm

### 2. Horizontal bar example
Basic horizontal bar

@image html screenshots/hbar_small.png  
@image latex screenshots/hbar.png width=4cm

### 3. Signal strength meter example
A widget indicate a signal strength in similar fashion as the signal strength meter on a phone.

@image html screenshots/power_meter_small.png  
@image latex screenshots/power_meter.png width=2cm

### 4. Radio sliding button example
Radio button/Slider with different on/off

@image html screenshots/radio_on_small.png    
@image latex screenshots/radio_on.png width=2cm    
@image html screenshots/radio_off_small.png    
@image latex screenshots/radio_off.png width=2cm
 
### 5. Boxed letters example
Highlight zero or more letters

@image html screenshots/boxed_letters_small.png
@image latex screenshots/boxed_letters.png width=3cm
 
## Widget functions

All the widgets are used in the same way. They are included as a part of a canvas callback function as 
installed by the `hpdftbl_set_canvas_cb()` and `hpdftbl_set_cell_canvas_cb()` functions. The callback function
itself has to follow the canvas callback signature which is defined as

```c
typedef void (*hpdftbl_canvas_callback_t)(HPDF_Doc, HPDF_Page, void *, size_t, size_t, HPDF_REAL, HPDF_REAL, HPDF_REAL,
                                          HPDF_REAL);
```

and a typical example of a canvas callback function and it's installation would be

```c
void 
cb_draw_segment_hbar(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r,
                     size_t c, HPDF_REAL xpos, HPDF_REAL ypos,
                     HPDF_REAL width, HPDF_REAL height) 
{ ... }

...

hpdftbl_set_cell_canvas_cb(t, wrow, wcol, cb_draw_segment_hbar);
```

Each widget has its on function that should be included in the canvas callback to display and size the
widget. The different widgets has 
slightly different defining functions depending on what they display and are defined as follows.

### Segmented horizontal bar defining function

```c
void
hpdftbl_widget_segment_hbar(const HPDF_Doc doc, const HPDF_Page page,
                            const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                            const size_t num_segments, const HPDF_RGBColor on_color, const double val_percent,
                            const _Bool hide_val)
```

### Horizontal bar defining function

```c
void
hpdftbl_widget_hbar(const HPDF_Doc doc, const HPDF_Page page,
                    const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                    const HPDF_RGBColor color, const float val, const _Bool hide_val)
```

### Signal strength defining function

```c
void
hpdftbl_widget_strength_meter(const HPDF_Doc doc, const HPDF_Page page,
                              const HPDF_REAL xpos, const HPDF_REAL ypos, const HPDF_REAL width, const HPDF_REAL height,
                              const size_t num_segments, const HPDF_RGBColor on_color, const size_t num_on_segments) 
```

### Radio sliding button defining function

```c
void
hpdftbl_widget_slide_button(HPDF_Doc doc, HPDF_Page page,
                            HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, _Bool state)
```

### Boxed letters defining function

```c
void
hpdftbl_table_widget_letter_buttons(HPDF_Doc doc, HPDF_Page page,
                                    HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                                    const HPDF_RGBColor on_color, const HPDF_RGBColor off_color,
                                    const HPDF_RGBColor on_background, const HPDF_RGBColor off_background,
                                    const HPDF_REAL fsize,
                                    const char *letters, _Bool *state) 
```

## Usage

The widget function is included in either a table canvas callback or more commonly in a cell canvas callback.
Let's construct a basic example with a 1x2 table that shows a segmented horizontal bar indicating a fictive
battery charge level and signal strength meter as shown in the figure below

@image html screenshots/tut_ex14.png "tut_ex14.c"
@image latex screenshots/tut_ex14.png "tut_ex14.c" width=10cm

For this we start by constructing the callback for the battery display. In a real application the value
would probably be read from a database but here we just use a hard coded value

```c
cb_draw_battery_widget(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r,
                       size_t c, HPDF_REAL xpos, HPDF_REAL ypos,
                       HPDF_REAL width, HPDF_REAL height) {
    const HPDF_REAL segment_tot_width = width * 0.5;
    const HPDF_REAL segment_height = height / 3;
    const HPDF_REAL segment_xpos = xpos + 40;
    const HPDF_REAL segment_ypos = ypos + 4;
    const size_t num_segments = 10;
    const _Bool val_text_hide = FALSE; // Display the percentage
    const HPDF_RGBColor on_color = COLOR_DARK_GREEN;
    
    // This should in reality be retrieved programmatically (for example from a DB)
    const double val_percent = 0.4;
  

    hpdftbl_widget_segment_hbar(
            doc, page, segment_xpos, segment_ypos, segment_tot_width,
            segment_height, num_segments, on_color, val_percent, val_text_hide);
}
```
Some comments:

 - In the callback we get the bounding box for the cell as arguments
 - We adjust the position and height/width so that the widget is centered in the cell

The next callback is the signal strength widget and we construct that as follows

```c
void
cb_draw_signal_widget(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r,
                      size_t c, HPDF_REAL xpos, HPDF_REAL ypos,
                      HPDF_REAL width, HPDF_REAL height) {
    const HPDF_REAL wwidth = 35;
    const HPDF_REAL wheight = 20;
    const HPDF_REAL wxpos = xpos + 70;
    const HPDF_REAL wypos = ypos + 4;
    const size_t num_segments = 5;
    const HPDF_RGBColor on_color = COLOR_DARK_RED;

    // This should in reality be retrieved programmatically (for example from a DB)
    const size_t num_on_segments = 3;

    hpdftbl_widget_strength_meter(doc, page, wxpos, wypos, wwidth, wheight,
                                  num_segments, on_color, num_on_segments);
}

```

Some comments:

 - In the callback we get the bounding box for the cell as arguments
 - We adjust the position and height/width so that the widget is centered in the cell

With these callbacks it is now straightforward to construct the table with as follows
 
```c
void
create_table_ex14(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 2;
    const size_t num_cols = 2;

    char *table_title = "tut_ex14: 2x2 table widget callbacks";
    hpdftbl_t tbl = hpdftbl_create_title(num_rows, num_cols, table_title);
    hpdftbl_use_labels(tbl, TRUE);
    hpdftbl_use_labelgrid(tbl, TRUE);

    // Use one label callback for the entire table
    hpdftbl_set_label_cb(tbl, cb_labels);

    // Name in top left corner
    hpdftbl_set_cell_content_cb(tbl, 0, 0, cb_device_name);

    // Date in top right corner
    hpdftbl_set_cell_content_cb(tbl, 0, 1, cb_date);

    // Draw battery strength
    hpdftbl_set_cell_canvas_cb(tbl, 1, 0, cb_draw_battery_widget);

    // Draw signal strength
    hpdftbl_set_cell_canvas_cb(tbl, 1, 1, cb_draw_signal_widget);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(12);
    HPDF_REAL height = 0;  // Calculate height automatically

    // Stroke the table to the page
    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
}
```

Some comments:

 - For brevity, we have not shown the label and other content callback.
 - The complete code is available as @ref tut_ex14.c "tut_ex14.c"

