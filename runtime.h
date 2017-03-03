#pragma once

#include <stdarg.h>
#include <stdlib.h>

struct Object;
struct Class_Object;

typedef struct Object* (*obj_selector)(struct Object*, va_list);

typedef void (*obj_class_initializer_callback)(struct Class_Object*);

void obj_add_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

void obj_add_class_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName);

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...);

struct Object* obj_send_message_to_super(struct Object* obj, const char* selectorName, ...);

void obj_unload_class(struct Class_Object* klass);

struct Class_Object* Class();

void obj_class_initializer(struct Class_Object* klass);

void obj_init_runtime();

void obj_shutdown_runtime();

void obj_initialize_class(struct Class_Object* klass, obj_class_initializer_callback initializer);

void obj_print_class_diagram();

void obj_set_class_parent(struct Class_Object* klass, struct Class_Object* parent);

void obj_set_class_name(struct Class_Object* klass, const char* name);

const char* obj_class_name(struct Class_Object* klass);

struct Class_Object* obj_class_for_object(struct Object* object);

#define RETAIN(OBJ) obj_send_message(OBJ, "retain")
#define RELEASE(OBJ) obj_send_message(Object(), "release", &OBJ)
