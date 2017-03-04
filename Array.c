#include "Array.h"

#include "Class.h"
#include "runtime.h"
#include "Object.h"

struct Array
{
	struct Object super;
	struct Object** elements;
	size_t capacity;
	size_t length;
};

static struct Class_Object _Array;

struct Class_Array* Array()
{
	return &_Array;
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Array);
	return NULL;
}

static struct Array* array_with_elements_selector(struct Class_Array* self, va_list arguments)
{
	return obj_send_message_with_arguments(obj_send_message(self, "alloc"), "initWithElements", arguments);
}

static struct Array* init_with_elements_selector(struct Array* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		size_t number_of_arguments = obj_number_of_call_arguments_ending_on_null(arguments);

		self->capacity = number_of_arguments;
		self->length = number_of_arguments;

		self->elements = calloc(sizeof(struct Object*), number_of_arguments);

		for (size_t i = 0; i < number_of_arguments; i++) {
			struct Object* element = va_arg(arguments, struct Object*);
			self->elements[i] = element;
		}
	}

	return self;
}

static struct Object* dealloc_selector(struct Array* self, va_list arguments)
{
	if (self->elements != NULL) {
		for (size_t i = 0; i < self->length; i++) {
			RELEASE(self->elements[i]);
		}
		free(self->elements);
	}

	obj_send_message_to_super(self, "dealloc");

	return NULL;
}


void obj_array_initializer(struct Class_Array* klass)
{
	obj_set_class_parent(klass, Object());
	obj_set_class_name(klass, "Array");

	obj_add_class_selector(klass, "objectSize", object_size_selector);
	obj_add_class_selector(klass, "arrayWithElements", array_with_elements_selector);

	obj_add_selector(klass, "initWithElements", init_with_elements_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);
}
