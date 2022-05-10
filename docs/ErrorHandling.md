# Error handling

All library function will return an error code < 0 and also set a global variable to a specific error code that can later be read by an error handler. In order to translate the error to a human-readable string the function `hpdftbl_get_last_errcode()` can be used as the following error handling snippet exemplified by a call to `hpdftbl_set_colwidth_percent()`

```c
if( hpdftbl_set_colwidth_percent(tbl, 5, 110) ) {
    // This is an error
    char *err_str;
    int err_code, r, c;
    err_code=hpdftbl_get_last_errcode(&err_str, &r, &c);
    if( err_code ) {
        printf("*ERROR*: \"%s\" at cell (%d, %d)",err_str,r,c);
        exit(1);
    }
}
```

As can be seen from the snippet above it would yield quite long winding error handling if one where to check every single library call. 
Instead, there is the option of installing an error handler that would be called in the event of an error.

The table error handle has the signature

```c
void hpdftbl_error_handler_t)(hpdftbl_t tbl, int r, int c, int err)
```

Where the arguments are

| Argument | Description                                                                                                                           |
|----------|---------------------------------------------------------------------------------------------------------------------------------------|
| `tbl`    | The table in where the error happened. <br/> **Note:** This might be `NULL` since not all errors happen within the context of a table |
| `r`, `c` | The row and column if the error happens in a specified cell, otherwise these will be (-1,-1)                                          |
| `err`    | The internal error code. This si always a negative number.                                                                            |


The error handler is set with the `hpdftbl_set_errhandler()` method. An example of a very simple error handle is:

```c
void
my_table_error_handler(hpdftbl_t t, int r, int c, int err) {
    if( r>-1 && c>-1 ) {
        fprintf(stderr, "*** Table Error: [%d] \"%s\" at cell (%d, %d)\n", err, hpdftbl_get_errstr(err), r, c);
    } else {
        fprintf(stderr, "*** Table Error: [%d] \"%s\" \n", err, hpdftbl_get_errstr(err));
    }
    exit(1);
}
```

In the above error handler we have made use of the utility function `hpdftbl_get_errstr()` that translates the
internal error code to a human-readable string. 

In fact this exact error handler is available as a convenience in the library under the name `hpdftbl_default_table_error_handler` so to use this trivial error handler just add the following line to your code

```c
hpdftbl_set_errhandler(hpdftbl_default_table_error_handler);
```

More advanced error handler must be written for the particular application they are to be used in. 

@note A common way to extend the error handling is to log the errors to syslog. When the library is used on OSX from 11.0 and onwards it should be remembered that OSX is broken by design as far as syslog logging is concerned. Apple in its wisdom introduced "Unified logging" which breaks the `syslog()` function and no logging is ever produced in the filesystem directly (i.e. to `/var/log/system.log`).   
Instead, the only way to view the logs is by using the utility `log`. So in order to view the log from a particular application the following command has to be given  
`log stream --info --debug --predicate 'sender == "APPLICATION_NAME" --style syslog`

## Translating HPDF error codes

The standard error handler for the plain HPDF library is specified when a new document is created, for example as'

```c
...
pdf_doc = HPDF_New(error_handler, NULL);
HPDF_SetCompressionMode(pdf_doc, HPDF_COMP_ALL);
...
```

The error handler signature is defined by Haru PDF library as

```c
static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data);
```

It is then up to the application code to decide how to handle the error. To simplify the handling of core HPDF error the
library also offer a convenience function to translate the Haru library error code into a human-readable string. This function 
is

```c
const char *
hpdftbl_hpdf_get_errstr(const HPDF_STATUS err_code) 
```

and is used in the error handler in all the examples.

## Example of setting up error handler

The following table creation code have a deliberate error in that it tries to assign a total column width of more than
100% which of course isn't possible.

@dontinclude tut_ex10.c

@skip void
@until }


This is available in the example directory as @ref tut_ex10.c "tut_ex10.c". 

If we simulate a "typo" and add a deliberate error by making the column widths larger than 100% by writing

```c
    hpdftbl_set_colwidth_percent(tbl, 0, 80);
    hpdftbl_set_colwidth_percent(tbl, 1, 30);
```

When this code is then executed the following will be printed to standard error and the process will be stopped.

```
*** Table Error: [-12] "Total column width exceeds 100%"
```

