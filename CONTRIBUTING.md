# Contributing

Contributions are welcome. Please follow the guidelines below.

## What to contribute

- New GPU API backends (e.g., Metal for macOS, NVML for NVIDIA-specific queries)
- Bug fixes or accuracy improvements to existing backends
- Cross-platform build improvements (Linux support via CMake)
- Documentation improvements

## Getting started

1. Fork the repository
2. Create a branch: `git checkout -b feature/your-feature`
3. Make your changes
4. Open a pull request against `main`

## Code style

- C++17, no external dependencies beyond the target API SDK
- No Chinese (or other non-English) comments — English only
- No `Co-Authored-By` lines in commit messages
- Each backend lives in its own `main_<api>.cpp` file
- Support `--json` and `--watch` flags for consistency with existing backends

## Adding a new backend

1. Create `main_<api>.cpp`
2. Add a corresponding target in `CMakeLists.txt`
3. Add a build step in `.github/workflows/build.yml` if the SDK is available on the CI runner
4. Update the Files table and Build section in `README.md`

## Pull request checklist

- [ ] Code compiles cleanly with no warnings
- [ ] `--json` output is valid JSON
- [ ] `--watch` mode exits cleanly on Ctrl+C
- [ ] README updated if a new backend was added
