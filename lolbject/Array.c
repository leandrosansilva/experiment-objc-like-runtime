#include <lolbject/Array.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/Lolbject.h>
#include <lolbject/Box.h>
#include <lolbject/macros.h>

struct Array
{
	struct Lolbject super;
	struct Lolbject** elements;
	size_t capacity;
	size_t length;
};

struct LolClass* Array;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Array);
	return NULL;
}

static struct Array* array_with_elements_selector(struct LolClass* self, va_list arguments)
{
	return lolbj_send_message_with_arguments(lolbj_send_message(self, "alloc"), "initWithElements", arguments);
}

static struct Array* init_with_elements_selector(struct Array* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, Array, "init")) {
		size_t number_of_arguments = lolbj_number_of_call_arguments_ending_on_null(arguments);

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

	lolbj_send_message_to_super(self, Array, "dealloc");

	return NULL;
}

static struct Lobject* get_element_selector(struct Array* self, va_list arguments)
{
	// NOTE: takes ownership over the index, making its usage easier
	// TODO: maybe receive an Index Object instead?
	struct Number* indexObj = va_arg(arguments, struct Number*);
	struct Box* indexValue = lolbj_send_message(indexObj, "boxedValue");

	int index = *((int*)indexValue->value);

	struct Lolbject* result = NULL;

	if (index >= 0 && index < (int)self->length) {
		result = self->elements[(size_t)index];
	}

	RELEASE(indexValue);
	RELEASE(indexObj);

	return result;
}

void lolbj_array_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);
	lolbj_add_class_selector(klass, "arrayWithElements", array_with_elements_selector);

	lolbj_add_selector(klass, "initWithElements", init_with_elements_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
	lolbj_add_selector(klass, "objectAtIndex", get_element_selector);
}
