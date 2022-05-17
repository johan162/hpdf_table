# Using themes {#sec_themes}

A theme (or style theme) is a definition of the "look & feel" of a table. It doesn't affect the structure of the table
such as the size of the table or how many columns or rows a cell spans. It is practical shortcut
when many tables should be displayed in the same style. It allows the compact specification
of the table by applying a theme to the table instead of having to call multiple functions to
achieve the same thing. In addition, if the design should be changed there is only one place to
update instead of for each table.

A theme can also be serialized to and from a file/string buffer

A theme controls the following aspects of a table

- The content and label text style
- The header and title text style
- The inner and outer border style
- The usage (or not) of labels and whether the shorter label grind lines should be used
- If a header row should be used or not
- If a title should be used or not


if you have multiple table in a document it is possible to create a *table theme* which
consists of some core styling
of a table that can be reused.

@note By design any specific settings for individual cells is not saved in a theme as a theme
can be applied to any table regardless of the specific table structure.

All information for a theme is encapsulated in the hpdftbl_theme structure.

This structure can be set up manually and then applied to a table. However, the recommended way is to first
use the "theme getter" function to get the default theme and then modify this default theme as needed since
it allows you to only have to update the parts affected by a change.

The functions to work with a theme are :

| API                         | Description                                          |
|-----------------------------|------------------------------------------------------|
| hpdftbl_apply_theme()       | Apply the given theme to a table                     |
| hpdftbl_get_default_theme() | Get the default theme into a new allocated structure |
| hpdftbl_destroy_theme()     | Free the memory used by a theme                      |
| hpdftbl_get_theme()         | Extract a theme from specified table                 |  
| hpdftbl_theme_dump()        | Serialize a theme to a file                          |
| hpdftbl_theme_dumps()       | Serialize a theme to a string buffert                |
| hpdftbl_theme_load()        | Load a serialized theme from file                    |
| hpdftbl_theme_loads()       | Load a serialized theme from a string buffert        |


It is the responsibility of the user of the library to destroy the theme structure by ensuring that `hpdftbl_destroy_theme()` is called when a theme goes out of scope.

The default font styles for the default theme are shown in table 1.

| Style   | Font                   | Size | Color     | Background | Alignment | 
|---------|------------------------|------|-----------|------------|-----------|
| content | HPDF_FF_COURIER        | 10   | Black     | White      | Left      |
| label   | HPDF_FF_TIMES_ITALIC   | 9    | Dark gray | White      | Left      |
| header  | HPDF_FF_HELVETICA_BOLD | 10   | Black     | Light gray | Center    |
| title   | HPDF_FF_HELVETICA_BOLD | 11   | Black     | Light gray | Left      |

***Table 1:*** *Default font styles.*

&nbsp;

| Theme parameter      | Default value |
|----------------------|---------------|
| use_labels           | FALSE         |
| use_label_grid_style | FALSE         |
| use_header_row       | FALSE         |
| use_zebra            | FALSE         |

***Table 2:*** *Default table structure parameters.*

&nbsp;

| Border       | Color     | Width (pt) |
|--------------|-----------|------------|
| inner_border | Grey      | 0.7        |
| outer_grid   | Dark Grey | 1.0        |

***Table 3:*** *Default border parameters.* 

## Example of serializing theme and table

In tut_ex41.c an example ofmhow to read a theme and table back from their serialized 
representation can be found as also shown below.

```c
    hpdftbl_t tbl = calloc(1, sizeof (struct hpdftbl));
    hpdftbl_theme_t theme;

    if(0 == hpdftbl_load(tbl, "tests/tut_ex41.json")  ) {

        if(0 == hpdftbl_theme_load(&theme, "tests/tut41_theme.json")) {
            hpdftbl_apply_theme(tbl, &theme);
            hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
        } else {
            fprintf(stderr,"%s\n","Failed to load 'tests/default_theme.json'\n");
            exit(1);
        }
    } else {
        fprintf(stderr,"%s\n","Failed to load 'tests/tut_ex41.json'\n");
        exit(1);
    }
```

