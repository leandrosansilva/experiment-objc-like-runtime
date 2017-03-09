#include <lolbject/String.h>

#include <lolbject/Class.h>
#include <lolbject/Number.h>
#include <lolbject/Box.h>
#include <lolbject/runtime.h>

#include <stdlib.h>
#include <string.h>

struct String
{
	struct Lolbject super;
	char* content;
};

static struct LolClass _String;

struct LolClass* String()
{
	return &_String;
}

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct String);
	return NULL;
}

static struct Number* string_length_selector(struct String* self, va_list arguments)
{
	int l = self->content != NULL ? strlen(self->content) : 0;
	return obj_send_message(obj_send_message(Number(), "alloc"), "initWithInt", l);
}

static struct Lolbject* string_dealloc_selector(struct String* self, va_list arguments)
{
	if (self->content != NULL) {
		free(self->content);
	}

	obj_send_message_to_super(self, "dealloc");

	return NULL;
}

// the description of a string is the string itself!
static struct String* description_selector(struct String* self, va_list arguments)
{
	return RETAIN(self);
}

static struct String* string_init_with_format_selector(struct String* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		// TODO: implement!
	}

	return self;
}

static struct String* string_init_with_string_selector(struct String* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		char* string = va_arg(arguments, char*);
		self->content = malloc(strlen(string) + 1);
		strcpy(self->content, string);
	}

	return self;
}

static struct String* string_from_string_selector(struct LolClass* self, va_list arguments)
{
	return obj_send_message_with_arguments(obj_send_message(self, "alloc"), "initWithString", arguments);
}

static struct Box* get_boxed_value(struct String* self, va_list arguments)
{
	return obj_send_message(obj_send_message(Box(), "alloc"), "initWithValue", self, self->content);
}

void obj_string_initializer(struct LolClass* klass)
{
	obj_set_class_parent(klass, Lolbject());
	obj_set_class_name(klass, "String");

	obj_add_class_selector(klass, "objectSize", object_size_selector);
	obj_add_class_selector(klass, "stringWithString", string_from_string_selector);

	obj_add_selector(klass, "length", string_length_selector);
	obj_add_selector(klass, "description", description_selector);
	obj_add_selector(klass, "dealloc", string_dealloc_selector);
	obj_add_selector(klass, "initWithFormat", string_init_with_format_selector);
	obj_add_selector(klass, "initWithString", string_init_with_string_selector);
	obj_add_selector(klass, "boxedValue", get_boxed_value);
}
