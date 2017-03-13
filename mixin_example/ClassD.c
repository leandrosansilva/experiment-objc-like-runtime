#include "module.h"
#include "ClassC.h"

#include <lolbject/Class.h>
#include <lolbject/String.h>
#include <lolbject/Box.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>

#include <stddef.h>
#include <stdio.h>

struct ClassD
{
	struct ClassC super;
	struct String* name;
};

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassD);
	return NULL;
}

static struct ClassD* init_with_name(struct ClassD* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, ClassD, "init")) {
		struct String* name = va_arg(arguments, struct String*);
		lolbj_set_object_property(self, "name", name);
	}

	return self;
}

static struct Lolbject* dealloc_selector(struct ClassD* self, va_list arguments)
{
	RELEASE(self->name);
	return lolbj_send_message_to_super(self, ClassD, "dealloc");
}

static struct Lolbject* hello_from_a_selector(struct ClassD* self, va_list arguments)
{
	struct Box* nameValue = lolbj_send_message(self->name, "boxedValue");
	printf("ClassD overrides ClassC with helloFromA: %s\n", (const char*)nameValue->value);
	RELEASE(nameValue);
	return NULL;
}

void lolbj_class_d_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, lolbj_class_with_name(mixin_module, "ClassC"));

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "initWithName", init_with_name);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
	lolbj_add_selector(klass, "helloFromA", hello_from_a_selector);

	lolbj_add_property(klass, "name", String, offsetof(struct ClassD, name));
}
