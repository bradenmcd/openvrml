// This file is required for making dll entry (_declspec(dllexport) for win32)
// of js_InitArrayClass, as this is not defined in jsapi.c (interface between application
// program and javascript library). For making javascript library for libvrml97
// this file must be included in the project file.



#include "jsstddef.h"
#include "jsarray.h"
#include "jspubtd.h"
#include "jscntxt.h"
#include "jsobj.h"

#if defined(JS_PARANOID_REQUEST) && defined(JS_THREADSAFE)
#define CHECK_REQUEST(cx)	JS_ASSERT(cx->requestDepth)
#else
#define CHECK_REQUEST(cx)	((void)0)
#endif


extern JS_PUBLIC_API(JSObject *)
JS_InitArrayClass(JSContext *cx,JSObject *obj);


JS_PUBLIC_API(JSObject *)
JS_InitArrayClass(JSContext *cx,JSObject *obj)
{
    CHECK_REQUEST(cx);
    return  js_InitArrayClass(cx,obj);
}
