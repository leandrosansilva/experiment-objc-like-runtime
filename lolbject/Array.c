#include <lolbject/Array.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/Lolbject.h>

struct Array
{
	struct Lolbject super;
	struct Lolbject** elements;
	size_t capacity;
	size_t length;
};

static struct LolClass _Array;

struct LolClass* Array()
{
	return &_Array;
}

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Array);
	return NULL;
}

static struct Array* array_with_elements_selector(struct LolClass* self, va_list arguments)
{
	return obj_send_message_with_arguments(obj_send_message(self, "alloc"), "initWithElements", arguments);
}

static struct Array* init_with_elements_selector(struct Array* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		size_t number_of_arguments = obj_number_of_call_arguments_ending_on_null(arguments);

		self->capacity = number_of_arguments;
		self->length = number_of_arguments;

		self->elements = calloc(sizeof(struct Lolbject*), number_of_arguments);

		for (size_t i = 0; i < number_of_arguments; i++) {
			struct Lolbject* element = va_arg(arguments, struct Lolbject*);
			self->elements[i] = element;
		}
	}

	return self;
}

static struct Lolbject* dealloc_selector(struct Array* self, va_list arguments)
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


void obj_array_initializer(struct LolClass* klass)
{
	obj_set_class_parent(klass, Lolbject());
	obj_set_class_name(klass, "Array");

	obj_add_class_selector(klass, "objectSize", object_size_selector);
	obj_add_class_selector(klass, "arrayWithElements", array_with_elements_selector);

	obj_add_selector(klass, "initWithElements", init_with_elements_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);
}
