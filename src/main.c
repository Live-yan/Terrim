/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */

#include <stdio.h>
#include <stdlib.h>
#include <pixels.h>



int main(int argc, char *argv[]) {
    if (argc != 8) {
        printf("Usage: %s <wld_file> <png_file> <save_dir> <ctile_array_file> <ctile_index_file> <pixel_pos_x> <pixel_pos_y>\n", argv[0]);
        printf("Example: %s ../world/123.wld ../images/1698206295782.png ../output ../files/ctile_array_tile.bin ../files/ctile_index_tile.bin 100 100",argv[0]);
        return 1;
    }

    const char* wld_file = argv[1];
    const char* png_file = argv[2];
    const char* files_dir = argv[3];
    const char* ctile_array_file = argv[4];
    const char* ctile_index_file = argv[5];
    int pixel_pos_x = atoi(argv[6]);
    int pixel_pos_y = atoi(argv[7]);

    create_pixels_worlds(wld_file, png_file, files_dir, ctile_array_file, ctile_index_file, pixel_pos_x, pixel_pos_y);

    return 0;
}