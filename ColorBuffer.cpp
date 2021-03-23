

#include "ColorBuffer.hpp"


#include <stdexcept>

namespace colorsair {

    ColorBuffer::ColorBuffer(uint8_t fansCount, uint8_t ledsPerFan) :
            fansCount(fansCount), ledsPerFan(ledsPerFan),
            rBuffer(fansCount, std::vector<uint8_t>(ledsPerFan, 0)),
            gBuffer(fansCount, std::vector<uint8_t>(ledsPerFan, 0)),
            bBuffer(fansCount, std::vector<uint8_t>(ledsPerFan, 0)) {
        if (fansCount * ledsPerFan > 59) {
            throw std::invalid_argument("fansCount * ledsPerFan should not exceed 59, found: " + std::to_string(fansCount * ledsPerFan));
        }
    }

    void ColorBuffer::rSet(uint8_t fan, uint8_t led, uint8_t value) {
        checkIndex(fan, led);
        rBuffer[fan][led] = value;
    }

    void ColorBuffer::gSet(uint8_t fan, uint8_t led, uint8_t value) {
        checkIndex(fan, led);
        gBuffer[fan][led] = value;
    }

    void ColorBuffer::bSet(uint8_t fan, uint8_t led, uint8_t value) {
        checkIndex(fan, led);
        bBuffer[fan][led] = value;
    }
            
    void ColorBuffer::set(uint8_t fan, uint8_t led, uint32_t rgba) {
        uint8_t a = rgba & 0x000000FF;
        rSet(fan, led, ((rgba >> 24) * a) / 0xFF);
        gSet(fan, led, (((rgba >> 16) & 0x000000FF) * a) / 0xFF);
        bSet(fan, led, (((rgba >> 8) & 0x000000FF) * a) / 0xFF);
    }
            
    void ColorBuffer::fanSet(uint8_t fan, uint32_t rgba) {
        for (uint8_t led = 0; led < ledsPerFan; led++) {
            set(fan, led, rgba);
        }
    }
            
    void ColorBuffer::ledSet(uint8_t led, uint32_t rgba) {
        for (uint8_t fan = 0; fan < fansCount; fan++) {
            set(fan, led, rgba);
        }
    }

            
    uint8_t ColorBuffer::rGet(uint8_t fan, uint8_t led) {
        checkIndex(fan, led);
        return rBuffer[fan][led];
    }

    uint8_t ColorBuffer::gGet(uint8_t fan, uint8_t led) {
        checkIndex(fan, led);
        return gBuffer[fan][led];
    }

    uint8_t ColorBuffer::bGet(uint8_t fan, uint8_t led) {
        checkIndex(fan, led);
        return bBuffer[fan][led];
    }

    void ColorBuffer::checkFanIndex(uint8_t index) {
        if (index >= fansCount) {
            throw std::out_of_range("Fan index out of bound, fansCount: " + std::to_string(fansCount) + ", found: " + std::to_string(index));
        }
    }

    void ColorBuffer::checkLedIndex(uint8_t index) {
        if (index >= ledsPerFan) {
            throw std::out_of_range("Fan index out of bound, ledsPerFan: " + std::to_string(ledsPerFan) + ", found: " + std::to_string(index));
        }
    }

    void ColorBuffer::checkIndex(uint8_t fan, uint8_t led) {
        checkFanIndex(fan);
        checkLedIndex(led);
    }
}