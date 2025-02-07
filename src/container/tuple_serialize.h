/*
//@HEADER
// *****************************************************************************
//
//                              tuple_serialize.h
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

#if !defined INCLUDED_SERDES_TUPLE_SERIALIZE
#define INCLUDED_SERDES_TUPLE_SERIALIZE

#include "serdes_common.h"
#include "serializers/serializers_headers.h"

#include <tuple>
#include <utility>

namespace serdes {

template <typename Serializer, typename... T, size_t... Idxs>
void tuple_helper(Serializer& s, std::tuple<T...>& tup, std::index_sequence<Idxs...>) {
  std::forward_as_tuple(
    ((s | std::get<Idxs>(tup)),0)...
  );
}

template <typename Serializer, typename... Args>
void serialize(Serializer& s, std::tuple<Args...>& tuple) {
  tuple_helper(s, tuple, std::index_sequence_for<Args...>());
}

template <typename Serializer, typename T, typename U>
void serialize(Serializer& s, std::pair<T, U>& pair) {
  s | pair.first;
  s | pair.second;
}

/* parserdes */
template <typename Serializer, typename... T, size_t... Idxs>
void par_tuple_helper(
  Serializer& s, std::tuple<T...>& tup, std::index_sequence<Idxs...>
) {
  std::forward_as_tuple(
    ((s & std::get<Idxs>(tup)),0)...
  );
}

template <typename Serializer, typename... Args>
void parserdes(Serializer& s, std::tuple<Args...>& tuple) {
  par_tuple_helper(s, tuple, std::index_sequence_for<Args...>());
}

template <typename Serializer, typename T, typename U>
void parserdes(Serializer& s, std::pair<T, U>& pair) {
  s & pair.first;
  s & pair.second;
}


} /* end namespace serdes */

#endif /*INCLUDED_SERDES_TUPLE_SERIALIZE*/
