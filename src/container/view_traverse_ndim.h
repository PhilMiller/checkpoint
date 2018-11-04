
#if !defined INCLUDED_CONTAINER_VIEW_TRAVERSE_NDIM_H
#define INCLUDED_CONTAINER_VIEW_TRAVERSE_NDIM_H

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
#include <cassert>
#include <type_traits>

namespace serdes { namespace detail {

/*
 * Traversal of data backing a view by recursively traversing each dimension and
 * every element of the data one-by-one using the Kokkos::View::operator()(...)
 * or Kokkos::DynamicView::operator()(...). The access operator is guaranteed to
 * correctly accesses the data regardless of the layout. This new code does a
 * recursive traversal so it works for n-dimensional views.
 *
 * This code generally allows you to specify any callable, and use k-views if
 * you want to operate by essentially zipping the elements and passing to the
 * call function. You can compare equality, apply element-by-element equality,
 * or other n-dim operators on k-views.
 *
 */

// Recursive callable apply for n-dimension automatic traversal of a view
template <
  typename ViewT, typename T, typename TupleT, unsigned d, typename Callable
>
struct TraverseRecurImpl;

/*
 * Simple meta-code for detecting a tuple type
 */
template <typename T>
struct IsTuple : std::false_type {};
template <typename... U>
struct IsTuple<std::tuple<U...>> : std::true_type {};

/*
 * Extract the size_type from the View, overload to extract from
 * std::tuple<Kokkos::View...> of
 */
template <typename T>
struct ViewSizeTraits {
  using SizeType = typename T::size_type;
};

template <typename T, typename... Args>
struct ViewSizeTraits<std::tuple<T,Args...>> {
  using SizeType = typename T::size_type;
};

/*
 * General code for traversing a static or dynamic dimension and recursively
 * preceding pushing the index into the tuple
 */
template <
  typename ViewT, typename T, typename TupleT, unsigned d, typename Callable
>
struct TraverseRecurImplBase {
  using SizeType       = typename ViewSizeTraits<ViewT>::SizeType;
  template <typename U>
  using ViewIsTuple    = typename std::enable_if<IsTuple<U>::value,ViewT>::type;
  template <typename U>
  using ViewNotTuple   = typename std::enable_if<!IsTuple<U>::value,ViewT>::type;

  using TupleChildT = decltype(
    std::tuple_cat(
      std::declval<TupleT>(),
      std::declval<std::tuple<SizeType>>()
    )
  );
  using ChildT = TraverseRecurImpl<ViewT,T,TupleChildT,d-1,Callable>;

  template <typename U = ViewT>
  static void applyImpl(
    ViewT const& view, unsigned nd, TupleT idx, Callable call,
    ViewIsTuple<U>* x_ = nullptr
  ) {
    auto const ex1 = std::get<0>(view).extent(nd-d);
    auto const ex2 = std::get<1>(view).extent(nd-d);
    assert(ex1 == ex2 && "Matching extents must be equal");
    for (SizeType i = 0; i < ex1; i++) {
      ChildT::apply(view,nd,std::tuple_cat(idx,std::tuple<SizeType>(i)),call);
    }
  }

  template <typename U = ViewT>
  static void applyImpl(
    ViewT const& view, unsigned nd, TupleT idx, Callable call,
    ViewNotTuple<U>* x_ = nullptr
  ) {
    for (SizeType i = 0; i < view.extent(nd-d); i++) {
      ChildT::apply(view,nd,std::tuple_cat(idx,std::tuple<SizeType>(i)),call);
    }
  }

  static void apply(ViewT const& view, unsigned nd, TupleT idx, Callable call) {
    applyImpl<ViewT>(view,nd,idx,call);
  }
};

// Overload for traversing a static T[N] dimension
template <
  typename ViewT, typename T, unsigned N, typename TupleT, unsigned d,
  typename Callable
>
struct TraverseRecurImpl<ViewT, T[N], TupleT, d, Callable>
  : TraverseRecurImplBase<ViewT,T,TupleT,d,Callable>
{ };

// Overload for traversing a dynamic (T*) dimension
template <
  typename ViewT, typename T, typename TupleT, unsigned d, typename Callable
>
struct TraverseRecurImpl<ViewT, T*, TupleT, d, Callable>
  : TraverseRecurImplBase<ViewT,T,TupleT,d,Callable>
{ };

// Overload for traversing after we reach recursively the last dimension and now
// will dispatch the View::operator()()
template <
  typename ViewT, typename T, typename TupleT, unsigned d, typename Callable
>
struct TraverseRecurImpl {
  using SizeType     = typename ViewSizeTraits<ViewT>::SizeType;
  template <typename U>
  using ViewIsTuple  = typename std::enable_if<IsTuple<U>::value,ViewT>::type;
  template <typename U>
  using ViewNotTuple = typename std::enable_if<!IsTuple<U>::value,ViewT>::type;
  template <typename U>
  using GetBaseType  = typename CountDims<U>::BaseT;

  // Unwind the inner tuple for operator()(...)
  template <typename ViewU, std::size_t... I>
  static GetBaseType<ViewU>& expandTupleToOp(
    ViewU const& view, TupleT tup, std::index_sequence<I...> idx
  ) {
    return view.operator()(std::get<I>(tup)...);
  }

  // Unwind the inner tuple for operator()(...)
  template <typename ViewU>
  static GetBaseType<ViewU>& expandTupleToOp(ViewU const& view, TupleT tup) {
    constexpr auto size = std::tuple_size<TupleT>::value;
    return expandTupleToOp(view,tup,std::make_index_sequence<size>{});
  }

  // If it's a std::tuple<Kokkos::vie<T>...>, first unwind the outer tuple
  template <std::size_t... I>
  static void dispatchViewTuple(
    ViewT const& view, Callable call, TupleT tup, std::index_sequence<I...>
  ) {
    // Dispatch the call operator on the view
    call(expandTupleToOp(std::get<I>(view),tup)...);
  }

  // Test whether the ViewT is actually a std::tuple<Kokkos::View<T>...>
  template <typename U = ViewT>
  static void dispatchViewType(
    ViewT const& view, Callable call, TupleT tup, ViewIsTuple<U>* x_ = nullptr
  ) {
    constexpr auto size = std::tuple_size<ViewT>::value;
    return dispatchViewTuple(view,call,tup,std::make_index_sequence<size>{});
  }

  template <typename U = ViewT>
  static void dispatchViewType(
    ViewT const& view, Callable call, TupleT tup, ViewNotTuple<U>* x_ = nullptr
  ) {
    call(expandTupleToOp(view,tup));
  }

  static void apply(ViewT const& view, unsigned, TupleT tup, Callable call) {
    return dispatchViewType(view,call,tup);
  }
};


}} /* end namespace serdes::detail */

namespace serdes {

template <typename ViewT, typename T, unsigned nd, typename Callable>
struct TraverseRecursive {
  using RecurT = detail::TraverseRecurImpl<ViewT,T,std::tuple<>,nd,Callable>;
  static void apply(ViewT const& view, Callable call) {
    return RecurT::apply(view,nd,std::tuple<>{},call);
  }
};

} /* end namespace serdes */

#endif /*KOKKOS_ENABLED_SERDES*/

#endif /*INCLUDED_SERDES_CONTAINER_VIEW_SERIALIZE_H*/
