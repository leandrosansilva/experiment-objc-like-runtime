#pragma once

struct Class_Number;

struct Number;

void obj_number_initializer(struct Class_Number* klass);

struct Class_Number* Number();

#define Int(OBJ) obj_send_message(obj_send_message(Number(), "alloc"), "initWithInt", OBJ)
