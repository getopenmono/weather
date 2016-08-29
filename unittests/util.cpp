// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "util.hpp"
#include <fstream>
#include <sstream>

std::string readFile (char const * fileName)
{
    std::ifstream file;
    file.open(fileName);
    if (! file.is_open()) throw std::string("Could not open file ") + fileName;
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

uint8_t * copyBytes (std::string const & contents)
{
    uint8_t * bytes = new uint8_t[contents.size()];
    memcpy(bytes,contents.c_str(),contents.size());
    return bytes;
}

uint8_t * castToBytes (std::string const & contents)
{
	return (uint8_t *) contents.c_str();
}
