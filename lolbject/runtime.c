#define _DEFAULT_SOURCE 1

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include "Lolbject.h"
#include "runtime.h"
#include "Class.h"
#include "Lolbject_Private.h"
#include "String.h"
#include "Number.h"
#include "Lolbject.h"
#include "Box.h"
#include "Array.h"
#include "MutableArray.h"
#include "DefaultAllocator.h"
#include "TreeObject.h"
#include "SignalSender.h"
#include "Anonymous.h"
#include "macros.h"

#define XDOT "xdot -"
//#define XDOT "cat"
//#define XDOT "dot -Tpng -Ooutput"

struct SelectorPair {
	const char* name; // TODO: do not use pointers, but contiguous memory
	const char* propertyName; // property name used as "self", or null
	lolbj_selector selector;
};

struct LolbjectSelectors
{
	struct SelectorPair selectors[32];
	size_t size;
};

struct LolbjectPropertyPair
{
	const char* name; // TODO: do not use pointers, but contiguous memory
	size_t offset;
	struct LolClass* klass;
};

struct LolbjectProperties
{
	size_t size;
	struct LolbjectPropertyPair properties[32];
};

struct LolClass_Private
{
	struct LolClass* parent;
	const char* name;
	struct LolbjectSelectors selectors;
	struct LolbjectProperties properties;
	struct LolClass_Descriptor* descriptor;
	struct LolModule* module;
};

struct LolClass* Class;

struct LolClass_List
{
	struct LolClass* classes[64];
	size_t size;
};

struct LolModule_List
{
	struct LolModule* modules[64];
	size_t size;
};

struct LolRuntime;

struct LolModule
{
	struct Lolbject super;
	struct LolModule_Descriptor* descriptor;
	struct LolClass_List classes;
	void* handler;
	struct LolRuntime* runtime;
};

struct LolRuntime {
	struct Lolbject super;
	struct LolModule* coreModule;
	struct LolModule_List modules;
};

static struct LolRuntime* lol_runtime_object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct LolRuntime);
	return NULL;
}

static struct LolClass* find_class_in_module(struct LolModule* module, const char* className);

static struct LolModule* lol_runtime_create_module_selector(struct LolRuntime* self, va_list arguments)
{
	struct LolClass* moduleClass = find_class_in_module(self->coreModule, "Class");
	assert(moduleClass);
	struct LolModule* module = lolbj_send_message_with_arguments(lolbj_send_message(moduleClass, "alloc"), "initWithDescriptor", arguments);

	// Agh!!!!!, small hack to prevent change in the API. It looks dirty, I know
	if (module != NULL) {
		module->runtime = self;
	}

	return module;
}

static struct LolModule* lol_runtime_module_with_name_selector(struct LolRuntime* self, va_list arguments)
{
	struct String* moduleNameObj = va_arg(arguments, struct String*);
	struct Box* moduleNameBox = lolbj_send_message(moduleNameObj, "boxedValue");
	const char* moduleName = (const char*)moduleNameBox->value;

	for (size_t i = 0; i < self->modules.size; i++) {
		struct LolModule* module = self->modules.modules[i];
		if (strcmp(module->descriptor->name, moduleName) == 0) {
			RELEASE(moduleNameBox);
			RELEASE(moduleNameObj);
			return module;
		}
	}

	RELEASE(moduleNameBox);
	RELEASE(moduleNameObj);

	return NULL;
}

static struct LolModule* lol_class_get_module_selector(struct LolClass* klass, va_list arguments)
{
	return klass->priv->module;
}

static struct LolClass* lol_runtime_class_by_name_selector(struct LolRuntime* self, va_list arguments)
{
	// TODO: load modules on demand based on the query
	struct String* moduleName = va_arg(arguments, struct String*);
	struct String* className = va_arg(arguments, struct String*);

	struct LolModule* module = lolbj_send_message(self, "moduleWithName", moduleName);
	struct LolClass* klass = lolbj_send_message(module, "classWithName", className);

	return klass;
}

static struct LolModule* lol_runtime_core_module_selector(struct LolRuntime* self, va_list arguments)
{
	return self->coreModule;
}

static void lolbj_register_module(struct LolRuntime* runtime, struct LolModule* module);

static struct LolRuntime* lol_runtime_register_module_selector(struct LolRuntime* self, va_list arguments)
{
	struct LolModule* module = va_arg(arguments, struct LolModule*);
	lolbj_register_module(self, module);
	return self;
}

static struct LolModule* lol_runtime_load_module_from_file_selector(struct LolRuntime* self, va_list arguments)
{
	// TODO: handle errors, empty filename, etc.
	struct String* filename = va_arg(arguments, struct String*);
	struct Box* filenameBox = lolbj_send_message(filename, "boxedValue");
	const char *f = (const char*)filenameBox->value;

	void *handler = dlopen(f, RTLD_NOW|RTLD_LOCAL);

	if (handler == NULL) {
		fprintf(stderr, "Error opening module from file \"%s\": %s\n", f, dlerror());
		return NULL;
	}

	struct LolModule* (*init_module)(struct LolRuntime*) = dlsym(handler, "init_lol_module");

	if (init_module == NULL) {
		fprintf(stderr, "Error opening module from file %s: %s\n", f, dlerror());
		RELEASE(filenameBox);
		RELEASE(filename);
		dlclose(handler);
		return NULL;
	}

	struct LolModule* module = init_module(self);
	module->handler = handler;
	module->runtime = self;

	RELEASE(filenameBox);
	RELEASE(filename);

	return module;
}

static struct LolClass* find_class_in_module(struct LolModule* module, const char* className)
{
	for (size_t i = 0; i < module->classes.size; i++) {
		if (strcmp(lolbj_class_name(module->classes.classes[i]), className) == 0) {
				return module->classes.classes[i];
		}
	}

	return NULL;
}

static struct LolClass* lol_module_class_with_name_selector(struct LolModule* self, va_list arguments)
{
	struct String* klassNameObj = va_arg(arguments, struct String*);
	struct Box* klassNameBox = lolbj_send_message(klassNameObj, "boxedValue");

	const char* klassName = (const char*)klassNameBox->value;

	struct LolClass* klass = find_class_in_module(self, klassName);

	RELEASE(klassNameBox);
	RELEASE(klassNameObj);

	return klass;
}

static struct LolModule* lol_module_init_with_descriptor(struct LolModule* self, va_list arguments)
{
	struct LolClass* moduleClass = find_class_in_module(self, "LolModule");
	assert(moduleClass);

	if (self = lolbj_send_message_to_super(self, moduleClass, "init")) {
		struct LolModule_Descriptor* descriptor = va_arg(arguments, struct LolModule_Descriptor*);
		self->descriptor = descriptor;
	}

	return self;
}

static struct LolModule* lol_module_object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct LolModule);
	return NULL;
}

static void privUnloadClass(struct LolClass* klass);

static struct LolModule* lol_module_dealloc_selector(struct LolModule* self, va_list arguments)
{
	struct LolRuntime* runtime = lolbj_send_message(self, "runtime");
	assert(runtime != NULL);

	struct LolClass* moduleClass = find_class_in_module(self, "LolModule");
	assert(moduleClass);

	if (self->descriptor->shutdown_module != NULL) {
		self->descriptor->shutdown_module(self);
	}

	for (size_t j = self->classes.size; j > 0; j--) {
		privUnloadClass(self->classes.classes[j - 1]);
		self->classes.classes[j - 1] = NULL;
	}

	if (self->handler != NULL) {
		if (dlclose(self->handler) != 0) {
			printf("Error closing module %s: %s\n", self->descriptor->name, dlerror());
		}
	}

	return self == runtime->coreModule
		? NULL
		: lolbj_send_message_to_super(self, moduleClass, "dealloc");
}

static struct LolModule* lol_module_get_runtime_selector(struct LolModule* self, va_list arguments)
{
	return self->runtime;
}

static void lolbj_runtime_initializer(struct LolRuntime*, struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);
	lolbj_add_selector(klass, "createModuleWithDescriptor", lol_runtime_create_module_selector);
	lolbj_add_selector(klass, "objectSize", lol_runtime_object_size_selector);
	lolbj_add_selector(klass, "moduleWithName", lol_runtime_module_with_name_selector);
	lolbj_add_selector(klass, "coreModule", lol_runtime_core_module_selector);
	lolbj_add_selector(klass, "classByModuleAndName", lol_runtime_class_by_name_selector);
	lolbj_add_selector(klass, "registerModule", lol_runtime_register_module_selector);
	lolbj_add_selector(klass, "loadModuleFromFile", lol_runtime_load_module_from_file_selector);
}

static struct LolClass* register_class_with_descriptor(struct LolModule* module, struct LolClass_Descriptor *descriptor);

static struct LolClass* module_register_class_with_selector_selector(struct LolModule* self, va_list arguments)
{
	struct LolClass_Descriptor* descriptor = va_arg(arguments, struct LolClass_Descriptor*);
	return register_class_with_descriptor(self, descriptor);
}

static void lolbj_module_initializer(struct LolRuntime* runtime, struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);
	lolbj_add_selector(klass, "registerClassWithDescriptor", module_register_class_with_selector_selector);
	lolbj_add_class_selector(klass, "objectSize", lol_module_object_size_selector);
	lolbj_add_selector(klass, "initWithDescriptor", lol_module_init_with_descriptor);
	lolbj_add_selector(klass, "classWithName", lol_module_class_with_name_selector);
	lolbj_add_selector(klass, "dealloc", lol_module_dealloc_selector);
	lolbj_add_selector(klass, "runtime", lol_module_get_runtime_selector);
}

static struct LolClass* privRegisterClass(struct LolModule* module, struct LolClass_Descriptor *descriptor, bool isNormalClass);

static struct LolClass* privCreateClass(struct LolClass_Descriptor *descriptor, bool isNormalClass);

static void lolbj_runtime_initializer(struct LolRuntime* runtime, struct LolClass* klass);

static void lolbj_class_initializer(struct LolRuntime* runtime, struct LolClass* klass);

static void privAddSelector(struct LolClass* klass, const char* selectorName, const char* propertyName, lolbj_selector selector);

static void lolbj_object_initializer_wrapper(struct LolRuntime* runtime, struct LolClass* klass)
{
	lolbj_object_initializer(runtime, klass);
	privAddSelector(lolbj_class_for_object(klass), "module", NULL, lol_class_get_module_selector);
}

struct LolRuntime* lolbj_init_runtime()
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
		.initializer = lolbj_object_initializer_wrapper,
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

	static struct LolClass_Descriptor mutableArrayDescriptor = {
		.name = "MutableArray",
		.version = 1,
		.initializer = lolbj_mutablearray_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor anonymousDescriptor = {
		.name = "Anonymous",
		.version = 1,
		.initializer = lolbj_anonymous_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor runtimeDescriptor = {
		.name = "LolRuntime",
		.version = 1,
		.initializer = lolbj_runtime_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor lolModuleDescriptor = {
		.name = "LolModule",
		.version = 1,
		.initializer = lolbj_module_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor defaultAllocatorDescriptor = {
		.name = "DefaultAllocator",
		.version = 1,
		.initializer = lolbj_default_allocator_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor treeObjectDescriptor = {
		.name = "TreeObject",
		.version = 1,
		.initializer = lolbj_treeobject_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor signalSenderDescriptor = {
		.name = "SignalSender",
		.version = 1,
		.initializer = lolbj_signalsender_initializer,
		.unloader = NULL
	};

	static struct LolModule_Descriptor coreDescriptor = {
		.version = 1,
		.name = "core",
		.init_module = NULL,
		.shutdown_module = NULL
	};

	Class = privCreateClass(&classDescriptor, false);
	Lolbject = privCreateClass(&objectDescriptor, true);
	DefaultAllocator = privCreateClass(&defaultAllocatorDescriptor, true);
	struct LolClass* runtimeClass = privCreateClass(&runtimeDescriptor, true);
	struct LolClass* moduleClass  = privCreateClass(&lolModuleDescriptor, true);

	struct LolRuntime* runtime = lolbj_send_message(lolbj_send_message(runtimeClass, "alloc"), "init");

	assert(runtime != NULL);

	runtime->coreModule = lolbj_send_message(runtime, "createModuleWithDescriptor", &coreDescriptor);

	Class->priv->module = runtime->coreModule;
	Lolbject->priv->module = runtime->coreModule;
	DefaultAllocator->priv->module = runtime->coreModule;
	runtimeClass->priv->module = runtime->coreModule;
	moduleClass->priv->module = runtime->coreModule;

#define ADD_CLASS(__klass__) runtime->coreModule->classes.classes[runtime->coreModule->classes.size++] = __klass__
	ADD_CLASS(Class);
	ADD_CLASS(Lolbject);
	ADD_CLASS(runtimeClass);
	ADD_CLASS(moduleClass);
	ADD_CLASS(DefaultAllocator);
#undef ADD_CLASS

	assert(descriptor->initializer && "class initializer callback must be defined!");
	descriptor->initializer(runtime, klass);



#define REGISTER_CLASS(__klass__, __descriptor__) __klass__ = register_class_with_descriptor(runtime->coreModule, &__descriptor__)
	REGISTER_CLASS(String, stringDescriptor);
	REGISTER_CLASS(Number, numberDescriptor);
	REGISTER_CLASS(Box, boxDescriptor);
	REGISTER_CLASS(MutableArray, mutableArrayDescriptor);
	REGISTER_CLASS(Array, arrayDescriptor);
	REGISTER_CLASS(TreeObject, treeObjectDescriptor);
	REGISTER_CLASS(SignalSender, signalSenderDescriptor);
	REGISTER_CLASS(Anonymous, anonymousDescriptor);
#undef REGISTER_CLASS

	lolbj_register_module(runtime, runtime->coreModule);

	return runtime;
}

static void privDeleteCoreModule(struct LolModule* module, ...)
{
	va_list arguments;
	va_start(arguments, module);
	lol_module_dealloc_selector(module, arguments);
	va_end(arguments);
}

static void privDeleteModule(struct LolModule* module)
{
	struct LolRuntime* runtime = lolbj_send_message(module, "runtime");

	assert(runtime != NULL);

	if (module == runtime->coreModule) {
		privDeleteCoreModule(module);
		return;
	}

	RELEASE(module);
}

void lolbj_shutdown_runtime(struct LolRuntime* runtime)
{
	for (size_t i = runtime->modules.size; i > 0; i--) {
		struct LolModule* module = runtime->modules.modules[i - 1];
		privDeleteModule(module);
		runtime->modules.modules[i - 1] = NULL;
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

	if (klass->priv->properties.size > 0) {
		fprintf(p, "|");

		for (size_t i = 0; i < klass->priv->properties.size; i++) {
			fprintf(p, "- %s: %s\\l", klass->priv->properties.properties[i].name,
					lolbj_class_name(klass->priv->properties.properties[i].klass));
		}
	}

	fprintf(p, "}\"\n  ]\n");

	fprintf(p, "  class_addr_%lld -> class_addr_%lld [style=\"dotted\" arrowhead=\"open\"] // %s -> %s\n",
			(unsigned long long)klass,
			(unsigned long long)lolbj_class_for_object(klass),
			lolbj_class_name(klass),
			lolbj_class_name(lolbj_class_for_object(klass)));
	
	for (size_t i = 0; i < klass->priv->properties.size; i++) {
		struct LolClass* memberClass = klass->priv->properties.properties[i].klass;	
		fprintf(p, "  class_addr_%lld -> class_addr_%lld [arrowtail = \"odiamond\", dir=back] // %s -> %s\n",
			(unsigned long long)klass,
			(unsigned long long)memberClass,
			lolbj_class_name(klass),
			lolbj_class_name(memberClass));
	}

	if (lolbj_class_parent(klass) != NULL) {
		// The inheritance arrow
		fprintf(p, "  class_addr_%lld -> class_addr_%lld [dir=\"back\" arrowtail = \"empty\"] // %s -> %s\n",
			(unsigned long long)lolbj_class_parent(klass),
			(unsigned long long)klass,
			lolbj_class_name(lolbj_class_parent(klass)),
			lolbj_class_name(klass));
	}
}

void lolbj_print_class_diagram(struct LolRuntime* runtime)
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
	for (size_t i = 0; i < runtime->modules.size; i++) {
		struct LolModule* module = runtime->modules.modules[i];
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

	//printf("Calling %s::%s\n", lolbj_class_name(lolbj_class_for_object(self)), selectorPair.name);

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

static void lolbj_class_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, NULL);
}

static void privInitializeClass(struct LolClass* klass, bool createStatic)
{
	uint8_t* baseAddress = (uint8_t*)klass;
	size_t staticClassOffset = sizeof(struct LolClass);
	size_t klassProtoPrivOffset = staticClassOffset + sizeof(struct LolClass);
	size_t staticClassProtoPrivOffset = klassProtoPrivOffset + sizeof(struct Lolbject_Private);
	size_t klassPrivateOffset = staticClassProtoPrivOffset + sizeof(struct Lolbject_Private);
	size_t staticClassPrivateOffset = klassPrivateOffset + sizeof(struct LolClass_Private);

	klass->priv = baseAddress + klassPrivateOffset;
	klass->proto.priv = baseAddress + klassProtoPrivOffset;
	klass->proto.klass = Lolbject;

	klass->proto.priv->tag = lolbj_runtime_type_class;

	if (createStatic) {
		klass->proto.klass = baseAddress + staticClassOffset;
		klass->proto.klass->proto.priv = baseAddress + staticClassProtoPrivOffset;
		klass->proto.klass->priv = baseAddress + staticClassPrivateOffset;
		lolbj_set_class_parent(klass->proto.klass, Class);
		klass->proto.klass->proto.priv->tag = lolbj_runtime_type_class;
	}
}

static void privUnloadClass(struct LolRuntime* runtime, struct LolClass* klass)
{
	if (klass->priv->descriptor != NULL && klass->priv->descriptor->unloader != NULL) {
		klass->priv->descriptor->unloader(runtime, klass);
	}

	free(klass);
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
		for (size_t i = 0; i < k->priv->properties.size; i++) {
			struct LolbjectPropertyPair pair = k->priv->properties.properties[i];
			if (strcmp(pair.name, propertyName) == 0) {
				size_t offset = pair.offset;
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
		for (size_t i = 0; i < k->priv->properties.size; i++) {
			struct LolbjectPropertyPair pair = k->priv->properties.properties[i];
			if (strcmp(pair.name, propertyName) == 0) {
				size_t offset = pair.offset;
				*(struct Lolbject**)((uint8_t*)object + offset) = value;
				return object;
			}
		}
	}

	// TODO: what if the property was not found?

	return object;
}

static void lolbj_add_selector_from_property(struct LolClass* klass, struct LolClass* memberClass, const char* propertyName, const char* selectorName)
{
	struct SelectorPair selectorPair = privSelectorPairForSelectorName(memberClass, selectorName);

	if (selectorPair.selector == NULL) {
		return;
	}

	privAddSelector(klass, selectorName, propertyName, selectorPair.selector);
}

void lolbj_add_property(struct LolClass* klass, const char* propertyName, struct LolClass* type, size_t offset, ...)
{
	klass->priv->properties.properties[klass->priv->properties.size++] = (struct LolbjectPropertyPair){
		.name = propertyName,
		.offset = offset,
		.klass = type
	};

	va_list arguments;
	va_start(arguments, offset);
	const char* propertySelectorName = NULL;
	while (propertySelectorName = va_arg(arguments, const char*)) {
		lolbj_add_selector_from_property(klass, type, propertyName, propertySelectorName);
	}
	va_end(arguments);
}

static struct LolClass* privCreateClass(struct LolClass_Descriptor *descriptor, bool isNormalClass)
{
	size_t totalSize = sizeof(struct LolClass) // main class
											+ sizeof(struct LolClass) // static-class
											+ sizeof(struct Lolbject_Private) // main class - prototype private
											+ sizeof(struct Lolbject_Private) // static class - prototype private
											+ sizeof(struct LolClass_Private) // main class - private
											+ sizeof(struct LolClass_Private); // static-class - private

	struct LolClass* klass = malloc(totalSize);
	memset(klass, 0, totalSize);

	privInitializeClass(klass, isNormalClass);
	lolbj_set_class_name(klass, descriptor->name);

	klass->priv->descriptor = descriptor;

	return klass;
}

static struct LolClass* privRegisterClass(struct LolModule* module, struct LolClass_Descriptor *descriptor, bool isNormalClass)
{
	struct LolClass* klass = privCreateClass(descriptor, isNormalClass);

	module->classes.classes[module->classes.size++] = klass;

	klass->priv->module = module;

	return klass;
}

static struct LolClass* register_class_with_descriptor(struct LolModule* module, struct LolClass_Descriptor *descriptor)
{
	return privRegisterClass(module, descriptor, true);
}

static void lolbj_register_module(struct LolRuntime* runtime, struct LolModule* module)
{
	if (module == NULL) {
		return;
	}

	if (module->descriptor->init_module) {
		module->descriptor->init_module(module);
	}

	runtime->modules.modules[runtime->modules.size++] = module;
}

struct Lolbject* lolbj_cast(struct LolClass* klass, struct Lolbject* obj)
{
	struct LolClass* k = lolbj_class_for_object(obj);

	for (; k != NULL && k != klass; k = k->priv->parent) {}

	return k == klass ? obj : NULL;
}
