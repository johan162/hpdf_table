/*
 * Example on how to use the HPDF_Table module to facilitate the creation of structured tables
 * with Haru PF library.
 * You need to link with libraries:
 * - math (i.e. "-lm")
 * - libhpdf-2 (of course)
 * - libiconv
 *
 * So on Mac OS Compile this with:
 *
 *  gcc --std=c99 -lm /usr/local/lib/libhpdf.dylib /usr/lib/libiconv.2.dylib hpdf_*.c
 *
 * However, it is usually a good idea to enable as many compiler warnings as possible so the
 * recommended way to compile is:
 *
 *  gcc --std=c99 -Wall -Wpedantic -Wextra -Wpointer-arith -lm /usr/local/lib/libhpdf.dylib /usr/lib/libiconv.2.dylib hpdf_*.c
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
#include "hpdf_table_widget.h"

// The output after running the program will be written to this file
#ifdef _WIN32
#define OUTPUT_FILE "hpdf_table.pdf"
#else
#define OUTPUT_FILE "/tmp/hpdf_table.pdf"
#endif
#define TRUE 1
#define FALSE 0

// Utility macro to create a HPDF color constant from integer RGB values
#ifdef	__cplusplus
#define _TO_HPDF_RGB(r,g,b) (HPDF_RGBColor){r/255.0f,g/255.0f,b/255.0f}
#else
#define _TO_HPDF_RGB(r,g,b) {r/255.0f,g/255.0f,b/255.0f}
#endif
/**
 * Color constants
 */
#ifdef	__cplusplus

#define COLOR_DARK_RED {0.6f,0.0f,0.0f}
#define COLOR_LIGHT_GREEN {0.9f,1.0f,0.9f}
#define COLOR_DARK_GRAY {0.2f,0.2f,0.2f}
#define COLOR_LIGHT_GRAY {0.9f,0.9f,0.9f}
#define COLOR_GRAY {0.5f,0.5f,0.5f}
#define COLOR_LIGHT_BLUE {1.0f,1.0f,0.9f}
#define COLOR_WHITE {1.0f,1.0f,1.0f}
#define COLOR_BLACK {0.0f,0.0f,0.0f}

#else

#define COLOR_DARK_RED (HPDF_RGBColor){0.6f,0.0f,0.0f}
#define COLOR_LIGHT_GREEN (HPDF_RGBColor){0.9f,1.0f,0.9f}
#define COLOR_GREEN (HPDF_RGBColor){0.4f,0.9f,0.4f}
#define COLOR_DARK_GRAY (HPDF_RGBColor){0.2f,0.2f,0.2f}
#define COLOR_LIGHT_GRAY (HPDF_RGBColor){0.9f,0.9f,0.9f}
#define COLOR_GRAY (HPDF_RGBColor){0.5f,0.5f,0.5f}
#define COLOR_LIGHT_BLUE (HPDF_RGBColor){1.0f,1.0f,0.9f}
#define COLOR_WHITE (HPDF_RGBColor){1.0f,1.0f,1.0f}
#define COLOR_BLACK (HPDF_RGBColor){0.0f,0.0f,0.0f}

#endif

#define COLOR_ORANGE _TO_HPDF_RGB(0xF5,0xD0,0x98);
#define COLOR_ALMOST_BLACK _TO_HPDF_RGB(0xF5,0xD0,0x98);

// For simulated exception handling
jmp_buf env;

// Global handlers to the HPDF document and page
HPDF_Doc pdf_doc;
HPDF_Page pdf_page;

// We use some dummy data to populate the tables
#define MAX_NUM_ROWS 10
#define MAX_NUM_COLS 10

// Data array with string pointers to dummy data and cell labels
// The actual storage for the strings are dynamically allocated.
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
// Silent gcc about unused "arg" in the callback and error functions
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


#if !(defined _WIN32 || defined __WIN32__)
// We don't use the page header on Windooze systems

/**
 * Callback to display some system information in the header.
 * The header will be displayed on each page.
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
 * This callback gets called by the table rendering mechanism after the table overall
 * background and lines have been stroked but before the text content (label + data)
 * of the sell is stroked. 
 * @param doc
 * @param page
 * @param tag
 * @param r
 * @param c
 * @param xpos
 * @param ypos
 * @param width
 * @param height
 */
void
cb_draw_segment_hbar(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r, size_t c,
                HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height) {
    
    
    const HPDF_REAL segment_tot_width = width * 0.5;
    const HPDF_REAL segment_height = height/3;
    const HPDF_REAL segment_xpos = xpos+5;
    const HPDF_REAL segment_ypos = ypos+4;
    const size_t num_segments=10;
    
    const HPDF_RGBColor on_color = COLOR_GREEN;
    const double val_percent = 0.4;
    const _Bool val_text_hide = FALSE;    
    
    hpdf_table_widget_segment_hbar(doc, page,segment_xpos, segment_ypos, 
            segment_tot_width, segment_height,
            num_segments, on_color, val_percent, 
            val_text_hide);
}

void
cb_draw_hbar(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r, size_t c,
                HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height) {
    
    
    const HPDF_REAL wwidth = width * 0.5;
    const HPDF_REAL wheight = height/3 ;
    const HPDF_REAL wxpos = xpos+5;
    const HPDF_REAL wypos = ypos+4;
    
    const HPDF_RGBColor color = COLOR_GREEN;
    const double val = 0.6;
    const _Bool val_text_hide = FALSE;    

    hpdf_table_widget_hbar(doc, page, wxpos, wypos, 
            wwidth, wheight,
            color, val, 
            val_text_hide);
}

void
cb_draw_slider(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r, size_t c,
               HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height) {
    /*
     * void
hpdf_table_widget_slide_button(HPDF_Doc doc, HPDF_Page page,
                     HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height, _Bool state)
     */
    const HPDF_REAL wwidth = 37 ;
    const HPDF_REAL wheight = 12 ;
    const HPDF_REAL wxpos = xpos+75;
    const HPDF_REAL wypos = ypos+5;
    
    // The slide is on for third row and off otherwise
    _Bool state = (r==2);
    
    hpdf_table_widget_slide_button(doc, page, wxpos, wypos, wwidth, wheight,state); 
}

void
cb_draw_strength_meter(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r, size_t c,
                  HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height) {
    
    const HPDF_REAL wwidth = 35 ;
    const HPDF_REAL wheight = 20 ;
    const HPDF_REAL wxpos = xpos+75;
    const HPDF_REAL wypos = ypos+4;
    const size_t num_segments = 5;
    const HPDF_RGBColor on_color = COLOR_GREEN;
    const size_t num_on_segments = 3;
    
    hpdf_table_widget_strength_meter(doc, page, wxpos, wypos, wwidth, wheight,
            num_segments, on_color, num_on_segments);
}

void
cb_draw_boxed_letter(HPDF_Doc doc, HPDF_Page page, void *tag, size_t r, size_t c,
                  HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height) {
    /*
     * void
hpdf_table_widget_letter_buttons(HPDF_Doc doc, HPDF_Page page,
                     HPDF_REAL xpos, HPDF_REAL ypos, HPDF_REAL width, HPDF_REAL height,
                     const HPDF_RGBColor on_color, const HPDF_RGBColor off_color,
                     const HPDF_RGBColor on_background, const HPDF_RGBColor off_background,
                     const HPDF_REAL fsize,
                     const char *letters, _Bool *state )
     */
    
    const HPDF_REAL wwidth = 60 ;
    const HPDF_REAL wheight = 15 ;
    const HPDF_REAL wxpos = xpos+75;
    const HPDF_REAL wypos = ypos+4;
    const HPDF_RGBColor on_color = COLOR_DARK_GRAY;
    const HPDF_RGBColor off_color = COLOR_GRAY;
    const HPDF_RGBColor on_background = COLOR_GREEN;
    const HPDF_RGBColor off_background = COLOR_LIGHT_GRAY;
    const HPDF_REAL fsize = 11;
    const char *letters = "ABCD"; 
    _Bool state[] = {TRUE,FALSE,TRUE,FALSE};   
    
    hpdf_table_widget_letter_buttons(doc, page, wxpos, wypos, wwidth, wheight,
            on_color, off_color, on_background, off_background, fsize, letters, state);
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

/**
 * Stroke a page header with system information and date at top of example page
 */
void
example_page_header(void) {

    // Specified the layout of each row
    // For a cell where we want dynamic content we must make use of a content-callback
    // that will return a pointer to a static buffer whose content will be displayed
    // in the cell.
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
        tbl1_data,       /* A pointer to the specification of each row in the table */
        NULL             /* Sentinel to mark end of data */
    };

    // Show how to set a specified theme to the table. Since we only use the
    // default theme here we could equally well just have set NULL as the last
    // argument to the hpdf_table_stroke_from_data() function since this is the same
    // specifying the default theme.
    hpdf_table_theme_t *theme = hpdf_table_get_default_theme();
    int ret=hpdf_table_stroke_from_data(pdf_doc, pdf_page, tbl1, theme);

    // SHould always check for any error
    if( -1 == ret ) {
        char *buf;
        int r,c;
        int tbl_err = hpdf_table_get_last_errcode(&buf,&r,&c);
        fprintf(stderr,"*** ERROR in creating table from data. ( %d : \"%s\" ) @ [%d,%d]\n",tbl_err,buf,r,c);
    }

    // Remember to clean up to avoid memory leak
    hpdf_table_destroy_theme(theme);
}

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
    const HPDF_RGBColor title_text_color = COLOR_DARK_RED;
    const HPDF_RGBColor title_bg_color = COLOR_LIGHT_GRAY;
    hpdf_table_set_title_style(t,HPDF_FF_HELVETICA_BOLD,14,title_text_color,title_bg_color);
    hpdf_table_set_title_halign(t,CENTER);

    // Use bold font for content. Use the C99 way to specify constant structure constants
    const HPDF_RGBColor content_text_color = COLOR_DARK_GRAY;
    const HPDF_RGBColor content_bg_color = COLOR_LIGHT_BLUE;
    hpdf_table_set_content_style(t,HPDF_FF_COURIER_BOLD,10,content_text_color,content_bg_color);

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
    const HPDF_RGBColor title_text_color = COLOR_DARK_RED;
    const HPDF_RGBColor title_bg_color = COLOR_LIGHT_GRAY;
    hpdf_table_set_title_style(t,HPDF_FF_HELVETICA_BOLD,14,title_text_color,title_bg_color);
    hpdf_table_set_title_halign(t,CENTER);

    // Use specially formatted header row
    hpdf_table_use_header(t,TRUE);

    // Use full grid and not just the short labelgrid
    hpdf_table_use_labelgrid(t,FALSE);

    // Use bold font for content. Use the C99 way to specify constant structure constants
    const HPDF_RGBColor content_text_color = COLOR_DARK_GRAY;
    const HPDF_RGBColor content_bg_color = COLOR_WHITE;
    hpdf_table_set_content_style(t,HPDF_FF_COURIER_BOLD,10,content_text_color,content_bg_color);

    hpdf_table_set_content(t,content);
    hpdf_table_set_labels(t,labels);

    // Spanning for the header row (row==0))
    // Span cell=(0,1) one row and three columns
    hpdf_table_set_cellspan(t,0,1,1,3);

    // Span cell=(1,1) one row and three columns
    hpdf_table_set_cellspan(t,1,1,1,3);

    // Span cell=(2,2) one row and two columns
    hpdf_table_set_cellspan(t,2,2,1,2);

    // Span cell=(4,1) two rows and three columns
    hpdf_table_set_cellspan(t,4,1,2,3);

    // Span cell=(7,2) two rows and two columns
    hpdf_table_set_cellspan(t,7,2,2,2);

    HPDF_REAL xpos=100;
    HPDF_REAL ypos=500;
    HPDF_REAL width=400;
    HPDF_REAL height=0; // Calculate height automatically
    hpdf_table_stroke(pdf_doc,pdf_page,t,xpos,ypos,width,height);

}

/**
 * Table 4 example - Adjusting look and feel of single cell
 */
void
ex_tbl4(void) {
    const size_t num_rows=5;
    const size_t num_cols=4;
    char *table_title="Example 4: Adjusting look and feel of single cell";
    hpdf_table_t t = hpdf_table_create(num_rows,num_cols,table_title);

    // Use a red title and center the text
    const HPDF_RGBColor title_text_color = COLOR_DARK_RED;
    const HPDF_RGBColor title_bg_color = COLOR_LIGHT_GRAY;
    hpdf_table_set_title_style(t,HPDF_FF_HELVETICA_BOLD,14,title_text_color,title_bg_color);
    hpdf_table_set_title_halign(t,CENTER);

    // Set the top left and bottom right with orange bg_color
    const HPDF_RGBColor content_bg_color = COLOR_ORANGE;
    const HPDF_RGBColor content_text_color = COLOR_ALMOST_BLACK;
    hpdf_table_set_cell_content_style(t,0,0,HPDF_FF_COURIER_BOLD,10,content_text_color,content_bg_color);
    hpdf_table_set_cell_content_style(t,4,3,HPDF_FF_COURIER_BOLD,10,content_text_color,content_bg_color);

    hpdf_table_set_content(t,content);
    hpdf_table_set_labels(t,labels);

    // First column should be 40% of the total width
    hpdf_table_set_colwidth_percent(t,0,40);

    // Span cell=(1,0) one row and two columns
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


/**
 * Table 5 example - Adding predefined widgets
 */
void
ex_tbl5(void) {
    const int num_rows=6;
    const int num_cols=4;
    char *table_title="Example 5: Using widgets in cells";
    hpdf_table_t t = hpdf_table_create(num_rows,num_cols,table_title);

    // Use a red title and center the text
    const HPDF_RGBColor title_text_color = COLOR_DARK_RED;
    const HPDF_RGBColor title_bg_color = COLOR_LIGHT_GRAY;
    hpdf_table_set_title_style(t,HPDF_FF_HELVETICA_BOLD,14,title_text_color,title_bg_color);
    hpdf_table_set_title_halign(t,CENTER);

    
    // Install callback for the specified cell where the graphical meter will be drawn
    size_t wrow=0;
    size_t wcol=0;
    
    content[wrow*num_cols+wcol] = NULL;
    labels[wrow*num_cols+wcol] = "Horizontal seg bar:";    
    hpdf_table_set_cell_canvas_callback(t,wrow,wcol,cb_draw_segment_hbar);

    wrow += 1;
    content[wrow*num_cols+wcol] = NULL;
    labels[wrow*num_cols+wcol] = "Horizontal bar:";        
    hpdf_table_set_cell_canvas_callback(t,wrow,wcol,cb_draw_hbar);

    wrow += 1;
    content[wrow*num_cols+wcol] = NULL;
    labels[wrow*num_cols+wcol] = "Slider on:";        
    hpdf_table_set_cell_canvas_callback(t,wrow,wcol,cb_draw_slider);

    wrow += 1;
    content[wrow*num_cols+wcol] = NULL;
    labels[wrow*num_cols+wcol] = "Slider off:";        
    hpdf_table_set_cell_canvas_callback(t,wrow,wcol,cb_draw_slider);

    wrow += 1;
    content[wrow*num_cols+wcol] = NULL;
    labels[wrow*num_cols+wcol] = "Strength meter:";        
    hpdf_table_set_cell_canvas_callback(t,wrow,wcol,cb_draw_strength_meter);
    
    wrow += 1;
    content[wrow*num_cols+wcol] = NULL;
    labels[wrow*num_cols+wcol] = "Boxed letters:";        
    hpdf_table_set_cell_canvas_callback(t,wrow,wcol,cb_draw_boxed_letter);    
    
    hpdf_table_set_content(t,content);
    hpdf_table_set_labels(t,labels);
    
    // First column should be 40% of the total width
    hpdf_table_set_colwidth_percent(t,0,40);
        
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


// Type for the pointer to example stroking functions "void fnc(void)"
typedef void (*t_func_tbl_stroke)(void);

// Silent gcc about unused arguments in the main functions
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

int
main(int argc, char** argv) {

    t_func_tbl_stroke examples[] = {ex_tbl1, ex_tbl2, ex_tbl3, ex_tbl4, ex_tbl5};
    const size_t num_examples = sizeof(examples)/sizeof(t_func_tbl_stroke);

    printf("Stroking %ld examples.\n",num_examples);

    // Setup fake exception handling
    if (setjmp(env)) {
        HPDF_Free (pdf_doc);
        return EXIT_FAILURE;
    }

    // Get some dummy data to fill the table§
    setup_dummy_data();

    // Setup the basic PDF document
    pdf_doc = HPDF_New(error_handler, NULL);
    HPDF_SetCompressionMode(pdf_doc, HPDF_COMP_ALL);


    for( size_t i=0; i < num_examples; i++ ) {
      add_a4page();
#if !(defined _WIN32 || defined __WIN32__)
      example_page_header();
#endif
      (*examples[i])();
    }

    printf("Sending to file \"%s\" ...\n",OUTPUT_FILE);
    stroke_page_tofile();
    printf("Done.\n");

    return (EXIT_SUCCESS);
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
