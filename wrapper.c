#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

void phpjs_php__call(duk_context * ctx, char * fnc, zval * a_args, zval * return_value TSRMLS_DC)
{
    duk_get_prop_string(ctx, -1, fnc);

    if (!duk_is_function(ctx, -1)) {
        duk_pop(ctx);
        char * exception = emalloc(1024 + strlen(fnc));
        sprintf(exception, "%s() is not a javascript function", fnc);
        THROW_EXCEPTION(exception);
        efree(exception);
        RETURN_FALSE;
    }
    
    int argc = zval_array_to_stack(ctx, a_args);
    if (duk_pcall(ctx, argc) != 0) {
        duk_php_throw(ctx, -1 TSRMLS_CC);
        RETURN_FALSE;
    }

    duk_to_zval(return_value, ctx, -1);
    php_duk_free_return(ctx);
}

int zval_array_to_stack(duk_context * ctx, zval * a_args)
{
    int argc = 0;
    zval * data;
    HashTable *myht = Z_ARRVAL_P(a_args);

	ZEND_HASH_FOREACH_VAL(myht, data)
		zval_to_duk(ctx, NULL, data);
		argc++;
	ZEND_HASH_FOREACH_END();

    return argc;
}

void phpjs_wrapped_free(zend_object * object TSRMLS_DC)
{
	phpjs_wrap_duk_t *obj = (phpjs_wrap_duk_t *)((char *)object - XtOffsetOf(phpjs_wrap_duk_t, zobj));
    
    zend_object_std_dtor(&obj->zobj TSRMLS_CC);
    if (obj->vm) {
        duk_to_undefined(obj->ctx, obj->idx);
        zval_ptr_dtor(obj->vm);
    }
    efree(obj);
}

void phpjs_add_duk_context(zval * _this, duk_context * duk_ctx, duk_idx_t idx TSRMLS_DC)
{
    FETCH_THIS_WRAPPER(_this);
    
    zval * value;
    TSRMLS_FETCH();
    if((value = zend_hash_str_find(getht(), "main_obj", 8)) != NULL) {
        Z_ADDREF_P(value);
        obj->vm = value;
    }
    
    obj->ctx = duk_ctx;
    obj->idx = duk_require_normalize_index(duk_ctx, idx);
    duk_dup(duk_ctx, idx);
}
