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
#include <lolbject/Lolbject.h>
#include "Lolbject_Private.h"
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/Lolbject.h>
#include <lolbject/Box.h>
#include <lolbject/Array.h>

#define XDOT "xdot -"

struct SelectorPair {
	const char* name; // TODO: do not use pointers, but contiguous memory
	const char* propertyName; // property name used as "self", or null
	lolbj_selector selector;
};

struct LolbjectSelectors
{
	struct SelectorPair selectors[1024];
	size_t size;
};

struct LolClass_Private
{
	struct LolClass* parent;
	const char* name;
	struct LolbjectSelectors selectors;
	struct LolbjectPropertyPair* properties;
	struct LolClass_Descriptor* descriptor;
};

// NOTE: the property list is also a linked list.
struct LolbjectPropertyPair
{
	const char* name;
	size_t offset;
	struct LolClass* klass;
	struct LolbjectPropertyPair* next;
};

struct LolClass* Class;

struct LolClass_List
{
	struct LolClass* classes[1024];
	size_t size;
};

struct LolModule_List
{
	struct LolModule* modules[1024];
	size_t size;
};

struct LolModule
{
	struct LolModule_Descriptor* descriptor;
	struct LolClass_List classes;
	void* handler;
};

static struct LolModule_List registred_modules;

static struct LolClass* privRegisterClass(struct LolModule* module, struct LolClass_Descriptor *descriptor, bool isNormalClass);

void lolbj_init_runtime()
{
	static struct LolClass_Descriptor classDescriptor = {
		.name = "Class",
		.version = 1,
		.initializer = lolbj_class_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor objectDescriptor = {
		.name = "Lolbject",
		.version = 1,
		.initializer = lolbj_object_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor stringDescriptor = {
		.name = "String",
		.version = 1,
		.initializer = lolbj_string_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor numberDescriptor = {
		.name = "Number",
		.version = 1,
		.initializer = lolbj_number_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor boxDescriptor = {
		.name = "Box",
		.version = 1,
		.initializer = lolbj_box_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor arrayDescriptor = {
		.name = "Array",
		.version = 1,
		.initializer = lolbj_array_initializer,
		.unloader = NULL
	};

	static struct LolModule_Descriptor coreDescriptor = {
		.version = 1,
		.name = "core",
		.init_module = NULL,
		.shutdown_module = NULL
	};

	struct LolModule* module = lolbj_create_module(&coreDescriptor);

	Class = privRegisterClass(module, &classDescriptor, false);
	Lolbject = lolbj_register_class_with_descriptor(module, &objectDescriptor);
	String = lolbj_register_class_with_descriptor(module, &stringDescriptor);
	Number = lolbj_register_class_with_descriptor(module, &numberDescriptor);
	Box = lolbj_register_class_with_descriptor(module, &boxDescriptor);
	Array = lolbj_register_class_with_descriptor(module, &arrayDescriptor);

	lolbj_register_module(module);
}

static void privUnloadNormalClass(struct LolClass* klass);
static void privUnloadClassClass(struct LolClass* klass);

void lolbj_unload_module(struct LolModule* module)
{
	if (module->descriptor->shutdown_module != NULL) {
		module->descriptor->shutdown_module(module);
	}

	for (size_t j = 0; j < module->classes.size; j++) {
		void (*unload_class)(struct LolClass*) = module->classes.classes[j] == Class 
			? privUnloadClassClass
			: privUnloadNormalClass;

		unload_class(module->classes.classes[j]);
		module->classes.classes[j] = NULL;
	}

	if (module->handler != NULL) {
		if (dlclose(module->handler) != 0) {
			printf("Error closing module %s: %s\n", module->descriptor->name, dlerror());
		}
	}

	free(module);
}

void lolbj_shutdown_runtime()
{
	for (size_t i = 0; i < registred_modules.size; i++) {
		struct LolModule* module = registred_modules.modules[i];
		lolbj_unload_module(module);
		registred_modules.modules[i] = NULL;
	}
}

static void print_diagram_for_class(FILE* p, struct LolClass* klass)
{
	fprintf(p, "  class_addr_%lld [\n    label = \"{%s| ", (unsigned long long)klass, klass->priv->name);

	struct LolClass* typeKlass = klass->proto.klass;

	if (typeKlass != NULL && klass != Class) {
		for (size_t i = 0; i < typeKlass->priv->selectors.size; i++) {
			fprintf(p, "+ %s \\<\\<static\\>\\>\\l", typeKlass->priv->selectors.selectors[i].name);
		}
	}

	for (size_t i = 0; i < klass->priv->selectors.size; i++) {
		fprintf(p, "+ %s\\l", klass->priv->selectors.selectors[i].name);
	}

	if (klass->priv->properties != NULL) {
		fprintf(p, "|");

		for (struct LolbjectPropertyPair* pair = klass->priv->properties; pair != NULL; pair = pair->next) {
			fprintf(p, "- %s: %s\\l", pair->name, lolbj_class_name(pair->klass));
		}
	}

	fprintf(p, "}\"\n  ]\n");

	// The "type" class
	fprintf(p, "  class_addr_%lld -> class_addr_%lld [arrowhead = \"vee\"]\n",
			(unsigned long long)klass,
			(unsigned long long)lolbj_class_for_object(klass));
	
	// Properties
	for (struct LolbjectPropertyPair* pair = klass->priv->properties; pair != NULL; pair = pair->next) {
		struct LolClass* memberClass = pair->klass;	
		fprintf(p, "  class_addr_%lld -> class_addr_%lld [arrowtail = \"odiamond\", dir=back]\n",
			(unsigned long long)klass,
			(unsigned long long)memberClass);
	}

	if (lolbj_class_parent(klass) != NULL) {
		// The inheritance arrow
		fprintf(p, "  class_addr_%lld -> class_addr_%lld [arrowhead = \"empty\"]\n",
			(unsigned long long)klass,
			(unsigned long long)lolbj_class_parent(klass));
	}
}

void lolbj_print_class_diagram()
{
	FILE* p = popen(XDOT, "w");

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
	for (size_t i = 0; i < registred_modules.size; i++) {
		struct LolModule* module = registred_modules.modules[i];
		fprintf(p, "subgraph cluster_module_%zu {\nlabel=\"%s\"\n", i, module->descriptor->name);
		for (size_t j = 0; j < module->classes.size; j++) {
			struct LolClass* klass = module->classes.classes[j];
			print_diagram_for_class(p, klass);
		}
		fprintf(p, "}\n");
	}
	fprintf(p, "}\n");

	pclose(p);
}

static void privAddSelector(struct LolClass* klass, const char* selectorName, const char* propertyName, lolbj_selector selector)
{
	klass->priv->selectors.selectors[klass->priv->selectors.size++] = (struct SelectorPair){
		.name = selectorName,
		.selector = selector,
		.propertyName = propertyName
	};
}

void lolbj_add_selector(struct LolClass* klass, const char* selectorName, lolbj_selector selector)
{
	privAddSelector(klass, selectorName, NULL, selector);
}

void lolbj_add_class_selector(struct LolClass* klass, const char* selectorName, lolbj_selector selector)
{
	lolbj_add_selector(klass->proto.klass, selectorName, selector);
}

static struct SelectorPair privSelectorPairForSelectorName(struct LolClass* klass, const char* selectorName)
{
	// NOTE: this is the worst implementation of method lookup ever!
	for (struct LolClass* k = klass; k != NULL; k = lolbj_class_parent(k)) {
		for (size_t i = 0; i < k->priv->selectors.size; i++) {
			struct SelectorPair pair = k->priv->selectors.selectors[i];
			if (strcmp(pair.name, selectorName) == 0) {
				return pair;
			}
		}
	}

	return (struct SelectorPair){.name = NULL, .propertyName = NULL, .selector = NULL};
}

lolbj_selector lolbj_selector_for_name(struct LolClass* klass, const char* selectorName)
{
	return privSelectorPairForSelectorName(klass, selectorName).selector;
}

static struct Lolbject* privSendMessageWithArguments(struct Lolbject* obj, struct LolClass* klass, const char* selectorName, va_list arguments)
{
	struct SelectorPair selectorPair = privSelectorPairForSelectorName(klass, selectorName);
	
	// TODO: maybe return SelectorNotFound()?
	if (selectorPair.selector == NULL) {
		return NULL;
	}

	struct Lolbject* self = selectorPair.propertyName == NULL ? obj : lolbj_get_object_property(obj, selectorPair.propertyName);

	struct Lolbject* result = selectorPair.selector(self, arguments);

	return result;
}

struct Lolbject* lolbj_send_message_with_arguments(struct Lolbject* obj, const char* selectorName, va_list arguments)
{
	return privSendMessageWithArguments(obj, obj ? obj->klass : NULL, selectorName, arguments);
}

struct Lolbject* lolbj_send_message_to_super_with_arguments(struct Lolbject* obj, struct LolClass* klass, const char* selectorName, va_list arguments)
{
	struct LolClass* parentClass = lolbj_class_parent(klass);
	return privSendMessageWithArguments(obj, parentClass, selectorName, arguments);
}

struct Lolbject* lolbj_send_message_to_super(struct Lolbject* obj, struct LolClass* klass, const char* selectorName, ...)
{
	va_list arguments;
	va_start(arguments, selectorName);
	struct Lolbject* result = lolbj_send_message_to_super_with_arguments(obj, klass, selectorName, arguments);
	va_end(arguments);

	return result;
}

struct Lolbject* lolbj_send_message(struct Lolbject* obj, const char* selectorName, ...)
{
	va_list arguments;
	va_start(arguments, selectorName);
	struct Lolbject* result = lolbj_send_message_with_arguments(obj, selectorName, arguments);
	va_end(arguments);

	return result;
}

void lolbj_class_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, NULL);
}

static void classStaticInitializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Class);
	lolbj_set_class_name(klass, NULL);
}

static void privInitializeClass(struct LolClass* klass, lolbj_class_initializer_callback initializer, bool createStatic);

static struct LolClass* createClassWithStaticMethods(struct LolClass* klass, bool shouldCreate)
{
	if (!shouldCreate) {
		return Lolbject;
	}

	struct LolClass* class_with_class_methods = malloc(sizeof(struct LolClass));
	privInitializeClass(class_with_class_methods, classStaticInitializer, false);

	return class_with_class_methods;
}

static void privInitializeClass(struct LolClass* klass, lolbj_class_initializer_callback initializer, bool createStatic)
{
	klass->priv = malloc(sizeof(struct LolClass_Private));
	memset(klass->priv, 0, sizeof(struct LolClass_Private));

	klass->proto.priv = malloc(sizeof(struct Lolbject_Private));
	memset(klass->proto.priv, 0, sizeof(struct Lolbject_Private));

	klass->proto.priv->tag = lolbj_runtime_type_class;
	klass->proto.klass = createClassWithStaticMethods(klass, createStatic);

	if (initializer != NULL) {
		initializer(klass);
	}
}

static void deleteClassProperties(struct LolClass* klass, struct LolbjectPropertyPair* pair)
{
	// FIXME: a copy of deleteClassSelector
	struct LolbjectPropertyPair* tmp;

	while(pair != NULL) {
		tmp = pair;
		pair = pair->next;
		free(tmp);
	}
}

static void privUnloadClass(struct LolClass* klass, bool ownStaticClass)
{
	if (klass->priv->descriptor != NULL && klass->priv->descriptor->unloader != NULL) {
		klass->priv->descriptor->unloader(klass);
	}

	deleteClassProperties(klass, klass->priv->properties);

	if (ownStaticClass) {
		privUnloadClass(klass->proto.klass, false);
	}

	free(klass->proto.priv);
	free(klass->priv);
	free(klass);
}

static void privUnloadNormalClass(struct LolClass* klass)
{
	privUnloadClass(klass, true);
}

static void privUnloadClassClass(struct LolClass* klass)
{
	// Remember, class Class uses Lolbject as type
	privUnloadClass(klass, false);
}

void lolbj_unload_class(struct LolClass* klass)
{
	privUnloadNormalClass(klass);
}

void lolbj_set_class_parent(struct LolClass* klass, struct LolClass* parent)
{
	klass->priv->parent = parent;
}

void lolbj_set_class_name(struct LolClass* klass, const char* name)
{
	klass->priv->name = name;
}

const char* lolbj_class_name(struct LolClass* klass)
{
	return klass == NULL ? NULL : klass->priv->name;
}

struct LolClass* lolbj_class_for_object(struct Lolbject* object)
{
	if (object == NULL) {
		return NULL;
	}

	if (object->priv->tag == lolbj_runtime_type_object) {
		return object->klass;
	}

	if (object == (struct Lolbject*)Class) {
		return Lolbject;
	}

	if (object == (struct Lolbject*)Lolbject) {
		return Class;
	}

	assert(object->klass->priv->parent == Class);

	return object->klass->priv->parent;
}

struct LolClass* lolbj_class_parent(struct LolClass* klass)
{
	return klass == NULL ? NULL : klass->priv->parent;
}

size_t lolbj_number_of_call_arguments_ending_on_null(va_list arguments)
{
	va_list copy;
	va_copy(copy, arguments);

	size_t number_of_arguments = 0;

	struct Lolbject* obj = NULL;

	while (obj = va_arg(copy, struct Lolbject*)) {
		number_of_arguments++;
	}
	
	va_end(copy);

	return number_of_arguments;
}

bool lolbj_object_is_class(struct Lolbject* object)
{
	return object->priv->tag == lolbj_runtime_type_class;
}

struct Lolbject* lolbj_get_object_property(struct Lolbject* object, const char* propertyName)
{
	struct LolClass* klass = lolbj_class_for_object(object);

	for (struct LolClass* k = klass; k != NULL; k = lolbj_class_parent(k)) {
		for (struct LolbjectPropertyPair* prop = k->priv->properties; prop != NULL; prop = prop->next) {
			if (strcmp(prop->name, propertyName) == 0) {
				size_t offset = prop->offset;
				return *(struct Lolbject**)((uint8_t*)object + offset);
			}
		}
	}

	return NULL;
}

struct Lolbject* lolbj_set_object_property(struct Lolbject* object, const char* propertyName, struct Lolbject* value)
{
	struct LolClass* klass = lolbj_class_for_object(object);

	for (struct LolClass* k = klass; k != NULL; k = lolbj_class_parent(k)) {
		for (struct LolbjectPropertyPair* prop = k->priv->properties; prop != NULL; prop = prop->next) {
			if (strcmp(prop->name, propertyName) == 0) {
				size_t offset = prop->offset;
				*(struct Lolbject**)((uint8_t*)object + offset) = value;
				return object;
			}
		}
	}

	// TODO: what if the property was not found?

	return object;
}

void lolbj_add_property(struct LolClass* klass, const char* propertyName, struct LolClass* type, size_t offset)
{
	// FIXME: code copied from lolbj_add_selector!!!
	struct LolbjectPropertyPair *pair = malloc(sizeof(struct LolbjectPropertyPair));
	pair->name = propertyName;
	pair->offset = offset;
	pair->klass = type;

	// insert at the beginning of the list
	pair->next = klass->priv->properties;
	klass->priv->properties = pair;
}

void lolbj_add_selector_from_property(struct LolClass* klass, struct LolClass* memberClass, const char* propertyName, const char* selectorName)
{
	struct SelectorPair selectorPair = privSelectorPairForSelectorName(memberClass, selectorName);

	if (selectorPair.selector == NULL) {
		return;
	}

	privAddSelector(klass, selectorName, propertyName, selectorPair.selector);
}

struct LolClass* lolbj_class_with_name(struct LolModule* module, const char* klassName)
{
	if (module == NULL) {
		return NULL;
	}
	
	for (size_t i = 0; i < module->classes.size; i++) {
		if (strcmp(lolbj_class_name(module->classes.classes[i]), klassName) == 0) {
				return module->classes.classes[i];
		}
	}

	return NULL;
}

struct LolModule* lolbj_load_module_from_file(const char* filename)
{
	void *handler = dlopen(filename, RTLD_NOW|RTLD_LOCAL);

	if (handler == NULL) {
		fprintf(stderr, "Error opening module from file %s: %s\n", filename, dlerror());
		return NULL;
	}

	struct LolModule* (*init_module)() = dlsym(handler, "init_lol_module");

	if (init_module == NULL) {
		fprintf(stderr, "Error opening module from file %s: %s\n", filename, dlerror());
		dlclose(handler);
		return NULL;
	}

	// FIXME: pass runtime information to the module, so it can decide not to load, for instance
	// based on compatibility issues
	struct LolModule* module = init_module();
	module->handler = handler;

	return module;
}

static struct LolClass* privRegisterClass(struct LolModule* module, struct LolClass_Descriptor *descriptor, bool isNormalClass)
{
	struct LolClass* klass = malloc(sizeof(struct LolClass));

	privInitializeClass(klass, descriptor->initializer, isNormalClass);
	lolbj_set_class_name(klass, descriptor->name);

	klass->priv->descriptor = descriptor;

	module->classes.classes[module->classes.size++] = klass;

	return klass;
}

struct LolClass* lolbj_register_class_with_descriptor(struct LolModule* module, struct LolClass_Descriptor *descriptor)
{
	return privRegisterClass(module, descriptor, true);
}

struct LolModule* lolbj_create_module(struct LolModule_Descriptor* descriptor)
{
	struct LolModule* module = malloc(sizeof(struct LolModule));
	memset(module, 0, sizeof(struct LolModule));
	module->descriptor = descriptor;

	return module;
}

struct LolModule* lolbj_module_with_name(const char* name)
{
	for (size_t i = 0; i < registred_modules.size; i++) {
		struct LolModule* module = registred_modules.modules[i];
		if (strcmp(module->descriptor->name, name) == 0) {
			return module;
		}
	}

	return NULL;
}

struct LolModule* lolbj_core_module()
{
	return lolbj_module_with_name("core");
}

void lolbj_register_module(struct LolModule* module)
{
	if (module == NULL) {
		return;
	}

	if (module->descriptor->init_module) {
		module->descriptor->init_module(module);
	}

	registred_modules.modules[registred_modules.size++] = module;
}

struct Lolbject* lolbj_cast(struct LolClass* klass, struct Lolbject* obj)
{
	struct LolClass* k = lolbj_class_for_object(obj);

	for (; k != NULL && k != klass; k = k->priv->parent) {}

	return k == klass ? obj : NULL;
}
