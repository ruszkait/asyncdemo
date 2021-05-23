import re, os
from conans import ConanFile, CMake, tools

class SensorDataPackage(ConanFile):
    name = 'asyncdemo'
    description = 'async patterns in c++'

    build_policy = 'never'
    generators = "cmake_find_package_multi"

    settings = 'os', 'arch', 'compiler', 'build_type'

    exports_sources = [
        "CMakeLists.txt",
        "src/*",
    ]

    def requirements(self):
        self.requires('boost/1.76.0')

    def configure_cmake(self, BuildTypeString):
        cmake = CMake(self, build_type=BuildTypeString)
        cmake.configure()
        return cmake

    def imports(self):
        self.copy('lib/*')
        self.copy('*.dll')

    def build(self):
        build_types = ["Debug"]
        for build_type in build_types:
            cmake = self.configure_cmake(build_type)
            cmake.build()
