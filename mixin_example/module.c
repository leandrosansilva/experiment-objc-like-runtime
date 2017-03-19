#include <lolbject/runtime.h>
#include "module.h"

#include <stdio.h>
#include <assert.h>

void load_module(struct LolModule* module)
{
	printf("loading module Mixin Example!\n");
}

void unload_module(struct LolModule* module)
{
	printf("unloading module Mixin Example!\n");
}

void unload_class_b(struct LolClass* klass)
{
	printf("unloading ClassB!\n");
}

struct LolModule* init_lol_module()
{	
	static struct LolClass_Descriptor classADescriptor = {
		.name = "ClassA",
		.version = 1,
		.initializer = lolbj_class_a_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor classBDescriptor = {
		.name = "ClassB",
		.version = 1,
		.initializer = lolbj_class_b_initializer,
		.unloader = unload_class_b
	};

	static struct LolClass_Descriptor classCDescriptor = {
		.name = "ClassC",
		.version = 1,
		.initializer = lolbj_class_c_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor classDDescriptor = {
		.name = "ClassD",
		.version = 1,
		.initializer = lolbj_class_d_initializer,
		.unloader = NULL
	};

	static struct LolModule_Descriptor moduleDescriptor = {
		.version = 1,
		.name = "mixin_example",
		.init_module = load_module,
		.shutdown_module = unload_module
	};

	mixin_module = lolbj_send_message(LolRuntime, "createModuleWithDescriptor", &moduleDescriptor);

	assert(mixin_module);

	ClassA = lolbj_send_message(mixin_module, "registerClassWithDescriptor", &classADescriptor);

	ClassB = lolbj_send_message(mixin_module, "registerClassWithDescriptor", &classBDescriptor);

	ClassC = lolbj_send_message(mixin_module, "registerClassWithDescriptor", &classCDescriptor);

	ClassD = lolbj_send_message(mixin_module, "registerClassWithDescriptor", &classDDescriptor);

	return mixin_module;
}
