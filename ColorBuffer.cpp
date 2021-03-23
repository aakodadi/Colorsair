

#include "ColorBuffer.hpp"

namespace colorsair {

    ColorBuffer::ColorBuffer(unsigned char fansCount, unsigned char ledsPerFan) :
            fansCount(fansCount), ledsPerFan(ledsPerFan),
            rBuffer(fansCount, std::vector<unsigned char>(ledsPerFan, 0)),
            gBuffer(fansCount, std::vector<unsigned char>(ledsPerFan, 0)),
            bBuffer(fansCount, std::vector<unsigned char>(ledsPerFan, 0)) {
        
    }

    void ColorBuffer::rSet(unsigned char fan, unsigned char led, unsigned char value) {
        rBuffer[fan][led] = value;
    }

    void ColorBuffer::gSet(unsigned char fan, unsigned char led, unsigned char value) {
        gBuffer[fan][led] = value;
    }

    void ColorBuffer::bSet(unsigned char fan, unsigned char led, unsigned char value) {
        bBuffer[fan][led] = value;
    }
            
    void ColorBuffer::set(unsigned char fan, unsigned char led, uint32_t rgba) {
        unsigned char a = rgba & 0x000000FF;
        rSet(fan, led, ((rgba >> 24) * a) / 0xFF);
        gSet(fan, led, (((rgba >> 16) & 0x000000FF) * a) / 0xFF);
        bSet(fan, led, (((rgba >> 8) & 0x000000FF) * a) / 0xFF);
    }
            
    void ColorBuffer::fanSet(unsigned char fan, uint32_t rgba) {
        for (unsigned char led = 0; led < ledsPerFan; led++) {
            set(fan, led, rgba);
        }
    }
            
    void ColorBuffer::ledSet(unsigned char led, uint32_t rgba) {
        for (unsigned char fan = 0; fan < fansCount; fan++) {
            set(fan, led, rgba);
        }
    }

            
    unsigned char ColorBuffer::rGet(unsigned char fan, unsigned char led) {
        return rBuffer[fan][led];
    }

    unsigned char ColorBuffer::gGet(unsigned char fan, unsigned char led) {
        return gBuffer[fan][led];
    }

    unsigned char ColorBuffer::bGet(unsigned char fan, unsigned char led) {
        return bBuffer[fan][led];
    }
}