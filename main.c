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
	
	releaseObject(&format);
	releaseObject(&name);
	releaseObject(&length);
	releaseObject(&formattedString);
	releaseObject(&stringDescription);
	releaseObject(&numberDescription);

	unloadClass(Object());
	unloadClass(String());
	unloadClass(Number());

	return 0;
}
