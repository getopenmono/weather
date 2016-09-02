// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "forecastview.hpp"
using mono::geo::Point;
using mono::geo::Rect;
using mono::String;
using mono::ui::TextLabelView;
using mono::display::Color;

ForecastView::ForecastView
(
    String timeStr,
    char const * imagePath,
    String temperatureStr,
    String windStr,
    String rainStr
) :
    position(0,0),
    time(Rect(0,0,96,20),timeStr),
    image(imagePath),
    icon(&image),
    temperature(temperatureStr),
    wind(windStr),
    rain(rainStr)
{
    time.setAlignment(TextLabelView::ALIGN_CENTER);
    time.setBackground(WhiteColor);
    time.setTextColor(MidnightBlueColor);
    temperature.setAlignment(TextLabelView::ALIGN_CENTER);
    temperature.setBackground(WhiteColor);
    temperature.setTextColor(MidnightBlueColor);
    wind.setAlignment(TextLabelView::ALIGN_CENTER);
    wind.setBackground(WhiteColor);
    wind.setTextColor(MidnightBlueColor);
    rain.setAlignment(TextLabelView::ALIGN_CENTER);
    rain.setBackground(WhiteColor);
    rain.setTextColor(MidnightBlueColor);
    setPosition(Point());
}

void ForecastView::show ()
{
    time.show();
    icon.show();
    temperature.show();
    wind.show();
    rain.show();
    mono::ui::View::show();
}

void ForecastView::setPosition (Point const & point)
{
    position = point;
    uint16_t picSz = 80;
    uint16_t margin = 5;
    uint16_t x1 = position.X() + margin;
    uint16_t x2 = position.X() + margin + picSz;
    icon.setRect(Rect(x1,position.Y(),picSz,picSz));
    time.setRect(Rect(x2,position.Y()+margin,96,20));
    temperature.setRect(Rect(x2,position.Y()+25,96,20));
    wind.setRect(Rect(x2,position.Y()+45,96,20));
    rain.setRect(Rect(x2,position.Y()+65,96,20));
}

void ForecastView::repaint ()
{
    time.repaint();
    icon.repaint();
    temperature.repaint();
    wind.repaint();
    rain.repaint();
    painter.setForegroundColor(ConcreteColor);
    painter.drawHLine(position.X()+30,position.X()+146,position.Y()+85);
}
