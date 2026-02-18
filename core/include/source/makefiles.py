import os

def generate_guard(file):
    name = os.path.splitext(os.path.basename(file))[0]
    return f"_$_HX_CORE_M{len(name)}{name.upper()}"

dir_path = os.path.dirname(__file__)
tpp_files = [f for f in os.listdir(dir_path) if f.endswith(".tpp")]

for file in tpp_files:
    path = os.path.join(dir_path, file)
    with open(path, "r") as f:
        lines = f.readlines()

    # Find the end of the block comment
    insert_idx = 0
    for i, line in enumerate(lines):
        if "lib-helix" in line:
            insert_idx = i + 1
            break

    guard = generate_guard(file)

    # Construct new file content
    new_lines = (
        lines[:insert_idx] +
        [f"#ifndef {guard}\n", f"#define {guard}\n\n"] +
        lines[insert_idx:] +
        [f"\n#endif  // {guard}\n"]
    )

    with open(path, "w") as f:
        f.writelines(new_lines)
