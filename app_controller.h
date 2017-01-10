// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef app_controller_h
#define app_controller_h
#include <mono.h>
#include <vector>
#include "forecastview.hpp"
#include "lib/weather.hpp"
#include "sdcardbytebuffer.hpp"
#include "sdcardconfiguration.hpp"
#include "toucher.hpp"
#include "wifi.hpp"

class AppController
:
    public mono::IApplication
{
    Toucher toucher;
    mono::ui::BackgroundView background;
    mono::ui::TextLabelView topLabel;
    Wifi * wifi;
    SdCardByteBuffer buffer;
    SdCardConfiguration conf;
    ByteString timeZone;
    ByteString unit;
    mono::Timer dimmer;
    mono::Timer sleeper;
    std::vector<weather::Entry> forecast;
    ForecastView * view1;
    ForecastView * view2;
    void readForecastFromSdCardAndShow ();
    void getNewForecast ();
    void interpretForecast ();
    void dim ();
    void undim ();
    void networkReadyHandler ();
    void handleWifiStatus (Wifi::Status);
    void restartRequestWithError (char const * message);
    void handleWifiResult (IByteBuffer * result);
    void showForecast1 (weather::Entry const &);
    void showForecast2 (weather::Entry const &);
    void showForecast (weather::Entry const & entry, ForecastView * & view, size_t yPosition);
    char const * conditionToIcon (weather::Condition condition);
    mono::String convertTemperatureAccordingToConf (uint8_t const * temperatureK);
    mono::String convertTimeAccordingToConf (int hours, int minutes);
    mono::String convertWindAccordingToConf (uint8_t const * windSpeedMs, uint8_t const * windDirection);
    mono::String convertRainAccordingToConf (uint8_t const * rain);
    void setupTimersAndHandler ();
    void error (mono::String shortMsg, mono::String longMsg);
    void debug (mono::String msg);
    void asyncCall (void (AppController::*method)());
public:
    AppController ();
    void monoWakeFromReset ();
    void monoWillGotoSleep ();
    void monoWakeFromSleep ();
    void handleTouch ();
};

#endif // app_controller_h