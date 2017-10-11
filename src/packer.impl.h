
#include "serdes_common.h"
#include "serdes.h"
#include "memory_serializer.h"
#include "managed_buffer.h"

namespace serdes {

template <typename BufferT>
PackerBuffer<BufferT>::PackerBuffer(SizeType const& in_size)
  : MemorySerializer(ModeType::Packing), size_(in_size),
    buffer_(std::make_unique<BufferT>(size_))
{
  MemorySerializer::initializeBuffer(buffer_->getBuffer());
  debug_serdes(
    "PackerBuffer: size=%ld, start_=%p, cur_=%p\n", size_, start_, cur_
  );
}

template <typename BufferT>
typename PackerBuffer<BufferT>::BufferPtrType
PackerBuffer<BufferT>::extractPackedBuffer() {
  auto ret = std::move(buffer_);
  buffer_ = nullptr;
  return ret;
}

template <typename BufferT>
void PackerBuffer<BufferT>::contiguousBytes(
  void* ptr, SizeType size, SizeType num_elms
) {
  debug_serdes(
    "PackerBuffer: offset=%ld, size=%ld, num_elms=%ld, ptr=%p, cur_=%p, val=%d\n",
    cur_ - start_, size_, num_elms, ptr, cur_, *reinterpret_cast<int*>(ptr)
  );

  SizeType const len = size * num_elms;
  SerialByteType* spot = this->getSpotIncrement(len);
  std::memcpy(spot, ptr, len);
}

} /* end namespace serdes */
