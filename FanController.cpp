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

    const std::array<uint8_t, 64> FanController::MAGIC_FRAME_1 {0x33, 0xFF};
    const std::array<uint8_t, 64> FanController::MAGIC_FRAME_2 {0x38, 0x01, 0x02};
    const std::array<uint8_t, 64> FanController::MAGIC_FRAME_3 {0x34, 0x01};
    const std::array<uint8_t, 64> FanController::MAGIC_FRAME_4 {0x38, 0x00, 0x02};
    const std::array<std::array<uint8_t, 64>, 5> FanController::MAGIC_FRAME {
        FanController::MAGIC_FRAME_1,
        FanController::MAGIC_FRAME_2,
        FanController::MAGIC_FRAME_3,
        FanController::MAGIC_FRAME_1,
        FanController::MAGIC_FRAME_4
    };
    std::array<uint8_t, 64> FanController::COLOR_FRAME {0x32, 0x00, 0x00};

    std::ostream &operator<<(std::ostream& os, const std::array<uint8_t, 64>& list) {
        cout << "{ ";
        for(uint8_t val : list)
            os << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)val << ", ";
        os << "}";
        return os;
    }
    std::ostream &operator<<(std::ostream& os, const std::vector<uint8_t>& list) {
        cout << "{ ";
        for(uint8_t val : list)
            os << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)val << ", ";
        os << "}";
        return os;
    }
    
    FanController::FanController(Device& dev, uint8_t fansCount, uint8_t ledsPerFan, uint8_t framerate) :
            dev(dev), fansCount(fansCount), ledsPerFan(ledsPerFan), framerate(framerate),
            timestep(1000/framerate), frameCountTime(std::chrono::system_clock::now()),
            colorBuffer(fansCount, ledsPerFan) {
        effects.reserve(fansCount);
        for(int i = 0; i < fansCount; ++i)
            effects.push_back(nullptr);
    }

    FanController::~FanController() {
    }
    
    void FanController::loop() {
        dev.reset();

        const uint8_t ledsCount = fansCount * ledsPerFan;
        COLOR_FRAME[3] = ledsCount;

        // colorBuffer.rSet(0, 0, 0xFF);
        // colorBuffer.rSet(1, 0, 0xFF);
        // colorBuffer.rSet(2, 0, 0xFF);

        // colorBuffer.rSet(0, 1, 0xFF);
        // colorBuffer.rSet(1, 1, 0xFF);
        // colorBuffer.rSet(2, 1, 0xFF);

        // colorBuffer.set(0, 0, 0xFF000040);
        
        // colorBuffer.fanSet(0, 0xFF000040);

        colorBuffer.ledSet(0, 0xFF000040);


        for(;;) {
            frameStart = std::chrono::system_clock::now();
            for(int i = 0; i < effects.size(); ++i) {
                std::lock_guard<std::mutex> lock(stateMutex);
                if(effects[i])
                    effects[i]->tick();
            }

            // write constant boilerplate
            for(auto line : MAGIC_FRAME) {
                WriteResult wr = dev.writeInterrupt(1, line);
                if(wr.result != 0 || wr.written != 64)
                    std::cout << "Write Error #" << wr.result << " | written " << wr.written << endl;
            }
            
            // write colors
            for(int channel = 0; channel < 3; channel++) {
                COLOR_FRAME[4] = channel;
                for (int fan = 0; fan < fansCount; fan++) {
                    for (int led = 0; led < ledsPerFan; led++){
                        switch(channel) {
                            case 0:
                        COLOR_FRAME[fan * ledsPerFan + led + 5] = colorBuffer.rGet(fan, led);
                                break;
                            case 1:
                        COLOR_FRAME[fan * ledsPerFan + led + 5] = colorBuffer.gGet(fan, led);
                                break;
                            case 2:
                        COLOR_FRAME[fan * ledsPerFan + led + 5] = colorBuffer.bGet(fan, led);
                                break;
                            default:;
                        }
                    }
                }
                
/*                 for(int i = 0; i < fansCount * ledsPerFan; i++) {
                    std::lock_guard<std::mutex> lock(stateMutex);
                    switch(channel) {
                        case 0:
                            COLOR_FRAME[5 + i] = effects[i/ledsPerFan]->colors[i%ledsPerFan].r;
                            break;
                        case 1:
                            COLOR_FRAME[5 + i] = effects[i/ledsPerFan]->colors[i%ledsPerFan].g;
                            break;
                        case 2:
                            COLOR_FRAME[5 + i] = effects[i/ledsPerFan]->colors[i%ledsPerFan].b;
                            break;
                        default:;
                    }
                } */
                std::cout << "COLOR_FRAME: " << COLOR_FRAME << std::endl << std::endl;
                WriteResult wr = dev.writeInterrupt(1, COLOR_FRAME);
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
    
    uint8_t FanController::getFansCount() {
        return fansCount;
    }
}