/*
//@HEADER
// *****************************************************************************
//
//                            view_traverse_manual.h
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

#if !defined INCLUDED_CONTAINER_VIEW_TRAVERSE_MANUAL_H
#define INCLUDED_CONTAINER_VIEW_TRAVERSE_MANUAL_H

#include "serdes_common.h"
#include "serializers/serializers_headers.h"
#include "container/view_traits_extract.h"

#if KOKKOS_ENABLED_SERDES

#include <Kokkos_Core.hpp>
#include <Kokkos_View.hpp>
#include <Kokkos_DynamicView.hpp>
#include <Kokkos_Serial.hpp>

#include <utility>
#include <tuple>
#include <type_traits>

namespace serdes {


/*
 *                     === Manual Traversal ===
 *
 * This is left here to test the automatic code.
 *
 * Serialization of data backing a view by manually traversing the data using
 * the Kokkos::View::operator()(...) or
 * Kokkos::DynamicView::operator()(...). The access operator is guaranteed to
 * correctly accesses the data regardless of the layout.
 */

template <typename ViewT, typename = void>
struct SerializeMemory;

template <typename ViewT>
using KokkosAtomicArchetype = typename std::is_same<
  typename ViewT::traits::memory_traits, ::Kokkos::MemoryTraits<Kokkos::Atomic>
>;

template <typename ViewT>
struct SerializeMemory<
  ViewT,
  std::enable_if_t<KokkosAtomicArchetype<ViewT>::value>
>
{
  template <typename SerializerT>
  static void apply(SerializerT& s, ViewT const& v) {
    using DataType = typename ViewGetType<ViewT>::DataType;
    if (s.isUnpacking()) {
      DataType val;
      s | val;
      v.operator()().operator=(val);
    } else {
      auto const& val = v.operator()().operator const DataType();
      //DataType const& val = v.operator()();
      s | val;
    }
  }
};

template <typename ViewT>
struct SerializeMemory<
  ViewT,
  std::enable_if_t<!KokkosAtomicArchetype<ViewT>::value>
>
{
  template <typename SerializerT>
  static void apply(SerializerT& s, ViewT const& v) {
    s | v.operator()();
  }
};

template <typename SerializerT, typename ViewType, std::size_t dims>
struct TraverseManual;

template <typename SerializerT, typename ViewType>
struct TraverseManual<SerializerT,ViewType,0> {
  static void apply(SerializerT& s, ViewType const& v) {
    SerializeMemory<ViewType>::apply(s,v);

    //
    // This code works for both atomic and non-atomic views but I've specially
    // above for more clarity.
    //
    // if (s.isUnpacking()) {
    //   BaseType val;
    //   s | val;
    //   v.operator()() = val;
    // } else {
    //   /*
    //    * Be very careful here, do not use "auto const& val". If the view is
    //    * atomic we need to get the conversion operator to `BaseType`. In full
    //    * glory, we are invoking the following implicitly:
    //    *
    //    *   auto const& val = v.operator()().operator const BaseType();
    //    */
    //   BaseType const& val = v.operator()();
    //   s | val;
    // }
    //
  }
};

template <typename SerializerT, typename ViewType>
struct TraverseManual<SerializerT,ViewType,1> {
  static void apply(SerializerT& s, ViewType const& v) {
    using BaseType = typename CountDims<ViewType>::BaseT;
    using SizeType = typename ViewType::size_type;
    for (SizeType i = 0; i < v.extent(0); i++) {
      if (s.isUnpacking()) {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunknown-pragmas"
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
        BaseType val;
        s | val;
        v.operator()(i) = val;
        #pragma GCC diagnostic pop
      } else {
        BaseType const& val = v.operator()(i);
        s | val;
      }
    }
  }
};

template <typename SerializerT, typename ViewType>
struct TraverseManual<SerializerT,ViewType,2> {
  static void apply(SerializerT& s, ViewType const& v) {
    using BaseType = typename CountDims<ViewType>::BaseT;
    using SizeType = typename ViewType::size_type;
    for (SizeType i = 0; i < v.extent(0); i++) {
      for (SizeType j = 0; j < v.extent(1); j++) {
        if (s.isUnpacking()) {
          #pragma GCC diagnostic push
          #pragma GCC diagnostic ignored "-Wunknown-pragmas"
          #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
          BaseType val;
          s | val;
          v.operator()(i,j) = val;
          #pragma GCC diagnostic pop
        } else {
          BaseType const& val = v.operator()(i,j);
          s | val;
        }
      }
    }
  }
};

template <typename SerializerT, typename ViewType>
struct TraverseManual<SerializerT,ViewType,3> {
  static void apply(SerializerT& s, ViewType const& v) {
    using BaseType = typename CountDims<ViewType>::BaseT;
    using SizeType = typename ViewType::size_type;
    for (SizeType i = 0; i < v.extent(0); i++) {
      for (SizeType j = 0; j < v.extent(1); j++) {
        for (SizeType k = 0; k < v.extent(2); k++) {
          if (s.isUnpacking()) {
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wunknown-pragmas"
            #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            BaseType val;
            s | val;
            v.operator()(i,j,k) = val;
            #pragma GCC diagnostic pop
          } else {
            BaseType const& val = v.operator()(i,j,k);
            s | val;
          }
        }
      }
    }
  }
};

template <typename SerializerT, typename ViewType>
struct TraverseManual<SerializerT,ViewType,4> {
  static void apply(SerializerT& s, ViewType const& v) {
    using BaseType = typename CountDims<ViewType>::BaseT;
    using SizeType = typename ViewType::size_type;
    for (SizeType i = 0; i < v.extent(0); i++) {
      for (SizeType j = 0; j < v.extent(1); j++) {
        for (SizeType k = 0; k < v.extent(2); k++) {
          for (SizeType l = 0; l < v.extent(3); l++) {
            if (s.isUnpacking()) {
              BaseType val;
              s | val;
              v.operator()(i,j,k,l) = val;
            } else {
              BaseType const& val = v.operator()(i,j,k,l);
              s | val;
            }
          }
        }
      }
    }
  }
};

} /* end namespace serdes */

#endif /*KOKKOS_ENABLED_SERDES*/

#endif /*INCLUDED_CONTAINER_VIEW_TRAVERSE_MANUAL_H*/
