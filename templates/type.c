// Drop-generated meta definition for {{ type.name }} ({{ type.source.name }}).
const struct droppp_meta_type_s {{ type.name }}_meta = {
    .name = "{{ type.name }}",
    {% if type.source.value != 2 %}
    .fields = {
        {% for field in type.fields %}
        {
            .name = "{{ field.field_name }}",
            .type = &{{ field.type.name }}_meta,
            .offset = (void*)offsetof({{ type.name }}, {{ field.field_name }}),
            .initialized = true,
            .is_array = {{ "true" if field.is_array else "false" }},
            .array_count = {{ field.count if field.is_array else 0 }}
        },
        {% endfor %}
    },
    {% endif %}
    .size = sizeof({{ type.name }})
};
