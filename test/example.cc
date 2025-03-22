#include <stdio.h>

#include "example.h"
#include "example2.h"
#include "droppp.h"

int main(int argc, char* argv[]) {
    printf("Hello World!\n");

    Actor* actor = new Actor();
    actor->x = 20.f;
    void* x_data = droppp_meta_read_field(droppp_meta_get_field_by_name(&Actor_meta, "y", 2), actor);
    printf("X: %.3f\n", *(float*)x_data);

    return 0;
}