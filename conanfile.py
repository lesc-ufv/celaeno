# vim: set ts=2 sw=2 tw=0 et :

# BSD 2-Clause License

# Copyright (c) 2020, Ruan Evangelista Formigoni
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.

# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from conans import ConanFile, CMake, tools


class CelaenoConan(ConanFile):
  name = "celaeno"
  version = "0.1"
  license = "BSD 2-clause \"Simplified\" License"
  author = "Ruan Evangelista Formigoni ruanformigoni@gmail.com"
  url = "https://gitlab.com/formigoni/celaeno"
  description = "C++ Computer Science Algorithms Collection"
  topics = ("c++", "generic", "nanocomputing", "algorithms")
  generators = "cmake"
  exports_sources = "include/*", "cmake/*", "LICENSE", "CMakeLists.txt"
  no_copy_source = True
  requires = 'range-v3/0.10.0@ericniebler/stable', \
    'catch2/2.5.0@bincrafters/stable', \
    'taygete/0.1@pleiades/testing', \
    'asterope/0.1@pleiades/testing', \
    'functionalplus/v0.2.6-p0@dobiasd/stable'

  def source(self):
    self.run("git clone https://gitlab.com/formigoni/celaeno.git")

    # Remove examples
    tools.replace_in_file(
      "CMakeLists.txt",
      "add_subdirectory(test)",
      '',
      strict=True
    )

  def package(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.install()
    self.copy("*.hpp", dst="include", src="include")
    self.copy("LICENSE", ignore_case=True, keep_path=False)

  def package_id(self):
    self.info.header_only()
