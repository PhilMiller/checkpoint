/*
//@HEADER
// *****************************************************************************
//
//                              test_byte_macro.cc
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

#include <gtest/gtest.h>

#include "test_harness.h"

#include "serdes_headers.h"
#include "serialization_library_headers.h"

#include <vector>
#include <cstdio>

namespace userTest {

static constexpr int const my_val_init = 29;

struct UserObject1 {
  int my_val;

  void init() {
    my_val = my_val_init;
  }

  void check() {
    EXPECT_EQ(my_val, my_val_init);
  }
};

} // end namespace userTest

SERDES_BYTES(::userTest::UserObject1)

namespace serdes { namespace tests { namespace unit {

struct TestByteMacro : TestHarness { };

TEST_F(TestByteMacro, test_bytecopy_trait) {
  using namespace ::serialization::interface;

  using TestType = ::userTest::UserObject1;
  TestType t;
  t.init();
  t.check();

  auto ret = serialization::interface::serialize<TestType>(t);

  auto tptr = serialization::interface::deserialize<TestType>(
    ret->getBuffer(), ret->getSize()
  );
  auto& t_final = *tptr;

  t_final.check();
}

}}} // end namespace serdes::tests::unit
