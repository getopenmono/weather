# Weather

This is a weather forecast app for [Mono](http://openmono.com).

## Configuration

The app expects the following directories and files on an SD card to tell the app how to connect to the internet, how to display numerical values, and the city to get the forecast for.

```
/mono/wifi/ssid.txt
/mono/wifi/password.txt
/mono/weather/city.txt
/mono/weather/countrycode.txt
/mono/weather/timezone.txt
/mono/weather/unit.txt
```

All the above files should be a single line of text without newlines.

The city and country code must be something that [OpenWeatherMap](http://openweathermap.org/forecast5) understands (eg. `London` and `gb`).

The timezone must be in [ISO format](https://en.wikipedia.org/wiki/ISO_8601) (eg. `-0600`).

The units can be `metric` or `imperial`.

## Graphics

In addition to the configuration files, the app needs the following graphic files:

```
/mono/weather/Day_ClearSky.bmp
/mono/weather/Day_FewClouds.bmp
/mono/weather/Day_Mist.bmp
/mono/weather/Day_Overcast.bmp
/mono/weather/Day_Rain.bmp
/mono/weather/Day_ScatteredClouds.bmp
/mono/weather/Day_ShowerRain.bmp
/mono/weather/Day_Snow.bmp
/mono/weather/Day_Thunder.bmp
/mono/weather/Night_ClearSky.bmp
/mono/weather/Night_FewClouds.bmp
/mono/weather/Night_Overcast.bmp
/mono/weather/Night_Rain.bmp
/mono/weather/Night_ScatteredClouds.bmp
/mono/weather/Night_Snow.bmp
```

