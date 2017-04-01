#include <lolbject/runtime.h>
#include <lolbject/macros.h>
#include <lolbject/Lolbject.h>
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/Box.h>
#include <lolbject/Array.h>
#include <lolbject/TreeObject.h>
#include <lolbject/SignalSender.h>

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	lolbj_init_runtime();

	//lolbj_print_class_diagram();

	for (size_t i = 1; i < (size_t)argc; i++) {
		struct LolModule* module = lolbj_send_message(LolRuntime, "loadModuleFromFile", STRING(argv[i]));

		if (module) {
			lolbj_send_message(LolRuntime, "registerModule", module);
			lolbj_print_class_diagram();
		}
	}

	struct String* name = STRING("Leandro");
	
	struct Number* length = lolbj_send_message(name, "length");

	struct Box* lengthValue = lolbj_send_message(length, "boxedValue");

	struct Box* nameValue = lolbj_send_message(name, "boxedValue");

	struct Object* nameValueCaller = RETAIN(lolbj_get_object_property(nameValue, "caller"));

	struct Array* names = ARRAY(RETAIN(length), STRING("Maria"), RETAIN(name), String);
	assert(lolbj_send_message(names, "objectAtIndex", INT(0)) == length);
	assert(lolbj_send_message(names, "objectAtIndex", INT(2)) == name);
	assert(lolbj_send_message(names, "objectAtIndex", INT(3)) == String);
	assert(lolbj_send_message(names, "objectAtIndex", INT(4)) == NULL);
	RELEASE(names);

	struct TreeObject* tree = lolbj_send_message(lolbj_send_message(TreeObject, "alloc"), "init");
	struct Number* tree_child_1 = lolbj_send_message(lolbj_send_message(tree, "allocChild", Number), "initWithInt", 42);
	struct String* tree_child_2 = lolbj_send_message(lolbj_send_message(tree, "allocChild", String), "initWithString", "Hello world");

	struct TreeObject* sub_tree_1 = lolbj_send_message(lolbj_send_message(tree, "allocChild", TreeObject), "init");

	assert(tree_child_1);
	assert(tree_child_2);

	RELEASE(tree);

	assert(nameValue);
	assert(lengthValue);

	printf("String \"%s\", Length == %d\n", (const char*)nameValue->value, *(int*)lengthValue->value);

	struct String* format = STRING("Name: %@, length: %@");

	struct String* formattedString = lolbj_send_message(lolbj_send_message(String, "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = lolbj_send_message(name, "description");
	struct String* numberDescription = lolbj_send_message(length, "description");

	struct SignalSender* s = lolbj_send_message(lolbj_send_message(SignalSender, "alloc"), "init");

	lolbj_send_message(s, "connect", "void_method", format, "description");
	lolbj_send_message(s, "connect", "void_method", format, "description");

	lolbj_send_message(s, "emit", "void_method"); //, STRING("Manoel"), STRING("Munich"), STRING("Lalaland"), NULL);

	RELEASE(s);

	struct LolModule* mixinExampleModule;
	
	if (mixinExampleModule = lolbj_send_message(LolRuntime, "loadModuleFromFile", STRING("./mixin_example/libmixin_example.so"))) {
		struct Lolbject* c = lolbj_send_message(lolbj_send_message(lolbj_send_message(mixinExampleModule, "classWithName", STRING("ClassC")), "alloc"), "init");

		struct Lolbject* d = lolbj_send_message(lolbj_send_message(lolbj_send_message(mixinExampleModule, "classWithName", STRING("ClassD")), "alloc"), "initWithName", STRING("Lalalala"));

		lolbj_send_message(c, "helloFromA");
		lolbj_send_message(c, "helloFromB");

		// must be the implementation in D
		lolbj_send_message(d, "helloFromA");

		assert(lolbj_cast(Lolbject, format) == format);
		assert(lolbj_cast(Number, format) == NULL);
		assert(lolbj_cast(Number, NULL) == NULL);
		assert(lolbj_cast(NULL, format) == format);
		assert(lolbj_cast(NULL, NULL) == NULL);

		RELEASE(c);
		RELEASE(d);

		RELEASE(mixinExampleModule);
	}

	RELEASE(format);
	RELEASE(name);
	RELEASE(length);
	RELEASE(formattedString);
	RELEASE(stringDescription);
	RELEASE(numberDescription);
	RELEASE(nameValue);
	RELEASE(lengthValue);
	RELEASE(nameValueCaller);

	lolbj_shutdown_runtime();

	return 0;
}
