import enum, re, os, sys, json, jinja2, time

class FieldDeclarationException(Exception):
    pass

class Field:
    def __init__(self, type_name: str, field_name: str):
        self.type_name = type_name
        self.field_name = field_name
        self.count = 0
        self.is_array = False
        
        self.type = None

    def from_declaration(decl: str):
        decl_tokens = decl.split()

        if len(decl_tokens) != 2:
            raise FieldDeclarationException(f"Wrong number of tokens in field declaration (given {len(decl_tokens)})")
        
        if "(" in decl:
            raise FieldDeclarationException(f"Function declaration (not field)")

        type_name = decl_tokens[0]
        field_name = decl_tokens[1].replace(";", "") # Cut out ;'s.

        field = Field(type_name, field_name)

        if re.match(r"([A-Za-z_])+\[([0-9])*\]", field_name):
            count_string = field_name[field_name.find("[")+1:field_name.find("]")]
            base_name = field_name[:field_name.find("[")]
            try:
                count = int(count_string)
                field.field_name = base_name
                field.count = count
                field.is_array = True
            except Exception:
                raise FieldDeclarationException(f"Bad array count for field {field_name}")

        return field
    
    def __repr__(self):
        if self.is_array:
            return f"<Field '{self.field_name}' ({self.type}) (size {self.get_size()}) [{self.count}]>"
        else:
            return f"<Field '{self.field_name}' ({self.type}) (size {self.get_size()})>"
        
    def get_size(self):
        if not self.type:
            return 0

        if self.is_array:
            return self.type.get_size() * self.count
        else:
            return self.type.get_size()
    
class TypeDB:
    pass

class Scope:
    pass

class TypeSource(enum.Enum):
    CLASS = 0
    STRUCT = 1
    BUILTIN = 2

class Type:
    def __init__(self, name: str, source: TypeSource):
        self.name = name
        self.source = source

        self.fields: list[Field] = []
        self.resolved = False
        self.size = 0

    def add_fields_from_code(self, code: list[str], scope: Scope):
        for line_index in scope.get_contained_range():
            line = code[line_index - 1]
            try:
                field = Field.from_declaration(line)
                self.fields.append(field)
            except FieldDeclarationException:
                # Invalid field, move on.
                pass

    def resolve(self, db: TypeDB):
        for field in self.fields:
            if field:
                field.type = db.find_type(field.type_name)

        # Filter out bad/unknown fields
        self.fields = list(filter(lambda field: field and field.type is not None, self.fields))

        self.resolved = True

    def get_size(self):
        if self.source == TypeSource.BUILTIN:
            return self.size
        else:
            size = 0
            for field in self.fields:
                size += field.get_size()

            return size

class TypeDBException(Exception):
    pass

class TypeDB:
    def __init__(self, raw_types: list[Type]):
        self.types = raw_types

    def resolve(self):
        for type_ in self.types:
            if not type_.resolved:
                type_.resolve(self)

    def print_type(self, type_: Type, depth=0):
        text = ""

        if type_.source == TypeSource.BUILTIN:
            text += (depth) * "\t" + "BUILTIN (" + str(type_.get_size()) + ")\n"
            return text
        else:
            text += depth * "\t" + type_.source.name + " " + type_.name + " (" + str(type_.get_size()) + "):\n"
            for field in type_.fields:
                text += (depth + 1) * "\t" + field.type_name + " " + field.field_name + " (" + str(field.get_size()) + ") "
                if field.is_array:
                    text += f"[{field.count}]"
                text += "\n"
                text += self.print_type(field.type, depth + 1)
                text += "\n"

        return text

    def __repr__(self):
        text = ""

        for type_ in self.types:
            text += self.print_type(type_) + "\n"

        return text
    
    def find_type(self, type_name: str):
        for type_ in self.types:
            if type_.name == type_name:
                return type_
        return None
    
    def add_builtin_type(self, type_name: str, size: int):
        type_ = Type(type_name, TypeSource.BUILTIN)
        type_.size = size
        self.types.append(type_)

    def add_type_db(self, other_db: TypeDB):
        for type_ in other_db.types:
            existing_type = self.find_type(type_.name)
            if not existing_type:
                self.types.append(type_)
            else:
                if type_.get_size() != existing_type.get_size():
                    raise TypeDBException(f"Differing sizes for type {type_.name}")


class Scope:
    def __init__(self, start_line: int, end_line: int):
        self.start_line = start_line
        self.end_line = end_line

    def __repr__(self):
        return f"<Scope {self.start_line}:{self.end_line}>"
    
    def get_line_count(self):
        return self.end_line - self.start_line

    def get_contained_range(self):
        return range(self.start_line + 1, self.end_line)

class CodeFile:
    def __init__(self, code_file_path: str):
        self.source_file_path = code_file_path
        with open(code_file_path, "r") as source_file:
            self.source_code = source_file.read()
        
        self.scopes = CodeFile._parse_scopes(self.source_code)
        self.reflected_type_names = CodeFile._parse_reflected_type_names(self.source_code)

        self.types = CodeFile._parse_types_from_scopes(self.source_code, self.scopes, self.reflected_type_names)
        self.db = TypeDB(self.types)

    def _parse_reflected_type_names(code: str):
        type_names = []
        code_lines = code.splitlines()

        for line in code_lines:
            result = re.match("DROPPP_REFLECT\(([A-Za-z_]+)\)", line)
            if result:
                type_names.append(result.group(1))

        return type_names
                
        
    def _parse_scopes(code: str) -> list[Scope]:
        scope_stack = []
        scopes = []

        line_index = 1
        for line in code.splitlines():
            if "{" in line:
                new_scope = Scope(line_index, 0)
                scope_stack.append(new_scope)
            if "}" in line:
                scope = scope_stack.pop()
                scope.end_line = line_index

                scopes.append(scope)

            line_index += 1

        return scopes
    
    def _parse_types_from_scopes(code: str, scopes: list[Scope], reflected_type_names: list[str]) -> list[Type]:
        types = []
        code_lines = code.splitlines()

        for scope in scopes:
            start_code = code_lines[scope.start_line - 1]

            start_code_tokens = start_code.split()
            if len(start_code_tokens) > 2:
                keyword = start_code_tokens[0]
                name = start_code_tokens[1]
                type_ = None

                if name not in reflected_type_names:
                    continue

                if keyword == "class":
                    type_ = Type(name, TypeSource.CLASS)
                elif keyword == "struct":
                    type_ = Type(name, TypeSource.STRUCT)
                else:
                    continue

                type_.add_fields_from_code(code_lines, scope)

                types.append(type_)



        return types

def get_code_type_db(code_path: str):
    code = CodeFile(code_path)
    return code.db

def main():
    config_path = sys.argv[1]
    config_file = open(config_path, "r")
    config_json = json.load(config_file)
    config_file.close()
    config_base_path = os.path.dirname(config_path)

    base_type_db = TypeDB([])
    for base_type in config_json["base_types"]:
        base_type_db.add_builtin_type(base_type["name"], base_type["size"])

    for directory in config_json["directories"]:
        for root, dirs, files in os.walk(os.path.realpath(os.path.join(config_base_path, directory["path"]))):
            for file in files:
                if re.match(directory["filter"], file):
                    base_type_db.add_type_db(get_code_type_db(os.path.join(root, file))) 

    base_type_db.resolve()

    env = jinja2.Environment(loader=jinja2.FileSystemLoader(os.path.join(config_base_path, "templates/")))
    template = env.get_template("meta.c")
    rendered_code = template.render({
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "types": base_type_db.types,
        "includes": config_json["includes"]
    })

    output_code_path = os.path.join(config_base_path, config_json["output"])
    os.makedirs(os.path.dirname(output_code_path), exist_ok=True)
    with open(output_code_path, "w+") as output_code_file:
        output_code_file.write(rendered_code)
    


if __name__ == "__main__":
    main()