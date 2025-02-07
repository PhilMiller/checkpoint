/*
//@HEADER
// *****************************************************************************
//
//                        serdes_example_nonintrusive.cc
//                           DARMA Toolkit v. 1.0.0
//                 DARMA/checkpoint => Serialization Library
//
// Copyright 2019 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

#include "serdes_headers.h"

#include <cstdio>

namespace serdes { namespace examples {

struct MyTest3 {
  int a = 1, b = 2 , c = 3;

  void print() {
    printf("MyTest3: a=%d, b=%d, c=%d\n", a, b, c);
  }
};

template <typename Serializer>
void serialize(Serializer& s, MyTest3& my_test3) {
  s | my_test3.a;
  s | my_test3.b;
  s | my_test3.c;
}

}} // end namespace serdes::examples

int main(int, char**) {
  using namespace serdes::examples;

  MyTest3 my_test3;
  my_test3.print();

  auto serialized = serdes::serializeType<MyTest3>(my_test3);

  auto const& buf = std::get<0>(serialized);
  auto const& buf_size = std::get<1>(serialized);

  printf("ptr=%p, size=%ld\n", static_cast<void*>(buf->getBuffer()), buf_size);

  auto tptr = serdes::deserializeType<MyTest3>(buf->getBuffer(), buf_size);
  auto& t = *tptr;

  t.print();

  delete tptr;

  return 0;
}
