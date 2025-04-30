#pragma once

#include <stdint.h>

#include "droppp.h"

class Example {
   public:
    DROPPP_REFLECT(Example);

    Example();

    uint32_t foo;
    // BadType badThing;
    char text[64];

    uint32_t getFoo();
};

class OtherExample {
   public:
    DROPPP_REFLECT(OtherExample);

    float thing2;

    Example otherThing;
};

class AActor {
   public:
    DROPPP_REFLECT(AActor);

    float x;
    float y;
};

class AOtherGuy : public AActor {
   public:
    DROPPP_REFLECT(AOtherGuy);

    float z;
};