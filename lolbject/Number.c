#include <lolbject/Number.h>

#include <lolbject/Class.h>
#include <lolbject/Box.h>
#include <lolbject/String.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct Number
{
	struct Lolbject super;
	int value;
};

struct LolClass* Number;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Number);
	return NULL;
}

static struct Number* init_with_integer(struct Number* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, Number, "init")) {
		self->value = va_arg(arguments, int);
	}

	return self;
}

static struct String* description_selector(struct Number* self, va_list arguments)
{
	char buff[32];
	sprintf(buff, "%d", self->value);
	return STRING(buff);
}

static struct Box* get_boxed_value(struct Number* self, va_list arguments)
{
	return lolbj_send_message(lolbj_send_message(Box, "alloc"), "initWithValue", self, &self->value);
}

void lolbj_number_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "initWithInt", init_with_integer);
	lolbj_add_selector(klass, "boxedValue", get_boxed_value);
	lolbj_add_selector(klass, "description", description_selector);
}
