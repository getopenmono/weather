// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_heapbytebuffer_h
#define __com_openmono_heapbytebuffer_h
#include "ibytebuffer.hpp"

/**
 * HeapByteBuffer can be used to collect data that arrives in chunks, such as
 * an HTTP response.  The data is copied into dynamically allocated memory.
 */
class HeapByteBuffer
:
    public IByteBuffer
{
public:
    HeapByteBuffer ();
    virtual ~HeapByteBuffer ();
    virtual void add (uint8_t const * chunk, size_t length);
    virtual size_t bytes () const;
    virtual size_t chunks () const;
    virtual size_t chunkBytes (size_t index) const;
    virtual uint8_t operator[] (size_t position) const;
    virtual uint8_t const * const chunk (size_t index) const;
    virtual void clear ();
private:
    size_t storeSize;
    size_t storeCapacity;
    uint8_t const ** byteStore;
    size_t * lengthStore;
    void enlargeStoreIfTooSmall ();
    void createNewStore ();
    void doubleStoreCapacity ();
};

#endif // __com_openmono_heapbytebuffer_h
