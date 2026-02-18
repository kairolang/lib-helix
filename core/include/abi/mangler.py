import re

class KairoMangler:
    def mangle(self, entity_type: str, name: str, parameters: list = None, qualifiers: list = None) -> str:
        """
        Mangles a Kairo entity according to the Kairo ABI specification.
        
        :param entity_type: The type of entity (e.g., FN, RM, VT, TI, NV, CT, DT, OP, LM).
        :param name: The name of the entity, including namespaces or modules if applicable.
        :param parameters: A list of parameters, if applicable.
        :param qualifiers: A list of qualifiers, if applicable.
        :return: The mangled name.
        """
        entity_type = entity_type.lstrip('_')
        
        mangled = f"_HX_{entity_type}{len(name)}{name}"
        
        if parameters:
            for param in parameters:
                mangled += self.encode_type(param)
        
        if qualifiers:
            for qualifier in qualifiers:
                mangled += f"{qualifier}"
        
        return mangled

    def encode_type(self, param: str) -> str:
        """
        Encodes a type according to the Kairo ABI type encoding scheme.
        """
        type_map = {
            'void': '_v', 'bool': '_b',
            'usize': '_us', 'isize': '_is',
            'f32': '_F32', 'f64': '_F64', 'f80': '_F80',
            'u8': '_U8', 'u16': '_U16', 'u32': '_U32',
            'i8': '_I8', 'i16': '_I16', 'i32': '_I32',
        }
        return type_map.get(param, f"_{param}")

class KairoDemangler:
    def demangle(self, mangled_name: str) -> str:
        """
        Demangles a Kairo entity name according to the Kairo ABI specification.
        """
        if not mangled_name.startswith("_HX_"):
            return "Invalid Kairo mangled name"

        entity_type_map = {
            'FN': 'Function', 'RM': 'Requires',
            'VT': 'VirtualTable', 'TI': 'TypeInfo',
            'NV': 'NonVirtualMember', 'CT': 'Constructor',
            'DT': 'Destructor', 'OP': 'Operator',
            'LM': 'Lambda'
        }

        qualifiers_map = {
            '_C': 'const', '_U': 'unsafe',
            '_Q': 'optional', '_R': 'return'
        }

        type_map = {
            '_v': 'void', '_b': 'bool',
            '_us': 'usize', '_is': 'isize',
            '_F32': 'f32', '_F64': 'f64', '_F80': 'f80',
            '_U8': 'u8', '_U16': 'u16', '_U32': 'u32',
            '_I8': 'i8', '_I16': 'i16', '_I32': 'i32',
        }

        pattern = r"_HX_(\w{2})(\d+)([a-zA-Z_]+)(.*)"
        match = re.match(pattern, mangled_name)
        if not match:
            return "Invalid Kairo mangled name"

        entity_type_code, length, name, remainder = match.groups()
        entity_type = entity_type_map.get(entity_type_code, "UnknownEntity")
        
        try:
            name_length = int(length)
            name = name[:name_length]
        except ValueError:
            return "Invalid name length in mangled name"

        demangled = f"{entity_type} {name}"
        
        param_pattern = r"(_[A-Za-z0-9]+)"
        params = re.findall(param_pattern, remainder)

        param_str = []
        qualifiers = []

        for param in params:
            if param in type_map:
                param_str.append(type_map[param])
            elif param in qualifiers_map:
                qualifiers.append(qualifiers_map[param])
            else:
                param_str.append(param.strip('_'))

        if param_str:
            demangled += f"({', '.join(param_str)})"
        
        if qualifiers:
            demangled += f" {' '.join(qualifiers)}"
        
        return demangled

# Example usage
mangler = KairoMangler()
demangler = KairoDemangler()

mangled_name = mangler.mangle('FN', 'add', ['i32', 'i32'], ['_C'])
demangled_name = demangler.demangle(mangled_name)

print("Mangled:", mangled_name)
print("Demangled:", demangled_name)
