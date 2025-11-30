import logging
from conans import ConanFile, tools
from conans.util.files import save

required_conan_version = ">=1.43.0"


class ConanConfiguration(ConanFile):
    settings = "arch", "os", "compiler", "build_type"
    options = {"shared": [True, False], "interface": [True, False], "vulkan_reference":["ANY"], "gtest_reference": ["ANY"]}
    default_options = {"shared": True, "interface": False, "vulkan_reference": "vulkan-headers/1.3.231.1", "gtest_reference": "gtest/1.14.0"}
    generators = "cmake_find_package"

    def requirements(self):
        try:
            self.requires(str(self.options.gtest_reference))
            self.requires("easyloggingpp/9.89")
            self.requires("glm/cci.20230113")
            self.requires("tinyobjloader/1.0.6")
            self.requires("lodepng/cci.20200615")
            if self.settings.os == "Windows":
                self.requires("glfw/3.3.8")
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e

    def configure(self):
        try:
            self.options["gtest"].shared = self.options.shared
            self.options["easyloggingpp"].interface = True
            self.options["glfw"].shared = self.options.shared
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
            self.copy(pattern="*.json", dst="bin", src="bin")
            self.copy(pattern="*.so", dst="lib", src="lib")
            self.copy(pattern="*.so.*", dst="lib", src="lib")
            self.copy(pattern="*.dylib", dst="lib", src="lib")
        except Exception as e:
            logging.error(e, exc_info=True)
            raise e


if __name__ == "__main__":
    pass
