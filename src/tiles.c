/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
// #include <stdbool.h> // 引入stdbool.h以使用bool类型
#include <libgen.h>
#include <tiles.h>
#include <header.h>

#define CHUNK_SIZE 16384



size_t parse_tile_data(uint8_t **buf_ptr, Tile *tile, const uint8_t *tile_frame_important) {

    if (buf_ptr == NULL || *buf_ptr == NULL || tile == NULL) {
        return 0;
    }
    size_t bytes_parsed = 0;
    tile->flag1 = **buf_ptr;
    (*buf_ptr)++;
    bytes_parsed++;

    if (tile->flag1 & 1) {
        tile->flag2 = **buf_ptr;
        (*buf_ptr)++;
        bytes_parsed++;
        if (tile->flag2 & 1) {
            tile->flag3 = **buf_ptr;
            (*buf_ptr)++;
            bytes_parsed++;
            if (tile->flag3 & 1) {
                tile->flag4 = **buf_ptr;
                (*buf_ptr)++;
                bytes_parsed++;
            } else {
                tile->flag4 = 0;
            }
        } else {
            tile->flag3 = 0;
            tile->flag4 = 0;
        }
    } else {
        tile->flag2 = 0;
        tile->flag3 = 0;
        tile->flag4 = 0;
    }

    if (tile->flag1 & 2) {
        if (tile->flag1 & 32) {
            memcpy(&(tile->tileType), *buf_ptr, sizeof(uint16_t));
            *buf_ptr += sizeof(uint16_t);
            bytes_parsed += sizeof(uint16_t);
        } else {
            tile->tileType = **buf_ptr;
            (*buf_ptr)++;
            bytes_parsed++;
        }
        if (tile_frame_important[tile->tileType] != 0) {
            memcpy(&(tile->multi), *buf_ptr, sizeof(uint16_t) * 2);
            *buf_ptr += sizeof(uint16_t) * 2;
            bytes_parsed += sizeof(uint16_t) * 2;
        } else {
            tile->multi[0] = 0;
            tile->multi[1] = 0;
        }

        if ((tile->flag1 & 2) && (tile->flag3 & 8)) {
            tile->tileTypeColor = **buf_ptr;
            (*buf_ptr)++;
            bytes_parsed++;
        } else {
            tile->tileTypeColor = 0;
        }

    } else {
        tile->tileType = 0;
        tile->multi[0] = 0;
        tile->multi[1] = 0;
        tile->tileTypeColor = 0;
    }


    if (tile->flag1 & 4) {
        tile->wall1 = **buf_ptr;
        (*buf_ptr)++;
        bytes_parsed++;
        if (tile->flag3 & 16) {
            tile->wallColor = **buf_ptr;
            (*buf_ptr)++;
            bytes_parsed++;
        } else {
            tile->wallColor = 0;
        }
    } else {
        tile->wall1 = 0;
        tile->wallColor = 0;
    }

    if ((tile->flag1 & 24) >> 3) {
        tile->liquid = **buf_ptr;
        (*buf_ptr)++;
        bytes_parsed++;
    } else {
        tile->liquid = 0;
    }

    if (tile->flag3 & 64) {
        tile->wall2 = **buf_ptr;
        (*buf_ptr)++;
        bytes_parsed++;
    } else {
        tile->wall2 = 0;
    }

    switch ((tile->flag1 & 192) >> 6) {
        case 1: {
            tile->rle = **buf_ptr;
            (*buf_ptr)++;
            bytes_parsed++;
            break;
        }
        case 2:
            memcpy(&(tile->rle), *buf_ptr, sizeof(uint16_t));
            *buf_ptr += sizeof(uint16_t);
            bytes_parsed += sizeof(uint16_t);
            break;
        default:
            tile->rle = 0;
            break;
    }

    tile->liquidType = (tile->flag1 & 24)  | (tile->flag3 & 128);
    tile->wallType = (tile->wall2 << 8) | tile->wall1;

    return bytes_parsed;
}



int parse_tiles_2D(Tile ***tiles, uint8_t *tile_frame_important, uint32_t *width, uint32_t *height, uint8_t *data) {
    // 1. 参数校验
    if (tiles == NULL || data == NULL) return 0;

    int total_rle = (*width) * (*height);
    int tile_count = 0;
    *tiles = malloc((*width) * sizeof(Tile *));
    for (int j = 0; j < *width; j++) {
        (*tiles)[j] = malloc((*height) * sizeof(Tile));
    }
    uint8_t *buf_ptr = data;
    int row = 0;
    int col = 0;
    while (tile_count < total_rle) {
        parse_tile_data(&buf_ptr, &((*tiles)[row][col]), tile_frame_important);
        int count = (*tiles)[row][col].rle + 1;
        (*tiles)[row][col].rle = 0;
        // 将压缩的方块展开
        for (int j = 1; j < count; j++) {
            (*tiles)[row][col+j] = (*tiles)[row][col];
        }
        col+=count;
        tile_count += count;
        // 如果够一列，到下一列，重置索引
        if (tile_count% *height == 0) {
            col = 0;
            row++;
        }
    }
    printf("tile_count: %d\n", tile_count);
    return 0;
}



uint8_t compare_tiles(Tile *tile1, Tile *tile2) {
    if (tile1 == NULL || tile2 == NULL) return 0;
    uint8_t tile1_flag1 = tile1->flag1;
    uint8_t tile2_flag1 = tile2->flag1;
    if ((tile1_flag1&63) != (tile2_flag1&63)) {
        return 0;
    }
    if (tile1->flag2 != tile2->flag2) {
        return 0;
    }
    if (tile1->flag3 != tile2->flag3) {
        return 0;
    }
    if (tile1->flag4 != tile2->flag4) {
        return 0;
    }
    if (tile1->tileType != tile2->tileType) {
        return 0;
    }
    if (tile1->multi[0] != tile2->multi[0]) {
        return 0;
    }
    if (tile1->multi[1] != tile2->multi[1]) {
        return 0;
    }
    if (tile1->tileTypeColor != tile2->tileTypeColor) {
        return 0;
    }

    if (tile1->wallColor != tile2->wallColor) {
        return 0;
    }
    if (tile1->wallType != tile2->wallType) {
        return 0;
    }
    if (tile1->wall1 != tile2->wall1) {
        return 0;
    }
    if (tile1->wall2 != tile2->wall2) {
        return 0;
    }
    if (tile1->liquid != tile2->liquid){
        return 0;
    }

    return 1;
}





void compute_tile_flags(Tile *tile, const uint8_t *tile_frame_important) {
    tile->flag1 = 0;
    tile->flag2 = 0;
    tile->flag3 = 0;
    tile->flag4 = 0;
    if (tile->tileType) {
        tile->flag1 |= 2;
        if (tile->tileType > 255) {
            tile->flag1 |= 32;
        }
        if (tile->tileTypeColor ) {
            tile->flag3 |= 8;
        }

        if (tile_frame_important[tile->tileType] ) {
            tile->flag1 |= 32;
        }
    }


    if (tile->wall1 || tile->wall2 ) {
        tile->flag1 |= 4;
        if (tile->wallColor) {
            tile->flag3 |= 16;
        }
        if (tile->wall2) {
            tile->flag3 |= 64;
        }
    }
    if (tile->liquid ) {
        tile->flag1 |= ((tile->liquidType & 3) << 3);
    }



    switch (tile->rle) {
        case 0:
            break;
        case 1:
            tile->flag1 |= (1 << 6);
            break;
        default:
            tile->flag1 |= (2 << 6);
            break;
    }

    tile->flag1 |= ((tile->liquidType & 12) << 6);
    tile->flag3 |= ((tile->liquidType & 128) << 1);
    if (tile->flag4) {
        tile->flag3 |= 1;
    }
    if (tile->flag3 || tile->flag4) {
        tile->flag2 |= 1;
    }
    if (tile->flag2 || tile->flag3 || tile->flag4) {
        tile->flag1 |= 1;
    }
}


size_t write_tiles_2D_data_to_buffer(Tile **tiles, uint32_t width, uint32_t height, uint8_t **buffer, Format *format) {
    if (tiles == NULL) {
        return 0;
    }
    size_t buffer_size = width * height * sizeof(Tile);
    *buffer = malloc(buffer_size);
    uint8_t *buffer_ptr = *buffer;
    size_t bytes_written = 0;
    // 按列进行 RLE 压缩
    for (int i = 0; i < width; i++) {
        Tile current_tile = tiles[i][0];
        int rle_count = 0;  // 初始化相同方块的数量为0

        for (int j = 1; j < height; j++) {  // 注意从索引1开始而不是0
            Tile next_tile = tiles[i][j];
            if (compare_tiles(&current_tile, &next_tile)) {
                rle_count++;  // 相同方块数量加1s
            } else {
                // 写入当前方块及其 RLE 数量
                current_tile.rle = rle_count;
                compute_tile_flags(&current_tile, format->tileframeimportant);
                write_tile_data_to_buffer(&current_tile, &buffer_ptr, &bytes_written, format);

                current_tile = next_tile;  // 更新当前方块
                rle_count = 0;  // 重置相同方块的数量为0
            }
        }

        // 处理最后一个方块的 RLE 数量
        current_tile.rle = rle_count;
        compute_tile_flags(&current_tile, format->tileframeimportant);
        write_tile_data_to_buffer(&current_tile, &buffer_ptr, &bytes_written, format);
    }
    *buffer = realloc(*buffer, bytes_written);
    int pointer_offset = bytes_written - (format->pointers[2] - format->pointers[1]);
    for (size_t i = 2; i <= 10; ++i) {
        format->pointers[i] += pointer_offset;
    }
    return bytes_written;
}
void write_tile_data_to_buffer(Tile *tile, uint8_t **buffer_ptr, size_t *bytes_written, Format *format) {
    *(*buffer_ptr) = tile->flag1;
    (*buffer_ptr)++;
    (*bytes_written)++;
    if (tile->flag1 & 1) {
        *(*buffer_ptr) = tile->flag2;
        (*buffer_ptr)++;
        (*bytes_written)++;
        if (tile->flag2 & 1) {
            *(*buffer_ptr) = tile->flag3;
            (*buffer_ptr)++;
            (*bytes_written)++;
            if (tile->flag3 & 1) {
                *(*buffer_ptr) = tile->flag4;
                (*buffer_ptr)++;
                (*bytes_written)++;
            }
        }
    }

    if (tile->flag1 & 2) {
        if (tile->flag1 & 32) {
            memcpy(*buffer_ptr, &(tile->tileType), sizeof(uint16_t));
            (*buffer_ptr) += sizeof(uint16_t);
            (*bytes_written) += sizeof(uint16_t);
        } else {
            *(*buffer_ptr) = tile->tileType;
            (*buffer_ptr)++;
            (*bytes_written)++;
        }
    }

    if (format->tileframeimportant[tile->tileType]) {
        memcpy(*buffer_ptr, &(tile->multi), sizeof(uint16_t)*2 );
        (*buffer_ptr) += sizeof(uint16_t)*2;
        (*bytes_written) += sizeof(uint16_t) * 2;
    }

    if ((tile->flag1 & 2) && (tile->flag3 & 8)) {
        *(*buffer_ptr) = tile->tileTypeColor;
        (*buffer_ptr)++;
        (*bytes_written)++;
    }

    if (tile->flag1 &4) {
        *(*buffer_ptr) = tile->wall1;
        (*buffer_ptr)++;
        (*bytes_written)++;
        if (tile->flag3 & 16) {
            *(*buffer_ptr) = tile->wallColor;
            (*buffer_ptr)++;
            (*bytes_written)++;
        }
    }

    if ((tile->flag1 & 24) >> 3) {
        *(*buffer_ptr) = tile->liquid;
        (*buffer_ptr)++;
        (*bytes_written)++;
    }

    if (tile->flag3 & 64) {
        *(*buffer_ptr) = tile->wall2;
        (*buffer_ptr)++;
        (*bytes_written)++;
    }

    switch ((tile->flag1 & 192) >> 6) {
        case 1: {
            *(*buffer_ptr) = tile->rle;
            (*buffer_ptr)++;
            (*bytes_written)++;
            break;
        }
        case 2:
            memcpy(*buffer_ptr, &(tile->rle), sizeof(uint16_t));
            (*buffer_ptr) += sizeof(uint16_t);
            (*bytes_written) += sizeof(uint16_t);
            break;
    }
}
void print_tile(Tile *tile) {
    printf("Tile:\n");
    printf("Flag1: %d\n", tile->flag1);
    printf("Flag2: %d\n", tile->flag2);
    printf("Flag3: %d\n", tile->flag3);
    printf("Flag4: %d\n", tile->flag4);
    printf("Tile Type: %d\n", tile->tileType);
    printf("Multi: %d %d\n ", tile->multi[0], tile->multi[0]);
    printf("Tile Type Color: %d\n", tile->tileTypeColor);
    printf("Wall 1: %d\n", tile->wall1);
    printf("Wall 2: %d\n", tile->wall2);
    printf("Wall Color: %d\n", tile->wallColor);
    printf("Liquid: %d\n", tile->liquid);
    printf("Liquid Type: %d\n", tile->liquidType);
    printf("Wall Type: %d\n", tile->wallType);
    // printf("Has Wall: %d\n", tile->hasWall);
    // printf("Has Tile: %d\n", tile->hasTile);
    // printf("Invisible Wall: %d\n", tile->invisibleWall);
    // printf("Invisible Block: %d\n", tile->invisibleBlock);
    printf("RLE: %d\n", tile->rle);

}

