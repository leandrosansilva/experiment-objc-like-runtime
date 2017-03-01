#include "Object.h"
#include "runtime.h"
#include "String.h"
#include "Number.h"

static struct Class_Object _Object;

struct Class_Object* Object()
{
	return &_Object;
}

static struct String* description_selector(struct Object* self, va_list arguments)
{
	struct String* nameAsDescription = obj_send_message(obj_send_message(String(), "alloc"), "initWithString", self->klass->objectName);
	return nameAsDescription; // TODO: format like: "Object <address>"
}

void obj_object_initializer(struct Class_Object* klass)
{
	klass->objectName = "Object";
	obj_add_selector(klass, "description", description_selector);
}
