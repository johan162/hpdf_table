@mainpage Libhpdftbl

# libhpdftbl

## What is this?
The Haru PDF library is a great way to programmatically produce PDFs from programs. However, in many instances the best way to present data produced is as a grid (or table). To manually create and setup such tables
int Haru PDF library is possible but only painstakingly so. 

This C/C++ library `libhpdftbl` will facilitate the creation of tables with the Haru PDF library as well as handling the pesky issue of character conversion needed between UTF-8 and the internal standard used by PDF and Lib Haru.

This library provides a flexible abstraction for creating advanced tables with
a model-view-controller setup. This allows an easy way to separate the layout
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

## Library documentation

The documentation is built using Doxygen and available as HTML or PDF.

- HTML: [libhpdftbl](https://johan162.github.io/libhpdftbl/html/index.html)
- PDF:  [libhpdftbl](https://johan162.github.io/libhpdftbl/lihpdftbl_manual_v1.0.0-beta.pdf)

## Some Examples

> **Note:** Several more examples can be found in the "`examples/`" directory and in the
> documentation "Examples" section.

### Example 1 - Plain table with headers
![example1](docs/screenshots/tut_ex02_1.png)

### Example 2 - Plain table with cell labels
![example1](docs/screenshots/tblex01.png)

### Example 3 - Plain table with row/column spanning
![example2](docs/screenshots/tblex02.png)

### Example 4 - Table with cell widgets
![example3](docs/screenshots/tblex03.png)


<!-- 
EOF
-->
