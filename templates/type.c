// Drop-generated meta definition for {{ type.name }} ({{ type.source.name }}).
{% if type.is_base() %}
// Base type!
const struct droppp_meta_type_s BaseTypes::{{ type.name }}_meta =
{% else %}
// Standalone type!
const struct droppp_meta_type_s {{ type.name }}::meta =
{% endif %}
    (struct droppp_meta_type_s) {
    .name = "{{ type.name }}",
    {% if type.source.value != 2 %}
    .fields = {
        {% for field in type.fields %}
        {
            .name = "{{ field.field_name }}",
            {% if field.type.is_base() %}.type = &BaseTypes::{{ field.type.name }}_meta,
            {% else %}.type = &{{ field.type.name }}::meta, {% endif %}
            .offset = (void*)DROPPP_OFFSETOF({{ type.name }}, {{ field.field_name }}),
            .initialized = true,
            .is_array = {{ "true" if field.is_array else "false" }},
            .array_count = {{ field.count if field.is_array else 0 }}
        },
        {% endfor %}
    },
    {% endif %}
    .size = sizeof({{ type.name }}),
    .parent = {{ "&" + type.parent_type.name + "::meta" if type.parent_type else "NULL" }}
};
