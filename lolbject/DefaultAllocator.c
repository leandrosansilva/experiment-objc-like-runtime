#include <lolbject/DefaultAllocator.h>
#include <lolbject/Lolbject.h>
#include <lolbject/Lolbject_Private.h>
#include <lolbject/runtime.h>

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

struct DefaultAllocator
{
	struct Lolbject super;
};

static struct Lolbject* allocator_deallocate_selector(struct DefaultAllocator* self, va_list arguments)
{
	struct Lolbject* obj = va_arg(arguments, struct Lolbject*);
	free(obj->priv);
	free(obj);

	return self;
}

static struct Lolbject* allocator_allocate_selector(struct DefaultAllocator* self, va_list arguments)
{
	size_t size = va_arg(arguments, size_t);

	if (size == 0) {
		return NULL;
	}

	assert(size >= sizeof(struct Lolbject) && "Object is too small!");

	// TODO: alloc both object and private stuff in the a single block of memory
	struct Lolbject* obj = malloc(size);
	memset(obj, 0, size);

	obj->priv = malloc(sizeof(struct Lolbject_Private));
	memset(obj->priv, 0, sizeof(struct Lolbject_Private));

	return obj;
}

void lolbj_default_allocator_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "allocateMemory", allocator_allocate_selector);
	lolbj_add_class_selector(klass, "deleteMemory", allocator_deallocate_selector);
}
