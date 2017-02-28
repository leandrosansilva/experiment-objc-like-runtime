#include "object.h"
#include "String.h"
#include "Number.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	initializeClass(Object_Class_Instance(), objectInitializer, NULL);
	initializeClass(String_Class_Instance(), stringInitializer, Object_Class_Instance());
	initializeClass(Number_Class_Instance(), numberInitializer, Object_Class_Instance());

	struct String* name = obj_send_message(allocString(), "initWithString", "Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	printf("String Length == %d\n", length->value);

	struct String* format = obj_send_message(allocString(), "initWithString", "Name: %@, length: %@");

	struct String* formattedString = obj_send_message(allocString(), "initWithFormat", format, name, length); 

	struct String* stringDescription = obj_send_message(name, "description");
	struct String* numberDescription = obj_send_message(length, "description");
	
	releaseObject(&format);
	releaseObject(&name);
	releaseObject(&length);
	releaseObject(&formattedString);
	releaseObject(&stringDescription);
	releaseObject(&numberDescription);

	unloadClass(Object_Class_Instance());
	unloadClass(String_Class_Instance());
	unloadClass(Number_Class_Instance());

	return 0;
}
