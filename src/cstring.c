/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring.h>


Base128Varint parse_base128_varint_string_from_file(FILE *fp) {
    Base128Varint str;
    uint8_t group;
    uint32_t value = 0;
    uint32_t shift = 0;
    bool continuation = true;

    // 读取字节组，直到结束标志为 false
    while (continuation) {
        fread(&group, sizeof(uint8_t), 1, fp);
        continuation = group & 0x80;
        value |= (group & 0x7F) << shift;
        shift += 7;
    }

    // 为字符串值分配内存
    str.length = value;
    str.value = malloc(str.length + 1);

    // 读取字符串值
    fread(str.value, sizeof(char), str.length, fp);
    str.value[str.length] = '\0';

    return str;
}

int encode_base128_varint_string(FILE *fp, Base128Varint str) {

    // 参数校验
    if (fp == NULL || str.value == NULL) return -1;

    size_t length = str.length; // 使用size_t 更安全

    size_t group_count = 0; // 编码组的数量

    uint8_t buffer[128]; // 定义缓冲区

    uint8_t *buf_ptr = buffer; // 缓冲区当前写入位置

    // 编码长度
    for (size_t value = length; value > 0; value >>= 7) {
        uint8_t group = value & 0x7F;
        if (value > 0x7F) group |= 0x80; // 设置continuation位
        *buf_ptr++ = group;
        group_count++;
    }

    // 写入字符串值
    memcpy(buf_ptr, str.value, length);
    buf_ptr += length;

    // 写入文件
    size_t write_size = group_count + length;
    if (fwrite(buffer, sizeof(uint8_t), write_size, fp) != write_size) {
        return -2; // 写入失败
    }
    return 0;
}


