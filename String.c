#include "String.h"
#include "Number.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct Class_String _String_Class_Instance;

struct Class_String* String_Class_Instance()
{
	return &_String_Class_Instance;
}

struct String* allocString()
{
	struct String* string = allocObject(sizeof(struct String));
	string->super.klass = &_String_Class_Instance;
	string->content = NULL;
	return string;
}

// Selectors

static struct Number* string_length_selector(struct String* string, va_list arguments)
{
	int l = string->content != NULL ? strlen(string->content) : 0;
	return obj_send_message(allocNumber(), "initWithInt", l);
}

static struct Number* string_dealloc_selector(struct String* string, va_list arguments)
{
	if (string->content != NULL) {
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

void stringInitializer(struct Class_String* klass)
{
	klass->super.objectName = "String";

	obj_add_selector(klass, "length", string_length_selector);
	obj_add_selector(klass, "dealloc", string_dealloc_selector);
	obj_add_selector(klass, "initWithFormat", string_init_with_format_selector);
	obj_add_selector(klass, "initWithString", string_init_with_string_selector);
}
