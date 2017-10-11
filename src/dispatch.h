
#if !defined INCLUDED_SERDES_DISPATCH
#define INCLUDED_SERDES_DISPATCH

#include "serdes_common.h"
#include "managed_buffer.h"
#include "dispatch_serializer.h"
#include "dispatch_deserializer.h"

#include <tuple>

namespace serdes {

using SerializedType = std::tuple<SerialByteType*, SizeType>;
using ManagedBufferPtrType = std::unique_ptr<ManagedBuffer>;
using ManagedSerializedType = std::tuple<ManagedBufferPtrType, SizeType>;

template <typename T>
struct Dispatch {
  static SizeType sizeType(T& to_size);
  static ManagedBufferPtrType packType(T& to_pack, SizeType const& size);
  static T& unpackType(
    SerialByteType* buf, SerialByteType* data, SizeType const& size
  );
};

template <typename Serializer, typename T>
inline void operator|(Serializer& s, T& target);

template <typename Serializer, typename T>
inline void serializeArray(Serializer& s, T* array, SizeType const num_elms);

template <typename T>
ManagedSerializedType serializeType(T& to_serialize);

template <typename T>
T& deserializeType(SerialByteType* data, SizeType const& size);

} /* end namespace serdes */

#include "dispatch.impl.h"

#endif /*INCLUDED_SERDES_DISPATCH*/
