#pragma once

#include <stdarg.h>
#include <stdlib.h>

struct Object;
struct Class_Object;

typedef struct Object* (*obj_selector)(struct Object*, va_list);

struct ObjectSelectorPair;

enum obj_runtime_type
{
	obj_runtime_type_class = 0x01,
	obj_runtime_type_object = 0x02
};

typedef unsigned char obj_runtime_type;

typedef void (*obj_class_initializer_callback)(struct Class_Object*);

void obj_add_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

void obj_add_class_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName);

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...);

void obj_unload_class(struct Class_Object* klass);

struct Class_Object* Class();

void obj_class_initializer(struct Class_Object* klass);
