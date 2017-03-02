#include "runtime.h"
#include "Object.h"
#include "String.h"
#include "Number.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	obj_init_runtime();

	//obj_print_class_diagram();

	struct String* name = obj_send_message(obj_send_message(String(), "alloc"), "initWithString", "Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	printf("String Length == %d\n", length->value);

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

	obj_shutdown_runtime();

	return 0;
}
