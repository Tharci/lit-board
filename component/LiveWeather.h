//
// Created by tmarc on 14/07/2021.
//

#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif

#ifndef LITBOARD_LIVEWEATHER_H
#define LITBOARD_LIVEWEATHER_H

#include "CyclicComponent.h"
#include "WeatherData.h"

#include <atomic>
#include <thread>
#include <future>


namespace lbd::comp {
    class LiveWeather : public CyclicComponent {
    public:

    struct ALIGNED_(4) Time {
        uint8_t hour   = 0;
        uint8_t minute = 0;
        uint8_t second = 0;
    };


    struct ALIGNED_(4) WeatherData {
        Time time;
        Time sunriseTime;
        Time sunsetTime;
        int8_t temp = 0;
        int8_t tempMin = 0;
        int8_t tempMax = 0;
        uint8_t sunIntensity = 0;
        uint8_t cloudDensity = 0;
        uint8_t windIntensity = 0;
        uint8_t rainIntensity = 0;
        uint8_t stormIntensity = 0;
        uint8_t snowIntensity = 0;
        bool mist = false;
    };

        explicit LiveWeather(std::mutex& sleepMutex, std::condition_variable& sleepCondVar);
        [[nodiscard]] ComponentId getComponentId() const override;
        static WeatherData* getWeatherData();
        static Time unixToTime(time_t timeUnix) ;

    private:
        void asyncTaskCycle();
    };
}


#endif //LITBOARD_LIVEWEATHER_H
