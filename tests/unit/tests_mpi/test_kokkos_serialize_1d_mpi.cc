/*
//@HEADER
// *****************************************************************************
//
//                       test_kokkos_serialize_1d_mpi.cc
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
#if KOKKOS_ENABLED_SERDES

#include "test_kokkos_1d_commons.h"
#include "tests_mpi/test_commons_mpi.h"

template <typename ParamT> struct KokkosViewTest1DMPI : KokkosViewTest<ParamT> { };

TYPED_TEST_CASE_P(KokkosViewTest1DMPI);

TYPED_TEST_P(KokkosViewTest1DMPI, test_1d_any) {
  using namespace serialization::interface;

  using LayoutType = typename std::tuple_element<1,TypeParam>::type;
  using DataType   = typename std::tuple_element<0,TypeParam>::type;
  using ViewType          = Kokkos::View<DataType, LayoutType>;
  using NonConstT         = typename ViewType::traits::non_const_data_type;
  using NonConstViewType  = Kokkos::View<NonConstT, LayoutType>;
  using ConstT         = typename ViewType::traits::const_data_type;
  using ConstViewType  = Kokkos::View<ConstT, LayoutType>;

  static constexpr size_t const N = 241;

  LayoutType layout = layout1d<LayoutType>(N);
  NonConstViewType in_view("test", layout);

  init1d(in_view);

  if (std::is_same<NonConstViewType, ViewType>::value) {
    serializeAnyMPI<NonConstViewType>(in_view, &compare1d<NonConstViewType>);
  }
  else {
    ConstViewType const_in_view = in_view;
    serializeAnyMPI<ConstViewType>(const_in_view, &compare1d<ConstViewType>);
  }
}


REGISTER_TYPED_TEST_CASE_P(KokkosViewTest1DMPI, test_1d_any);

#if DO_UNIT_TESTS_FOR_VIEW

INSTANTIATE_TYPED_TEST_CASE_P(test_1d_L, KokkosViewTest1DMPI, Test1DTypesLeft);
INSTANTIATE_TYPED_TEST_CASE_P(test_1d_R, KokkosViewTest1DMPI, Test1DTypesRight);
INSTANTIATE_TYPED_TEST_CASE_P(test_1d_S, KokkosViewTest1DMPI, Test1DTypesStride);
INSTANTIATE_TYPED_TEST_CASE_P(test_1d_L_C, KokkosViewTest1DMPI, Test1DConstTypesLeft);
INSTANTIATE_TYPED_TEST_CASE_P(test_1d_R_C, KokkosViewTest1DMPI, Test1DConstTypesRight);
INSTANTIATE_TYPED_TEST_CASE_P(test_1d_S_C, KokkosViewTest1DMPI, Test1DConstTypesStride);

#endif

///////////////////////////////////////////////////////////////////////////////
// Kokkos::DynamicView Unit Tests: dynamic view is restricted to 1-D in kokkos
///////////////////////////////////////////////////////////////////////////////

template <typename ParamT>
struct KokkosDynamicViewTestMPI : KokkosViewTest<ParamT> { };

TYPED_TEST_CASE_P(KokkosDynamicViewTestMPI);

TYPED_TEST_P(KokkosDynamicViewTestMPI, test_dynamic_1d) {
  using namespace serialization::interface;

  using DataType = TypeParam;
  using ViewType = Kokkos::Experimental::DynamicView<DataType>;

  static constexpr std::size_t const N = 64;
  static constexpr unsigned const min_chunk = 8;
  static constexpr unsigned const max_extent = 1024;

  ViewType in_view("my-dynamic-view", min_chunk, max_extent);
  in_view.resize_serial(N);

  init1d(in_view);

  serializeAnyMPI<ViewType>(in_view, &compare1d<ViewType>);
}

REGISTER_TYPED_TEST_CASE_P(KokkosDynamicViewTestMPI, test_dynamic_1d);

#if DO_UNIT_TESTS_FOR_VIEW
INSTANTIATE_TYPED_TEST_CASE_P(
  test_dynamic_view_1, KokkosDynamicViewTestMPI, DynamicTestTypes
);
#endif

#endif
