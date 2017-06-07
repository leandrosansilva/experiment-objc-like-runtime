#include <lolbject/DefaultAllocator.h>
#include <lolbject/Lolbject.h>
#include <lolbject/Lolbject_Private.h>
#include <lolbject/runtime.h>

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

struct LolClass* DefaultAllocator;

struct DefaultAllocator
{
	struct Lolbject super;
};

static struct Lolbject* allocator_deallocate_selector(struct DefaultAllocator* self, va_list arguments)
{
	struct Lolbject* obj = va_arg(arguments, struct Lolbject*);
	free(obj);

	return self;
}

static struct Lolbject* allocator_allocate_selector(struct DefaultAllocator* self, va_list arguments)
{
	size_t size = va_arg(arguments, size_t);

	assert(size >= sizeof(struct Lolbject) && "Object is too small!");

	// NOTE: object and private stuff are allocated contiguously
	size_t totalSize = size + sizeof(struct Lolbject_Private);
	struct Lolbject* obj = malloc(totalSize);
	memset(obj, 0, totalSize);
	obj->priv = ((uint8_t*)obj) + size;

	return obj;
}

void lolbj_default_allocator_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "allocateObject", allocator_allocate_selector);
	lolbj_add_class_selector(klass, "deleteObject", allocator_deallocate_selector);
}
