#include "runtime.h"
#include "Object.h"
#include "String.h"
#include "Number.h"
#include "Box.h"
#include "Array.h"

#include "ClassA.h"
#include "ClassB.h"
#include "ClassC.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	obj_init_runtime();

	obj_initialize_class(ClassA(), obj_class_a_initializer);
	obj_initialize_class(ClassB(), obj_class_b_initializer);
	obj_initialize_class(ClassC(), obj_class_c_initializer);

	obj_print_class_diagram();

	struct String* name = STRING("Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	struct Box* lengthValue = obj_send_message(length, "boxedValue");

	struct Box* nameValue = obj_send_message(name, "boxedValue");

	struct Object* nameValueCaller = obj_get_object_property(nameValue, "caller");

	struct Array* names = obj_send_message(Array(), "arrayWithElements", RETAIN(length), RETAIN(name), String(), NULL);

	printf("String \"%s\", Length == %d\n", (const char*)nameValue->value, *(int*)lengthValue->value);

	struct String* format = STRING("Name: %@, length: %@");

	struct String* formattedString = obj_send_message(obj_send_message(String(), "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = obj_send_message(name, "description");
	struct String* numberDescription = obj_send_message(length, "description");

	struct ClassC* c = obj_send_message(obj_send_message(ClassC(), "alloc"), "init");

	obj_send_message(c, "helloFromA");
	obj_send_message(c, "helloFromB");

	RELEASE(c);

	RELEASE(format);
	RELEASE(name);
	RELEASE(length);
	RELEASE(names);
	RELEASE(formattedString);
	RELEASE(stringDescription);
	RELEASE(numberDescription);
	RELEASE(nameValue);
	RELEASE(lengthValue);

	obj_shutdown_runtime();

	return 0;
}
