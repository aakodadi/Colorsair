/* 
 * File:   FanController.cpp
 * Author: chlorek
 * 
 * Created on June 10, 2018, 3:37 PM
 */

#include <cstring>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

#include <iostream>
#include <iomanip>

#include "FanController.hpp"

namespace colorsair {

    const std::array<unsigned char, 64> FanController::MAGIC_DATA_1 {0x33, 0xFF};
    const std::array<unsigned char, 64> FanController::MAGIC_DATA_2 {0x38, 0x01, 0x02};
    const std::array<unsigned char, 64> FanController::MAGIC_DATA_3 {0x34, 0x01};
    const std::array<unsigned char, 64> FanController::MAGIC_DATA_4 {0x38, 0x00, 0x02};
    const std::array<std::array<unsigned char, 64>, 5> FanController::MAGIC_DATA {
        FanController::MAGIC_DATA_1,
        FanController::MAGIC_DATA_2,
        FanController::MAGIC_DATA_3,
        FanController::MAGIC_DATA_1,
        FanController::MAGIC_DATA_4
    };
    std::array<unsigned char, 64> FanController::COLOR_DATA {0x32, 0x00, 0x00};

    std::ostream &operator<<(std::ostream& os, const std::array<unsigned char, 64>& list) {
        cout << "{ ";
        for(uint8_t val : list)
            os << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)val << ", ";
        os << "}";
        return os;
    }
    
    FanController::FanController(Device& dev, unsigned char fansCount, uint8_t framerate)
            : dev(dev), fansCount(fansCount), framerate(framerate),
              timestep(1000/framerate), frameCountTime(std::chrono::system_clock::now()) {
        effects.reserve(fansCount);
        for(int i = 0; i < fansCount; ++i)
            effects.push_back(nullptr);
    }

    FanController::~FanController() {
    }
    
    void FanController::loop() {
        dev.reset();

        COLOR_DATA[3] = (unsigned char) (fansCount * 4);

        for(;;) {
            frameStart = std::chrono::system_clock::now();
            for(int i = 0; i < effects.size(); ++i) {
                std::lock_guard<std::mutex> lock(stateMutex);
                if(effects[i])
                    effects[i]->tick();
            }

            // write constant boilerplate
            for(auto line : MAGIC_DATA) {
                WriteResult wr = dev.writeInterrupt(1, line);
                if(wr.result != 0 || wr.written != 64)
                    std::cout << "Write Error #" << wr.result << " | written " << wr.written << endl;
            }
            
            // write colors
            for(int channel = 0; channel < 3; channel++) {
                COLOR_DATA[4] = channel;
                for(int i = 0; i < fansCount * 4; i++) {
                    std::lock_guard<std::mutex> lock(stateMutex);
                    switch(channel) {
                        case 0:
                            COLOR_DATA[5 + i] = effects[i/4]->colors[i%4].r;
                            break;
                        case 1:
                            COLOR_DATA[5 + i] = effects[i/4]->colors[i%4].g;
                            break;
                        case 2:
                            COLOR_DATA[5 + i] = effects[i/4]->colors[i%4].b;
                            break;
                        default:;
                    }
                }
                // std::cout << "COLOR_DATA: " << COLOR_DATA << std::endl << std::endl;
                WriteResult wr = dev.writeInterrupt(1, COLOR_DATA);
                if(wr.result != 0 || wr.written != 64)
                    std::cout << "Write Error #" << wr.result << " | written " << wr.written << std::endl;
            }
            
            // Frames control
            unsigned int frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - frameStart).count();
            if(frameTime < timestep) {
                //std::cout << "Sleeping for " << timestep-frameTime << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(timestep-frameTime));
            }
            
            ++frames;
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - frameCountTime).count() > 1000) {
                frameCountTime = std::chrono::system_clock::now();
                //std::cout << "Framerate " << (int)frames << std::endl;
                frames = 0;
            }
        }
    }
    
    unsigned char FanController::getFansCount() {
        return fansCount;
    }
}