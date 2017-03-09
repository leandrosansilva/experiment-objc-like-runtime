#define _DEFAULT_SOURCE 1

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include <lolbject/runtime.h>
#include <lolbject/Class.h>
#include <lolbject/Object.h>
#include "Object_Private.h"
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/Object.h>
#include <lolbject/Box.h>
#include <lolbject/Array.h>

struct ObjectMemberPair {
	const char* selectorName;
	const char* propertyName;
}; 

struct ObjectMembers
{
	struct ObjectMemberPair values[1024];
	size_t count;
};

struct LolClass_Private
{
	struct LolClass* parent;
	const char* name;
	struct ObjectSelectorPair* selectors;
	struct ObjectPropertyPair* properties;
	struct ObjectMembers members;
};

// NOTE: the property list is also a linked list.
struct ObjectPropertyPair
{
	const char* name;
	size_t offset;
	struct LolClass* klass;
	struct ObjectPropertyPair* next;
};

static struct LolClass _Class;

struct LolClass* Class()
{
	return &_Class;
}

// NOTE: linked list. No need to say how inefficient it is :-)
// It should be replaced by an array based (to minimize cache misses) hash map,
// but who cares?
struct ObjectSelectorPair
{
	const char* selectorName;
	obj_selector selector;
	struct ObjectSelectorPair* next;
};

struct LolClass_List
{
	struct LolClass* klass;
	struct LolClass_List* next;
};

static struct LolClass_List* list_of_registred_classes;

void obj_init_runtime()
{
	list_of_registred_classes = NULL;

	obj_initialize_class(Class(), obj_class_initializer);
	obj_initialize_class(Object(), obj_object_initializer);
	obj_initialize_class(String(), obj_string_initializer);
	obj_initialize_class(Number(), obj_number_initializer);
	obj_initialize_class(Box(), obj_box_initializer);
	obj_initialize_class(Array(), obj_array_initializer);
}

void obj_shutdown_runtime()
{
	obj_unload_class(String());
	obj_unload_class(Number());
	obj_unload_class(Object());
	obj_unload_class(Class());

	// unregister all classes...
	struct LolClass_List* tmp;

	while(list_of_registred_classes != NULL) {
		tmp = list_of_registred_classes;
		list_of_registred_classes = list_of_registred_classes->next;
		free(tmp);
	}
}

static void print_diagram_for_class(FILE* p, struct LolClass* klass)
{
	fprintf(p, "  class_addr_%lld [\n    label = \"{%s| ", (unsigned long long)klass, klass->priv->name);

	struct LolClass* typeKlass = klass->proto.klass;

	if (typeKlass != NULL && klass != Class()) {
		for (struct ObjectSelectorPair* pair = typeKlass->priv->selectors; pair != NULL; pair = pair->next) {
			fprintf(p, "+ %s \\<\\<static\\>\\>\\l", pair->selectorName);
		}
	}

	for (struct ObjectSelectorPair* pair = klass->priv->selectors; pair != NULL; pair = pair->next) {
		fprintf(p, "+ %s\\l", pair->selectorName);
	}

	if (klass->priv->properties != NULL) {
		fprintf(p, "|\n");

		for (struct ObjectPropertyPair* pair = klass->priv->properties; pair != NULL; pair = pair->next) {
			fprintf(p, "- %s: %s\\l", pair->name, obj_class_name(pair->klass));
		}
	}

	fprintf(p, "}\"\n  ]\n");

	// The "type" class
	fprintf(p, "  class_addr_%lld -> class_addr_%lld [arrowhead = \"vee\"]\n",
			(unsigned long long)klass,
			(unsigned long long)obj_class_for_object(klass));
	
	// Properties
	for (struct ObjectPropertyPair* pair = klass->priv->properties; pair != NULL; pair = pair->next) {
		struct LolClass* memberClass = pair->klass;	
		fprintf(p, "  class_addr_%lld -> class_addr_%lld [arrowtail = \"odiamond\", dir=back]\n",
			(unsigned long long)klass,
			(unsigned long long)memberClass);
	}

	if (obj_class_parent(klass) != NULL) {
		// The inheritance arrow
		fprintf(p, "  class_addr_%lld -> class_addr_%lld [arrowhead = \"empty\"]\n",
			(unsigned long long)klass,
			(unsigned long long)obj_class_parent(klass));
	}
}

void obj_print_class_diagram()
{
	FILE* p = popen("xdot -", "w");

	if (p == NULL) {
		return;
	}

	fprintf(p, "digraph G {\n");
	fprintf(p,
"  fontname = \"Bitstream Vera Sans\"\n"
"  fontsize = 8\n"
"  node [\n"
"    fontname = \"Bitstream Vera Sans\"\n"
"    fontsize = 8\n"
"    shape = \"record\"\n"
"  ]\n"
"  edge [\n"
"    fontname = \"Bitstream Vera Sans\"\n"
"    fontsize = 8\n"
"  ]\n");
	for (struct LolClass_List* l = list_of_registred_classes; l != NULL; l = l->next) {
		print_diagram_for_class(p, l->klass);
	}
	fprintf(p, "}\n");

	pclose(p);
}

void obj_add_selector(struct LolClass* klass, const char* selectorName, obj_selector selector)
{
	struct ObjectSelectorPair *pair = malloc(sizeof(struct ObjectSelectorPair));
	pair->selectorName = selectorName;
	pair->selector = selector;

	// insert at the beginning of the list
	pair->next = klass->priv->selectors;
	klass->priv->selectors = pair;
}

void obj_add_class_selector(struct LolClass* klass, const char* selectorName, obj_selector selector)
{
	obj_add_selector(klass->proto.klass, selectorName, selector);
}

obj_selector obj_selector_for_name(struct LolClass* klass, const char* selectorName)
{
	// NOTE: this is the worst implementation of method lookup ever!
	for (struct LolClass* k = klass; k != NULL; k = obj_class_parent(k)) {
		for (struct ObjectSelectorPair* pair = k->priv->selectors; pair != NULL; pair = pair->next) {
			if (strcmp(pair->selectorName, selectorName) == 0) {
				return pair->selector;
			}
		}
	}

	return NULL;
}

struct Object* obj_self_for_selector(struct Object* obj, struct LolClass* klass, const char* selectorName)
{
	for (size_t i = 0; i < klass->priv->members.count; i++) {
		struct ObjectMemberPair pair = klass->priv->members.values[i];
		if (strcmp(pair.selectorName, selectorName) == 0) {
			return obj_get_object_property(obj, pair.propertyName);
		}
	}

	return obj;
}

static struct Object* privSendMessageWithArguments(struct Object* obj, struct LolClass* klass, const char* selectorName, va_list arguments)
{
	obj_selector selector = obj_selector_for_name(klass, selectorName);

	// TODO: maybe return SelectorNotFound()?
	if (selector == NULL) {
		return NULL;
	}

	struct Object* self = obj_self_for_selector(obj, klass, selectorName);

	struct Object* result = selector(self, arguments);

	return result;
}

struct Object* obj_send_message_with_arguments(struct Object* obj, const char* selectorName, va_list arguments)
{
	return privSendMessageWithArguments(obj, obj ? obj->klass : NULL, selectorName, arguments);
}

struct Object* obj_send_message_to_super_with_arguments(struct Object* obj, const char* selectorName, va_list arguments)
{
	struct LolClass* klass = obj_class_parent(obj_class_for_object(obj));
	return privSendMessageWithArguments(obj, klass, selectorName, arguments);
}

struct Object* obj_send_message_to_super(struct Object* obj, const char* selectorName, ...)
{
	va_list arguments;
	va_start(arguments, selectorName);
	struct Object* result = obj_send_message_to_super_with_arguments(obj, selectorName, arguments);
	va_end(arguments);

	return result;
}

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...)
{
	va_list arguments;
	va_start(arguments, selectorName);
	struct Object* result = obj_send_message_with_arguments(obj, selectorName, arguments);
	va_end(arguments);

	return result;
}

void obj_class_initializer(struct LolClass* klass)
{
	obj_set_class_name(klass, "Class");
	obj_set_class_parent(klass, NULL);
}

static void classStaticInitializer(struct LolClass* klass)
{
	obj_set_class_parent(klass, Class());
	obj_set_class_name(klass, NULL);
}

static void privInitializeClass(struct LolClass* klass, obj_class_initializer_callback initializer, bool createStatic);

static struct LolClass* createClassWithStaticMethods(struct LolClass* klass, bool shouldCreate)
{
	if (klass == Object()) {
		return Class();
	}

	if (!shouldCreate) {
		return Object();
	}

	struct LolClass* class_with_class_methods = malloc(sizeof(struct LolClass));
	privInitializeClass(class_with_class_methods, classStaticInitializer, false);

	return class_with_class_methods;
}

static void privInitializeClass(struct LolClass* klass, obj_class_initializer_callback initializer, bool createStatic)
{
	klass->priv = malloc(sizeof(struct LolClass_Private));
	memset(klass->priv, 0, sizeof(struct LolClass_Private));

	klass->proto.priv = malloc(sizeof(struct Object_Private));
	memset(klass->proto.priv, 0, sizeof(struct Object_Private));

	klass->proto.priv->tag = obj_runtime_type_class;
	klass->proto.klass = createClassWithStaticMethods(klass, createStatic);
	klass->priv->selectors = NULL;

	if (initializer != NULL) {
		initializer(klass);
	}
}

void obj_initialize_class(struct LolClass* klass, obj_class_initializer_callback initializer)
{
	bool createStatic = klass != Object() && klass != Class();
	privInitializeClass(klass, initializer, createStatic);

	struct LolClass_List* l = malloc(sizeof(struct LolClass_List));
	l->next = list_of_registred_classes;
	l->klass = klass;
	list_of_registred_classes = l;
}

static void deleteClassProperties(struct LolClass* klass, struct ObjectPropertyPair* pair)
{
	// FIXME: a copy of deleteClassSelector
	struct ObjectPropertyPair* tmp;

	while(pair != NULL) {
		tmp = pair;
		pair = pair->next;
		free(tmp);
	}
}


static void deleteClassSelector(struct LolClass* klass, struct ObjectSelectorPair* pair)
{
	struct ObjectSelectorPair* tmp;

	while(pair != NULL) {
		tmp = pair;
		pair = pair->next;
		free(tmp);
	}
}

static void privUnloadClass(struct LolClass* klass)
{
	deleteClassSelector(klass, klass->priv->selectors);
	deleteClassProperties(klass, klass->priv->properties);

	if (klass->proto.klass != Class() && klass->proto.klass != Object()) {
		deleteClassSelector(klass->proto.klass, klass->proto.klass->priv->selectors);
		deleteClassProperties(klass->proto.klass, klass->proto.klass->priv->properties);

		free(klass->proto.klass->proto.priv);
		free(klass->proto.klass->priv);
	}

	free(klass->proto.priv);
	free(klass->priv);
}

void obj_unload_class(struct LolClass* klass)
{
	privUnloadClass(klass);
}

void obj_set_class_parent(struct LolClass* klass, struct LolClass* parent)
{
	klass->priv->parent = parent;
}

void obj_set_class_name(struct LolClass* klass, const char* name)
{
	klass->priv->name = name;
}

const char* obj_class_name(struct LolClass* klass)
{
	return klass == NULL ? NULL : klass->priv->name;
}

struct LolClass* obj_class_for_object(struct Object* object)
{
	if (object == NULL) {
		return NULL;
	}

	if (object->priv->tag == obj_runtime_type_object) {
		return object->klass;
	}

	if (object == (struct Object*)Class()) {
		return Object();
	}

	if (object == (struct Object*)Object()) {
		return Class();
	}

	assert(object->klass->priv->parent == Class());

	return object->klass->priv->parent;
}

struct LolClass* obj_class_parent(struct LolClass* klass)
{
	return klass == NULL ? NULL : klass->priv->parent;
}

size_t obj_number_of_call_arguments_ending_on_null(va_list arguments)
{
	va_list copy;
	va_copy(copy, arguments);

	size_t number_of_arguments = 0;

	struct Object* obj = NULL;

	while (obj = va_arg(copy, struct Object*)) {
		number_of_arguments++;
	}
	
	va_end(copy);

	return number_of_arguments;
}

bool obj_object_is_class(struct Object* object)
{
	return object->priv->tag == obj_runtime_type_class;
}

struct Object* obj_get_object_property(struct Object* object, const char* propertyName)
{
	struct LolClass* klass = obj_class_for_object(object);

	for (struct LolClass* k = klass; k != NULL; k = obj_class_parent(k)) {
		for (struct ObjectPropertyPair* prop = k->priv->properties; prop != NULL; prop = prop->next) {
			if (strcmp(prop->name, propertyName) == 0) {
				size_t offset = prop->offset;
				return *(struct Object**)((uint8_t*)object + offset);
			}
		}
	}

	return NULL;
}

struct Object* obj_set_object_property(struct Object* object, const char* propertyName, struct Object* value)
{
	struct LolClass* klass = obj_class_for_object(object);

	for (struct LolClass* k = klass; k != NULL; k = obj_class_parent(k)) {
		for (struct ObjectPropertyPair* prop = k->priv->properties; prop != NULL; prop = prop->next) {
			if (strcmp(prop->name, propertyName) == 0) {
				size_t offset = prop->offset;
				*(struct Object**)((uint8_t*)object + offset) = value;
				return object;
			}
		}
	}

	// TODO: what if the property was not found?

	return object;
}

void obj_add_property(struct LolClass* klass, const char* propertyName, struct LolClass* type, size_t offset)
{
	// FIXME: code copied from obj_add_selector!!!
	struct ObjectPropertyPair *pair = malloc(sizeof(struct ObjectPropertyPair));
	pair->name = propertyName;
	pair->offset = offset;
	pair->klass = type;

	// insert at the beginning of the list
	pair->next = klass->priv->properties;
	klass->priv->properties = pair;
}

void obj_add_selector_from_property(struct LolClass* klass, struct LolClass* memberClass, const char* propertyName, const char* selectorName)
{
	obj_selector selector = obj_selector_for_name(memberClass, selectorName);

	if (selector == NULL) {
		return;
	}

	klass->priv->members.values[klass->priv->members.count++] = (struct ObjectMemberPair){
		.selectorName = selectorName,
		.propertyName = propertyName};

	obj_add_selector(klass, selectorName, selector);
}

struct LolClass* obj_class_with_name(const char* klassName)
{
	for (struct LolClass_List* l = list_of_registred_classes; l != NULL; l = l->next) {
		if (strcmp(obj_class_name(l->klass), klassName) == 0) {
			return l->klass;
		}
	}

	return NULL;
}

void obj_load_module_from_file(const char* filename)
{
	void *module = dlopen(filename, RTLD_NOW|RTLD_LOCAL);

	if (module == NULL) {
		fprintf(stderr, "Error opening module from file %s: %s\n", filename, dlerror());
		return;
	}

	void (*init_module)() = dlsym(module, "init_lol_module");

	if (init_module == NULL) {
		fprintf(stderr, "Error opening module from file %s: %s\n", filename, dlerror());
		return;
	}

	// FIXME: pass runtime information to the module, so it can decide not to load, for instance
	// based on compatibility issues
	init_module();
}
