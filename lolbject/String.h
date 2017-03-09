#pragma once

#include <lolbject/Class.h>
#include <lolbject/Lolbject.h>

struct String;

struct LolClass* String();

void obj_string_initializer(struct LolClass* klass);

#define STRING(S) obj_send_message(String(), "stringWithString", S)
