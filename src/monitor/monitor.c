#include "monitor.h"
#include "io.h"

// 显存的物理地址为 0xB8000，这里需要的是虚拟地址，已经映射到 3GB-4GB 的空间内（内核空间）
uint16_t* video_memory = (uint16_t*) 0xC00B8000;
// 光标位置
int16_t cursor_x = 0;
int16_t cursor_y = 0;

// 显示颜色（16进制转换为10进制表示）
const uint8_t backColour = COLOR_BLACK;
const uint8_t foreColour = COLOR_WHITE;

static void move_cursor_position() {
    // 一行最多显示 80 个字符。
    uint16_t cursorLocation = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);
    outb(0x3D5, cursorLocation >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursorLocation);
}

void move_cursor(int32_t delta_x, int32_t delta_y) {
    cursor_x += delta_x;
    int32_t y_offset = div(cursor_x, 80);
    cursor_x = mod(cursor_x, 80);
    cursor_y += (delta_y + y_offset);
    if (cursor_y < 0) {
        cursor_x = 0;
        cursor_y = 0;
    }
    if (cursor_y >= 25) {
        cursor_y = 24;
        cursor_x = 79;
    }
    move_cursor_position();
}

static void scroll() {
    const uint8_t attributeByte = ((backColour << 4) | (foreColour & 0x0F));
    const uint16_t blank = (0x20 | (attributeByte << 8));
    int i;
    for (i = 0; i < 80 * 25; i++) {
        video_memory[i] = blank;
    }
    cursor_x = 0;
    cursor_y = 0;
    move_cursor_position();
}

void monitor_clear() {

    // The attribute byte is made up of two nibbles - the lower being the
    // foreground colour, and the upper the background colour.
    const uint8_t attributeByte = ((backColour << 4) | (foreColour & 0x0F));
    // blank character (black backgroud with nothing)
    const uint16_t blank = (0x20 /* space */ | (attributeByte << 8));

    int i;
    for (i = 0; i < 80 * 25; i++) {
        video_memory[i] = blank;
    }

    // Move the hardware cursor back to the start.
    cursor_x = 0;
    cursor_y = 0;
    move_cursor_position();
}

void write_char_with_color(char c, uint8_t color) {
    const uint8_t attributeByte = ((backColour << 4) | (color & 0x0F));
    uint16_t attribute = attributeByte << 8;
    uint16_t* location;
    if (c == 0x08 && cursor_x) {
        cursor_x--;
    } else if (c == 0x09) {
        cursor_x = (cursor_x + 8) & ~(8 - 1);
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c >= ' ') {
        // Handle any other printable character.
        location = video_memory + (cursor_y * 80 + cursor_x);
        *location = (c | attribute);
        cursor_x++;
    }

    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    scroll();
    move_cursor_position();
}

void write_string_with_color(char* c, uint8_t color) {
    int i = 0;
    while (c[i]) {
        write_char_with_color(c[i++], color);
    }
}

void print(char* str) {
    write_string_with_color(str, COLOR_WHITE);
}