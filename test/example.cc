#include <stdio.h>

#include "example.h"
#include "example2.h"
#include "droppp.h"

int main(int argc, char* argv[]) {
    printf("Hello World!\n");

    AOtherGuy* actor = new AOtherGuy();
    actor->x = 20.f;
    actor->z = 30.f;

    void* x_data = droppp_meta_read_field("z", 2, &AOtherGuy_meta, actor);
    printf("X: %.3f\n", *(float*)x_data);

    return 0;
}