import logging
from pathlib import Path
from conans import ConanFile, tools
from conans.util.files import save

required_conan_version = ">=1.43.0"


class ConanConfiguration(ConanFile):
    settings = "arch", "os", "compiler", "build_type"
    options = {"shared": [True, False], "interface": [True, False], "dependencies": [True, False]}
    default_options = {"shared": True, "interface": False, "dependencies": True}
    generators = "cmake_find_package"

    def set_name(self):
        try:
            self.name = Path(__file__).parent.joinpath('name-version.txt').read_text().split(':')[0].strip()
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def set_version(self):
        try:
            self.version = Path(__file__).parent.joinpath('name-version.txt').read_text().split(':')[1].strip()
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def requirements(self):
        try:
            if self.options.dependencies:
                self.requires("vulkan-headers/1.3.250.0")
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def build_requirements(self):
        try:
            if not self.options.dependencies:
                self.requires("vulkan-headers/1.3.250.0")
            if self.options.dependencies:
                self.tool_requires("shaderc/2021.1")
                self.tool_requires("glm/cci.20230113")
                self.tool_requires("glfw/3.3.7")
                self.tool_requires("tinyobjloader/1.0.6")
                self.tool_requires("gtest/1.14.0")
                self.tool_requires("lodepng/cci.20200615")
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def configure(self):
        try:
            if self.options.dependencies:
                self.options["shaderc"].shared = self.options.shared
                self.options["glfw"].shared = self.options.shared
                self.options["gtest"].shared = self.options.shared
                self.options["lodepng"].shared = self.options.shared
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def generate(self):
        try:
            filename = 'conan-packages.cmake'
            content = ''

            content += 'set("${PROJECT_NAME}_CONAN_PACKAGE_NAMES"\n'
            for dep_name in self.deps_cpp_info.deps:
                content += '    "' + dep_name + '"' + '\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"\n'
            for dep_name, dep in self.deps_cpp_info.dependencies:
                content += '    "' + dep.get_name('cmake_find_package') + '" # ' + dep_name + '\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"\n'
            for dep_name, dep in self.deps_cpp_info.dependencies:
                content += '    "' + str(dep.version) + '" # ' + dep_name + '\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"\n'
            for dep_name, dep in self.deps_cpp_info.dependencies:
                content += '    "' + dep.rootpath.replace('\\', '/') + '" # ' + dep_name + '\n'
            content += ')\n'

            save(filename, content)
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def imports(self):
        try:
            self.copy(pattern="*.dll", dst="bin", src="bin")
            self.copy(pattern="*.dylib", dst="lib", src="lib")
            self.copy(pattern="*.json", dst="bin", src="bin")
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def package_info(self):
        try:
            if self.options.interface:
                self.cpp_info.libs = []
            else:
                self.cpp_info.libs = tools.collect_libs(self)
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def package_id(self):
        try:
            if self.options.interface:
                self.info.header_only()
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e


if __name__ == "__main__":
    pass
