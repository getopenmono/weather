// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "heapbytebuffer.hpp"
#include <string.h>

HeapByteBuffer::HeapByteBuffer ()
:
    storeSize(0),
    storeCapacity(0),
    byteStore(0),
    lengthStore(0)
{
}

HeapByteBuffer::~HeapByteBuffer ()
{
    clear();
}

void HeapByteBuffer::add (uint8_t const * bytes, size_t length)
{
    enlargeStoreIfTooSmall();
    uint8_t * memory = new uint8_t[length];
    memcpy((void*)memory,(void*)bytes,length);
    byteStore[storeSize] = memory;
    lengthStore[storeSize] = length;
    ++storeSize;
}

uint8_t HeapByteBuffer::operator[] (size_t position) const
{
    size_t remaining = position;
    for (uint8_t i = 0; i < storeSize; ++i)
        if (remaining < lengthStore[i])
            return byteStore[i][remaining];
        else
            remaining -= lengthStore[i];
    return 0;
}

void HeapByteBuffer::clear ()
{
    for (size_t i = 0; i < storeSize; ++i) delete byteStore[i];
    if (byteStore != 0) delete byteStore;
    if (lengthStore != 0) delete lengthStore;
    byteStore = 0;
    lengthStore = 0;
    storeSize = 0;
    storeCapacity = 0;
}

void HeapByteBuffer::enlargeStoreIfTooSmall ()
{
    if (0 == byteStore) createNewStore();
    else if (storeSize >= storeCapacity) doubleStoreCapacity();
}

void HeapByteBuffer::createNewStore ()
{
    storeSize = 0;
    storeCapacity = 2;
    byteStore = new uint8_t const *[storeCapacity];
    lengthStore = new size_t[storeCapacity];
}

void HeapByteBuffer::doubleStoreCapacity ()
{
    uint8_t const ** bytesOld = byteStore;
    size_t * lengthsOld = lengthStore;
    storeCapacity *= 2;
    byteStore = new uint8_t const *[storeCapacity];
    lengthStore = new size_t[storeCapacity];
    for (size_t i = 0; i < storeSize; ++i)
    {
        byteStore[i] = bytesOld[i];
        lengthStore[i] = lengthsOld[i];
    }
    delete bytesOld;
    delete lengthsOld;
}

size_t HeapByteBuffer::bytes () const
{
    size_t total = 0;
    for (uint8_t i = 0; i < storeSize; ++i) total += lengthStore[i];
    return total;
}

size_t HeapByteBuffer::chunks () const
{
    return storeSize;
}

uint8_t const * const HeapByteBuffer::chunk(size_t index) const
{
    return byteStore[index];
}

size_t HeapByteBuffer::chunkBytes(size_t index) const
{
    return lengthStore[index];
}
