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
