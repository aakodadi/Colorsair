#ifndef COLOR_BUFFER_HPP
#define COLOR_BUFFER_HPP

#include <vector>
#include <cstdint>

namespace colorsair {

    class ColorBuffer {
        public:
            ColorBuffer(unsigned char fansCount, unsigned char ledsPerFan);

            void rSet(unsigned char fan, unsigned char led, unsigned char value);
            void gSet(unsigned char fan, unsigned char led, unsigned char value);
            void bSet(unsigned char fan, unsigned char led, unsigned char value);
            
            void set(unsigned char fan, unsigned char led, uint32_t rgba);
            
            void fanSet(unsigned char fan, uint32_t rgba);
            
            void ledSet(unsigned char led, uint32_t rgba);
            
            unsigned char rGet(unsigned char fan, unsigned char led);
            unsigned char gGet(unsigned char fan, unsigned char led);
            unsigned char bGet(unsigned char fan, unsigned char led);

        private:
            std::vector<std::vector<unsigned char>> rBuffer;
            std::vector<std::vector<unsigned char>> gBuffer;
            std::vector<std::vector<unsigned char>> bBuffer;
            unsigned char fansCount;
            unsigned char ledsPerFan;

            void checkFanIndex(unsigned char index);
            void checkLedIndex(unsigned char index);
            void checkIndex(unsigned char fan, unsigned char led);
    };

}


#endif /* COLOR_BUFFER_HPP */