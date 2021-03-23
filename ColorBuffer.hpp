#ifndef COLOR_BUFFER_HPP
#define COLOR_BUFFER_HPP

#include <vector>
#include <cstdint>

namespace colorsair {

    class ColorBuffer {
        public:
            ColorBuffer(uint8_t fansCount, uint8_t ledsPerFan);

            void rSet(uint8_t fan, uint8_t led, uint8_t value);
            void gSet(uint8_t fan, uint8_t led, uint8_t value);
            void bSet(uint8_t fan, uint8_t led, uint8_t value);
            
            void set(uint8_t fan, uint8_t led, uint32_t rgba);
            
            void fanSet(uint8_t fan, uint32_t rgba);
            
            void ledSet(uint8_t led, uint32_t rgba);
            
            uint8_t rGet(uint8_t fan, uint8_t led);
            uint8_t gGet(uint8_t fan, uint8_t led);
            uint8_t bGet(uint8_t fan, uint8_t led);

        private:
            std::vector<std::vector<uint8_t>> rBuffer;
            std::vector<std::vector<uint8_t>> gBuffer;
            std::vector<std::vector<uint8_t>> bBuffer;
            uint8_t fansCount;
            uint8_t ledsPerFan;

            void checkFanIndex(uint8_t index);
            void checkLedIndex(uint8_t index);
            void checkIndex(uint8_t fan, uint8_t led);
    };

}


#endif /* COLOR_BUFFER_HPP */