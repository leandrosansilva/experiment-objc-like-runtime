#include <lolbject/runtime.h>
#include <lolbject/Lolbject.h>
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/Box.h>
#include <lolbject/Array.h>

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	obj_init_runtime();

	obj_print_class_diagram();

	obj_load_module_from_file("mixin_example/libmixin_example.so");

	obj_print_class_diagram();

	struct String* name = STRING("Leandro");
	
	struct Number* length = obj_send_message(name, "length");

	struct Box* lengthValue = obj_send_message(length, "boxedValue");

	struct Box* nameValue = obj_send_message(name, "boxedValue");

	struct Object* nameValueCaller = RETAIN(obj_get_object_property(nameValue, "caller"));

	struct Array* names = ARRAY(RETAIN(length), RETAIN(name), String());

	assert(obj_send_message(names, "objectAtIndex", INT(0)) == length);
	assert(obj_send_message(names, "objectAtIndex", INT(1)) == name);
	assert(obj_send_message(names, "objectAtIndex", INT(2)) == String());
	assert(obj_send_message(names, "objectAtIndex", INT(3)) == NULL);

	printf("String \"%s\", Length == %d\n", (const char*)nameValue->value, *(int*)lengthValue->value);

	struct String* format = STRING("Name: %@, length: %@");

	struct String* formattedString = obj_send_message(obj_send_message(String(), "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = obj_send_message(name, "description");
	struct String* numberDescription = obj_send_message(length, "description");

	struct Class* c = obj_send_message(obj_send_message(obj_class_with_name("ClassC"), "alloc"), "init");

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
	RELEASE(nameValueCaller);

	obj_shutdown_runtime();

	return 0;
}
