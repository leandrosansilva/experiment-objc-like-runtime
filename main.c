#include "object.h"
#include "String.h"
#include "Number.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	loadClassObject();
	initializeObject(Object_Class_Instance());

	loadClassString();
	initializeString(String_Class_Instance());

	loadClassNumber();
	initializeNumber(Number_Class_Instance());

	struct String* name = obj_send_message(allocString(), "initWithString", "Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	printf("String Length == %d\n", length->value);

	struct String* format = obj_send_message(allocString(), "initWithString", "Name: %@, length: %@");

	struct String* formattedString = obj_send_message(allocString(), "initWithFormat", format, name, length); 
	
	printf("Formatted string: %s\n", formattedString->content);

	deleteObject(name);
	deleteObject(length);
	deleteObject(formattedString);

	unloadClassString();
	unloadClassNumber();
	unloadClassObject();

	return 0;
}
