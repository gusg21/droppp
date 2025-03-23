#if !defined(DROPPP_H)
#define DROPPP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
    const struct droppp_meta_type_s* parent;
    const char* name;
    const struct droppp_meta_field_s fields[DROPPP_MAX_FIELD_COUNT];
    const size_t size;
};

void* droppp_meta_read_field(const char* name, size_t name_length, const struct droppp_meta_type_s* type, void* object);

#define DROPPP_REFLECT_STRUCT(struct_name) extern const struct droppp_meta_type_s struct_name##_meta
#define DROPPP_REFLECT(class_name) extern const struct droppp_meta_type_s class_name##_meta
#define DROPPP_OFFSETOF(type, field) ((size_t)&((type*)0)->field)

#endif  // DROPPP_H
