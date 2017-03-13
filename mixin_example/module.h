#pragma once

#include <lolbject/runtime.h>

void lolbj_class_a_initializer(struct LolClass* klass);
void lolbj_class_b_initializer(struct LolClass* klass);
void lolbj_class_c_initializer(struct LolClass* klass);
void lolbj_class_d_initializer(struct LolClass* klass);

struct LolClass* ClassA;
struct LolClass* ClassB;
struct LolClass* ClassC;
struct LolClass* ClassD;
struct LolModule* mixin_module;
