#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <cstdint>
//#include <string>
//#include <string_view>

#include "pico/stdlib.h"
//#include  "hardware/vreg.h"

#include "hub75.hpp"
#include "typewriter_font.hpp"

// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 64 pixel high displays (32*2).
const uint8_t WIDTH = 128;
const uint8_t HEIGHT = 64;

Hub75 hub75(WIDTH, HEIGHT, nullptr, PANEL_GENERIC, true);

void __isr dma_complete() {
    hub75.dma_complete();
}

inline uint32_t millis() {
    return to_ms_since_boot(get_absolute_time());
}

Pixel alpha_blend(unsigned char alpha, const Pixel& foreground, const Pixel& background) {
    if (alpha == 0) {
        return background;
    } else if (alpha == 255) {
        return foreground;
    }

    float a = (float)alpha / 255.f;
    float r = a * (float)( foreground.color      & 0x3ff) / 1023.f + (1.f - a)*(float)( background.color      & 0x3ff) / 1023.f;
    float g = a * (float)((foreground.color>>10) & 0x3ff) / 1023.f + (1.f - a)*(float)((background.color>>10) & 0x3ff) / 1023.f;
    float b = a * (float)((foreground.color>>20) & 0x3ff) / 1023.f + (1.f - a)*(float)((background.color>>20) & 0x3ff) / 1023.f;

    Pixel result = {};
    result.color = (unsigned int)(r * 1023.f) | (unsigned int)(g * 1023.f)<<10 | (unsigned int)(b * 1023.f) << 20;
    return result;
}

int main() {
    stdio_init_all();

    //vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_us(100);
    set_sys_clock_khz(266000, true);

    hub75.start(dma_complete);

    int frame = 0;
    const int num_frame = 50000;
    absolute_time_t prev_time = {};

    // Basic loop to draw something to the screen.
    // This gets the distance from the middle of the display and uses it to paint a circular colour cycle.
    while (true) {
        if (frame == 0) {
            absolute_time_t curr_time = get_absolute_time();
            int64_t delta = absolute_time_diff_us(prev_time, curr_time);
            prev_time = curr_time;
            double sec = (double)delta/1.e6;
            printf("Drew %d frames in %lf seconds (%lf)\n",
                   num_frame, sec, (double)num_frame / sec);
            frame = num_frame;
        }
        frame--;

        hub75.background = hsv_to_rgb(millis() / 10000.0f, 1.0f, 0.5f);
        Pixel foreground = Pixel(255,255,255);

        int x_off = 1;
        int y_off = 14;
        for (int d=4; d>=0; d--) {
            int digit = (frame/(int)pow(10,d)) % 10;
            unsigned int width = 0;
            unsigned int height = 0;
            const unsigned char* pixels;
            switch (digit) {
            case 0:
                width = zero::width;
                height = zero::height;
                pixels = &zero::pixels[0][0];
                break;
            case 1:
                width = one::width;
                height = one::height;
                pixels = &one::pixels[0][0];
                break;
            case 2:
                width = two::width;
                height = two::height;
                pixels = &two::pixels[0][0];
                break;
            case 3:
                width = three::width;
                height = three::height;
                pixels = &three::pixels[0][0];
                break;
            case 4:
                width = four::width;
                height = four::height;
                pixels = &four::pixels[0][0];
                break;
            case 5:
                width = five::width;
                height = five::height;
                pixels = &five::pixels[0][0];
                break;
            case 6:
                width = six::width;
                height = six::height;
                pixels = &six::pixels[0][0];
                break;
            case 7:
                width = seven::width;
                height = seven::height;
                pixels = &seven::pixels[0][0];
                break;
            case 8:
                width = eight::width;
                height = eight::height;
                pixels = &eight::pixels[0][0];
                break;
            case 9:
                width = nine::width;
                height = nine::height;
                pixels = &nine::pixels[0][0];
                break;
            default:
                break;
            }

            for (uint y=0; y<std::min((unsigned int)HEIGHT, height); y++) {
                for (uint x=0; x<width && (x+x_off)<WIDTH; x++) {
                    unsigned char alpha = pixels[y*width+x];
                    hub75.set_color(x + x_off, y + y_off, alpha_blend(alpha, foreground, hub75.background));
                }
            }
            x_off += width;

        }


        hub75.flip(false); // Flip and clear to the background colour
        sleep_ms(100 / 60);
    }
}
