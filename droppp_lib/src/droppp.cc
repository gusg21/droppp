#include "droppp.h"

#include <string.h>

static bool droppp_memcmp(const char* a, const char* b, size_t size) {
    for (uint32_t index = 0; index < size; index++) {
        if (a[index] != b[index]) {
            return false;
        }

        if (a[index] == '\0') {
            return true;
        }
    }

    return true;
}

void* droppp_meta_read_field(const char* name, size_t name_length, const struct droppp_meta_type_s* type,
                             void* object) {
    for (uint32_t field_index = 0; field_index < DROPPP_MAX_FIELD_COUNT; field_index++) {
        const struct droppp_meta_field_s* field = &type->fields[field_index];
        if (field->initialized) {
            if (droppp_memcmp(field->name, name, name_length)) {
                // Index field->offset bytes into object.
                return (void*)((uintptr_t)object + (uintptr_t)field->offset);
            }
        } else {
            break;
        }
    }

    if (type->parent != NULL) {
        return droppp_meta_read_field(name, name_length, type->parent, object);
    }

    return NULL;
}