// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_forecastview_h
#define __com_openmono_forecastview_h
#include <mono.h>
#include "lib/weather.hpp"

class ForecastView
:
    public mono::ui::View
{
    mono::geo::Point position;
    mono::ui::TextLabelView time;
    mono::media::BMPImage image;
    mono::ui::ImageView icon;
    mono::ui::TextLabelView temperature;
    mono::ui::TextLabelView wind;
    mono::ui::TextLabelView rain;
    virtual void repaint ();
public:
    ForecastView
    (
        mono::String timeStr,
        char const * imagePath,
        mono::String temperatureStr,
        mono::String windStr,
        mono::String rainStr
    );
    virtual void show ();
    void setPosition (mono::geo::Point const & point);
};

#endif // __com_openmono_forecastview_h