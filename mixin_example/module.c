#include <lolbject/runtime.h>
#include "module.h"
#include "ClassA.h"
#include "ClassB.h"
#include "ClassC.h"

#include <stdio.h>

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
		.unloader = NULL
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
		.init_module = NULL,
		.shutdown_module = NULL
	};

	struct LolModule* module = obj_create_module(&moduleDescriptor);

	ClassA = obj_register_class_with_descriptor(module, &classADescriptor);

	ClassB = obj_register_class_with_descriptor(module, &classBDescriptor);

	ClassC = obj_register_class_with_descriptor(module, &classCDescriptor);

	return module;
}
