/*
//@HEADER
// *****************************************************************************
//
//                          dispatch_serializer_byte.h
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

#if ! defined INCLUDED_SERDES_DISPATCH_SERIALIZER_BYTE
#define INCLUDED_SERDES_DISPATCH_SERIALIZER_BYTE

#include "serdes_common.h"
#include "serdes_all.h"
#include "traits/serializable_traits.h"
#include "dispatch_serializer_nonbyte.h"

#include <type_traits>
#include <tuple>
#include <cstdlib>

namespace serdes {

#if !HAS_DETECTION_COMPONENT
template <typename T>
struct hasByteCopy {
  template <typename C, typename = typename C::isByteCopyable>
  static typename C::isByteCopyable test(int);

  template <typename C>
  static std::false_type test(...);

  static constexpr bool value = decltype(test<T>(0))::value;
};
#endif

template <typename SerializerT, typename T>
struct SerializerDispatchByte {
  #if HAS_DETECTION_COMPONENT
    template <typename U>
    using isByteCopyType =
    typename std::enable_if<SerializableTraits<U>::is_bytecopyable, T>::type;

    template <typename U>
    using isNotByteCopyType =
    typename std::enable_if<not SerializableTraits<U>::is_bytecopyable, T>::type;
  #else
    template <typename U>
    using isByteCopyType =
    typename std::enable_if<
      std::is_arithmetic<U>::value or hasByteCopy<U>::value, T
    >::type;

    template <typename U>
    using isNotByteCopyType =
    typename std::enable_if<
      not std::is_arithmetic<U>::value and not hasByteCopy<U>::value, T
    >::type;
  #endif

  template <typename U = T>
  void operator()(
    SerializerT& s, T* val, SerialSizeType num, isByteCopyType<U>* x = nullptr
  ) {
    SerializerT::contiguousTyped(s, val, num);
  }

  template <typename U = T>
  void operator()(
    SerializerT& s, T* val, SerialSizeType num, isNotByteCopyType<U>* x = nullptr
  ) {
    SerializerDispatchNonByte<SerializerT, T> dispatch;
    dispatch(s, val, num);
  }

  template <typename U = T>
  void partial(
    SerializerT& s, T* val, SerialSizeType num, isByteCopyType<U>* x = nullptr
  ) {
    SerializerT::contiguousTyped(s, val, num);
  }

  template <typename U = T>
  void partial(
    SerializerT& s, T* val, SerialSizeType num, isNotByteCopyType<U>* x = nullptr
  ) {
    SerializerDispatchNonByteParserdes<SerializerT, T> dispatch;
    dispatch.partial(s, val, num);
  }
};

} //end namespace serdes

#endif /*INCLUDED_SERDES_DISPATCH_SERIALIZER_BYTE*/
