// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "app_controller.h"
#include "lib/openweathermap.hpp"
#include "io/File.h"
#include "lib/iso.hpp"
#include "lib/bytestring.hpp"
#include "sdcard.hpp"
#include <stdio.h>
using mono::geo::Point;
using mono::geo::Rect;
using mono::IApplicationContext;
using mono::String;
using mono::Timer;
using mono::ui::BackgroundView;
using mono::ui::ImageView;
using mono::ui::TextLabelView;
using mono::ui::View;

/**
 * OpenWeatherMap allows free access to weather forecasts, but you need to get
 * an Application Identifier to use the API. There are some limits on how often
 * we can the same AppId, so if you create another app that uses OpenWeatherMap,
 * please create your own AppId.
 */
#define MONO_OPENWEATHERMAP_APPID "241d52af482f9f7ff00b0cc909229d92"

#define FORECASTS_TO_KEEP 5

#define MONO_WEATHER_CITY (uint8_t const *)(CONF_KEY_ROOT "/weather/city.txt")
#define MONO_WEATHER_COUNTRYCODE (uint8_t const *)(CONF_KEY_ROOT "/weather/countrycode.txt")
#define MONO_WEATHER_TIMEZONE (uint8_t const *)(CONF_KEY_ROOT "/weather/timezone.txt")
#define MONO_WEATHER_FORECAST (uint8_t const *)(CONF_KEY_ROOT "/weather/forecast.json")
#define MONO_WEATHER_UNIT (uint8_t const *)(CONF_KEY_ROOT "/weather/unit.txt")
#define MONO_WEATHER_METRIC "metric"

namespace
{
    static const int dimBrightness = 50;

    uint8_t * castToBytes (char const * contents)
    {
        return (uint8_t *) contents;
    }
} // namespace

AppController::AppController ()
:
    toucher(this),
    background(WhiteColor),
    topLabel(Rect(0,200,176,20),"Weather Forecast"),
    wifi(0),
    dimmer(20*10000,true),
    sleeper(10*1000,true),
    view1(0),
    view2(0)
{
    topLabel.setAlignment(TextLabelView::ALIGN_CENTER);
    topLabel.setBackground(WhiteColor);
    topLabel.setTextColor(MidnightBlueColor);
    dimmer.setCallback<AppController>(this,&AppController::dim);
    sleeper.setCallback(IApplicationContext::EnterSleepMode);
}

void AppController::debug (String msg)
{
    printf(String::Format("%s\r\n",msg())());
}

void AppController::error (String shortMsg, String longMsg)
{
    debug(longMsg);
    topLabel.setText(shortMsg());
}

void AppController::monoWakeFromReset ()
{
    debug("Woke from reset");
    background.show();
    topLabel.setText("Weather Forecast"),
    topLabel.show();
    if (conf.status() == SdCardConfiguration::Configuration_MissingSdCard)
        return error("Missing SD card","Missing SD card");
    asyncCall(&AppController::readForecastFromSdCardAndShow);
}

void AppController::asyncCall (void (AppController::*method)())
{
    mono::Timer::callOnce<AppController>(0,this,method);
}

void AppController::monoWakeFromSleep ()
{
    debug("Woke from sleep");
    IApplicationContext::SoftwareResetToApplication();
}

void AppController::readForecastFromSdCardAndShow ()
{
    debug("reading previous forecast from SD card");
    String previousForecast = (char const *)MONO_WEATHER_FORECAST;
    debug(String::Format("Reading %s",previousForecast()));
    buffer.attach(previousForecast);
    if (buffer.status() != SdCardByteBuffer::SdBuffer_OK)
        return error("SD card problem",String::Format("Could not create forecast buffer %s on SD card",previousForecast()));
    interpretForecast();
}

void AppController::interpretForecast ()
{
    if (conf.get(MONO_WEATHER_TIMEZONE) == 0)
        return error("Missing SD conf",String::Format("Missing conf file %s on SD card",MONO_WEATHER_TIMEZONE));
    timeZone = conf.get(MONO_WEATHER_TIMEZONE);
    if (conf.get(MONO_WEATHER_UNIT) == 0)
        return error("Missing SD conf",String::Format("Missing conf file %s on SD card",MONO_WEATHER_UNIT));
    unit = conf.get(MONO_WEATHER_UNIT);
    json::Json json(buffer);
    uint8_t const * city = json.lookup("/city/name");
    topLabel.setText((char const *)city);
    topLabel.show();
    forecast = openweathermap::parseForecast(buffer,FORECASTS_TO_KEEP);
    // Just show the next two forecasts.
    showForecast1(forecast[0]);
    showForecast2(forecast[1]);
}

void AppController::showForecast1 (weather::Entry const & entry)
{
    showForecast(entry,view1,5);
}

void AppController::showForecast2 (weather::Entry const & entry)
{
    showForecast(entry,view2,95);
    asyncCall(&AppController::undim);
}

void AppController::showForecast (weather::Entry const & entry, ForecastView * & view, size_t yPosition)
{
    time_t timeZoneDiff = Iso::timeZoneToUnixTimeStampDiff(timeZone.c_str());
    time_t unixTime = atoi((char const*)entry.timeUnix) + timeZoneDiff;
    tm * timeInfo = localtime(&unixTime);
    debug(String::Format("time %.2u:%.2u",timeInfo->tm_hour,timeInfo->tm_min));
    char const * icon = conditionToIcon(entry.condition);
    String time = convertTimeAccordingToConf(timeInfo->tm_hour,timeInfo->tm_min);
    String temperature = convertTemperatureAccordingToConf(entry.temperatureK);
    String wind = convertWindAccordingToConf(entry.windSpeedMs,entry.windDirection);
    String rain = convertRainAccordingToConf(entry.rain);
    view = new ForecastView(time,icon,temperature,wind,rain);
    view->setPosition(Point(0,yPosition));
    view->show();
}

char const * AppController::conditionToIcon (weather::Condition condition)
{
    debug(String::Format("condition %d", condition));
    switch (condition)
    {
        case weather::Day_ClearSky: return "/sd/mono/weather/Day_ClearSky.bmp";
        case weather::Day_FewClouds: return "/sd/mono/weather/Day_FewClouds.bmp";
        case weather::Day_ScatteredClouds: return "/sd/mono/weather/Day_ScatteredClouds.bmp";
        case weather::Day_Overcast: return "/sd/mono/weather/Day_Overcast.bmp";
        case weather::Day_Rain: return "/sd/mono/weather/Day_Rain.bmp";
        case weather::Day_ShowerRain: return "/sd/mono/weather/Day_ShowerRain.bmp";
        case weather::Day_Thunder: return "/sd/mono/weather/Day_Thunder.bmp";
        case weather::Day_Snow: return "/sd/mono/weather/Day_Snow.bmp";
        case weather::Day_Mist: return "/sd/mono/weather/Day_Mist.bmp";
        case weather::Night_ClearSky: return "/sd/mono/weather/Night_ClearSky.bmp";
        case weather::Night_FewClouds: return "/sd/mono/weather/Night_FewClouds.bmp";
        case weather::Night_ScatteredClouds: return "/sd/mono/weather/Night_ScatteredClouds.bmp";
        case weather::Night_Overcast: return "/sd/mono/weather/Night_Overcast.bmp";
        case weather::Night_Rain: return "/sd/mono/weather/Night_Rain.bmp";
        case weather::Night_ShowerRain: return "/sd/mono/weather/Night_Rain.bmp";
        case weather::Night_Thunder: return "/sd/mono/weather/Night_Thunder.bmp";
        case weather::Night_Snow: return "/sd/mono/weather/Night_Snow.bmp";
        case weather::Night_Mist: return "/sd/mono/weather/Day_Mist.bmp";
        default:
        case weather::Unknown: return "/sd/mono/weather/Night_Snow.bmp";
    }
}

String AppController::convertTemperatureAccordingToConf (uint8_t const * temperatureK)
{
    if (unit == MONO_WEATHER_METRIC)
    {
        int temp = atoi((char const *)temperatureK) - 273;
        debug(String::Format("temperature %d", temp));
        String temperature = String::Format("%d C",temp);
        return temperature;
    }
    else
    {
        int temp = (atoi((char const *)temperatureK) - 273) * 1.8 + 32;
        debug(String::Format("temperature %d", temp));
        String temperature = String::Format("%d F",temp);
        return temperature;
    }
}

String AppController::convertTimeAccordingToConf (int hours, int minutes)
{
    if (unit == MONO_WEATHER_METRIC)
        return String::Format("%.2u:%.2u",hours,minutes);
    if (hours > 12)
        return String::Format("%u pm",hours-12);
    return String::Format("%u am",hours);
}

String AppController::convertWindAccordingToConf (uint8_t const * windSpeedMs, uint8_t const * windDirection)
{
    debug(String::Format("wind: %s",(char const *)windSpeedMs));
    float msSpeed = atof((char const *)windSpeedMs);
    if (unit == MONO_WEATHER_METRIC)
        return String::Format("%u m/s",(unsigned)(msSpeed+0.5));
    else
        return String::Format("%u mi/h",(unsigned)(2.237*msSpeed+0.5));
}

String AppController::convertRainAccordingToConf (uint8_t const * rain)
{
    debug(String::Format("rain: %s",(char const *)rain));
    if (strlen((char const *)rain) == 0)
        return String((char const *)rain);
    float mmRain = atof((char const *)rain);
    if (unit == MONO_WEATHER_METRIC)
        return String::Format("%u mm",(unsigned)(mmRain+0.5));
    else
    {
        float inch = mmRain*0.03937;
        float fraction = inch - (unsigned)inch;
        return String::Format("%u.%.2u in",(unsigned)inch,(unsigned)(fraction*100));
    }
}

void AppController::dim ()
{
    dimmer.Stop();
    topLabel.setText("Touch to undim"),
    topLabel.show();
    IDisplayController * display = IApplicationContext::Instance->DisplayController;
    for (int i = display->Brightness(); i >= dimBrightness; --i)
    {
        display->setBrightness(i);
        wait_ms(2);
    }
    sleeper.Start();
}

void AppController::undim ()
{
    IDisplayController * display = IApplicationContext::Instance->DisplayController;
    display->setBrightness(255);
    topLabel.setText("Touch to update"),
    topLabel.show();
    setupTimersAndHandler();
}

void AppController::getNewForecast()
{
    debug("Initialising wifi");
    topLabel.setText("Getting forecast"),
    topLabel.show();
    if (0 == wifi)
        wifi = new Wifi(conf);
    wifi->connect<AppController>(this,&AppController::networkReadyHandler);
    if (wifi->status() == Wifi::Wifi_BrokenConfiguration)
        return error("SD card conf broken","Wifi conf on SD card broken");
    debug(String::Format("Looking for SSID %s\r\n",conf.get(MONO_WIFI_SSID)));
    if (wifi->status() != Wifi::Wifi_Connecting)
    {
        String msg = String::Format("Wifi problem %d",wifi->status());
        return error(msg,msg);
    }
}

void AppController::networkReadyHandler ()
{
    debug("Network ready");
    topLabel.setText("Using network");
    String previousForecast = (char const *)MONO_WEATHER_FORECAST;
    buffer.attach(previousForecast);
    buffer.clear();
    if (buffer.status() != SdCardByteBuffer::SdBuffer_OK)
        return error("SD card problem",String::Format("Could not create forecast buffer %s on SD card",previousForecast()));
    if (conf.get(MONO_WEATHER_CITY) == 0)
        return error("Missing SD conf",String::Format("Missing conf file %s on SD card",MONO_WEATHER_CITY));
    ByteString city = conf.get(MONO_WEATHER_CITY);
    if (conf.get(MONO_WEATHER_COUNTRYCODE) == 0)
        return error("Missing SD conf",String::Format("Missing conf file %s on SD card",MONO_WEATHER_COUNTRYCODE));
    ByteString country = conf.get(MONO_WEATHER_COUNTRYCODE);
    debug(String::Format("Get forecast %s (%s)",city.c_str(),country.c_str()));
    String url = String::Format
    (
        "http://api.openweathermap.org/data/2.5/forecast?q=%s,%s&mode=json&appid=%s",
        city.c_str(),
        country.c_str(),
        MONO_OPENWEATHERMAP_APPID
    );
    debug(url);
    wifi->get(castToBytes(url()),&buffer,this,&AppController::handleWifiResult,&AppController::handleWifiStatus);
}

void AppController::handleWifiStatus (Wifi::Status status)
{
    switch (status)
    {
        case Wifi::Wifi_Ready:
            printf("\r\n");
            break;
        case Wifi::Wifi_NotReady:
            return restartRequestWithError("wifi not ready");
        case Wifi::Wifi_BrokenUrl:
            printf("broken URL");
            break;
        case Wifi::Wifi_Receiving:
            printf(".");
            break;
        case Wifi::Wifi_BrokenCommunication:
            return restartRequestWithError("wifi communication error");
        case Wifi::Wifi_DnsResolutionFailed:
            return restartRequestWithError("DNS resolution failed");
        case Wifi::Wifi_UnknownError:
        default:
            return restartRequestWithError("Unknown wifi error");
    }
}

void AppController::restartRequestWithError (char const * message)
{
    printf(message);
    asyncCall(&AppController::getNewForecast);
}

void AppController::handleWifiResult (IByteBuffer *)
{
    debug("Wifi result arrived");
    topLabel.setText("Got forecast"),
    topLabel.show();
    setupTimersAndHandler();
    asyncCall(&AppController::interpretForecast);
}

void AppController::setupTimersAndHandler ()
{
    sleeper.Stop();
    dimmer.Start();
}

void AppController::monoWillGotoSleep ()
{
    printf("GoToSleep");
    dimmer.Stop();
    sleeper.Stop();
}

void AppController::handleTouch ()
{
    debug("touch");
    if (sleeper.Running())
        undim();
    else
    {
        undim();
        asyncCall(&AppController::getNewForecast);
    }
}
