// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "Iconfiguration.hpp"
#include <string.h>

IConfiguration::~IConfiguration ()
{}

bool IConfiguration::isInvalidKey (uint8_t const * key)
{
	if (0 == key) return true;
	size_t length = strlen((char const *)key);
	if (0 == length) return true;
	if (key[0] != '/') return true;
	for (size_t i = 1; i < length; ++i)
	{
		if ('0' <= key[i] && key[i] <= '9') continue;
		if ('A' <= key[i] && key[i] <= 'Z') continue;
		if ('a' <= key[i] && key[i] <= 'z') continue;
		if ('-' <= key[i] && key[i] <= '.') continue;
		if ('_' == key[i]) continue;
		if ('/' == key[i] && key[i-1] != '/') continue;
		return true;
	}
	return false;
}
