#include <lolbject/runtime.h>
#include <lolbject/macros.h>
#include <lolbject/Lolbject.h>
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/Box.h>
#include <lolbject/Array.h>
#include <lolbject/TreeObject.h>
#include <lolbject/SignalSender.h>
#include <lolbject/Anonymous.h>

#include <stdio.h>
#include <assert.h>

static struct Lolbject* anon_1_execute_selector(struct Lolbject* context, va_list arguments)
{
	printf("Call anonymous object with context of type %s\n", lolbj_class_name(lolbj_class_for_object(context)));

	struct String* param = va_arg(arguments, struct String*);

	RELEASE(param);

	return INT(42);
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	lolbj_init_runtime();

	//lolbj_print_class_diagram();

	for (size_t i = 1; i < (size_t)argc; i++) {
		struct LolModule* module = LOL(LolRuntime, "loadModuleFromFile", STRING(argv[i]));

		if (module) {
			LOL(LolRuntime, "registerModule", module);
			lolbj_print_class_diagram();
		}
	}

	struct Anon* anonymous1 = LOL(LOL(Anonymous, "alloc"), "initWithContextAndSelector", STRING("My Name"), anon_1_execute_selector);

	struct Lolbject* r = LOL(anonymous1, "execute", STRING("Some Param"));

	RELEASE(r);
	RELEASE(anonymous1);

	struct String* name = STRING("Leandro");

	struct Number* length = LOL(name, "length");

	struct Box* lengthValue = LOL(length, "boxedValue");

	struct Box* nameValue = LOL(name, "boxedValue");

	struct Object* nameValueCaller = RETAIN(lolbj_get_object_property(nameValue, "caller"));

	struct Array* names = ARRAY(RETAIN(length), STRING("Maria"), RETAIN(name), String);
	assert(LOL(names, "objectAtIndex", INT(0)) == length);
	assert(LOL(names, "objectAtIndex", INT(2)) == name);
	assert(LOL(names, "objectAtIndex", INT(3)) == String);
	assert(LOL(names, "objectAtIndex", INT(4)) == NULL);
	RELEASE(names);

	struct TreeObject* tree = LOL(LOL(TreeObject, "alloc"), "init");
	struct Number* tree_child_1 = LOL(LOL(tree, "allocChild", Number), "initWithInt", 42);
	struct String* tree_child_2 = LOL(LOL(tree, "allocChild", String), "initWithString", "Hello world");

	struct TreeObject* sub_tree_1 = LOL(LOL(tree, "allocChild", TreeObject), "init");

	assert(tree_child_1);
	assert(tree_child_2);

	RELEASE(tree);

	assert(nameValue);
	assert(lengthValue);

	printf("String \"%s\", Length == %d\n", (const char*)nameValue->value, *(int*)lengthValue->value);

	struct String* format = STRING("Name: %@, length: %@");

	struct String* formattedString = LOL(LOL(String, "alloc"), "initWithFormat", format, name, length); 

	struct String* stringDescription = LOL(name, "description");
	struct String* numberDescription = LOL(length, "description");

	struct SignalSender* s = LOL(LOL(SignalSender, "alloc"), "init");

	LOL_CONNECT(s, "void_method", format, "description");
	LOL_EMIT(s, "void_method");

	RELEASE(s);

	struct LolModule* mixinExampleModule;
	
	if (mixinExampleModule = LOL(LolRuntime, "loadModuleFromFile", STRING("./mixin_example/libmixin_example.so"))) {
		struct Lolbject* c = LOL(LOL(LOL(mixinExampleModule, "classWithName", STRING("ClassC")), "alloc"), "init");

		struct Lolbject* d = LOL(LOL(LOL(mixinExampleModule, "classWithName", STRING("ClassD")), "alloc"), "initWithName", STRING("Lalalala"));

		LOL(c, "helloFromA");
		LOL(c, "helloFromB");

		// must be the implementation in D
		LOL(d, "helloFromA");

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
