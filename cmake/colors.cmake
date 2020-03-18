# vim: set ts=2 sw=2 tw=0 et :

# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : colors
# @created     : Wednesday Aug 14, 2019 15:34:31 -03

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

macro(define_colors)
  string(ASCII 27 Esc)
  set(C_RESET "${Esc}[m" CACHE INTERNAL "")
  set(C_RED     "${Esc}[1;31m" CACHE INTERNAL "")
  set(C_GREEN   "${Esc}[1;32m" CACHE INTERNAL "")
  set(C_YELLOW  "${Esc}[1;33m" CACHE INTERNAL "")
  set(C_BLUE    "${Esc}[1;34m" CACHE INTERNAL "")
  set(C_MAGENTA "${Esc}[1;35m" CACHE INTERNAL "")
  set(C_CYAN    "${Esc}[1;36m" CACHE INTERNAL "")
  set(C_WHITE   "${Esc}[1;37m" CACHE INTERNAL "")
endmacro()
