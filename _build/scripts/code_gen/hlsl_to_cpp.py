import re
import argparse
import os
import sys

import regis.diagnostics

# Define mappings from HLSL types to GLM types
type_mapping = {
    "float4x4": "glm::mat4",
    "float4": "glm::vec4",
    "float3": "glm::vec3",
    "float2": "glm::vec2",
    "float": "float",
    "int": "int",
    "int4": "glm::ivec4",
    "int3": "glm::ivec3",
    "int2": "glm::ivec2",
    # Add more mappings as needed
}

def extract_cbuffers_as_structs(hlsl_code):
    # Extract the cbuffer name and its content
    cbuffer_pattern = re.compile(r"cbuffer\s+(\w+)\s*(:\s*((\w*)\(b\d+\)))?\s*\{\s*([^}]*)\};", re.DOTALL)
    matches = cbuffer_pattern.findall(hlsl_code)

    structs = []
    for match in matches:
        cbuffer_name = match[0]
        cbuffer_content = match[-1].strip()

        struct_lines = [f"struct {cbuffer_name} {{"]

        # Parse each line inside the cbuffer
        lines = cbuffer_content.split(';')
        for line in lines:
            line = line.strip()
            if line:
                # Match the type and variable name
                type_var_pattern = re.compile(r"(\w+)\s+(\w+)")
                type_var_match = type_var_pattern.match(line)
                if type_var_match:
                    hlsl_type = type_var_match.group(1)
                    var_name = type_var_match.group(2)
                    # Map HLSL type to GLM type
                    glm_type = type_mapping.get(hlsl_type, hlsl_type)
                    struct_lines.append(f"    {glm_type} {var_name};")

        struct_lines.append("};")
        structs.append('\n'.join(struct_lines))

    return '\n\n'.join(structs)

def main():
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-file", help="The shader file to convert")
    parser.add_argument("-output", help="File to save the output to")
    args, unknown = parser.parse_known_args()

    if not os.path.isfile(args.file):
        regis.diagnostics.log_err(f'"{args.file}" does not point to a existing file')
        sys.exit(1)

    hlsl_code = ''
    with open(args.file, 'r') as f:
        hlsl_code = f.read()

    cpp_structs = extract_cbuffers_as_structs(hlsl_code)

    with open(args.output, 'a') as f:
        f.write(cpp_structs)
        f.write('\n\n')
        
    sys.exit(0)

if __name__ == '__main__':
    main()