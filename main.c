#include "runtime.h"
#include "Object.h"
#include "String.h"
#include "Number.h"
#include "Box.h"
#include "Array.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	obj_init_runtime();

	obj_print_class_diagram();

	struct String* name = STRING("Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	struct Box* lengthValue = obj_send_message(length, "boxedValue");

	struct Box* nameValue = obj_send_message(name, "boxedValue");

	struct Array* names = obj_send_message(Array(), "arrayWithElements", RETAIN(length), RETAIN(name), String(), NULL);

	//printf("String \"%s\", Length == %d\n", (const char*)nameValue->value, *(int*)lengthValue->value);

	struct String* format = STRING("Name: %@, length: %@");

	struct String* formattedString = obj_send_message(obj_send_message(String(), "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = obj_send_message(name, "description");
	struct String* numberDescription = obj_send_message(length, "description");

	struct String* book = STRING("The book is on the table");
	struct Number* bookLength = obj_send_message(book, "length");
	struct Box* bookLengthBox = obj_send_message(bookLength, "boxedValue");

	struct Object* bookLengthCaller = obj_get_object_property(bookLengthBox, "caller");

	assert(bookLengthCaller == bookLength);

	RELEASE(bookLength);
	RELEASE(bookLengthBox);
	RELEASE(book);
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
