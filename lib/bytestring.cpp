// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "bytestring.hpp"
#include <string.h>

ByteString::ByteString ()
:
    data(0)
{
    createEmptyString();
}

ByteString::ByteString (char const * string)
:
    data(0)
{
    copyBareString((uint8_t const *)string);
}

ByteString::ByteString (uint8_t const * string)
:
    data(0)
{
    copyBareString(string);
}

ByteString::~ByteString ()
{
    deleteData();
}

void ByteString::deleteData ()
{
    if (data != 0) delete data;
    data = 0;
}

ByteString & ByteString::operator = (ByteString const & rhs)
{
    deleteData();
    copyBareString(rhs.data);
    return *this;
}

void ByteString::createEmptyString()
{
    data = new uint8_t[1];
    data[0] = '\0';
}

void ByteString::copyBareString (uint8_t const * string)
{
    if (0 == string)
    {
        createEmptyString();
        return;
    }
    size_t length = strlen((char const *)string);
    data = new uint8_t[length+1];
    strcpy((char *)data,(char const *)string);
}

bool ByteString::operator == (char const * rhs) const
{
    if (0 == rhs) return false;
    return (strcmp((char const *)data,rhs) == 0);
}

bool ByteString::operator != (char const * rhs) const
{
    return !operator==(rhs);
}

char const * ByteString::c_str () const
{
    return (char const *)data;
}
