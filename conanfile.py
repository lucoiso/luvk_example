# Author: Lucas Vilas-Boas
# Year: 2025
# Repo: https://github.com/lucoiso/luvk_example

from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain


class LuVK(ConanFile):
    name = "luvk"
    version = "0.0.1"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def requirements(self):
        # https://conan.io/center/recipes/imgui
        self.requires("imgui/1.92.0-docking")

    def configure(self):
        self.options["imgui/*"].shared = True

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.29]")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self, generator="Ninja")
        tc.generate()
