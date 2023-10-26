/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
 #include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <format.h>



FILE* topen(const char* file_path, const char* mode) {
    #ifdef _WIN32
        // 将UTF-8编码的路径转换为宽字符
        wchar_t wpath[1024];
        MultiByteToWideChar(CP_UTF8, 0, file_path, -1, wpath, sizeof(wpath) / sizeof(wpath[0]));
        wchar_t wmode[10];
        MultiByteToWideChar(CP_UTF8, 0, mode, -1, wmode, sizeof(wmode) / sizeof(wmode[0]));
        return _wfopen(wpath, wmode);
    #else
        setlocale(LC_ALL, ""); // 设置地域设置为用户的默认环境
        return fopen(file_path, mode);
    #endif
}

void parse_format(FILE *fp, Format *format) {
    uint32_t i;
    fseek(fp, 0, SEEK_SET);
    // Read format structure
    fread(&format->version, sizeof(uint32_t), 1, fp);
    fread(&format->magic, sizeof(char), 7, fp);
    format->magic[7] = '\0';
    fread(&format->type, sizeof(uint8_t), 1, fp);
    fread(&format->reversion, sizeof(uint32_t), 1, fp);
    fread(&format->favorited, sizeof(uint64_t), 1, fp);
    fread(&format->pointer_size, sizeof(uint16_t), 1, fp);
    format->pointers = malloc(format->pointer_size * sizeof(uint32_t));
    for (i = 0; i < format->pointer_size; i++) {
        fread(&format->pointers[i], sizeof(uint32_t), 1, fp);
    }
    fread(&format->frame_size, sizeof(uint16_t), 1, fp);
    uint32_t byte_count = (format->frame_size + 7) / 8;
    uint8_t *tileframeimportant_bytes = malloc(byte_count);
    fread(tileframeimportant_bytes, sizeof(uint8_t), byte_count, fp);
    format->tileframeimportant = malloc(format->frame_size * sizeof(uint8_t));
    for (i = 0; i < format->frame_size; i++) {
        format->tileframeimportant[i] = (tileframeimportant_bytes[i / 8] >> (i % 8)) & 1;
    }

    // Clean up
    free(tileframeimportant_bytes);
}




void free_format(Format *format) {
    free(format->pointers);
    free(format->tileframeimportant);
}

void write_format(FILE *fp, Format *format) {
    uint32_t i;
    // Write format structure
    fwrite(&format->version, sizeof(uint32_t), 1, fp);
    fwrite(&format->magic, sizeof(char), 7, fp);
    fwrite(&format->type, sizeof(uint8_t), 1, fp);
    fwrite(&format->reversion, sizeof(uint32_t), 1, fp);
    fwrite(&format->favorited, sizeof(uint64_t), 1, fp);
    fwrite(&format->pointer_size, sizeof(uint16_t), 1, fp);
    for (i = 0; i < format->pointer_size; i++) {
        fwrite(&format->pointers[i], sizeof(uint32_t), 1, fp);
    }
    fwrite(&format->frame_size, sizeof(uint16_t), 1, fp);
    uint32_t byte_count = (format->frame_size + 7) / 8;
    uint8_t *tileframeimportant_bytes = malloc(byte_count);
    memset(tileframeimportant_bytes, 0, byte_count);
    for (i = 0; i < format->frame_size; i++) {
        if (format->tileframeimportant[i]) {
            tileframeimportant_bytes[i / 8] |= 1 << (i % 8);
        }
    }
    fwrite(tileframeimportant_bytes, sizeof(uint8_t), byte_count, fp);

    // Clean up
    free(tileframeimportant_bytes);
}

void* read_sections(FILE* fp, uint32_t start, uint32_t end, int* length) {
    uint32_t size = end - start;
    void* data = malloc(size);
    // 检查内存分配是否成功
    if (data == NULL) {
        fprintf(stderr, "内存分配失败。\n");
        return NULL;
    }
    // 尝试定位到文件的指定位置
    if (fseek(fp, start, SEEK_SET) != 0) {
        fprintf(stderr, "文件定位失败。\n");
        free(data);
        return NULL;
    }
    // 从文件中读取数据
    size_t bytes_read = fread(data, 1, size, fp);
    // 检查是否读取了预期数量的字节
    if (bytes_read < size) {
        // 如果未读取预期数量的字节，但已到达文件末尾，那么可能并无错误
        // 所以我们不释放内存，并将实际读取的字节数返回
        if (feof(fp)) {
            *length = bytes_read;
            return data;
        } else { // 否则，释放内存并返回错误
            fprintf(stderr, "文件读取失败。\n");
            free(data);
            return NULL;
        }
    }
    *length = bytes_read;
    return data;
}