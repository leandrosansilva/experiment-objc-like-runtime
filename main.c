#include "object.h"
#include "String.h"
#include "Number.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	initializeClass(Object_Class_Instance(), NULL, NULL);
	initializeClass(String_Class_Instance(), stringInitializer, Object_Class_Instance());
	initializeClass(Number_Class_Instance(), numberInitializer, Object_Class_Instance());

	struct String* name = obj_send_message(allocString(), "initWithString", "Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	printf("String Length == %d\n", length->value);

	struct String* format = obj_send_message(allocString(), "initWithString", "Name: %@, length: %@");

	struct String* formattedString = obj_send_message(allocString(), "initWithFormat", format, name, length); 
	
	// TODO: rename to release Object and receive an object address,
	// so we can manage memory automatically
	deleteObject(format);
	deleteObject(name);
	deleteObject(length);
	deleteObject(formattedString);

	unloadClass(Object_Class_Instance());
	unloadClass(String_Class_Instance());
	unloadClass(Number_Class_Instance());

	return 0;
}
