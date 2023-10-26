/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#ifndef CSTRING_H
#define CSTRING_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t length;
    char *value;
} Base128Varint;

/**
 * @brief 从文件中解析出一个 Base128Varint 字符串
 * 
 * @param fp 文件指针，指向要解析的文件
 * 
 * @return Base128Varint 结构体，包含解析出的字符串
 */
Base128Varint parse_base128_varint_string_from_file(FILE *fp);




/**
 * @brief 编码Base128Varint字符串
 * 
 * @param fp 文件指针
 * @param str 要编码的字符串
 * @return int 成功返回0,失败返回非0值
*/
int encode_base128_varint_string(FILE *fp, Base128Varint str);

#endif /* CSTRING_H */