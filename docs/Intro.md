# Introduction to hpdftbl

## What is this?
The Haru PDF library is a great way to programmatically produce PDFs from programs. However, in many instances the best way to present data produced is as a grid (or table). To manually create and setup such tables
int Haru PDF library is of course possible but only painstakingly so.

This C/C++ library `libhpdftbl` will facilitate the creation of tables with the Haru PDF library as well as handling the pesky issue of character conversion needed between UTF-8 and the internal standard used by PDF and Lib Haru. In addition to mere normal table the library also supports the creation of forms where each cell has a label similar to "formal" paper forms. This is a great way to present structured data from a DB.
 
This library provides a flexible abstraction for creating advanced tables with
a model-view-controller like setup. This allows an easy way to separate the layout
of the table from the actual data in the table.

## Features
- Supports both C/C++
- Suports both OSX/Linux builds and theire different dynamic library variants
- Fully supports UTF-8 with automatic conversion to PDF character encoding
- Supports multple paradigms for creating and populating tables
    - Directly store value in table cell
    - Create a data structure (2D-Array) with all data to be set at once
    - Use callback populating functions with identifying tags for each table cell
- Options to use labels in table cell to create forms
- Support for predefined widgets in table cell to illustrate values
- Complete control of background color, fonts, and frame colors
- Possible to use table themes that provides pre-defined look-and-feel for table
- Both dynamic and static library provided
- Last but not least; extensive documentation and almost guaranteed to be bug free after beeing tested in production for over 7 years!

## Some Examples

> **Note:** All code examples can be found in the "`examples/`" directory and are thoroughly introduced over the following chapters.

### Example 1 - Plain table with cell labels  

@image html screenshots/tut_ex02_1.png 

@image latex screenshots/tut_ex02_1.png width=15cm

### Example 2 - Table with cell labels

@image html screenshots/tblex01.png 

@image latex screenshots/tblex01.png width=15cm

### Example 2 - Plain table with row/column spanning and table title

@image html screenshots/tblex02.png 
@image latex screenshots/tblex02.png width=15cm

### Example 3 - Table with labels and cell widgets

@image html screenshots/tblex03.png width=15cm
@image latex screenshots/tblex03.png width=15cm

