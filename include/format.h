/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <Windows.h> // for CP_UTF8
#else
#include <locale.h> // for setlocale
#endif


typedef struct {
    uint32_t version;
    char magic[7];
    uint8_t type;
    uint32_t reversion;
    uint64_t favorited;
    uint16_t pointer_size;
    uint32_t *pointers;
    uint16_t frame_size;
    uint8_t *tileframeimportant;
} Format;


/**
 * @brief 打开文件并返回文件指针
 * 
 * @param file_path 文件路径
 * @param mode 打开文件的模式
 * @return FILE* 文件指针，如果打开失败则返回NULL
 */
FILE* topen(const char* file_path, const char* mode);


void parse_format(FILE *fp, Format *format);
void write_format(FILE *fp, Format *format);
void free_format(Format *format);




/**
 * @brief 从 Terraria 世界文件中读取指定数据段的数据。
 *
 * 该函数根据 `Format` 结构体中的 `pointers` 字段，从 Terraria 世界文件中读取指定数据段的数据到内存中。
 * `start_index` 参数指定要读取的数据段的起始索引，`end_index` 参数指定要读取的数据段的结束索引。
 * 函数会自动分配一个缓冲区，用于存储要读取的数据。
 * `length` 参数是一个指向存储读取数据长度的变量的指针，`format` 参数是一个指向包含文件中各个部分数据结束位置的指针数组的 `Format` 结构体的指针。
 *
 * @param fp 指向 Terraria 世界文件的文件指针。
 * @param format 指向包含文件中各个部分数据结束位置的指针数组的 `Format` 结构体的指针。
 * @param start_index 要读取的数据段的起始索引。
 * @param end_index 要读取的数据段的结束索引。
 * @param length 指向存储读取数据长度的变量的指针。
 *
 * @return 指向存储数据的缓冲区的指针，如果出错则返回 NULL。
 *         调用者负责释放返回的缓冲区。
 */
void* read_sections(FILE* fp, uint32_t start, uint32_t end, int* length);

#endif