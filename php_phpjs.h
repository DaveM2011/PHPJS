/* This code was generated by phptoc.php */

#ifndef PHP_PHPJS_H
#define PHP_PHPJS_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "zend_operators.h"
#include "zend_constants.h"
#include "zend_ini.h"
#include "zend_interfaces.h"


#define _S(str) str, sizeof(str) - 1

extern zend_module_entry phpjs_module_entry;
#define phpext_phpjs_ptr &phpjs_module_entry

typedef struct {
    zend_object zo;
    duk_context * ctx;
    zval * vm;
    duk_idx_t idx;
} phpjs_wrap_duk_t;


BEGIN_EXTERN_C()

extern PHPAPI zend_class_entry *phpjs_JS_ptr;
extern PHPAPI zend_class_entry *phpjs_JSException_ptr;
extern PHPAPI zend_class_entry *phpjs_JSObjectWrapper_ptr;
extern PHPAPI zend_class_entry *phpjs_JSFunctionWrapper_ptr;
extern PHPAPI zend_class_entry *phpjs_JS_ptr;
extern void duk_to_zval(zval ** var, duk_context * ctx, duk_idx_t idx);
extern void zval_to_duk(duk_context * ctx, char * name, zval * value);
extern duk_idx_t duk_push_php_array_or_object(duk_context * ctx, HashTable * myht);
extern duk_ret_t php_get_function_wrapper(duk_context * ctx);
extern duk_ret_t duk_set_into_php(duk_context * ctx);
extern duk_ret_t duk_get_from_php(duk_context * ctx);
extern void php_register_function_handler(TSRMLS_D);
extern void php_register_object_handler(TSRMLS_D);
extern void phpjs_wrapped_free(phpjs_wrap_duk_t * obj TSRMLS_DC);
extern void phpjs_add_duk_context(zval * this, duk_context * ctx, duk_idx_t idx TSRMLS_DC);
extern int zval_array_to_stack(duk_context * ctx, zval * a_args);
extern int phpjs_php__call(duk_context * ctx, char * fnc, zval * a_args, zval * return_value TSRMLS_DC);

#define php_duk_free_return(ctx) duk_pop(ctx); 

END_EXTERN_C()

#define Z_EXCEPTION_PROP(x) Z_STRVAL_P(zend_read_property(zend_exception_get_default(TSRMLS_C), EG(exception), x, sizeof(x)-1, 0 TSRMLS_CC))

#define FETCH_THIS_WRAPPER \
    phpjs_wrap_duk_t * obj = (phpjs_wrap_duk_t *) zend_object_store_get_object(getThis() TSRMLS_CC );   \
    if (!this_ptr || obj->ctx == NULL) {    \
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unexpected error. This method cannot be called statically"); \
        return; \
    }   \
    duk_context * ctx = obj->ctx;

#define FETCH_THIS_EX(validate) \
    zval* object = getThis(); \
php_js_t*  obj = NULL;      \
obj = (php_js_t *) zend_object_store_get_object( object TSRMLS_CC );   \
if (!this_ptr || (validate && (!instanceof_function(Z_OBJCE_P(this_ptr), phpjs_JS_ptr TSRMLS_CC) || obj->ctx == NULL))) {    \
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unexpected error. This method cannot be called statically");    \
    return;    \
} \
duk_context * ctx = obj->ctx;

#define FETCH_THIS FETCH_THIS_EX(1)

#define SET_PROP(var, t, n, v)  zend_update_property_string(t, var, n, sizeof(n)-1, v TSRMLS_CC);

#define THROW_EXCEPTION(message) do { \
    zval * tc_ex; \
    MAKE_STD_ZVAL(tc_ex); \
    object_init_ex(tc_ex, phpjs_JSException_ptr); \
    SET_PROP(tc_ex, phpjs_JSException_ptr, "message", message); \
    zend_throw_exception_object(tc_ex TSRMLS_CC); \
} while (0); \


#endif
