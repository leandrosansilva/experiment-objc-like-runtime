#!/usr/bin/env python

"""
	NOTE: "final" classes, which do not accept being base of other classes
	do NOT expose their structure publicly, so they cannot be embedded.

	Non final classes, on the other hand, must expose them.

	FIXME: for now the current template generate only final classes

	TODO: maybe the type Class_${class_name} isn't really required after all,
	simplifying a lot the implementation, unless the class should store something.

	If it needs to store anything, this should be done using static variables instead,
	as classes should be static as well!
"""

from string import Template
from argparse import ArgumentParser
import re
import sys

header_template_text = """#pragma once

struct ${class_name};

struct Class_${class_name};

void obj_${class_name_snakecase}_initializer(struct Class_${class_name}* klass);

struct Class_${class_name}* ${class_name}();
"""

implementation_template_text = """#include "${class_name}.h"

#include "Class.h"
#include "runtime.h"
#include "${parent_class_name}.h"

struct ${class_name}
{
	struct ${parent_class_name} super;
};

static struct Class_Object _${class_name};

struct Class_${class_name}* ${class_name}()
{
	return &_${class_name};
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ${class_name});
	return NULL;
}

static struct ${class_name}* init_selector(struct ${class_name}* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
	    // Initialize Members
	}

	return self;
}

void obj_${class_name_snakecase}_initializer(struct Class_${class_name}* klass)
{
	obj_set_class_parent(klass, ${parent_class_name}());
	obj_set_class_name(klass, "${class_name}");

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "init", init_selector);
}
"""

def camel_to_snake_case(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

parser = ArgumentParser(description="Create new class")
parser.add_argument('--name')
parser.add_argument('--parent')

args = parser.parse_args()

if args.name == None or args.parent == None:
	sys.exit(1)

class_name = args.name
class_name_snakecase = camel_to_snake_case(class_name)
parent_class_name = args.parent

header_content = Template(header_template_text).substitute(class_name=class_name, class_name_snakecase=class_name_snakecase, parent_class_name=parent_class_name)

implementation_content = Template(implementation_template_text).substitute(class_name=class_name, class_name_snakecase=class_name_snakecase, parent_class_name=parent_class_name)

with open(class_name + ".h", "w") as f:
	f.write(header_content)

with open(class_name + ".c", "w") as f:
	f.write(implementation_content)
