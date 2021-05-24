import re, os
from conans import ConanFile, CMake, tools

class SensorDataPackage(ConanFile):
    name = 'asyncdemo'
    description = 'async patterns in c++'

    build_policy = 'never'
    generators = "cmake_find_package"

    settings = 'os', 'arch', 'compiler', 'build_type'

    exports_sources = [
        "CMakeLists.txt",
        "src/*",
    ]

    def requirements(self):
        self.requires('boost/1.76.0')

    def configure(self):
        # if self.settings.compiler == "Visual Studio":
        #     del self.settings.compiler.runtime

        self.options['boost'].shared = True

    def create_cmake(self):
        cmake = CMake(self, msbuild_verbosity=None)
        cmake.verbose = True
        return cmake

    def imports(self):
         if self.settings.compiler == "Visual Studio":
             self.copy("*.dll", dst=str(self.settings.build_type), keep_path = False)
         else:
            self.copy('lib/*')
            self.copy("*.dll")

    def build(self):
        cmake = self.create_cmake()
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = self.create_cmake()
        cmake.install()
