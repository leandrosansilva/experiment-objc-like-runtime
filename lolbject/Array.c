#include <lolbject/Array.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/Lolbject.h>
#include <lolbject/Number.h>
#include <lolbject/macros.h>
#include <lolbject/MutableArray.h>

#include <stddef.h>

struct Array
{
	struct Lolbject super;
	struct MutableArray* array;
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
		self->array = lolbj_send_message(lolbj_send_message(MutableArray, "alloc"), "initWithCapacity", INT(0));

		for (size_t i = 0; i < number_of_arguments; i++) {
			struct Lolbject* element = va_arg(arguments, struct Lolbject*);
			lolbj_send_message(self->array, "append", element);
		}
	}

	return self;
}

static struct Lolbject* dealloc_selector(struct Array* self, va_list arguments)
{
	RELEASE(self->array);
	return lolbj_send_message_to_super(self, Array, "dealloc");
}

void lolbj_array_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_property(klass, "storage", MutableArray, offsetof(struct Array, array));
	lolbj_add_selector_from_property(klass, MutableArray, "storage", "length");
	lolbj_add_selector_from_property(klass, MutableArray, "storage", "objectAtIndex");

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);
	lolbj_add_class_selector(klass, "arrayWithElements", array_with_elements_selector);

	lolbj_add_selector(klass, "initWithElements", init_with_elements_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
}
