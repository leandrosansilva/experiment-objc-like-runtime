#include <lolbject/runtime.h>
#include "module.h"
#include "ClassA.h"
#include "ClassB.h"
#include "ClassC.h"

#include <stdio.h>

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
		.initializer = obj_class_a_initializer,
		.unloader = NULL
	};

	static struct LolClass_Descriptor classBDescriptor = {
		.name = "ClassB",
		.version = 1,
		.initializer = obj_class_b_initializer,
		.unloader = unload_class_b
	};

	static struct LolClass_Descriptor classCDescriptor = {
		.name = "ClassC",
		.version = 1,
		.initializer = obj_class_c_initializer,
		.unloader = NULL
	};

	static struct LolModule_Descriptor moduleDescriptor = {
		.version = 1,
		.name = "mixin_example",
		.init_module = load_module,
		.shutdown_module = unload_module
	};

	struct LolModule* module = obj_create_module(&moduleDescriptor);

	ClassA = obj_register_class_with_descriptor(module, &classADescriptor);

	ClassB = obj_register_class_with_descriptor(module, &classBDescriptor);

	ClassC = obj_register_class_with_descriptor(module, &classCDescriptor);

	return module;
}
