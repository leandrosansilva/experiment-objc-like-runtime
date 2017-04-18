#include <lolbject/Anonymous.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>

struct Anonymous
{
	struct Lolbject super;
	struct Lolbject* context;
	lolbj_selector selector;
};

struct LolClass* Anonymous;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Anonymous);
	return NULL;
}

static struct Anonymous* init_with_context_and_selector(struct Anonymous* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, Anonymous, "init")) {
		self->context = va_arg(arguments, struct Lolbject*);
		self->selector = va_arg(arguments, lolbj_selector);
	}

	return self;
}

static struct Lolbject* execute_selector(struct Anonymous* self, va_list arguments)
{
	struct Lolbject* result = self->selector(self->context, arguments);
	return result;
}

static struct Lolbject* dealloc_selector(struct Anonymous* self, va_list arguments)
{
	RELEASE(self->context);
	return lolbj_send_message_to_super(self, Anonymous, "dealloc");
}

void lolbj_anonymous_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "initWithContextAndSelector", init_with_context_and_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
	lolbj_add_selector(klass, "execute", execute_selector);
}
