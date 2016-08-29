// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_ibytebuffer_h
#define __com_openmono_ibytebuffer_h
#include <stdint.h>
#include <stddef.h>

/**
 * ByteBuffer can be used to hold data that arrives in chunks, such as
 * an HTTP response.
 */
struct IByteBuffer
{
    virtual ~IByteBuffer () {};

    /**
     * Append a chunk of data to the end of the buffer.
     * @param chunk  temporary pointer to chunk, contents will be copied.
     * @param length size of chunk.
     */
    virtual void add (uint8_t const * chunk, size_t length) = 0;

    /**
     * @return total number of bytes in the buffer.
     */
    virtual size_t bytes () const = 0;

    /**
     * Get the total number of bytes in buffer.
     * @return total number of chunks in buffer.
     */
    virtual size_t chunks () const = 0;

    /**
     * Get the number of bytes in a particular chunk;
     * @param  index chunk index between 0 and chunks().
     * @return       number of bytes in chunk[index].
     */
    virtual size_t chunkBytes (size_t index) const = 0;

    /**
     * Get a specific byte in buffer.
     * @param  position buffer position.
     * @return          byte.
     */
    virtual uint8_t operator[] (size_t position) const = 0;

    /**
     * Get a specific chunk of the buffer.
     * @param  index chunk index between 0 and chunks().
     * @return       A pointer to the contents of the chunk, possibly in temporary storage.
     */
    virtual uint8_t const * const chunk (size_t index) const = 0;

    /**
     * Empty the buffer.
     */
    virtual void clear () = 0;
};

#endif // __com_openmono_ibytebuffer_h
