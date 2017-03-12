#pragma once

#define STRING(S) lolbj_send_message(String, "stringWithString", S)
#define INT(OBJ) lolbj_send_message(lolbj_send_message(Number, "alloc"), "initWithInt", OBJ)
#define ARRAY(...) lolbj_send_message(Array, "arrayWithElements", ##__VA_ARGS__, NULL)
#define RETAIN(OBJ) lolbj_send_message((OBJ), "retain")
#define RELEASE(OBJ) lolbj_send_message(lolbj_class_for_object(OBJ), "release", &OBJ)
