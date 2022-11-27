# How To

##### Build all packages

git-bash command line:
```
conan remove --force "$(basename "$(pwd)")"
rm -rf build
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target cmake-install || exit 255"
```

##### Remove and export all packages

git-bash command line:
```
conan remove --force "$(basename "$(pwd)")"
rm -rf build
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"
cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target conan-export || exit 255"
```

##### Conan Upload

``
conan upload 'vulkan/1.3.224.1' --all -r 'exqudens-jfrog-conan' -c
``
