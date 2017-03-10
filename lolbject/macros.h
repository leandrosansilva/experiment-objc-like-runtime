#pragma once

#define STRING(S) obj_send_message(String(), "stringWithString", S)
#define INT(OBJ) obj_send_message(obj_send_message(Number(), "alloc"), "initWithInt", OBJ)
#define ARRAY(...) obj_send_message(Array(), "arrayWithElements", ##__VA_ARGS__, NULL)
#define RETAIN(OBJ) obj_send_message((OBJ), "retain")
#define RELEASE(OBJ) obj_send_message(obj_class_for_object(OBJ), "release", &OBJ)
