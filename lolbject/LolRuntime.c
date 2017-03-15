#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

struct LolRuntime {
	struct Lolbject super;
};

struct LolClass* LolRuntime;

// Selectors
static struct LolModule* create_selector(struct LolRuntime* self, va_list arguments)
{
	struct LolModule_Descriptor* descriptor = va_arg(arguments, struct LolModule_Descriptor*);
	struct LolModule* module = lolbj_create_module(descriptor);
	return module;
}

void lolbj_runtime_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);
	lolbj_add_class_selector(klass, "createModuleFromDescriptor", create_selector);
}
