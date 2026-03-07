# Contributing

## Code Style

- C++17 standard, no compiler extensions.
- Use the `.clang-format` file for formatting: `clang-format -i src/*.cpp src/*.h`.
- Namespace: `particle_sim`.
- Member prefix: `m_` for private data members.
- Prefer `std::` qualified names over `using namespace std;`.

## Building

```bash
mkdir -p build && cd build
cmake .. && make
```

The build uses `-Wall -Wextra -Wpedantic -Werror` — all warnings must be resolved.

## Testing

Run the headless smoke test before submitting changes:

```bash
./tools/run_smoke_test.sh
```

## Guidelines

1. No raw `new`/`delete` — use RAII containers and smart pointers.
2. Keep classes small and focused (single responsibility).
3. Document non-obvious logic with brief inline comments.
4. Preserve the educational clarity of the codebase.
