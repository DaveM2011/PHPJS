#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "duktape.h"
#include "php_phpjs.h"

static HashTable *intern_hash = NULL;

HashTable *getht() {
    if (intern_hash == NULL) {
        ALLOC_HASHTABLE(intern_hash);
        zend_hash_init(intern_hash, 0, NULL, ZVAL_PTR_DTOR, 0);
    }
    return intern_hash;
}

static void dump_object(duk_context *ctx, duk_idx_t idx) {
	idx = duk_require_normalize_index(ctx, idx);

	/* The weird fn() helper is to handle lightfunc name printing (= avoid it). */
	duk_eval_string(ctx,
	    "(function (o) {\n"
	    "    Object.getOwnPropertyNames(o).forEach(function (k) {\n"
	    "        var pd = Object.getOwnPropertyDescriptor(o, k);\n"
	    "        function fn(x) { if (x.name !== 'getter' && x.name !== 'setter') { return 'func' }; return x.name; }\n"
	    "        print(Duktape.enc('jx', k), Duktape.enc('jx', pd), (pd.get ? fn(pd.get) : 'no-getter'), (pd.set ? fn(pd.set) : 'no-setter'));\n"
	    "    });\n"
	    "})");
	duk_dup(ctx, idx);
	duk_call(ctx, 1);
	duk_pop(ctx);
}

duk_ret_t duk_php_print(duk_context * ctx)
{
    int args = duk_get_top(ctx);
    int i;

    for (i = 0; i < args; i++) {
		//duk_int_t t = duk_get_type(ctx, i);
		//if (t == DUK_TYPE_UNDEFINED || t == DUK_TYPE_NONE)
			//continue;
        php_printf(i == args - 1 ? "%s\n" : "%s ", duk_safe_to_string( ctx, i ));
    }

    duk_push_true(ctx);

    return 1;
}

duk_ret_t myobject_constructor(duk_context *ctx) {
    /* Stack at entry is: [ name ] */

    /* All Duktape/C functions can be called both as constructors
     * ("new func()") and functions ("func()").  Sometimes objects
     * allow both calls, sometimes not.  Here we reject a normal
     * non-constructor call.
     */
    if (!duk_is_constructor_call(ctx)) {
        return DUK_RET_TYPE_ERROR;
    }

    /* Get access to the default instance. */
    duk_push_this(ctx);  /* -> stack: [ name this ] */

    /* Set this.name to name. */
    duk_dup(ctx, 0);  /* -> stack: [ name this name ] */
    duk_put_prop_string(ctx, -2, "name");  /* -> stack: [ name this ] */

    /* Return undefined: default instance will be used. */
    return 0;
}


void duk_php_init(duk_context * ctx)
{
    duk_push_global_object(ctx);
    duk_push_c_function(ctx, duk_php_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "print");

    duk_push_c_function(ctx, duk_set_into_php, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "__set_into_php");
    duk_push_c_function(ctx, duk_get_from_php, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "__get_from_php");
    duk_pop(ctx);
    //duk_dump_context(ctx);

    /*duk_get_prop_string(ctx, -1, "Proxy");
    duk_push_object(ctx);
    duk_push_object(ctx);
    duk_push_c_lightfunc(ctx, duk_set_into_php, 1, 1, 0);
    duk_put_prop_string(ctx, -2, "set");
    duk_push_c_lightfunc(ctx, duk_get_from_php, 1, 1, 0);
    duk_put_prop_string(ctx, -2, "get");
    duk_new(ctx, 2);
    duk_put_prop_string(ctx, -2, "PHP");
    //duk_put_global_string(ctx, "PHP");
    //duk_push_c_function(ctx, myobject_constructor, 1);
    dump_object(ctx, -1);*/


    /* Push MyObject.prototype object. */
    //duk_push_object(ctx);  /* -> stack: [ MyObject proto ] */

    /* Set MyObject.prototype.printName. */
    //duk_push_c_function(ctx, duk_php_print, 0 /*nargs*/);
    //duk_put_prop_string(ctx, -2, "print");

    /* Set MyObject.prototype = proto */
    //duk_put_prop_string(ctx, -2, "prototype");  /* -> stack: [ MyObject ] */

    /*duk_push_c_function(ctx, duk_get_from_php, DUK_VARARGS);
    duk_push_string(ctx, "name");
    duk_push_string(ctx, "getter");
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE);
    duk_push_c_function(ctx, duk_set_into_php, DUK_VARARGS);
    duk_push_string(ctx, "name");
    duk_push_string(ctx, "setter");
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE);*/
    //duk_dump_context(ctx);

    //duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_CONFIGURABLE | DUK_DEFPROP_CONFIGURABLE |
    //    DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER);

    /* Finally, register MyObject to the global object */
    //duk_put_global_string(ctx, "PHP");
    
    //duk_pop(ctx);
    //duk_dump_context(ctx);
    //duk_push_global_object(ctx);
    //duk_push_c_function(ctx, duk_set_into_php, DUK_VARARGS);
    //duk_put_prop_string(ctx, -2, "__set_into_php");
    //duk_pop(ctx);
    //duk_dump_context(ctx);
    //duk_push_global_object(ctx);
    //duk_push_c_function(ctx, duk_get_from_php, DUK_VARARGS);
    //duk_put_prop_string(ctx, -2, "__get_from_php");
    //duk_dump_context(ctx);
    //duk_dump_context(ctx);
    //duk_pop(ctx);
    //duk_push_global_object(ctx);
    //duk_dump_context(ctx);

    //duk_push_object(ctx);
    //duk_push_string(ctx, "PHP");
    //duk_push_c_function(ctx, duk_get_from_php, DUK_VARARGS);
    //duk_push_c_function(ctx, duk_set_into_php, DUK_VARARGS);
    //duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_GETTER |
    //         DUK_DEFPROP_HAVE_SETTER |
    //         DUK_DEFPROP_HAVE_CONFIGURABLE |
    //         DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE);
    //duk_dump_context(ctx);
    //duk_put_global_string(ctx, "PHP");

    duk_push_string(ctx, "var PHP=new Proxy({},{set:__set_into_php,get:__get_from_php})");

    if (duk_peval(ctx) != 0) {
        php_printf("eval failed: %s\n", duk_safe_to_string(ctx, -1));
    }
    //duk_pop(ctx);
}

void duk_php_throw(duk_context * ctx, duk_idx_t idx TSRMLS_DC)
{
    char * js_stack, * message;
    zval tc_ex;

    if (object_init_ex(&tc_ex, phpjs_JSException_ptr) != SUCCESS) {
        return;
    }

    duk_get_prop_string(ctx, idx, "stack");
    js_stack = estrdup(duk_safe_to_string(ctx, -1));
    duk_pop(ctx);

    message = estrdup(duk_safe_to_string(ctx, idx));
    duk_pop(ctx);

    SET_PROP(&tc_ex, phpjs_JSException_ptr, "message", message);
    SET_PROP(&tc_ex, phpjs_JSException_ptr, "js_stack", js_stack);

    zend_throw_exception_object(&tc_ex TSRMLS_CC);
    
    efree(js_stack);
    efree(message);
}

duk_idx_t duk_push_php_array_or_object(duk_context * ctx, HashTable * myht)
{
    zend_string *str_index;
    zend_ulong str_length;
    zend_ulong num_index;

	ZEND_HASH_FOREACH_KEY(myht, num_index, str_index)
		if (str_index) { //HASH_KEY_IS_STRING
			return duk_push_object(ctx);
		}
	ZEND_HASH_FOREACH_END();
    
    return duk_push_array(ctx);
}

static int duk_is_php_object(duk_context * ctx, duk_idx_t idx)
{
    if (duk_is_array(ctx, idx)) {
        return 0;
    }

    int cmp;

    duk_get_prop_string(ctx, idx, "constructor");
    duk_get_prop_string(ctx, -1, "name");
    cmp = strcmp("Object", duk_safe_to_string(ctx, -1));
    duk_pop_2(ctx);

    return cmp != 0;
}

void zval_to_duk(duk_context * ctx, char * name, zval * value)
{
    switch (Z_TYPE_P(value)) {
    case IS_ARRAY: {
        zval * data;
        HashTable *myht = Z_ARRVAL_P(value);
        zend_string *str_index;
        zend_ulong str_length;
        zend_ulong num_index;
		
        duk_idx_t arr_idx = duk_push_php_array_or_object(ctx, myht);

		ZEND_HASH_FOREACH_KEY_VAL(myht, num_index, str_index, data)
			zval_to_duk(ctx, NULL, data);
			if (str_index) { //HASH_KEY_IS_STRING
				duk_put_prop_string(ctx, arr_idx, ZSTR_VAL(str_index));
			} else {
				duk_put_prop_index(ctx, arr_idx, num_index);
			}
		ZEND_HASH_FOREACH_END();
		
        break;
    }
    case IS_STRING:
        duk_push_string(ctx, Z_STRVAL_P(value));
        break;
    case IS_LONG:
        duk_push_int(ctx, Z_LVAL_P(value));
        break;
    case IS_DOUBLE:
        duk_push_number(ctx, Z_DVAL_P(value));
        break;
    case IS_BOOL:
        if (Z_TYPE_P(value) == IS_TRUE || (Z_TYPE_P(value) == IS_LONG && Z_LVAL_P(value))) {
            duk_push_true(ctx);
        } else {
            duk_push_false(ctx);
        }
        break;
    case IS_NULL:
        duk_push_false(ctx);
        break;
	default:
		duk_push_undefined(ctx);
		break;
    }

    if (name) {
        // if there is a name, then create a variable
        // otherwise just put the value to the stack (most likely to return;)
        duk_put_global_string(ctx, name);
    }
}

void duk_to_zval(zval * var, duk_context * ctx, duk_idx_t idx)
{
    duk_size_t len;
    const char * str;
    
    switch (duk_get_type(ctx, idx)) {
    case DUK_TYPE_UNDEFINED:
    case DUK_TYPE_NULL:
    case DUK_TYPE_NONE:
        ZVAL_NULL(var);
        break;

    case DUK_TYPE_OBJECT: {
        if (duk_is_function(ctx, idx)) {
            TSRMLS_FETCH();
            object_init_ex(var, phpjs_JSFunctionWrapper_ptr);
            phpjs_add_duk_context(var, ctx, idx TSRMLS_CC);
            break;
        } else if (duk_is_php_object(ctx, idx)) {
            TSRMLS_FETCH();
            object_init_ex(var, phpjs_JSObjectWrapper_ptr);
            phpjs_add_duk_context(var, ctx, idx TSRMLS_CC);
            break;
        }

        // It's a hash or an array (AKA a PHP array)
        duk_idx_t idx1;
        duk_enum(ctx, idx, DUK_ENUM_OWN_PROPERTIES_ONLY);
        idx1 = duk_normalize_index(ctx, -1);
        array_init(var);

        while (duk_next(ctx, idx1, 1 /*get_value*/)) {
            zval value;
            duk_to_zval(&value, ctx, -1);

            if(duk_get_type(ctx, -2) == DUK_TYPE_NUMBER) {
                add_index_zval(var, duk_get_number(ctx,-2), &value);
            } else {
                add_assoc_zval(var, duk_get_string(ctx, -2), &value);
            }

            duk_pop_2(ctx);  /* pop_key */
        }
        duk_pop(ctx);
        break;
    }

    case DUK_TYPE_NUMBER:
        ZVAL_DOUBLE(var, duk_get_number(ctx, idx));
        break;

    case DUK_TYPE_BOOLEAN:
        if (duk_get_number(ctx, idx)) {
            ZVAL_TRUE(var);
        } else {
            ZVAL_FALSE(var);
        }
        break;

    case DUK_TYPE_STRING:
        str = duk_get_lstring(ctx, idx, &len);
        ZVAL_STRINGL(var, str, len);
        break;
    }
}

duk_ret_t php_get_function_wrapper(duk_context * ctx)
{
    char * fnc = "";
    zval retval, func;
    int catch = 0, i = 0;
    int args = duk_get_top(ctx); /* function args */
	char *error;
    int check_flags = 0;
    
    zval params[args];
	for(i = 0; i < args; i++) {
		zval val;
		duk_to_zval(&val, ctx, i);
		params[i] = val;
	}
    
    /* get function name (it's a __function property) */
    duk_push_current_function(ctx);
	duk_get_prop_string(ctx, -1, "__function");
    duk_to_zval(&func, ctx, -1);
    duk_pop(ctx);

	/* we got already the function name */
    TSRMLS_FETCH();
	
    if(call_user_function_ex(NULL, NULL, &func, &retval, args, params, 0, NULL) != SUCCESS) {
		duk_push_error_object(ctx, DUK_ERR_ERROR, "Unknown PHP function: \"%s\"", Z_STRVAL_P(&func));
		duk_throw(ctx);
    }
    if (EG(exception) != NULL) {
        catch = 1;
        
        zval tmp, *msg, rv;
        ZVAL_OBJ(&tmp, EG(exception));
        msg = zend_read_property(zend_ce_exception, &tmp, "message", sizeof("message")-1, 0, &rv);
        duk_push_string(ctx, Z_STRVAL_P(msg));

        zval_ptr_dtor(&tmp);
        EG(exception) = NULL;

        // There was an exception in the PHP side, let's catch it and throw as a JS exception
        zend_clear_exception(TSRMLS_C);
    }

    zval_ptr_dtor(&func);
    zval_ptr_dtor(&retval);
    
    if (catch) duk_throw(ctx);
    return 1;
}

duk_ret_t duk_set_into_php(duk_context * ctx)
{
    zval value;
    duk_size_t namelen;
    const char *name = estrdup(duk_get_lstring(ctx, 1, &namelen) + 1);
    duk_to_zval(&value, ctx, 2);
    
    TSRMLS_FETCH();
    zend_hash_str_add(getht(), name, namelen - 1, &value);
    duk_push_true(ctx);

    return 1;
}

duk_ret_t duk_get_from_php(duk_context * ctx)
{
    duk_size_t len;
    int args = duk_get_top(ctx);
    const char *name = duk_get_lstring(ctx, 1, &len);
    if (name[0] == '$') {
        name++;
        len--;
		zval * value;
		TSRMLS_FETCH();

        if((value = zend_hash_str_find(getht(), name, len)) != NULL) {
            zval_to_duk(ctx, NULL, value);
        } else {
			duk_push_undefined(ctx);
        }
    } else {
        // they expect a function wrapper
        duk_push_c_function(ctx, php_get_function_wrapper, DUK_VARARGS);
		duk_push_string(ctx, name);
        duk_put_prop_string(ctx, -2, "__function");
    }

    return 1;
}
