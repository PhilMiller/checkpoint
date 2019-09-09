/*
//@HEADER
// *****************************************************************************
//
//                                   counter.h
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

#if !defined INCLUDED_SERDES_COUNTER
#define INCLUDED_SERDES_COUNTER

#include "serdes_common.h"
#include "base_serializer.h"

namespace serdes {

/*
 * A class to use in automated testing that system/application code
 * changes don't break serialization routines by changing the number
 * of members. Produces a syntactic count of serialization calls
 * resulting from a call to a type's serializer. That count can then
 * be compared to results from an inspection utility indicating how
 * many members are present.
 */
struct Counter : Serializer {
  Counter()
    : Serializer{ModeType::Packing)
    { }

  SerialSizeType getCount() const { return num_units_; }

  template <typename T>
  static void contiguousTyped(Counter& s, T* ptr, SerialSizeType num_elms) {
    // This is a syntactic count of static serialization call sites
    // Hence, count calls and not elements
    s.num_units_++;
  }

private:
  int num_units_ = 0;
};

} /* end namespace serdes */

#endif /*INCLUDED_SERDES_COUNTER*/
