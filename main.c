#include "runtime.h"
#include "object.h"
#include "String.h"
#include "Number.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	initializeClass(Object(), objectInitializer, NULL);
	initializeClass(Class(), classInitializer, Object());
	initializeClass(String(), stringInitializer, Object());
	initializeClass(Number(), numberInitializer, Object());

	struct String* name = obj_send_message(obj_send_message(String(), "alloc"), "initWithString", "Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	printf("String Length == %d\n", length->value);

	struct String* format = obj_send_message(obj_send_message(String(), "alloc"), "initWithString", "Name: %@, length: %@");

	struct String* formattedString = obj_send_message(obj_send_message(String(), "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = obj_send_message(name, "description");
	struct String* numberDescription = obj_send_message(length, "description");
	
	obj_send_message(Object(), "releaseObject", &format);
	obj_send_message(Object(), "releaseObject", &name);
	obj_send_message(Object(), "releaseObject", &length);
	obj_send_message(Object(), "releaseObject", &formattedString);
	obj_send_message(Object(), "releaseObject", &stringDescription);
	obj_send_message(Object(), "releaseObject", &numberDescription);

	obj_unload_class(Class());
	obj_unload_class(Object());
	obj_unload_class(String());
	obj_unload_class(Number());

	return 0;
}
