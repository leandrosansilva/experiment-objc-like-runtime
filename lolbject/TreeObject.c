#include <lolbject/TreeObject.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/MutableArray.h>
#include <lolbject/macros.h>
#include <lolbject/Number.h>

#include <stdlib.h>
#include <stdint.h>

struct TreeObject
{
	struct Lolbject super;
	struct MutableArray* children;
};

struct LolClass* TreeObject;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct TreeObject);
	return NULL;
}

static struct TreeObject* init_selector(struct TreeObject* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, TreeObject, "init")) {
		self->children = lolbj_send_message(lolbj_send_message(MutableArray, "alloc"), "initWithCapacity", INT(0));
	}

	return self;
}

static struct Lolbject* alloc_child_selector(struct TreeObject* self, va_list arguments)
{
	struct LolClass* klass = va_arg(arguments, struct LolClass*);
	struct Lolbject* obj = lolbj_send_message(klass, "alloc");
	lolbj_send_message(self->children, "append", obj);
	return obj;
}

static struct Lolbject* dealloc_selector(struct TreeObject* self, va_list arguments)
{
	RELEASE(self->children);
	return lolbj_send_message_to_super(self, TreeObject, "dealloc");
}

void lolbj_treeobject_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "init", init_selector);
	lolbj_add_selector(klass, "allocChild", alloc_child_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
}
