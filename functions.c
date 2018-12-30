#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

PHPAPI zend_class_entry *phpjs_JSFunctionWrapper_ptr;
PHPAPI zend_object_handlers phpjs_JSFunctionWrapper_handlers;

static zend_object* phpjs_function_new(zend_class_entry * ce TSRMLS_DC)
{
    phpjs_wrap_duk_t * obj = (phpjs_wrap_duk_t *) emalloc(sizeof(phpjs_wrap_duk_t));
    zend_object_std_init(&obj->zobj, ce TSRMLS_CC);
    object_properties_init(&obj->zobj, ce);
    obj->zobj.handlers = &phpjs_JSFunctionWrapper_handlers;
    return &obj->zobj;
}

ZEND_METHOD(JSFunctionWrapper, __invoke)
{
    int argc = 0;
    FETCH_THIS_WRAPPER(getThis());

    duk_dup(ctx, obj->idx);
    if (duk_pcall(ctx, argc) != 0) {
        duk_php_throw(ctx, -1 TSRMLS_CC);
        RETURN_FALSE;
    }
    duk_to_zval(return_value, obj->ctx, -1);
    php_duk_free_return(ctx);

}

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JSFunctionWrapper___invoke, 0, 0, 0)
ZEND_END_ARG_INFO()

static zend_function_entry phpjs_JSFunctionWrapper_functions[] = {
    ZEND_ME(JSFunctionWrapper, __invoke, ai_phpjs_JSFunctionWrapper___invoke, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

void php_register_function_handler(TSRMLS_D)
{
    zend_class_entry _ce;
    INIT_CLASS_ENTRY(_ce, "JSFunctionWrapper", phpjs_JSFunctionWrapper_functions);
    phpjs_JSFunctionWrapper_ptr = zend_register_internal_class(&_ce TSRMLS_CC);
    phpjs_JSFunctionWrapper_ptr->create_object = phpjs_function_new;
	
	memcpy(&phpjs_JSFunctionWrapper_handlers, zend_get_std_object_handlers(), sizeof(phpjs_JSFunctionWrapper_handlers));
	//phpjs_js_handlers.dtor_obj = zend_objects_destroy_object; /* This is the dtor handler */
    phpjs_JSFunctionWrapper_handlers.offset = XtOffsetOf(phpjs_wrap_duk_t, zobj); /* Here, we declare the offset to the engine */
	phpjs_JSFunctionWrapper_handlers.free_obj = phpjs_wrapped_free; /* This is the free handler */

}
