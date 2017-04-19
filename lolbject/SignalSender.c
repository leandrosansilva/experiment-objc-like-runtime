#include <lolbject/SignalSender.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/MutableArray.h>
#include <lolbject/macros.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct Connector
{
	const char* signal_name;
	const char* slot_name;
	struct Lolbject* receiver;
};

struct SignalSender
{
	struct Lolbject super;
	size_t connectors_length;
	struct Connector connectors[64]; // TODO: replace by a dictionary
};

struct LolClass* SignalSender;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct SignalSender);
	return NULL;
}

static struct Lolbject* connect_selector(struct SignalSender* self, va_list arguments)
{
	const char* signal_name = va_arg(arguments, const char*);
	struct Lolbject* receiver = va_arg(arguments, struct Lolbject*);
	const char* receiver_slot = va_arg(arguments, const char*);

	self->connectors[self->connectors_length++] = (struct Connector){
		.receiver = receiver,
		.signal_name = signal_name,
		.slot_name = receiver_slot
	};

	return self;
}

static struct Lolbject* disconnect_selector(struct SignalSender* self, va_list arguments)
{
	return self;
}

static struct SignalSender* init_selector(struct SignalSender* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, SignalSender, "init")) {
	}

	return self;
}

static struct Lolbject* send_signal_selector(struct SignalSender* self, va_list arguments)
{
	struct MutableArray* result = lolbj_send_message(lolbj_send_message(MutableArray, "alloc"), "init");

	const char* signal_name = va_arg(arguments, const char*);

	for (size_t i = 0; i < self->connectors_length; i++) {
		if (strcmp(signal_name, self->connectors[i].signal_name) != 0) {
			continue;
		}

		const char* slot_name = self->connectors[i].slot_name;
		struct Lolbject* receiver = self->connectors[i].receiver;

		va_list arguments_tail;
		va_copy(arguments_tail, arguments);
		struct Lolbject* v = lolbj_send_message_with_arguments(receiver, slot_name, arguments_tail);
		lolbj_send_message(result, "append", v);
		va_end(arguments_tail);
	}

	return result;
}

static struct Lolbject* dealloc_selector(struct SignalSender* self, va_list arguments)
{
	return lolbj_send_message_to_super(self, SignalSender, "dealloc");
}

void lolbj_signalsender_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "init", init_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
	lolbj_add_selector(klass, "emit", send_signal_selector);
	lolbj_add_selector(klass, "connect", connect_selector);
	lolbj_add_selector(klass, "disconnect", disconnect_selector);
}
