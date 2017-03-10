#pragma once

#include <lolbject/Class.h>

struct Array;

void obj_array_initializer(struct LolClass* klass);

struct LolClass* Array();

#define ARRAY(...) obj_send_message(Array(), "arrayWithElements", ##__VA_ARGS__, NULL)
