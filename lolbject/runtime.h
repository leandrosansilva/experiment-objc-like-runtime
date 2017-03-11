#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct Lolbject;
struct LolClass;
struct LolModule;

extern struct LolClass* Class;

struct LolClass_Descriptor;

typedef void (*obj_class_initializer_callback)(struct LolClass*);
typedef void (*obj_class_unloader_callback)(struct LolClass*);

// FIXME: specify properly
struct LolModule_Descriptor
{
	uint16_t version;
	const char* name;
	void (*init_module)();
	void (*shutdown_module)();
};

struct LolClass_Descriptor
{
	const char* name;
	uint16_t version;
	obj_class_initializer_callback initializer;
	obj_class_unloader_callback unloader;
};

typedef struct Lolbject* (*obj_selector)(struct Lolbject*, va_list);

struct LolModule* obj_create_module(struct LolModule_Descriptor* descriptor);

// What about there was an error registring it?
void obj_register_module(struct LolModule* module);

struct LolClass* obj_register_class_with_descriptor(struct LolModule* module, struct LolClass_Descriptor *descriptor);

struct LolModule* obj_load_module_from_file(const char* filename);

void obj_add_selector(struct LolClass* klass, const char* selectorName, obj_selector selector);

void obj_add_class_selector(struct LolClass* klass, const char* selectorName, obj_selector selector);

obj_selector obj_selector_for_name(struct LolClass* klass, const char* selectorName);

struct Lolbject* obj_send_message(struct Lolbject* obj, const char* selectorName, ...);
struct Lolbject* obj_send_message_to_super(struct Lolbject* obj, const char* selectorName, ...);

struct Lolbject* obj_send_message_with_arguments(struct Lolbject* obj, const char* selectorName, va_list arguments);
struct Lolbject* obj_send_message_to_super_with_arguments(struct Lolbject* obj, const char* selectorName, va_list arguments);

void obj_unload_class(struct LolClass* klass);

void obj_class_initializer(struct LolClass* klass);

void obj_init_runtime();

void obj_shutdown_runtime();

void obj_print_class_diagram();

void obj_set_class_parent(struct LolClass* klass, struct LolClass* parent);

struct LolClass* obj_class_parent(struct LolClass* klass);

void obj_set_class_name(struct LolClass* klass, const char* name);

const char* obj_class_name(struct LolClass* klass);

struct LolClass* obj_class_for_object(struct Lolbject* object);

size_t obj_number_of_call_arguments_ending_on_null(va_list arguments);

bool obj_object_is_class(struct Lolbject* object);

struct Lolbject* obj_get_object_property(struct Lolbject* object, const char* propertyName);
struct Lolbject* obj_set_object_property(struct Lolbject* object, const char* propertyName, struct Lolbject* value);

void obj_add_property(struct LolClass* klass, const char* propertyName, struct LolClass* type, size_t offset);

void obj_add_selector_from_property(struct LolClass* klass, struct LolClass* memberClass, const char* memberName, const char* selectorName); 

struct LolClass* obj_class_with_name(struct LolModule* module, const char* klassName);

struct LolModule* obj_module_with_name(const char* name);

struct LolModule* obj_core_module();
