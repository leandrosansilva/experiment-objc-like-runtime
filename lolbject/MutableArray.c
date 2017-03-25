#include <lolbject/MutableArray.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/Lolbject.h>
#include <lolbject/Box.h>
#include <lolbject/macros.h>
#include <lolbject/Number.h>

#include <stdio.h>

struct MutableArray
{
	struct Lolbject super;
	struct Lolbject** elements;
	size_t capacity;
	size_t length;
};

struct LolClass* MutableArray;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct MutableArray);
	return NULL;
}

static struct MutableArray* init_with_capacity_selector(struct MutableArray* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, MutableArray, "init")) {
		struct Number* capacityObj = va_arg(arguments, struct Number*);
		struct Box* capacityValue = lolbj_send_message(capacityObj, "boxedValue");

		self->capacity = *((int*)capacityValue->value);
		self->length = 0;
		self->elements = calloc(sizeof(struct Lolbject*), self->capacity);

		RELEASE(capacityValue);
		RELEASE(capacityObj);
	}

	return self;
}

static struct Number* length_selector(struct MutableArray* self, va_list arguments)
{
	return INT(self->length);
}

static struct Lolbject* dealloc_selector(struct MutableArray* self, va_list arguments)
{
	if (self->elements != NULL) {
		for (size_t i = 0; i < self->length; i++) {
			RELEASE(self->elements[i]);
		}
		free(self->elements);
	}

	return lolbj_send_message_to_super(self, MutableArray, "dealloc");
}

static struct Lobject* get_element_selector(struct MutableArray* self, va_list arguments)
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

static struct MutableArray* append_element_selector(struct MutableArray* self, va_list arguments)
{
	struct Number* length = lolbj_send_message(self, "length");
	struct Lolbject* value = va_arg(arguments, struct Lolbject*);
	return lolbj_send_message(self, "setObjectAtIndex", length, value);
}

static struct MutableArray* set_element_selector(struct MutableArray* self, va_list arguments)
{
	// NOTE: takes ownership over the index, making its usage easier
	// TODO: maybe receive an Index Object instead?
	struct Number* indexObj = va_arg(arguments, struct Number*);
	struct Lolbject* value = va_arg(arguments, struct Lolbject*);

	struct Box* indexValue = lolbj_send_message(indexObj, "boxedValue");
	int index = *((int*)indexValue->value);

	if (index < 0) {
		printf("### array index cannot be negative!\n");
		RELEASE(indexObj);
		RELEASE(indexValue);
		RELEASE(value);
		return NULL;
	}

	if ((size_t)index >= self->capacity) {
		size_t old_capacity = self->capacity;
		// to prevent multiplication issues
		self->capacity += self->capacity == 0 ? 2 : 0;
		self->capacity *= 1.5;
		printf("reallocing array to capacity %d\n", self->capacity);
		self->elements = realloc(self->elements, self->capacity * sizeof(struct Lolbject*));

		size_t capacity_difference = self->capacity - old_capacity;
		memset(self->elements + old_capacity, 0, capacity_difference * sizeof(struct Lolbject*));
	}

	if ((size_t)index >= self->length) {
		self->length = (size_t)index + 1;
		printf("new array size: %d\n", self->length);
	}

	printf("Setting element of kind %s to position %d\n",lolbj_class_name(lolbj_class_for_object(value)), index);
	self->elements[index] = value;

	RELEASE(indexObj);
	RELEASE(indexValue);

	return self;
}

void lolbj_mutablearray_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "initWithCapacity", init_with_capacity_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
	lolbj_add_selector(klass, "objectAtIndex", get_element_selector);
	lolbj_add_selector(klass, "setObjectAtIndex", set_element_selector);
	lolbj_add_selector(klass, "append", append_element_selector);
	lolbj_add_selector(klass, "length", length_selector);
}
