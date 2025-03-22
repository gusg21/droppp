#if !defined(DROPPP_H)
#define DROPPP_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define DROPPP_MAX_FIELD_COUNT 32

struct droppp_meta_type_s;
struct droppp_meta_field_s;

struct droppp_meta_field_s {
    const struct droppp_meta_type_s* type;
    const char* name;
    const void* offset;
    const bool initialized;
    const bool is_array;
    const size_t array_count;
};

struct droppp_meta_type_s {
    const char* name;
    const struct droppp_meta_field_s fields[DROPPP_MAX_FIELD_COUNT];
    const size_t size;
};

void* droppp_meta_read_field(const struct droppp_meta_field_s* field, void* object);
const struct droppp_meta_field_s* droppp_meta_get_field_by_name(const struct droppp_meta_type_s* type, const char* name,
                                                            size_t max_name_size);

#define DROPPP_REFLECT_STRUCT(struct_name) extern const struct droppp_meta_type_s struct_name##_meta
#define DROPPP_REFLECT(class_name) extern const struct droppp_meta_type_s class_name##_meta

#endif  // DROPPP_H
