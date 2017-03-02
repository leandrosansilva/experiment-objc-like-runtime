#include "runtime.h"
#include "Object.h"
#include "String.h"
#include "Number.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	obj_init_runtime();

	obj_initialize_class(Object(), obj_object_initializer);
	obj_initialize_class(Class(), obj_class_initializer);
	obj_initialize_class(String(), obj_string_initializer);
	obj_initialize_class(Number(), obj_number_initializer);

	obj_print_class_diagram();

	struct String* name = obj_send_message(obj_send_message(String(), "alloc"), "initWithString", "Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	//printf("String Length == %d\n", length->value);

	struct String* format = obj_send_message(obj_send_message(String(), "alloc"), "initWithString", "Name: %@, length: %@");

	struct String* formattedString = obj_send_message(obj_send_message(String(), "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = obj_send_message(name, "description");
	struct String* numberDescription = obj_send_message(length, "description");
	
	obj_send_message(Object(), "release", &format);
	obj_send_message(Object(), "release", &name);
	obj_send_message(Object(), "release", &length);
	obj_send_message(Object(), "release", &formattedString);
	obj_send_message(Object(), "release", &stringDescription);
	obj_send_message(Object(), "release", &numberDescription);

	obj_unload_class(Class());
	obj_unload_class(Object());
	obj_unload_class(String());
	obj_unload_class(Number());

	return 0;
}
