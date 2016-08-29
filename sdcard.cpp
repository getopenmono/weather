// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "sdcard.hpp"

#define PATH_PREFIX "sd"
#define PATH_TEST_FILE "/__SdMono"

SdCard::SdCard ()
:
    _status(SdCard_MissingSdCard),
    fs(SD_SPI_MOSI,SD_SPI_MISO,SD_SPI_CLK,SD_SPI_CS,PATH_PREFIX)
{
	// Hack, there is no support for reading directory.
	FILE * file = fopen("/" PATH_PREFIX PATH_TEST_FILE,"w");
	if (file != 0)
	{
		_status = SdCard_OK;
		fclose(file);
	}

    mono::IApplicationContext::Instance->PowerManager->AppendToPowerAwareQueue(this);
}

SdCard::Status SdCard::status () const
{
	return _status;
}

SdCard & SdCard::get ()
{
    static SdCard instance;
    return instance;
}

String SdCard::fullPath (String const & path)
{
	if (path.Length() > 0 && path[0] != '/')
	    return String::Format("/" PATH_PREFIX "/%s",path());
    else
    	return String::Format("/" PATH_PREFIX "%s",path());
}

SdCard::Status SdCard::mkdirForFullPath (String const & path)
{
	char buffer[0x100];
	// Skip the initial mount point dir.
	size_t existingDir = strlen(PATH_PREFIX)+1;;
	for (size_t i = 0;i < path.Length(); ++i)
	{
		buffer[i] = path[i];
		if (i > existingDir && buffer[i] == '/')
		{
			// Temprarily terminate path.
			buffer[i] = '\0';
			mkdir(buffer,0777);
			buffer[i] = '/';
		}
		// If the last part of the path is a file, it is skipped automatically.
	}
	return SdCard_OK;
}


// MARK: Power Aware Interface

void SdCard::onSystemPowerOnReset()
{

}

void SdCard::onSystemEnterSleep()
{

}

void SdCard::onSystemWakeFromSleep()
{
    fs.initialize_card();
}

void SdCard::OnSystemBatteryLow()
{

}
