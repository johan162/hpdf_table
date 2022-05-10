# Content and label callbacks

In the "[Getting started](GettingStarted.md)" chapter we discussed the preferred way to specify data and labels 
in table using data arrays. This is a very good way to populate a table in the cases the data is fairly static. 

For data that is more dynamic and determined at runtime it is of course possible to construct the data array 
but the table library have one better way to do this and that is to set up label and content callbacks.


## Introducing content callback functions
**Content callbacks** are functions that are called by the library for each cell and returns a 
string which is used as the data to be displayed. The signature for a cell callback is defined by 
the type `hpdftbl_content_callback_t` which is a pointer to a function defined as:

```C
 typedef char * (*hpdftbl_content_callback_t)(void *, size_t, size_t);
```

To understand this lets start defining a callback function that follows this signature.

```c
char *
my_cell_cb(void *tag, size_t row, size_t col) { ... }
```

The parameters in the callback are

| Parameter | Description                                                                                                                                                                                                                                                                                                                         |
|-----------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `tag`     | Since a callback sometimes must know from what table or in what circumstances it is called it is possible to add a "tag" to ech table. This could be something as simple as pointer to a numeric identifier that uniquely identifies the table or perhaps a pointer to some function that retrieves data for this particular table. |
| `row`     |  The cell row                                                                                                                                                                                                                                                                                                                              |
| `col`     |  The cell column                                                                                                                                                                                                                                                                                                                              |
    

It is possible to specify a callback to adjust content, style, and labels. 
A callback function can be specified to be used for every cell in the table 
or only for a specific cell. This can also be mixed in order to have, for example, one generic callback
for most cells and have a different callback for a specific cell. Any callback set for a cell
will override the callback set for the table

The API to specify these callbacks are:

| API                                   | Description                                                                                                                                                                                                                                           |
|---------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `hpdftbl_set_content_cb()`            | Specify a content callback for the entire table.                                                                                                                                                                                                      |
| `hpdftbl_set_content_style_cb()`      | Specify a style callback for the entire table.                                                                                                                                                                                                        |
| `hpdftbl_set_label_cb()`              | Specify a label callback for the entire table.                                                                                                                                                                                                        |
| `hpdftbl_set_cell_content_cb()`       | Specify callback for an individual cell. A cell callback will override<br/> a potential table callback.                                                                                                                                               |
| `hpdftbl_set_cell_content_style_cb()` | Specify a style callback for an individual cell. A cell callback will override<br/> a potential table callback.                                                                                                                                       |
| `hpdftbl_set_canvas_cb()`             | This is an advanced callback to allow for low level painting directly on the <br/>canvas that is the cell area arguments to the callback is different as it <br/> includes the bounding-box for th cell area. We will not further discuss this. |



@note **Returned content string**. The string pointer returned from a callback is never stored in the table. 
only printed. It is therefore perfectly possible to have a static allocated buffer in the callback function that is 
used to construct the content and returned from the callback.


## A content callback example

Let's now construct a simple example where the content and the labels are specified with callbacks. 

We will create callbacks that will add a date string to the top left cell and just som dummy content in the rest of the cells. 
We could do this in two ways. 

1. Add a generic table callback for all cells and then in that callback check if the row and column is (0,0) 
   i.e. top-left and in that case create a date.
2. Add a generic table callback for all cells and then add a specific cell callback with the date for the (0,0) cell.

To illustrate both methods we will use method 1 for the labels and method 2 for the content.

Let's first create the date callback functions we need to add a date in the top left corner of the
table that reflects the current date and time.

```c
static char *
cb_date(void *tag, size_t r, size_t c) {
    static char buf[64];
    time_t t = time(NULL);
    ctime_r(&t, buf);
    return buf;
}
```

This would be sufficient for normal usage. However, the source code in @ref tut_ex06.c that illustrates callbacks is 
slightly different:

@dontinclude tut_ex06.c

@skip static
@until }
@until }
@until }


The reason for this is that all these examples also serve as unit tests for the library. The way the unit tests 
work is by comparing the output from all these examples with stored, manually checked "correct" versions of the
output. Since any date changes will make the file different we must make the dates a known value whe the 
examples are run as unit teets. This we know when the flag `run_as_unit_test` is true and in that case
a "dummy" static date is used.

The content and label functions can then be written as follows

```c
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

We note that we ignore the tag argument. Since we only have one table there is no need to use a tag to different 
from which table the callback comes.

For the table structure we will re-use our previous example and create a 2x2 table, and we get the 
following table creation code:

@dontinclude tut_ex06.c

@skip ex06
@until }

([tut_ex06.c](../examples/tut_ex06.c))

Running this example gives the result shown in **Figure 7.**  below

![tut_ex06.c](screenshots/tut_ex06.png)   

***Figure 7:*** *Using callbacks to populate the table and labels.*




