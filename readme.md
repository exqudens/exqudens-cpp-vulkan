# exqudens-cpp-vulkan

## list-presets

1. `git clean -xdf`
2. `cmake --list-presets`

## list-ctests

1. `git clean -xdf`
2. `cmake --preset ${preset}`
3. `cmake --build --preset ${preset} --target cmake-test-list`

## vscode

1. `git clean -xdf`
2. `cmake --preset ${preset}`
3. `cmake --build --preset ${preset} --target vscode`

## clion

1. `git clean -xdf`
2. `cmake --preset ${preset}`
3. `cmake --build --preset ${preset} --target clion`

## build-all-presets

1. `git clean -xdf`
2. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
3. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} || exit 255"`

## build-test-all-presets

1. `git clean -xdf`
2. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
3. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target test-app || exit 255"`

## test-all-presets

1. `git clean -xdf`
2. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
3. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target cmake-test || exit 255"`

## install-all-presets

1. `git clean -xdf`
2. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
3. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target cmake-install || exit 255"`

## export-all-presets

1. `git clean -xdf`
2. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
3. `conan list "exqudens-cpp-vulkan/*"`
4. `conan remove -c "exqudens-cpp-vulkan"`
5. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target conan-export || exit 255"`
