#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

PHPAPI zend_class_entry *phpjs_JSException_ptr;
PHPAPI zend_class_entry *phpjs_JS_ptr;
PHPAPI zend_object_handlers phpjs_JS_handlers;

typedef struct {
    duk_context *ctx;
	zend_object zobj;
} php_js_t;

static inline php_js_t * php_js_fetch_object(zend_object *obj) {
	return (php_js_t *)((char *)obj - XtOffsetOf(php_js_t, zobj));
}

#define Z_PHP_JS_OBJ_P(zv) php_js_fetch_object(Z_OBJ_P(zv));

ZEND_METHOD(JS, evaluate)
{
    FETCH_THIS;

    char * str;
    int len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &len) == FAILURE)
        return;

    if (duk_peval_lstring(ctx, str, len) != 0) {
        duk_php_throw(ctx, -1 TSRMLS_CC);
        RETURN_FALSE;
    }

    duk_to_zval(return_value, ctx, -1);
    duk_pop(ctx);
}

// public function load($path)
ZEND_METHOD(JS, __construct)
{
    php_js_t* obj = Z_PHP_JS_OBJ_P(getThis());
    obj->ctx = duk_create_heap(NULL, NULL, NULL, getThis(), NULL);
    duk_php_init(obj->ctx);
}

ZEND_METHOD(JS, load)
{
    FETCH_THIS;
    //char *varname = NULL;
    //size_t varname_len;
	zend_string *varname;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &varname) == FAILURE) {
        return;
    }

	zend_string *contents;
	php_stream *stream;
	long maxlen = PHP_STREAM_COPY_ALL;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	/*if (Z_TYPE_P(varname) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Invalid arguments supplied for zephir_file_get_contents()");
		RETURN_FALSE;
		return;
	}*/

	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(ZSTR_VAL(varname), "rb", 0 | REPORT_ERRORS, NULL, context);
	if (!stream) {
		RETURN_FALSE;
	}

	if ((contents = php_stream_copy_to_mem(stream, maxlen, 0)) != NULL) {
		if (duk_pcompile_string(ctx, 0, ZSTR_VAL(contents)) != 0) {
			php_printf("compile failed: %s\n", duk_safe_to_string(ctx, -1));
		} else {
			if (duk_pcall(ctx, 0) != 0) {
				php_printf("pcall failed: %s\n", duk_safe_to_string(ctx, -1));
			} else {
				//php_printf("program result: %s\n", duk_safe_to_string(ctx, -1));
			}
		}
		duk_pop(ctx);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
	
	php_stream_close(stream);
}

ZEND_METHOD(JS, offsetUnset)
{
}

ZEND_METHOD(JS, offsetExists)
{
    FETCH_THIS;
    zend_string * varname;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &varname) == FAILURE)
        return;

    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, ZSTR_VAL(varname));
    if (duk_get_type_mask(ctx, -3) & DUK_TYPE_UNDEFINED) {
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}

ZEND_METHOD(JS, offsetGet)
{
    FETCH_THIS;
    zend_string * varname;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &varname) == FAILURE)
        return;
	
    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, ZSTR_VAL(varname));
    duk_to_zval(return_value, ctx, -1);
}


// public function __get($name)
ZEND_METHOD(JS, __get)
{
    FETCH_THIS;
    zend_string * varname;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &varname) == FAILURE)
        return;
	
    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, ZSTR_VAL(varname));
    duk_to_zval(return_value, ctx, -1);
    php_duk_free_return(ctx);
}

// public function __set($name, $value)
ZEND_METHOD(JS, __set)
{
    FETCH_THIS;
    zval* a_value;
    char * name;
	size_t len;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &a_value) == FAILURE)
        return;

    zval_to_duk(ctx, name, a_value);
}

ZEND_METHOD(JS, offsetSet)
{
    FETCH_THIS;
    zval* a_value;
    char * name;
	size_t len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len, &a_value) == FAILURE)
        return;

    zval_to_duk(ctx, name, a_value);
}


// public function __call($name, $args)
ZEND_METHOD(JS, __call)
{
    FETCH_THIS;
    zval* a_args;
    char * fnc;
	size_t len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &fnc, &len, &a_args) == FAILURE)
        return;

    duk_push_global_object(ctx);
    phpjs_php__call(ctx, fnc, a_args, return_value TSRMLS_CC); 
}

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_evaluate, 0, 0, 1)
    ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetExists, 0, 0, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetGet, 0, 0, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetSet, 0, 0, 2)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_offsetUnset, 0, 0, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_load, 0, 0, 1)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS_export, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ai_phpjs_JS___call, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

    static zend_function_entry phpjs_JS_functions[] = {
        ZEND_ME(JS, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
        ZEND_ME(JS, evaluate, ai_phpjs_JS_evaluate, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, load, ai_phpjs_JS_load, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, offsetExists, ai_phpjs_JS_offsetExists, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, offsetGet, ai_phpjs_JS_offsetGet, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, offsetSet, ai_phpjs_JS_offsetSet, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, offsetUnset, ai_phpjs_JS_offsetUnset, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, __get, ai_phpjs_JS___get, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, __set, ai_phpjs_JS___set, ZEND_ACC_PUBLIC)
        ZEND_ME(JS, __call, ai_phpjs_JS___call, ZEND_ACC_PUBLIC)
        ZEND_FE_END
    };

static zend_function_entry phpjs_functions[] = {
    {NULL, NULL, NULL}
};


static void php_js_free_storage(zend_object *object)
{
	php_js_t *obj;
    obj = (php_js_t *)((char *)object - XtOffsetOf(php_js_t, zobj));

    /* Now we could do something with my_obj->my_custom_buffer, like sending it
       on a socket, or flush it to a file, or whatever, but not free it here */
	
	if (obj->ctx) {
        duk_destroy_heap(obj->ctx);
    }
    //zend_objects_destroy_object(object); /* call __destruct() from userland */
    zend_object_std_dtor(object);
    //efree(obj);
}

static zend_object *phpjs_new_vm(zend_class_entry *ce)
{
    //zend_object retval;
    php_js_t * obj;

    obj = (php_js_t *) ecalloc(1, sizeof(*obj) + zend_object_properties_size(ce));
    //memset(obj, 0, sizeof(php_js_t));
    
	zend_object_std_init(&obj->zobj, ce);
    object_properties_init((zend_object*) &(obj->zobj), ce);
	obj->zobj.handlers = &phpjs_JS_handlers;
	
	//obj->zo.handle = zend_objects_store_put(obj);
    //obj->zo.handlers = zend_get_std_object_handlers();

    //retval.handle = zend_objects_store_put(obj, (zend_objects_store)zend_objects_destroy_object, (zend_objects_free_object_storage_t)php_js_free_storage, NULL TSRMLS_CC);
    //retval.handlers = zend_get_std_object_handlers();

    return &obj->zobj;
}


PHP_MINIT_FUNCTION(phpjs)
{
    zend_class_entry _ce, *_if;
    zval* _val;

    INIT_CLASS_ENTRY(_ce, "JSException", NULL);
    phpjs_JSException_ptr = zend_register_internal_class_ex(&_ce, zend_exception_get_default(TSRMLS_C));
    zend_declare_property_string(phpjs_JSException_ptr, _S("js_stack"), "", ZEND_ACC_PROTECTED);

    INIT_CLASS_ENTRY(_ce, "JS", phpjs_JS_functions);
    phpjs_JS_ptr = zend_register_internal_class(&_ce);
    phpjs_JS_ptr->create_object = phpjs_new_vm;
    zend_do_implement_interface(phpjs_JS_ptr, zend_ce_arrayaccess);

	memcpy(&phpjs_JS_handlers, zend_get_std_object_handlers(), sizeof(phpjs_JS_handlers));
	phpjs_JS_handlers.free_obj = php_js_free_storage; /* This is the free handler */
    phpjs_JS_handlers.dtor_obj = zend_objects_destroy_object; /* This is the dtor handler */
    /* phpjs_ce_handlers.clone_obj is also available though we won't use it here */
    phpjs_JS_handlers.offset = XtOffsetOf(php_js_t, zobj); /* Here, we declare the offset to the engine */


    php_register_object_handler(TSRMLS_C);
    php_register_function_handler(TSRMLS_C);

    return SUCCESS;
}


PHP_MINFO_FUNCTION(phpjs)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "phpjs", "enabled");
    php_info_print_table_row(2, "Version", "1.0");
    php_info_print_table_end();
}

zend_module_entry phpjs_module_entry = {
    STANDARD_MODULE_HEADER,
    "phpjs",
    phpjs_functions,
    PHP_MINIT(phpjs),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(phpjs),
    "1.0",
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHPJS
ZEND_GET_MODULE(phpjs)
#endif
