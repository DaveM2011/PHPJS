#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

PHPAPI zend_class_entry *phpjs_JSObjectWrapper_ptr;
PHPAPI zend_object_handlers phpjs_JSObjectWrapper_handlers;

static zend_object* phpjs_object_new(zend_class_entry * ce TSRMLS_DC)
{
    phpjs_wrap_duk_t *obj = (phpjs_wrap_duk_t *)ecalloc(1, sizeof(phpjs_wrap_duk_t) + zend_object_properties_size(ce));
    zend_object_std_init(&obj->zobj, ce TSRMLS_CC);
    object_properties_init(&obj->zobj, ce);
	obj->zobj.handlers = &phpjs_JSObjectWrapper_handlers;
    return &obj->zobj;
}

ZEND_METHOD(JSObjectWrapper, __call)
{
    FETCH_THIS_WRAPPER(getThis());
    zval* a_args;
    char * fnc;
    int lfnc;
    int ind;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &fnc, &lfnc, &a_args) == FAILURE)
        return;
    
    duk_dup(ctx, obj->idx);
    phpjs_php__call(ctx, fnc, a_args, return_value TSRMLS_CC); 
    duk_pop(ctx);
}

ZEND_METHOD(JSObjectWrapper, __get)
{
    FETCH_THIS_WRAPPER(getThis());
    char * varname;
    int varname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE)
        return;
    
    duk_dup(ctx, obj->idx);
    duk_get_prop_string(ctx, -1, varname);
    duk_to_zval(return_value, ctx, -1);
    php_duk_free_return(ctx);
}

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___call, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

static zend_function_entry phpjs_JSObjectWrapper_functions[] = {
    ZEND_ME(JSObjectWrapper, __get, ai_phpjs_JS___get, ZEND_ACC_PUBLIC)
    ZEND_ME(JSObjectWrapper, __call, ai_phpjs_JS___call, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

void php_register_object_handler(TSRMLS_D)
{
    zend_class_entry _ce;
    INIT_CLASS_ENTRY(_ce, "JSObjectWrapper", phpjs_JSObjectWrapper_functions);
    phpjs_JSObjectWrapper_ptr = zend_register_internal_class(&_ce TSRMLS_CC);
    phpjs_JSObjectWrapper_ptr->create_object = phpjs_object_new;
	
	memcpy(&phpjs_JSObjectWrapper_handlers, zend_get_std_object_handlers(), sizeof(phpjs_JSObjectWrapper_handlers));
	phpjs_JSObjectWrapper_handlers.free_obj = phpjs_wrapped_free; /* This is the free handler */
    phpjs_JSObjectWrapper_handlers.offset = XtOffsetOf(phpjs_wrap_duk_t, zobj); /* Here, we declare the offset to the engine */
    //phpjs_JSObjectWrapper_handlers.dtor_obj = zend_objects_destroy_object; /* This is the dtor handler */
}
