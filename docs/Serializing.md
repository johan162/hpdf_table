# Serializing table data structures

A table and heme can be serialized to a JSON structure in a 
string or file and then read back. The 
serialization is a complete representation of the table and the theme. 
However, there is one
crucial caveat. If any callback functions is used in the table 
they can not be serialized since
they are represented by the address to the specific function.

However, this is a situation where a dynamic callback function can be used.
They way this works is that the callback function is specified by name 
with one if the dynamic callback functions (e.g. hpdftbl_set_content_dyncb() ).
Such a callback will then be serialized as the name of the function. 

When the table is de-serialized back into a table using the
function hpdftbl_load() the functions must be available in some 
linked images to be resolved.

Please note that the table needs to be stroked (via for example hpdftbl_stroke())
before it can be serialized since a number of calculations of internal
positions are not calculated until one of the stroke functions are called.

@note Some discussion can be held whether Yaml or Json should be used
as serializing format. The reason for choosing Json was primary based on
a) easier to manually manipulate a file where invisible spaces doesn't have 
grammatical meaning. b) existence of efficient libraries


## Serializing a table to file

After the table have been stroked it can be saved to a file as the
following snippet shows.

```c
    hpdftbl_stroke(pdf_doc, pdf_page, tbl, xpos, ypos, width, height);
    hpdftbl_dump(tbl, "table_serialized.json");
```

The snippet above will write a JSON representation of the table to the file
`table_serialized.json`. The full path is given and it is an error if some
intermediate directory does not exist.

An example of a json file can be found here: [tut_ex40.json](tut_ex40_8json-example.html)  

## Serializing a table to a string buffer

This is done with the hpdftbl_dumps() function as the following example shows.

```c
    const size_t buffsize=100*1024;
    char *sbuff=calloc(buffsize, sizeof(char));

    hpdftbl_dumps(theme, sbuff, buffsize);
    fprintf(stdout,"%s\n",sbuff);
    free(sbuff);
```


## Reading back a serialized table

The following snippet shows how the previously serialzed 
table can be read back and stroked to a PDF file

```c
    if( 0 == hpdftbl_load(&tbl, "table_serialized.json")  ) {
        hpdftbl_stroke_pos(pdf_doc, pdf_page, tbl);
    }
```
It can be noted that we use the alternative stroke function hpdftbl_stroke_pos() 
which is used to stroke a table when the position is set within the table
itself and don't need to be specified as arguments to the stroke functio
as is necessary with hpdftbl_stroke().

@note An error check should always be performed when reading back a table since 
it is possible that the data have been corrupted.

## Serializing a theme to a file

A theme can be serialized with the help of hpdftbl_theme_dump() as the following
example that serializes the default theme

```c
    hpdftbl_theme_t *theme = hpdftbl_get_default_theme();
    hpdftbl_theme_dump(theme,"out/default_theme.json");
```

## Serializing a theme to a string buffer

This is done with the hpdftbl_theme_dumps() function as the following example shows

```c
    const size_t buffsize=2*1024;
    char *sbuff=calloc(buffsize, sizeof(char));

    hpdftbl_theme_t *theme = hpdftbl_get_default_theme();
    hpdftbl_theme_dumps(theme, sbuff, buffsize);
    fprintf(stdout,"%s\n",sbuff);
    free(sbuff);
```

## Reading back a serialized theme

A theme can be read back with the hpdftbl_theme_load() and hpdftbl_theme_loads() functions.

## Example of reading back serialized theme and table

The following shows an example of creating a table with a theme where both theme and table 
are read back from previous serialized representation.

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
