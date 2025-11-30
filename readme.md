# exqudens-cpp-vulkan

## Build with VULKAN_SDK

git-bash command line:
```
rm -rf build
cmake -DCONAN_VULKAN_REFERENCE=None -DVULKAN_PACKAGE_NAME=Vulkan --preset <preset-name>
cmake --build --preset <preset-name>
```

## Build with custom conan package

git-bash command line:
```
rm -rf build
cmake -DCONAN_VULKAN_REFERENCE=vulkan/1.3.224.1 -DCONAN_VULKAN_BUILD_TYPE=Release -DCONAN_VULKAN_COMPILER_RUNTIME=MD -DVULKAN_PACKAGE_NAME=Vulkan -DVULKAN_PACKAGE_VERSION=1.3.224.1 --preset <preset-name>
cmake --build --preset <preset-name>
```

## Build all packages

git-bash command line:
```
conan remove --force "$(basename "$(pwd)")"
rm -rf build
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target cmake-install || exit 255"
```

## Remove and export all packages

git-bash command line:
```
conan remove --force "$(basename "$(pwd)")"
rm -rf build
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target conan-export || exit 255"
```

## Conan Upload

``
conan upload 'vulkan/1.3.224.1' --all -r 'exqudens-jfrog-conan' -c
``
