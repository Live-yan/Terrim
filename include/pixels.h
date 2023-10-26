/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#ifndef PIXELS_H
#define PIXELS_H

#include <stdint.h>
#include <tiles.h>

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} Color;


typedef struct {
    uint16_t id;
    uint8_t paint;
    uint8_t type;
    Color   color;
} CTile;




typedef enum {
    FIND_BLOCK_AND_WALL, // 通过颜色找到物块和墙壁
    FIND_BLOCK_ONLY,     // 只找物块
    FIND_WALL_ONLY       // 只找墙壁
} FindMode;






int init_colors_array( FindMode mode);
int pixels_to_tiles_by_bin(const unsigned char* image,Tile ***pixel_tiles,int pixel_width,int pixel_height,char* ctile_array_path,char *index_array_path);
Tile new_tile_from_ctile(CTile ctile);
void create_pixels_worlds(const char *world_name,const char *image_path,const char *save_path,const char *ctile_array_path,const char *index_array_path,int pos_x,int pos_y);
void copy_tiles(Tile **tiles,int width,int height, Tile **pixel_tiles,int pixel_width,int pixle_height, int x, int y);
int is_directory(const char *path);
#endif