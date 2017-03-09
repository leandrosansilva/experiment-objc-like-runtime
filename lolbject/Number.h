#pragma once

#include <lolbject/Class.h>

struct Number;

void obj_number_initializer(struct LolClass* klass);

struct LolClass* Number();

#define Int(OBJ) obj_send_message(obj_send_message(Number(), "alloc"), "initWithInt", OBJ)
