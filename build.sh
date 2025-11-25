rm build -rf
cmake --fresh -G Ninja -B build -S . --log-level DEBUG --log-context || exit
cmake --build build --clean-first -v --verbose -- -v || echo "\n\nfailed to build!"
# cmake --build build --help

