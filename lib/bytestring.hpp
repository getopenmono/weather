// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_bytestring_h
#define __com_openmono_bytestring_h
#include <stdint.h>

/**
 * Dynamically allocated null-terminated string of bytes.
 */
class ByteString
{
public:
	ByteString ();
	ByteString (char const * string);
	ByteString (uint8_t const * string);
	~ByteString ();
    ByteString & operator = (ByteString const & rhs);
    bool operator == (char const * rhs) const;
    bool operator != (char const * rhs) const;
    char const * c_str () const;
private:
	void deleteData ();
	void copyBareString (uint8_t const * string);
	void createEmptyString ();
	uint8_t * data;
};

#endif // __com_openmono_bytestring_h
