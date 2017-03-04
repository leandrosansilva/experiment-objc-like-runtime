#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

struct Object;
struct Class_Object;

typedef struct Object* (*obj_selector)(struct Object*, va_list);

typedef void (*obj_class_initializer_callback)(struct Class_Object*);

void obj_add_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

void obj_add_class_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName);

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...);
struct Object* obj_send_message_to_super(struct Object* obj, const char* selectorName, ...);

struct Object* obj_send_message_with_arguments(struct Object* obj, const char* selectorName, va_list arguments);
struct Object* obj_send_message_to_super_with_arguments(struct Object* obj, const char* selectorName, va_list arguments);

void obj_unload_class(struct Class_Object* klass);

struct Class_Object* Class();

void obj_class_initializer(struct Class_Object* klass);

void obj_init_runtime();

void obj_shutdown_runtime();

void obj_initialize_class(struct Class_Object* klass, obj_class_initializer_callback initializer);

void obj_print_class_diagram();

void obj_set_class_parent(struct Class_Object* klass, struct Class_Object* parent);

struct Class_Object* obj_class_parent(struct Class_Object* klass);

void obj_set_class_name(struct Class_Object* klass, const char* name);

const char* obj_class_name(struct Class_Object* klass);

struct Class_Object* obj_class_for_object(struct Object* object);

size_t obj_number_of_call_arguments_ending_on_null(va_list arguments);

bool obj_object_is_class(struct Object* object);

#define RETAIN(OBJ) obj_send_message((OBJ), "retain")
#define RELEASE(OBJ) obj_send_message(obj_class_for_object(OBJ), "release", &OBJ)
