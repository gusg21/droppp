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

void* droppp_meta_read_field(const struct droppp_meta_field_s* field, void* object) {
    // Index field->offset bytes into object.
    return (void*)((uintptr_t)object + (uintptr_t)field->offset);
}

const struct droppp_meta_field_s* droppp_meta_get_field_by_name(const struct droppp_meta_type_s* type, const char* name,
                                                            size_t max_name_length) {
    for (uint32_t field_index = 0; field_index < DROPPP_MAX_FIELD_COUNT; field_index++) {
        const struct droppp_meta_field_s* field = &type->fields[field_index];
        if (field->initialized) {
            if (droppp_memcmp(field->name, name, max_name_length)) {
                return field;
            }
        } else {
            break;
        }
    }

    return NULL;
}
