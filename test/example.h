#pragma once

#include <stdint.h>

#include "droppp.h"

class Example {
   public:
    Example();

    uint32_t foo;
    // BadType badThing;
    char text[64];

    uint32_t getFoo();
};
DROPPP_REFLECT(Example);

class OtherExample {
   public:
    float thing2;

    Example otherThing;
};
DROPPP_REFLECT(OtherExample);

class Actor {
   public:
    float x;
    float y;
};
DROPPP_REFLECT(Actor);