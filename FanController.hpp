/* 
 * File:   FanController.hpp
 * Author: chlorek
 *
 * Created on June 10, 2018, 3:37 PM
 */

#ifndef FANCONTROLLER_HPP
#define FANCONTROLLER_HPP

#include <vector>
#include <memory>
#include <mutex>

#include "Device.hpp"
#include "Types.hpp"
#include "Effect.hpp"
#include "ColorBuffer.hpp"

#include <iostream>
#include <iomanip>

namespace colorsair {
    class FanController {
        public:
            FanController(Device& dev, unsigned char fansCount, unsigned char ledsPerFan, uint8_t framerate = 20);
            FanController(const FanController& orig) = delete;
            virtual ~FanController();
            
            template<class EFFECT_T>
            void setEffect(unsigned char fanId, EFFECT_T effect) {
                std::lock_guard<std::mutex> lock(stateMutex);
                effects[fanId] = std::make_unique<EFFECT_T>(effect);
            }

            void loop();
            
            unsigned char getFansCount();
        private:
            static const std::array<unsigned char, 64> MAGIC_FRAME_1;
            static const std::array<unsigned char, 64> MAGIC_FRAME_2;
            static const std::array<unsigned char, 64> MAGIC_FRAME_3;
            static const std::array<unsigned char, 64> MAGIC_FRAME_4;
            static const std::array<std::array<unsigned char, 64>, 5> MAGIC_FRAME;
            static std::array<unsigned char, 64> COLOR_FRAME;
            ColorBuffer colorBuffer;
            Device& dev;
            unsigned char fansCount;
            unsigned char ledsPerFan;
            std::vector<std::unique_ptr<Effect>> effects;
            std::mutex stateMutex;
            
            unsigned short timestep;
            std::chrono::system_clock::time_point frameCountTime, frameStart;
            uint8_t frames = 0;
            uint8_t framerate;
    };
}

#endif /* FANCONTROLLER_HPP */

