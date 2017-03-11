#include <lolbject/runtime.h>

#include "ClassA.h"
#include "ClassB.h"
#include "ClassC.h"

#include <stdio.h>

void init_lol_module()
{	
	// struct Lol_Module* module = obj_create_module_from_description(description);
	// and then pass module to the class
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

	obj_register_class_with_descriptor(&classADescriptor);
	obj_register_class_with_descriptor(&classBDescriptor);
	obj_register_class_with_descriptor(&classCDescriptor);
}
