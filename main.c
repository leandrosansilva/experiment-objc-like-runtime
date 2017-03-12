#include <lolbject/runtime.h>
#include <lolbject/Lolbject.h>
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/Box.h>
#include <lolbject/Array.h>
#include <lolbject/macros.h>

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	lolbj_init_runtime();

	//lolbj_print_class_diagram();

	for (size_t i = 1; i < (size_t)argc; i++) {
		struct LolModule* module = lolbj_load_module_from_file(argv[i]);

		if (module) {
			lolbj_register_module(module);
			lolbj_print_class_diagram();
		}
	}

	struct String* name = STRING("Leandro");
	
	struct Number* length = lolbj_send_message(name, "length");

	struct Box* lengthValue = lolbj_send_message(length, "boxedValue");

	struct Box* nameValue = lolbj_send_message(name, "boxedValue");

	struct Object* nameValueCaller = RETAIN(lolbj_get_object_property(nameValue, "caller"));

	struct Array* names = ARRAY(RETAIN(length), RETAIN(name), String);

	assert(lolbj_send_message(names, "objectAtIndex", INT(0)) == length);
	assert(lolbj_send_message(names, "objectAtIndex", INT(1)) == name);
	assert(lolbj_send_message(names, "objectAtIndex", INT(2)) == String);
	assert(lolbj_send_message(names, "objectAtIndex", INT(3)) == NULL);

	printf("String \"%s\", Length == %d\n", (const char*)nameValue->value, *(int*)lengthValue->value);

	struct String* format = STRING("Name: %@, length: %@");

	struct String* formattedString = lolbj_send_message(lolbj_send_message(String, "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = lolbj_send_message(name, "description");
	struct String* numberDescription = lolbj_send_message(length, "description");

	struct LolModule* mixinExampleModule = lolbj_module_with_name("mixin_example");

	struct Class* c = lolbj_send_message(lolbj_send_message(lolbj_class_with_name(mixinExampleModule, "ClassC"), "alloc"), "init");

	lolbj_send_message(c, "helloFromA");
	lolbj_send_message(c, "helloFromB");

	assert(lolbj_cast(Lolbject, format) == format);
	assert(lolbj_cast(Number, format) == NULL);
	assert(lolbj_cast(Number, NULL) == NULL);
	assert(lolbj_cast(NULL, format) == format);
	assert(lolbj_cast(NULL, NULL) == NULL);

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

	lolbj_shutdown_runtime();

	return 0;
}
