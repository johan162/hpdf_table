# API Overview


## Table creation related functions

These calls relate to the creation, destruction and stroking of the table on the PDF page.

 - hpdftbl_create()
   *Create a handle for a new table.* 


 - hpdftbl_create_title()
   *Create a handle for a new  with a title.*


 - hpdftbl_destroy()
   *Destroy (return) memory used by a table.*


 - hpdftbl_stroke()
   *Stroke a table on the specified PDF page.*


 - hpdftbl_stroke_from_data()
   *Construct and stroke a table defined as a data structure.*


 - hpdftbl_get_last_auto_height()
   *Get the height of the last table stroked.*


 - hpdftbl_set_anchor_top_left()
   *Switch the anchor point of a table between top left and bottom left corner.*


 - hpdftbl_get_anchor_top_left()
   *Get the current anchor point of table.*


## Table error handling

 - hpdftbl_set_errhandler()
   *Set and error handler callback.*


 - hpdftbl_get_errstr()
   *Translate an error code into a human readable string.*


 - hpdftbl_get_last_errcode()
   *Get the error code from last error raised*


 - hpdftbl_default_table_error_handler()
   *A default error handler callback that print error to stdout and quits the process.*


## Theme handling methods

Themes is a technique to easier specify the look and feel to be re-used for multiple tables.

 - hpdftbl_apply_theme()
   *Use the specified theme for look & feel of table*

 - hpdftbl_get_default_theme()
   *Get the default theme. A good way to start and then modify.*

 - hpdftbl_destroy_theme()
   *Free all memory structures used by a theme.*


## Table layout adjusting functions

Adjusting the structure of the table (apart from number of rows and columns)

 - hpdftbl_set_colwidth_percent()
   *Set the column width as a percentage of the entire table width.*

 - hpdftbl_set_min_rowheight()
   *Specify the minimum row height in points*

 - hpdftbl_set_bottom_vmargin_factor()
   *Specify the bottom margin for content as a fraction of the specified fontsize*

 - hpdftbl_set_cellspan()
   *Define a cell to span multiple rows and columns.*

 - hpdftbl_clear_spanning()
   *Remove all previous set cell spanning.*


## Table style modifying functions

These functions are all about look and feel of the table.

 - hpdftbl_use_labels()
   *Use labels in each cell.*

 - hpdftbl_use_labelgrid()
   *Use shorter left gridlines that only goes down and cover labels* 

 - hpdftbl_set_background()
   *Set cell background color.*

 - hpdftbl_set_outer_grid_style()
   *Set style of the table outer grid lines.*

 - hpdftbl_set_inner_grid_style()
   *Set the style of both vertical and horizontal inner grid lines.*

 - hpdftbl_set_inner_vgrid_style()
   *Set the style of table inner vertical grid lines.*

 -  hpdftbl_set_inner_hgrid_style()
    *Set the style of table inner horizontal grid lines.*

 - hpdftbl_set_header_style()
   *Set the style for the table header row.*

 - hpdftbl_set_header_halign()
   *Set the horizontal alignment of the header row.*

 - hpdftbl_set_title_halign()
   *Set horizontal alignment for title.*

 - hpdftbl_use_header()
   *Make the top row a header.*

 - hpdftbl_set_label_style()
   *Set style for cell labels.*

 - hpdftbl_set_row_content_style()
   *Set the content style for an entire row.*

 - hpdftbl_set_col_content_style()
   *Set the content style for an entire column.*

 - hpdftbl_set_content_style()
   *Set the content style for the entire table.*

 - hpdftbl_set_cell_content_style()
   *Set the style for specified cell. This overrides andy style on the table level.*

 - hpdftbl_set_title_style()
   *Set the style for the table title.*

## Content handling

Content in a table can be specified in three ways

 1. Manually for each cell by calling the hpdftbl_set_cell() function
 2. In one go by creating a 1D data array for all cell
 3. Creating a callback which returns the wanted value

 - hpdftbl_set_cell()
   *Set content text in specified cell.*

 - hpdftbl_set_tag()
   *Set the table tag. The tag is a `void *` an can be anything. The tag is the first parameter of all callbacks.*

 - hpdftbl_set_title()
   *Set title text of table.*

 - hpdftbl_set_labels()
   *Set label texts for the table from 1D-data array.*

 - hpdftbl_set_content()
   *Set the content text for the entire table from a 1D-data array.* 


## Callback handling

Callbacks can be specified on both table but also on cell level. The simple rule is that
if a cell has a callback that is used, otherwise the table callback is used.

 - hpdftbl_set_content_cb()
   *Set table content callback.*

 - hpdftbl_set_cell_content_cb()
   *Set cell content callback.*

 - hpdftbl_set_cell_content_style_cb()
   *Set the cell style callback.*

 - hpdftbl_set_content_style_cb()
   *Set the table style callback.*

 - hpdftbl_set_label_cb()
   *Set table label callback.*

 - hpdftbl_set_cell_label_cb()
   *Set the cell label callback.*
   
 - hpdftbl_set_canvas_cb()
   *Set table canvas callback.*

 - hpdftbl_set_cell_canvas_cb()
   *Set the cell canvas callback.*

## Dynamic (late binding) callback handling

These are callbacks which set a function at runtime to be used as callback. This is useful when
specifying the table for example as a structure stored in a database or in a file. The callback
function is then specified as a string (the name of the callback function which is then resolved 
at runtime.

 - hpdftbl_set_dlhandle()  
   *Set the dynamic library load handle as returned by dlopen() or one of the predefined handles. By default, the handle is set to the predefined handle `RTLD_DEFAULT`. See `man dlsym`. This handle will control how the search for the name of the function will 
    be conducted. The default will find any functions defined in any images linked
    and any libraries linked at compile time. It will **not** find functions defined
    in libraries that are dynamically loaded. In that case you shoult specify the handle
    returned by `dlopen()`.

 - hpdftbl_set_content_dyncb()
   *Set the name for the table content callback.*

 - hpdftbl_set_cell_content_dyncb()
   *Set the name for the cell content callback.*

 - hpdftbl_set_label_dyncb()

 - hpdftbl_set_cell_label_dyncb()
   *Set the name for the cell label content callback.*

 - hpdftbl_set_content_style_dyncb()
   *Set the name for the table content style callback.*

 - hpdftbl_set_cell_content_style_dyncb()
   *Set the name for the cell content style callback.*

 - hpdftbl_set_cell_canvas_dyncb()
   *Set the name for the cell canvas callback.*

## Text encoding

 - hpdftbl_set_text_encoding()
   *Specify text encodation to use.*

 - hpdftbl_encoding_text_out()
   *Stroke a text with current encoding.*


## Misc utility function

 - HPDF_RoundedCornerRectangle()
   *Draw a rectangle with rounded corners.*

 - hpdftbl_stroke_grid()
   *Stroke a grid on the PDF page (entire page). This is useful to position the table on a page. The grid is measured in points i.e. postscript natural units.


