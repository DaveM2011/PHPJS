#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

void phpjs_php__call(duk_context * ctx, char * fnc, zval * a_args, zval * return_value TSRMLS_DC)
{
    duk_get_prop_string(ctx, -1, fnc);

    //debug_print("phpjs_php__call(%s, %d)\n", fnc, duk_is_function(ctx, -1));

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
        //debug_print("__call failed: %s, %d\n", fnc, argc);
        duk_php_throw(ctx, -1 TSRMLS_CC);
        RETURN_FALSE;
    }

    duk_to_zval(return_value, ctx, -1);
    
    //debug_print("__call: %s\n", duk_safe_to_string(ctx, -1));
    //php_debug_zval_dump(return_value, 0);

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
    //FETCH_THIS_WRAPPER(object);
	phpjs_wrap_duk_t *obj = (phpjs_wrap_duk_t *)((char *)object - XtOffsetOf(phpjs_wrap_duk_t, zobj));
    //debug_print("phpjs_wrapped_free %d, %d, %d\n", obj, obj->ctx, obj->vm);

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
    //phpjs_wrap_duk_t * obj = (phpjs_wrap_duk_t *) ((char*)Z_OBJ_P(_this) - XtOffsetOf(phpjs_wrap_duk_t, zobj));
    //duk_memory_functions mem;
    //duk_get_memory_functions(duk_ctx, &mem);

    //php_js_t *main_obj = mem.udata;
    //zval *main_obj = (zval *)mem.udata;

    //debug_print("phpjs_add_duk_context ptr: %d, ref: %d\n", main_obj, Z_REFCOUNTED_P(main_obj) ? Z_REFCOUNT_P(main_obj) : 0);
    //Z_ADDREF_P(main_obj);

    zval * value;
    TSRMLS_FETCH();
    if((value = zend_hash_str_find(getht(), "main_obj", 8)) != NULL) {
        //debug_print("phpjs_add_duk_context ptr: %d, ref: %d\n", value, Z_REFCOUNTED_P(value) ? Z_REFCOUNT_P(value) : 0);
        Z_ADDREF_P(value);
        obj->vm = value;
    }
    
    //ZVAL_MAKE_REF(mem.udata);
    //ZVAL_COPY_VALUE(mem.udata, obj->vm);
    obj->ctx = duk_ctx;
    obj->idx = duk_require_normalize_index(duk_ctx, idx);
    //debug_print("phpjs_add_duk_context %d, %d, %d, %d\n", obj, duk_ctx, obj->vm, Z_REFCOUNTED_P(obj->vm));
    //debug_print("phpjs_add_duk_context %d\n", idx);
    duk_dup(duk_ctx, idx);
    //debug_print("phpjs_add_duk_context %d, %d, %d, %s\n", obj->vm, idx, obj->idx, duk_safe_to_string(duk_ctx, obj->idx));
    //Z_ADDREF_P(obj->vm);
    //Z_ADDREF_P(obj->vm);
    //if (Z_REFCOUNTED_P(obj->vm)) {
        //debug_print("phpjs_add_duk_context addref\n");
        //(obj->vm);
    //}
    //debug_print("phpjs_add_duk_context %d\n", obj->idx);
}
