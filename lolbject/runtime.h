#pragma once

#include <lolbject/api.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct Lolbject;
struct LolClass;
struct LolModule;

LOL_EXPORT struct LolClass* LolModule;
LOL_EXPORT struct LolClass* LolRuntime;
LOL_EXPORT struct LolClass* LolModule;
LOL_EXPORT struct LolClass* Lolbject;

struct LolClass_Descriptor;

typedef void (*lolbj_class_initializer_callback)(struct LolClass*);
typedef void (*lolbj_class_unloader_callback)(struct LolClass*);

struct LolModule_Descriptor
{
	uint16_t version;
	const char* name;
	void (*init_module)(struct LolModule*);
	void (*shutdown_module)(struct LolClass*);
};

struct LolClass_Descriptor
{
	const char* name;
	uint16_t version;
	lolbj_class_initializer_callback initializer;
	lolbj_class_unloader_callback unloader;
};

typedef struct Lolbject* (*lolbj_selector)(struct Lolbject*, va_list);

void lolbj_add_selector(struct LolClass* klass, const char* selectorName, lolbj_selector selector);

void lolbj_add_class_selector(struct LolClass* klass, const char* selectorName, lolbj_selector selector);

lolbj_selector lolbj_selector_for_name(struct LolClass* klass, const char* selectorName);

struct Lolbject* lolbj_send_message(struct Lolbject* obj, const char* selectorName, ...);
struct Lolbject* lolbj_send_message_to_super(struct Lolbject* obj, struct LolClass* klass, const char* selectorName, ...);

struct Lolbject* lolbj_send_message_with_arguments(struct Lolbject* obj, const char* selectorName, va_list arguments);
struct Lolbject* lolbj_send_message_to_super_with_arguments(struct Lolbject* obj, struct LolClass* klass, const char* selectorName, va_list arguments);

void lolbj_init_runtime();

void lolbj_shutdown_runtime();

void lolbj_print_class_diagram();

void lolbj_set_class_parent(struct LolClass* klass, struct LolClass* parent);

struct LolClass* lolbj_class_parent(struct LolClass* klass);

void lolbj_set_class_name(struct LolClass* klass, const char* name);

const char* lolbj_class_name(struct LolClass* klass);

struct LolClass* lolbj_class_for_object(struct Lolbject* object);

size_t lolbj_number_of_call_arguments_ending_on_null(va_list arguments);

bool lolbj_object_is_class(struct Lolbject* object);

struct Lolbject* lolbj_get_object_property(struct Lolbject* object, const char* propertyName);
struct Lolbject* lolbj_set_object_property(struct Lolbject* object, const char* propertyName, struct Lolbject* value);

void lolbj_add_property(struct LolClass* klass, const char* propertyName, struct LolClass* type, size_t offset, ...);

struct Lolbject* lolbj_cast(struct LolClass* klass, struct Lolbject* obj);
