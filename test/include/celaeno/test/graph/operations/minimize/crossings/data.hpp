// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : data
// @created     : domingo set 06, 2020 17:40:42 -03
//
// BSD 2-Clause License

// Copyright (c) 2020, Ruan Evangelista Formigoni
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include <vector>
#include <celaeno/aliases.hpp>

// namespace data {{{
namespace data
{

// Barycenter row ordering {{{

// Base matrix sample 1
std::vector<std::vector<bool>> bs1
{{
  {1,1,0,0,0}, // 1 + 2 / 2 = 1.5
  {1,0,0,1,1}, // 1 + 4 + 5 / 3 == 3.33...
  {0,1,0,1,1}, // 2 + 4 + 5 / 3 == 3.66...
  {1,0,1,0,1}, // 1 + 3 + 5 / 3 == 3
}};

// Expected row sorted by barycenter matrix 1
std::vector<std::vector<bool>> br1
{{
  {1,1,0,0,0}, // 1.5
  {1,0,1,0,1}, // 1 + 3 + 5 / 3 == 3
  {1,0,0,1,1}, // 1 + 4 + 5 / 3 == 3.33...
  {0,1,0,1,1}, // 2 + 4 + 5 / 3 == 3.66...
}};

// Base matrix sample 2
std::vector<std::vector<bool>> bs2
{{
  {0,1,1,0,0}, // 2 + 3 / 2 = 2.5
  {1,1,0,1,0}, // 1 + 2 + 4 / 3 = 2.33...
  {0,1,0,1,1}, // 2 + 4 + 5 / 3 = 3.66...
  {0,0,1,1,1}, // 3 + 4 + 5 / 3 = 4
}};

// Expected row sorted by barycenter matrix 2
std::vector<std::vector<bool>> br2
{{
  {1,1,0,1,0}, // 1 + 2 + 4 / 3 = 2.33...
  {0,1,1,0,0}, // 2 + 3 / 2 = 2.5
  {0,1,0,1,1}, // 2 + 4 + 5 / 3 = 3.66...
  {0,0,1,1,1}, // 3 + 4 + 5 / 3 = 4
}};

// Base matrix sample 3
std::vector<std::vector<bool>> bs3
{{
  {0,0,0,1}, // 4 / 1 = 4
  {0,0,1,1}, // 3 + 4 / 2 = 3.5
  {1,0,0,1}, // 1 + 4 / 2 = 2.5
  {0,1,1,1}, // 2 + 3 + 4 / 3 = 3
}};

// Expected row sorted by barycenter matrix 3
std::vector<std::vector<bool>> br3
{{
  {1,0,0,1}, // 1 + 4 / 2 = 2.5
  {0,1,1,1}, // 2 + 3 + 4 / 3 = 3
  {0,0,1,1}, // 3 + 4 / 2 = 3.5
  {0,0,0,1}, // 4 / 1 = 4
}};

// Base matrix sample 4
std::vector<std::vector<bool>> bs4
{{
  {0}, // 0
}};

// Expected row sorted by barycenter matrix 4
std::vector<std::vector<bool>> br4
{{
  {0}, // 0
}};

// Base matrix sample 5
std::vector<std::vector<bool>> bs5
{{
  {1,1}, // 1 + 2 / 2 = 1.5
  {1,0}, // 1
}};

// Expected row sorted by barycenter matrix 5
std::vector<std::vector<bool>> br5
{{
  {1,0}, // 1
  {1,1}, // 1 + 2 / 2 = 1.5
}};

// }}}

} // namespace data }}}
