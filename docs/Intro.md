# Overview

## What is this?
The Haru PDF library is a great way to programmatically produce PDFs from programs. However, in many instances the best way to present data produced is as a grid (or table). To manually create and setup such tables
int Haru PDF library is of course possible but only painstakingly so.

This C/C++ library `libhpdftbl` will facilitate the creation of tables with the Haru PDF library as well as handling the pesky issue of character conversion needed between UTF-8 and the internal standard used by PDF and Lib Haru. In addition to mere normal table the library also supports the creation of forms where each cell has a label similar to "formal" paper forms. This is a great way to present structured data from a DB.
 
This library provides a flexible abstraction for creating advanced tables with
a model-view-controller like setup. This allows an easy way to separate the layout
of the table from the actual data in the table.

## Features
- Supports both OSX/Linux builds and their different dynamic library variants
- Fully supports UTF-8 with automatic conversion to PDF character encoding
- Supports multiple paradigms for creating and populating tables
  - Directly store value in table cell
  - Create a data structure (2D-Array) with all data to be set at once
  - Use callback populating functions with identifying tags for each table cell
- Allow table to be serialized to JSON and read back
- Options ot use labels in table cell to create forms
- Row and column spanning of cells
- Support for predefined widgets in table cell to illustrate values
- Complete control of background color, fonts, and frame colors
- Possible to use table themes that provides pre-defined look-and-feel for table
- Allows theme to be serialized to JSON and read beck
- Both dynamic and static library provided
- Last but not least; extensive [documentation](https://johan162.github.io/libhpdftbl/html/index.html) which is also included in the distribution in both
  PDF and HTML format.

## Some Examples

@note All code examples can be found in the `examples/` directory and in the 
[Examples](examples.html) section in the documentations.


### Example 1 - Plain table with header  

@ref tut_ex02_1.c "tut_ex02_1.c"

@image html screenshots/tut_ex02_1.png
@image latex screenshots/tut_ex02_1.png width=15cm

### Example 2 - Table with cell labels

@ref example01.c "example01.c"

@image html screenshots/tblex01.png
@image latex screenshots/tblex01.png width=15cm

### Example 2 - Plain table with row/column spanning and table title

@ref example01.c "example01.c"

@image html screenshots/tblex02.png 
@image latex screenshots/tblex02.png width=15cm

### Example 3 - Table with labels and cell widgets

@ref example01.c "example01.c"

@image html screenshots/tblex03.png 
@image latex screenshots/tblex03.png width=15cm



