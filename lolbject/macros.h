#pragma once

#define LOL(...) lolbj_send_message(__VA_ARGS__)
#define LOL_A(...) lolbj_send_message_with_arguments(__VA_ARGS__)
#define STRING(S) LOL(String, "stringWithString", S)
#define INT(OBJ) LOL(LOL(Number, "alloc"), "initWithInt", (OBJ))
#define ARRAY(...) lolbj_send_message(Array, "arrayWithElements", ##__VA_ARGS__, NULL)
#define RETAIN(OBJ) LOL((OBJ), "retain")
#define RELEASE(OBJ) LOL(lolbj_class_for_object(OBJ), "release", &OBJ)

#define LOL_CONNECT(SENDER, SIGNAL, RECEIVER, SLOT) LOL(SENDER, "connect", SIGNAL, RECEIVER, SLOT)
#define LOL_EMIT(SENDER, SIGNAL, ...) LOL(SENDER, "emit", SIGNAL, ##__VA_ARGS__)
