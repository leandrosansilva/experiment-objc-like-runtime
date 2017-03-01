#include "String.h"
#include "Number.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Class_String
{
	struct Class_Object super;
};

static struct Class_String _String;

struct Class_String* String()
{
	return &_String;
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct String);
	return NULL;
}

static struct Number* string_length_selector(struct String* string, va_list arguments)
{
	int l = string->content != NULL ? strlen(string->content) : 0;
	return obj_send_message(obj_send_message(Number(), "alloc"), "initWithInt", l);
}

static struct Number* string_dealloc_selector(struct String* string, va_list arguments)
{
	if (string != NULL && string->content != NULL) {
		free(string->content);
	}
}

static struct String* string_init_with_format_selector(struct String* string, va_list arguments)
{
	// TODO: implement!
	return string;
}

static struct String* string_init_with_string_selector(struct String* self, va_list arguments)
{
	char* string = va_arg(arguments, char*);
	self->content = malloc(strlen(string) + 1);
	strcpy(self->content, string);
	return self;
}

void obj_string_initializer(struct Class_String* klass)
{
	klass->super.objectName = "String";

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "length", string_length_selector);
	obj_add_selector(klass, "dealloc", string_dealloc_selector);
	obj_add_selector(klass, "initWithFormat", string_init_with_format_selector);
	obj_add_selector(klass, "initWithString", string_init_with_string_selector);
}
