// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#if !defined(__com_openmono_unittest_util_hpp)
#define __com_openmono_unittest_util_hpp
#include <string>

#define FIXTUREDIR "./unittests/fixtures"

std::string readFile (char const * fileName);
uint8_t * copyBytes (std::string const & contents);
uint8_t * castToBytes (std::string const & contents);

#endif // __com_openmono_unittest_util_hpp
