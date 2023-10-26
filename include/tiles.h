/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#ifndef TILES_H
#define TILES_H

#include <stdint.h>
#include <format.h>


typedef enum {
    WORLD_REPLACEMENT_PURIFICATION,
    WORLD_REPLACEMENT_CRIMSON,
    WORLD_REPLACEMENT_CORRUPTION,
    WORLD_REPLACEMENT_HALLOW,
} WorldReplacement;

typedef struct {
    uint8_t flag1;
    uint8_t flag2;
    uint8_t flag3;
    uint8_t flag4;
    uint16_t tileType;
    uint16_t multi[2];
    uint8_t tileTypeColor;
    uint8_t wall1;
    uint8_t wallColor;
    uint8_t liquid;
    uint8_t wall2;
    uint16_t wallType;
    uint16_t rle;
    uint8_t  liquidType;
} Tile;



/**
 * 解析tile数据
 * 
 * @param tile_data tile数据指针
 * @param tile 要填充的tile结构  
 * @param tile_frame_important tile信息bitmap
 * @return 成功返回解析的字节数,失败返回0
*/
size_t parse_tile_data(uint8_t **buf_ptr, Tile *tile, const uint8_t *tileframeimportant);




/**
 * 比较两个tile是否相同
 *
 * @param tile1 第一个tile 
 * @param tile2 第二个tile
 * @return 相同返回1,不同返回0
*/
uint8_t compare_tiles(Tile *tile1, Tile *tile2);




/**
 * 计算并设置 Tile 结构中的四个 flag 字段的值。
 * 
 * 这个函数会根据 Tile 结构中除了 flag 字段之外的其他字段的值，来计算四个 flag 字段的值。
 * 函数假设 Tile 结构中的其他字段已经被正确地设置，并且不会修改它们。
 * 
 * @param tile 指向要计算 flag 的 Tile 结构的指针。
 * @param tile_frame_important 指向一个数组的指针，数组的索引是 tile 类型，值是一个布尔值，
 *                             指示这种类型的 tile 是否重要。
 * 
 * @note 这个函数只设置 flag 字段，不会修改 Tile 结构中的其他字段。
 */
void compute_tile_flags(Tile *tile, const uint8_t *tile_frame_important);


void print_tile(Tile *tile);

/**
 * @brief 解析二维瓦片数据，并将结果存储在指定的 Tile 数组中
 *
 * @param[out] tiles 二维 Tile 数组的指针，用于存储解析结果
 * @param[out] tiles_index 一维 int 数组的指针，用于存储每一行 Tile 数组的长度
 * @param[in] tile_frame_important 用于指示每个 Tile 是否为关键帧的 uint8_t 数组
 * @param[in] width 瓦片图像的宽度
 * @param[in] height 瓦片图像的高度
 * @param[in] data 包含瓦片数据的 uint8_t 数组
 * @param[in] data_size 瓦片数据的字节数
 *
 * @return 成功返回 0，失败返回非零值
 */
int parse_tiles_2D(Tile ***tiles, uint8_t *tile_frame_important, uint32_t *width, uint32_t *height, uint8_t *data);
size_t write_tiles_2D_data_to_buffer(Tile **tiles, uint32_t width, uint32_t height, uint8_t **buffer, Format *format);
void write_tile_data_to_buffer(Tile *tile, uint8_t **buffer_ptr, size_t *bytes_written, Format *format);


#endif /* TILES_H */