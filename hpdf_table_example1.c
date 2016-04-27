/*
 * Example on how to use the HPDF_Table module to facilitate the creation of structured tables
 * with Haru PF library.
 * You need to link with libraries:
 * - math (i.e. "-lm")
 * - libhpdf-2 (of course)
 * - libiconv
 *
 * So on OS X Compile this would compile with:
 *
 *  gcc --std=c99 -lm /usr/local/Cellar/libharu/2.2.1_1/lib/libhpdf-2.2.1.dylib /usr/lib/libiconv.2.dylib hpdf_*.c
 *
 *  Adjust as needed for other environments
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !(defined _WIN32 || defined __WIN32__)
#include <unistd.h>
#endif
#include <math.h>
#include <setjmp.h>
#include <hpdf.h>
#include <time.h>
#if !(defined _WIN32 || defined __WIN32__)
#include <sys/utsname.h>
#endif

// These two includes should always be used
#include "hpdf_table.h"
#include "hpdf_errstr.h"

// The output after running the program will be written to this file
#ifdef _WIN32
#define OUTPUT_FILE "hpdf_table.pdf"
#else
#define OUTPUT_FILE "/tmp/hpdf_table.pdf"
#endif
#define TRUE 1
#define FALSE 0

// For simulated exception handling
jmp_buf env;

// Global handlers to the HPDF document and page
HPDF_Doc pdf_doc;
HPDF_Page pdf_page;

// We use some dummy data to populate the tables
#define MAX_NUM_ROWS 10
#define MAX_NUM_COLS 10

char *labels[MAX_NUM_ROWS*MAX_NUM_COLS];
char *content[MAX_NUM_ROWS*MAX_NUM_COLS];

// Create two arrays with dummy data to populate the tables
void
setup_dummy_data(void) {
    char buff[255];
    size_t cnt=0;
     for (size_t r = 0; r < MAX_NUM_ROWS; r++) {
        for (size_t c = 0; c < MAX_NUM_COLS; c++) {

#if (defined _WIN32 || defined __WIN32__)
			sprintf(buff, "Label %i:", cnt);
			labels[cnt] = _strdup(buff);
			sprintf(buff, "Content %i", cnt);
			content[cnt] = _strdup(buff);
#else
			snprintf(buff,sizeof(buff),"Label %zu:",cnt);
			labels[cnt] = strdup(buff);
			snprintf(buff, sizeof(buff), "Content %zu", cnt);
			content[cnt] = strdup(buff);
#endif
            cnt++;
        }
    }
}

#ifndef _MSC_VER
// Silent gcc about unused "arg"in the widget functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// A standard hpdf error handler which also translates the hpdf error code to a human
// readable string
static void
error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data) {
    fprintf (stderr, "*** PDF ERROR: \"%s\", [0x%04X : %d]\n", hpdf_errstr(error_no), (unsigned int) error_no, (int) detail_no);
    longjmp (env, 1);
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

// Setup a PDF document with one page
static void
add_a4page(void) {
    pdf_page = HPDF_AddPage (pdf_doc);
    HPDF_Page_SetSize (pdf_page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
}

// Stroke the generated PDF to a fil
static void
stroke_page_tofile(void) {
    if( HPDF_OK != HPDF_SaveToFile (pdf_doc, OUTPUT_FILE) ) {
        fprintf(stderr,"ERROR: Cannot save to file!");
    }
    HPDF_Free (pdf_doc);
}

/**
 * Table 1 example - Basic table with default settings
 * A table
 */
void
ex_tbl1(void) {
    int num_rows=5;
    int num_cols=4;
    char *table_title="Example 1: Basic table with default theme";
    hpdf_table_t t = hpdf_table_create(num_rows,num_cols,table_title);

    hpdf_table_set_content(t,content);
    hpdf_table_set_labels(t,labels);

    HPDF_REAL xpos=100;
    HPDF_REAL ypos=630;
    HPDF_REAL width=400;
    HPDF_REAL height=0; // Calculate height automatically
    hpdf_table_stroke(pdf_doc,pdf_page,t,xpos,ypos,width,height);

}

/**
 * Table 2 example - Basic table with almost default settings
 * A table
 */
void
ex_tbl2(void) {
    int num_rows=5;
    int num_cols=4;
    char *table_title="Example 2: Basic table with adjusted font styles";
    hpdf_table_t t = hpdf_table_create(num_rows,num_cols,table_title);

    // Use a red title and center the text
    HPDF_RGBColor color = {0.6f,0,0};
    HPDF_RGBColor background = {0.9f,1.0f,0.9f};
    hpdf_table_set_title_style(t,HPDF_FF_HELVETICA_BOLD,14,color,background);
    hpdf_table_set_title_halign(t,CENTER);

    // Use bold font for content. Use the C99 way to specify constant structure constants
#ifdef	__cplusplus
	hpdf_table_set_content_style(t, HPDF_FF_COURIER_BOLD, 10, { 0.1f, 0.1f, 0.1f }, { 1.0, 1.0, 0.9f });
#else
	hpdf_table_set_content_style(t,HPDF_FF_COURIER_BOLD,10,(HPDF_RGBColor){0.1f,0.1f,0.1f},(HPDF_RGBColor){1.0,1.0,0.9f});
#endif
    hpdf_table_set_content(t,content);
    hpdf_table_set_labels(t,labels);

    HPDF_REAL xpos=100;
    HPDF_REAL ypos=630;
    HPDF_REAL width=400;
    HPDF_REAL height=0; // Calculate height automatically
    hpdf_table_stroke(pdf_doc,pdf_page,t,xpos,ypos,width,height);

}

/**
 * Table 3 example - Use of row and column spanning, full grid and a header row
 */
void
ex_tbl3(void) {
    int num_rows=9;
    int num_cols=4;
    char *table_title="Example 3: Table cell spannings and full grid and header";
    hpdf_table_t t = hpdf_table_create(num_rows,num_cols,table_title);

    // Use a red title and center the text
    HPDF_RGBColor color = {0.6f,0,0};
    HPDF_RGBColor background = {0.9f,1.0,0.9f};
    hpdf_table_set_title_style(t,HPDF_FF_HELVETICA_BOLD,14,color,background);
    hpdf_table_set_title_halign(t,CENTER);

    // Use specially formatted header row
    hpdf_table_use_header(t,TRUE);

    // Use full grid and not just the short labelgrid
    hpdf_table_use_labelgrid(t,FALSE);


    // Use bold font for content. Use the C99 way to specify constant structure constants
#ifdef	__cplusplus
	hpdf_table_set_content_style(t, HPDF_FF_COURIER_BOLD, 10, { 0.1f, 0.1f, 0.1f }, { 1.0, 1.0, 1.0 });
#else
	hpdf_table_set_content_style(t,HPDF_FF_COURIER_BOLD,10,(HPDF_RGBColor){0.1f,0.1f,0.1f},(HPDF_RGBColor){1.0,1.0,1.0});
#endif
    hpdf_table_set_content(t,content);
    hpdf_table_set_labels(t,labels);

    // Spanning for the header row (row==0))
    hpdf_table_set_cellspan(t,0,1,1,3);

    hpdf_table_set_cellspan(t,1,1,1,3);
    hpdf_table_set_cellspan(t,2,2,1,2);

    hpdf_table_set_cellspan(t,4,1,2,3);
    hpdf_table_set_cellspan(t,7,2,2,2);


    HPDF_REAL xpos=100;
    HPDF_REAL ypos=500;
    HPDF_REAL width=400;
    HPDF_REAL height=0; // Calculate height automatically
    hpdf_table_stroke(pdf_doc,pdf_page,t,xpos,ypos,width,height);

}

// Utility macro to create a HPDF color constant from integer RGB values
#ifdef	__cplusplus
#define _TO_HPDF_RGB(r,g,b) {r/255.0f,g/255.0f,b/255.0f}
#else
#define _TO_HPDF_RGB(r,g,b) (HPDF_RGBColor){r/255.0f,g/255.0f,b/255.0f}
#endif

/**
 * Table 4 example - Adjusting look and feel of single cell
 */
void
ex_tbl4(void) {
    int num_rows=5;
    int num_cols=4;
    char *table_title="Example 2: Basic table with adjusted font styles";
    hpdf_table_t t = hpdf_table_create(num_rows,num_cols,table_title);

    // Use a red title and center the text
    HPDF_RGBColor color = {0.6f,0,0};
    HPDF_RGBColor background = {0.9f,1.0,0.9f};
    hpdf_table_set_title_style(t,HPDF_FF_HELVETICA_BOLD,14,color,background);
    hpdf_table_set_title_halign(t,CENTER);

    // Use bold font for content. Use the C99 way to specify constant structure constants
#ifdef	__cplusplus
	hpdf_table_set_content_style(t, HPDF_FF_COURIER_BOLD, 10, { 0.1f, 0.1f, 0.1f }, { 1.0, 1.0, 0.9f });
#else
	hpdf_table_set_content_style(t,HPDF_FF_COURIER_BOLD,10,(HPDF_RGBColor){0.1f,0.1f,0.1f},(HPDF_RGBColor){1.0,1.0,0.9f});
#endif
    hpdf_table_set_content(t,content);
    hpdf_table_set_labels(t,labels);

    // Set the top left and bottom right with orange background
    const HPDF_RGBColor orange = _TO_HPDF_RGB(0xF5,0xD0,0x98);
    const HPDF_RGBColor almost_black = _TO_HPDF_RGB(0xF5,0xD0,0x98);
    hpdf_table_set_cell_content_style(t,0,0,HPDF_FF_COURIER_BOLD,10,almost_black,orange);
    hpdf_table_set_cell_content_style(t,4,3,HPDF_FF_COURIER_BOLD,10,almost_black,orange);

    hpdf_table_set_colwidth_percent(t,0,40);

    hpdf_table_set_cellspan(t,1,0,1,2);

    HPDF_REAL xpos=100;
    HPDF_REAL ypos=600;
    HPDF_REAL width=400;
    HPDF_REAL height=0; // Calculate height automatically

    if( -1 == hpdf_table_stroke(pdf_doc,pdf_page,t,xpos,ypos,width,height) ) {
        char *errstr;
        int row,col;
        hpdf_table_get_last_errcode(&errstr, &row, &col);
        fprintf(stderr,"ERROR: \"%s\"\n",errstr);
    }

}

#ifndef _MSC_VER
// Silent gcc about unused "arg"in the widget functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#if !(defined _WIN32 || defined __WIN32__)
/**
 * Callback to display some system information in the header
 * @param tag The optional table tag
 * @param r The row the call is made for
 * @param c The column the call is made for
 * @return The content string to be display
 */
static char *
cb_name(void *tag, size_t r, size_t c) {
    static char buf[128];
    struct utsname sysinfo;
    if( -1 == uname(&sysinfo) ) {
        return "???";
    }
    else {
        snprintf(buf,sizeof(buf),"Name: %s, Kernel: %s %s",sysinfo.nodename, sysinfo.sysname, sysinfo.release);
        return buf;
    }
}

/**
 * Callback to display date and time in header
 * @param tag The optional table tag
 * @param r The row the call is made for
 * @param c The column the call is made for
 * @return The content string to be display
 */
static char *
cb_date(void *tag, size_t r, size_t c) {
    static char buf[64];
    time_t t = time(NULL);
    ctime_r(&t,buf);
    return buf;
}

/**
 * Stroke a page header with system information and date at top of example page
 */
void
example_page_header(void) {


    // Specified the layout of each row
    hpdf_table_data_spec_t tbl1_data[] = {
        // row,col,rowspan,colspan,lable-string,content-callback
        {0,0,1,4,"Server info:",cb_name,NULL,NULL},
        {0,4,1,2,"Date:",cb_date,NULL,NULL},
        {0,0,0,0,NULL,NULL,NULL,NULL}  /* Sentinel to mark end of data */
    };
    // Overall table layout
    hpdf_table_spec_t tbl1 = {
        NULL, 1, 6,      /* Title, rows, cols   */
        70, 800,         /* xpos, ypos          */
        470, 0,          /* width, height       */
        tbl1_data,        /* A pointer to the specification of each row in the table */
        NULL
    };

    // Show how to set a specified theme to the table. Since we only use the
    // default theme  here we could equally well just have set NULL as the last
    // argument to the hpdf_table_stroke_from_data() function since this is the same
    // specifying the default theme.
    hpdf_table_theme_t *theme = hpdf_table_get_default_theme();
    int ret=hpdf_table_stroke_from_data(pdf_doc, pdf_page, tbl1, theme);
    if( -1 == ret ) {
        char *buf;
        int r,c;
        int tbl_err = hpdf_table_get_last_errcode(&buf,&r,&c);
        fprintf(stderr,"*** ERROR in creating table from data. ( %d : \"%s\" ) @ [%d,%d]\n",tbl_err,buf,r,c);
    }
    hpdf_table_destroy_theme(theme);
}
#endif

int
main(int argc, char** argv) {

    if (setjmp(env)) {
        HPDF_Free (pdf_doc);
        return EXIT_FAILURE;
    }

    setup_dummy_data();

    // Setup the PDF document
    pdf_doc = HPDF_New(error_handler, NULL);
    HPDF_SetCompressionMode(pdf_doc, HPDF_COMP_ALL);

    // Example 1
    add_a4page();
#if !(defined _WIN32 || defined __WIN32__)
    example_page_header();
#endif
    ex_tbl1();

    // Example 2
    add_a4page();
#if !(defined _WIN32 || defined __WIN32__)
	example_page_header();
#endif
    ex_tbl2();

    // Example 3
    add_a4page();
#if !(defined _WIN32 || defined __WIN32__)
	example_page_header();
#endif
    ex_tbl3();

    // Example 4
    add_a4page();
#if !(defined _WIN32 || defined __WIN32__)
	example_page_header();
#endif
    ex_tbl4();

    stroke_page_tofile();

    printf("Done. \"%s\"\n",OUTPUT_FILE);

    return (EXIT_SUCCESS);
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
