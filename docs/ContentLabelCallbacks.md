# Content and label callbacks

In the "[Getting started](GettingStarted.md)" chapter we discussed the preferred way to specify data and labels in table using data arrays. This is a very good way to populate a table in the cases the data is fairly static. 

For data that is more dynamic and determined at runtime it is of course possible to construct the data array but the table library have one better way to do this and that is to set up label and content callbacks.


## Introducing content callback functions
**Content callbacks** are functions that are called by the table library for each cell and returns a string which is used as tne data to be displayed. The signature for a cell callback is defined by the type `hpdftbl_content_callback_t` which is a pointer to a function defined as:

```C
 typedef char * (*hpdftbl_content_callback_t)(void *, size_t, size_t);
```

To understand this lets start with a callback function that follows this signature.

```c
char *
my_cell_cb(void *tag, size_t row, size_t col) { ... }
```

The parameters in the callback are

1. **`tag`**: Since a callback sometimes must know from what table or in what circumstances it is called it is possible to add a "tag" to ech table. This could be something as simple as pointer to a numeric identifier that uniquely identifies the table or perhaps a pointer to some function that retrieves data for this particular table. The `tag` for a table is specified with the `hpdftbl_set_tag()` function.
 When the callback is made this table tag is provided as the first argument. 


2. **`row`**: The cell row


3. **`col`**: The cell column


It is possible to specify a callback to adjust content, style, and labels. 
A callback function can be specified either for both the entire table or for individual cells. 
The API to specify these callbacks are:

1. `hpdftbl_set_content_cb()`:  
   Specify a content callback for the entire table.   


2. `hpdftbl_set_content_style_cb()`:  
   Specify a style callback for the entire table.   


3. `hpdftbl_set_label_cb()`:   
   Specify a label callback for the entire table.   

  
4. `hpdftbl_set_cell_content_cb()`:     
    Specify callback for an individual cell. A cell callback will override a potential table callback.  


6. `hpdftbl_set_cell_content_style_cb()`:   
    Specify a style callback for an individual cell. A cell callback will override a potential table callback.   


7. `hpdftbl_set_canvas_cb()`: 
    This is an advanced callback to allow for low level painting directly on the canvas that is the cell area. The arguments to the callback is different as it includes the bounding-box for th cell area. We will not further discuss this.

@note **Returned content string**. When a content string is added in the table it is added as a copy of the string pointed to by the returned string pointer from the callback function. It is therefore perfectly possible to have a static allocated buffer in the callback function that is used to construct the content. When the table is destroyed using `hpdftbl_destroy()` all used memory will be freed.


## A content callback example

Let's now construct a simple example where the content and the labels are specified with callbacks. 

We will create callbacks that will add a date string to the top left cell and just som dummy content in the rest of the cells. We could do this in two ways. 

1. Add a generic table callback for all cells and then in that callback check if the row and column is (0,0) i.e. top-left and in that case create a date.
2. Add a generic table callback for all cells and then add a specific cell callback with the date for the (0,0) cell.

To illustrate both methods we will use method 1 for the labels and method 2 for the content.

Let's first create the three callback functions we need

```c
static char * cb_date(void *tag, size_t r, size_t c) {
    static char buf[64];
    time_t t = time(NULL);
    ctime_r(&t, buf);
    return buf;
}

static char * cb_content(void *tag, size_t r, size_t c) {
    static char buf[32];
    snprintf(buf, sizeof buf, "Content %02zu x %02zu", r, c);
    return buf;
}

static char * cb_labels(void *tag, size_t r, size_t c) {
    static char buf[32];
    if (0==r && 0==c) { // Top-left cell
        snprintf(buf, sizeof buf, "Date:");
    } else {
        snprintf(buf, sizeof buf, "Label %zux%zu:", r, c);
    }
    return buf;
}
```

We note that we ignore the tag argument. Since we only have one table there is no need to use a tag to different from which table a callback comes.

For the table structure we will re-use our previous example and create a 2x2 table, and we get the following table creation code:

```c
void
create_table_ex06(HPDF_Doc pdf_doc, HPDF_Page pdf_page) {
    const size_t num_rows = 2;
    const size_t num_cols = 2;

    char *table_title = "tut_ex06: 2x2 table with callbacks";
    hpdftbl_t tbl = hpdftbl_create_title(num_rows, num_cols, table_title);
    hpdftbl_use_labels(tbl, TRUE);
    hpdftbl_use_labelgrid(tbl, TRUE);

    hpdftbl_set_label_cb(tbl, cb_labels);
    hpdftbl_set_content_cb(tbl, cb_content);
    hpdftbl_set_cell_content_cb(tbl, 0, 0, cb_date);

    HPDF_REAL xpos = hpdftbl_cm2dpi(1);
    HPDF_REAL ypos = hpdftbl_cm2dpi(A4PAGE_HEIGHT_CM - 1);
    HPDF_REAL width = hpdftbl_cm2dpi(12);
    HPDF_REAL height = 0;  // Calculate height automatically
    
    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
}
```
([`tut_ex05.c`](../examples/tut_ex05.c))

Running this example gives the result shown in **Figure 7.**  below

![tut_ex06.c](screenshots/tut_ex06.png)   

***Figure 7:*** *Using callbacks to populate the table and labels.*




