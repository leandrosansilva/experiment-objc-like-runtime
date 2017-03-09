#include <lolbject/runtime.h>

#include "ClassA.h"
#include "ClassB.h"
#include "ClassC.h"

#include <stdio.h>

void init_lol_module()
{	
	obj_initialize_class(ClassA(), obj_class_a_initializer);
	obj_initialize_class(ClassB(), obj_class_b_initializer);
	obj_initialize_class(ClassC(), obj_class_c_initializer);
}
