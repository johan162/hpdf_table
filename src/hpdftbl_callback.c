/**
 * @file
 * @brief    Routines for plain and dynamic callback function.
 *
 * All functions ending with `_cb` are used to specify standard callback functions which stores
 * a function pointer bounded at compile time. All functions ending i `_dyncb` are used to
 * set dynamic callback functions which are bound at run time. The function name are stored
 * as string and resolved at runtime.
 *
 * @author   Johan Persson (johan162@gmail.com)
 *
 * Copyright (C) 2022 Johan Persson
 *
 * @see LICENSE
 *
 */

#include <stdio.h>
#include <stdlib.h>

#if !(defined _WIN32 || defined __WIN32__)

#include <unistd.h>

#endif

#include <string.h>
#include <iconv.h>
#include <hpdf.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "hpdftbl.h"

/**
 * @brief The handle to use when searching for late loaded callbacks.
 *
 * By default this is specified as `RTLD_DEFAULT` which searches through all
 * loaded images. If you for example link with a static library you can make
 * this more efficient by specifying RTLD_SELF. See `man dlsym` for more
 * information.
 *
 * @see hpdftbl_set_dlhandle()
 */
static void *dl_handle = RTLD_DEFAULT;


/**
 * @brief Set the handle for scope of dynamic function search.
 *
 * When using late binding (some os _dyncb() functions) the scope for where the runtime
 * searches for the functions can be specified as is discussed in `man 3 dlsym`. By default
 * the library uses dl_handle which make the library first searches the current image and then
 * all images it was built against.
 *
 * If the dynamic callbacks are located in a runtime loaded library then the handle returned
 * by dlopen() must be specified as the function will not be found otherwise.
 *
 * @param handle Predefined values or the handle returned by dlopen() (see man dlopen)
 */
void
hpdftbl_set_dlhandle(void *handle) {
    dl_handle = handle;
}

/**
 * @brief Set table content callback
 *
 * This callback gets called for each cell in the
 * table and the returned string will be used as the content. The string
 * will be duplicated so it is safe for a client to reuse the string space.
 * If NULL is returned from the callback then the content will be set to the
 * content specified with the direct content setting.
 * The callback function will receive the Table tag and the row and column for the
 * cell the callback is made for.
 * @param t Table handle
 * @param cb Callback function
 * @return -1 for error , 0 otherwise
 *
 * @see hpdftbl_set_cell_content_cb()
 */
int
hpdftbl_set_content_cb(hpdftbl_t t, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->content_cb = cb;
    return 0;
}

/**
 * @brief Set cell content callback.
 *
 * Set a content callback for an individual cell. This will override the table content
 * callback. The callback function will receive the Table tag and the row and column for the
 * cell the callback is made for.
 * @param t Table handle
 * @param cb Callback function
 * @param r Cell row
 * @param c Cell column
 * @return -1 on failure, 0 otherwise
 *
 * @see hpdftbl_set_content_cb()
 */
int
hpdftbl_set_cell_content_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c))
        return -1;

    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    cell->content_cb = cb;
    return 0;
}

/**
 * @brief Set cell label callback
 *
 * Set a label callback for an individual cell. This will override the table label
 * callback. The callback function will receive the Table tag and the row and column for the
 * cell the callback is made for.
 * @param t Table handle
 * @param cb Callback function
 * @param r Cell row
 * @param c Cell column
 * @return -1 on failure, 0 otherwise
 *
 * @see hpdftbl_set_label_cb()
 */
int
hpdftbl_set_cell_label_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c))
        return -1;

    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    cell->label_cb = cb;
    return 0;
}

/**
 * @brief Set cell canvas callback
 *
 * Set a canvas callback for an individual cell. This will override the table canvas
 * callback. The canvas callback is called with arguments that give the bounding box for the
 * cell. In that way a callback function may draw arbitrary graphic in the cell.
 * The callback is made before the cell border and content is drawn making
 * it possible to for example add a background color to individual cells.
 * The callback function will receive the Table tag, the row and column,
 * the x, y position of the lower left corner of the table and the width
 * and height of the cell.
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdftbl_canvas_callback_t
 * @see hpdftbl_set_canvas_cb()
 */
int
hpdftbl_set_cell_canvas_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_canvas_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c))
        return -1;

    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    cell->canvas_cb = cb;
    return 0;
}

/**
 * @brief Set table label callback
 *
 * Set label callback. This callback gets called for each cell in the
 * table and the returned string will be used as the label. The string
 * will be duplicated so it is safe for a client to reuse the string space.
 * If NULL is returned from the callback then the label will be set to the
 * content specified with the direct label setting.
 * The callback function will receive the Table tag and the row and column
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdftbl_content_callback_t
 * @see hpdftbl_set_cell_label_cb()
 */
int
hpdftbl_set_label_cb(hpdftbl_t t, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->label_cb = cb;
    return 0;
}

/**
 * @brief Set table post processing callback
 *
 * This is an optional post processing callback for anything in general to do
 * after the table has been constructed.
 * The callback happens after the table has been fully constructed and just
 * before it is stroked.
 *
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdftbl_callback_t
 */
int
hpdftbl_set_post_cb(hpdftbl_t t, hpdftbl_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->post_cb = cb;
    return 0;
}

/**
 * @brief Set cell canvas callback
 *
 * Set cell canvas callback. This callback gets called for each cell in the
 * table. The purpose is to allow the client to add dynamic content to the
 * specified cell.
 * The callback is made before the cell border and content is drawn making
 * it possible to for example add a background color to individual cells.
 * The callback function will receive the Table tag, the row and column,
 * the x, y position of the lower left corner of the table and the width
 * and height of the cell.
 * To set the canvas callback only for a specific cell use the
 * hpdftbl_set_cell_canvas_cb() function
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 *
 * @see hpdftbl_set_cell_canvas_cb()
 */
int
hpdftbl_set_canvas_cb(hpdftbl_t t, hpdftbl_canvas_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->canvas_cb = cb;
    return 0;
}


/**
 * @brief Specify dynamic (late) loading callback content function.
 *
 * The dynamic loading of callback function is a runtime binding of the named function
 * as a callback. The library uses the dlsym() loading of external symbols. For the external
 * symbol to be found it can not be defined as a `static` symbol.
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param cb_name Name of the function to be used as content callback. This function must follow the
 * signature of a callback function as specified in hpdftbl_content_callback_t.
 * @return -1 on failure, 0 on success
 *
 * @see hpdftbl_set_content_cb(), hpdftbl_content_callback_t
 */
int
hpdftbl_set_content_dyncb(hpdftbl_t t, const char *cb_name) {
    hpdftbl_content_callback_t dyn_content_cb = (hpdftbl_content_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_content_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->content_dyncb = strdup(cb_name);
    hpdftbl_set_content_cb(t, dyn_content_cb);
    return 0;
}

/**
 * @brief Specify dynamic (late) loading callback content function.
 *
 * The dynamic loading of callback function is a runtime binding of the named function
 * as a callback. The library uses the dlsym() loading of external symbols. For the external
 * symbol to be found it can not be defined as a `static` symbol.
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param cb_name Name of the function to be used as canvas callback. This function must follow the
 * signature of a callback function as specified in hpdftbl_canvas_callback_t.
 * @return -1 on failure, 0 on success
 *
 * @see hpdftbl_set_canvas_cb(), hpdftbl_canvas_callback_t
 */
int
hpdftbl_set_canvas_dyncb(hpdftbl_t t, const char *cb_name) {
    hpdftbl_canvas_callback_t dyn_canvas_cb = (hpdftbl_canvas_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_canvas_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->canvas_dyncb = strdup(cb_name);
    hpdftbl_set_canvas_cb(t, dyn_canvas_cb);
    return 0;
}


/**
 * @brief Specify dynamic (late) loading callback for table label function.
 *
 * The dynamic loading of callback function is a runtime binding of the named function
 * as a callback. The library uses the dlsym() loading of external symbols. For the external
 * symbol to be found it can not be defined as a `static` symbol.  symbols. The callback function
 * must have the signature defined by hpdftbl_content_callback_t
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param cb_name Name of the function to be used as label callback. This function must follow the
 * signature of a callback function as specified in hpdftbl_content_callback_t.
 * @return -1 on failure, 0 on success
 *
 * @see hpdftbl_set_label_cb(), hpdftbl_content_callback_t
 */
int
hpdftbl_set_label_dyncb(hpdftbl_t t, const char *cb_name) {
    hpdftbl_content_callback_t dyn_labels_cb = (hpdftbl_content_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_labels_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->label_dyncb = strdup(cb_name);
    hpdftbl_set_label_cb(t, dyn_labels_cb);
    return 0;
}

/**
 * @brief Specify dynamic (late) loading callback for cell label function.
 *
 * The dynamic loading of callback function is a runtime binding of the named function
 * as a callback. The library uses the dlsym() loading of external symbols. For the external
 * symbol to be found it can not be defined as a `static` symbol. The callback function
 * must have the signature defined by hpdftbl_content_callback_t
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb_name Name of the function to be used as label callback. This function must follow the
 * signature of a callback function as specified in hpdftbl_content_callback_t.
 * @return -1 on failure, 0 on success
 *
 * @see hpdftbl_set_cell_label_cb(), hpdftbl_content_callback_t
 */
int
hpdftbl_set_cell_label_dyncb(hpdftbl_t t, size_t r, size_t c, const char *cb_name) {
    hpdftbl_content_callback_t dyn_labels_cb = (hpdftbl_content_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_labels_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->cells[_HPDFTBL_IDX(r,c)].label_dyncb = strdup(cb_name);
    hpdftbl_set_cell_label_cb(t, r, c,dyn_labels_cb);
    return 0;
}

/**
 * @brief Specify dynamic (late) loading callback for table style function.
 *
 * The dynamic loading of callback function is a runtime binding of the named function
 * as a callback. The library uses the dlsym() loading of external symbols. For the external
 * symbol to be found it can not be defined as a `static` symbol. The callback function
 * must have the signature defined by hpdftbl_content_style_callback_t
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param cb_name Name of the function to be used as label callback.
 * This function must follow the
 * signature of a callback function as specified in hpdftbl_content_style_callback_t.
 * @return -1 on failure, 0 on success
 *
 * @see hpdftbl_set_content_style_cb(), hpdftbl_content_style_callback_t
 */
int
hpdftbl_set_content_style_dyncb(hpdftbl_t t, const char *cb_name) {
    hpdftbl_content_style_callback_t dyn_style_cb = (hpdftbl_content_style_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_style_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->content_style_dyncb = strdup(cb_name);
    hpdftbl_set_content_style_cb(t, dyn_style_cb);
    return 0;
}



/**
 * @brief Specify dynamic (late) loading callback for cell style function.
 *
 * The dynamic loading of callback function is a runtime binding of the named function
 * as a callback. The library uses the dlsym() loading of external symbols. For the external
 * symbol to be found it can not be defined as a `static` symbol. The callback function
 * must have the signature defined by hpdftbl_content_style_callback_t
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb_name Name of the function to be used as label callback.
 * This function must follow the
 * signature of a callback function as specified in hpdftbl_content_style_callback_t.
 * @return -1 on failure, 0 on success
 *
 * @see hpdftbl_set_cell_content_style_cb(), hpdftbl_content_style_callback_t
 */
int
hpdftbl_set_cell_content_style_dyncb(hpdftbl_t t, size_t r, size_t c, const char *cb_name) {
    hpdftbl_content_style_callback_t dyn_style_cb = (hpdftbl_content_style_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_style_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->cells[_HPDFTBL_IDX(r,c)].content_style_dyncb = strdup(cb_name);
    hpdftbl_set_cell_content_style_cb(t, r, c,dyn_style_cb);
    return 0;
}

/**
 * @brief Specify dynamic (late) loading callback cell content function.
 *
 * The dynamic loading of callback function is a runtime binding of the named function
 * as a callback. The library uses the dlsym() loading of external symbols. For the external
 * symbol to be found it can not be defined as a `static` symbol.
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb_name Name of the function to be used as content callback. This function must follow the
 * signature of a callback function as specified in hpdftbl_content_callback_t.
 * @return -1 on failure, 0 on success
 *
 * @see hpdftbl_set_content_cb(), hpdftbl_content_callback_t
 */
int
hpdftbl_set_cell_content_dyncb(hpdftbl_t t, size_t r, size_t c, const char *cb_name) {
    hpdftbl_content_callback_t dyn_content_cb = (hpdftbl_content_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_content_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->cells[_HPDFTBL_IDX(r,c)].content_dyncb = strdup(cb_name);
    hpdftbl_set_cell_content_cb(t, r, c, dyn_content_cb);
    return 0;
}

/**
 * @brief Specify dynamic (late) loading callback cell canvas function.
 *
 * In case of error the `extrainfo` extra information is set to
 * the name of the callback which failed to be resolved at run time. This
 * can be retrieved in an error handler by using the hpdftbl_get_last_err_file() to read
 * it back.
 *
 * @param t Table handle
 * @param r Celll row
 * @param c Cell column
 * @param cb_name Name of the function to be used as canvas callback. This function must follow the
 * signature of a callback function as specified in hpdftbl_canvas_callback_t
 * @return
 *
 * @see hpdftbl_set_cell_canvas_cb(), hpdftbl_canvas_callback_t
 */
int
hpdftbl_set_cell_canvas_dyncb(hpdftbl_t t, size_t r, size_t c, const char *cb_name) {
    hpdftbl_canvas_callback_t dyn_canvas_cb = (hpdftbl_canvas_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_canvas_cb) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->cells[_HPDFTBL_IDX(r,c)].canvas_dyncb = strdup(cb_name);
    hpdftbl_set_cell_canvas_cb(t, r, c, dyn_canvas_cb);
    return 0;
}

/**
 * @brief Set table post processing callback
 *
 * This is an optional post processing callback for anything in general to do
 * after the table has been constructed. The callback only gets the table
 * as its first and only argument.
 * The callback happens after the table has been fully constructed and just
 * before it is stroked.
 *
 * @param t Table handle
 * @param cb_name Callback function name
 * @return -1 on failure, 0 otherwise
 * @see hpdftbl_callback_t, hpdftbl_set_post_cb()
 */
int
hpdftbl_set_post_dyncb(hpdftbl_t t, const char *cb_name) {
    hpdftbl_callback_t dyn_post_cb = (hpdftbl_callback_t)dlsym(dl_handle, cb_name);
    if( NULL == dyn_post_cb ) {
        _HPDFTBL_SET_ERR_EXTRA(cb_name);
        _HPDFTBL_SET_ERR(t, -14, -1, -1);
        return -1;
    }
    t->post_dyncb = strdup(cb_name);
    hpdftbl_set_post_cb(t, dyn_post_cb);
    return 0;
}


/**
 * @brief Set cell specific callback to specify cell content style.
 *
 * Set callback to format the style for the specified cell
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb Callback function
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_ontent_style_cb()
 */
int
hpdftbl_set_cell_content_style_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_style_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    chktbl(t, r, c);
    // If this cell is part of another cells spanning then
    // indicate this as an error
    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }
    cell->style_cb = cb;
    return 0;
}

/**
 * @brief Set callback to specify cell content style
 *
 * Set callback to format the style for cells in the table. If a cell has its own content style
 * callback that callback will override the generic table callback.
 * @param t Table handle
 * @param cb Callback function
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_cell_content_style_cb()
 */
int
hpdftbl_set_content_style_cb(hpdftbl_t t, hpdftbl_content_style_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->content_style_cb = cb;
    return 0;
}

/* EOF */
