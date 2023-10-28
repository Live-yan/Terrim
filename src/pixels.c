/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#include <stdint.h>
#include <stdio.h>
#include <tiles.h>
#include <pixels.h>
#include <format.h>
#include <header.h>
#include <lodepng.h>
#include <sys/stat.h>

Tile new_tile_from_ctile(CTile ctile) {
    Tile tile = {0};
    if (ctile.type){
        tile.tileType = ctile.id;
        tile.flag1 |= 2;
        if (ctile.id > 255) {
            tile.flag1 |= 32;
        }
        if (ctile.paint != 0) {
            tile.flag1 |= 1;
            tile.flag2 |= 1;
            tile.flag3 |= 8;
            tile.tileTypeColor = ctile.paint;

        }

    }else{
        tile.flag1 |= 4;
        tile.wall1 = ctile.id & 0xFF;
        tile.wall2 = (ctile.id >> 8) & 0xFF;
        if (ctile.paint != 0) {
            tile.flag1 |= 1;
            tile.flag2 |= 1;
            tile.flag3 |= 16;
            tile.wallColor = ctile.paint;
        }
        if (ctile.id > 255) {
            tile.flag1 |= 1;
            tile.flag2 |= 1;
            tile.flag3 |= 64;
        }
    }
    return tile;
}





void rgb_to_hex(int r, int g, int b, char *hex) {
    sprintf(hex, "&h00%02X%02X%02X", r, g, b);
}



int pixels_to_tiles_by_bin(const unsigned char* image,Tile ***pixel_tiles,int pixel_width,int pixel_height,char* ctile_array_path,char *index_array_path){
    int channels = 4; // RGBA
    *pixel_tiles = malloc(pixel_width * sizeof(Tile *));
    for (int i = 0; i < pixel_width; i++) {
        (*pixel_tiles)[i] = malloc(pixel_height * sizeof(Tile));
    }

    uint16_t ctile_array_size;
    // 从文件中读取ctile_array
    FILE *fp = topen(ctile_array_path, "rb");
    if (fp == NULL) {
        printf("Failed to open file\n");
        return 1;
    }
    fread(&ctile_array_size, sizeof(uint16_t), 1, fp);
    CTile *ctile_array = malloc(ctile_array_size*sizeof(CTile));;
    fread(ctile_array, sizeof(CTile), ctile_array_size, fp);
    fclose(fp);

    // 从文件中读取Ctile_And_Index
    fp = topen(index_array_path, "rb");
    if (fp == NULL) {
        printf("Failed to open file\n");
        return 1;
    }
    // 分配内存
    uint16_t ***ctile_index;
    ctile_index = malloc(256 * sizeof(uint16_t **));
    for (int i = 0; i < 256; i++) {
        ctile_index[i] = malloc(256 * sizeof(uint16_t *));
        for (int j = 0; j < 256; j++) {
            ctile_index[i][j] = malloc(256 * sizeof(uint16_t));
        }
    }

    // 读取数据
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            fread(ctile_index[i][j], sizeof(uint16_t), 256, fp);
        }
    }

    // 关闭文件
    fclose(fp);

    for (int x = 0; x < pixel_width; x++) { // 先遍历列
        for (int y = 0; y < pixel_height; y++) { // 再遍历行
            int i = (y * pixel_width + x) * channels;
            uint16_t index =  ctile_index[image[i]][image[i+1]][image[i+2]];
            (*pixel_tiles)[x][y] = new_tile_from_ctile(ctile_array[index]);
        }

    }
    free(ctile_array);
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            free(ctile_index[i][j]);
        }
        free(ctile_index[i]);
    }
    free(ctile_index);
    return 0;
}

void create_pixels_worlds(const char *world_name,const char *image_path,const char *save_path,const char *ctile_array_path,const char *index_array_path,int pos_x,int pos_y){

    FILE *fp = topen(world_name, "rb");
    if (fp == NULL) {
        printf("无法打开文件%s\n",world_name);
        return ;
    }
    Format format;
    Header header;
    void *tiles_data;
    int data_length;

    parse_format(fp,&format);
    parse_header(fp, &header, &format.version,format.pointers[0],format.pointers[1]);
    // 读取瓦片数据
    tiles_data = read_sections(fp, format.pointers[1], format.pointers[2], &data_length);
    if (tiles_data == NULL) {
        printf("无法读取瓦片数据\n");
        free_format(&format);
        free_header(&header);
        return ;
    }
    // print_header(&header);
    // 读取不需要修改的数据
    void *other_data;
    int other_data_length;
    other_data = read_sections(fp, format.pointers[2], format.pointers[10], &other_data_length);
    if (other_data == NULL) {
        printf("无法读取其他数据\n");
        free_format(&format);
        free_header(&header);
        free(tiles_data);
        return ;
    }
    fclose(fp);

    Tile **tiles;
    parse_tiles_2D(&tiles,format.tileframeimportant,(uint32_t *)&header.Width, (uint32_t *)&header.Height,tiles_data);

    Tile **pixel_tiles ;
    int pixel_width;
    int pixel_height;

    unsigned char* image = NULL;

    // 加载图片
    unsigned error = lodepng_decode32_file(&image, &pixel_width, &pixel_height, image_path);
    if (error) {
        printf("Error %u: %s\n", error, lodepng_error_text(error));
        free_format(&format);
        free_header(&header);
        free(tiles_data);
        free(other_data);
        for (int i = 0;i<header.Width;i++){
            free(tiles[i]);
        }

        return ;
    }

    // 如果比地图还大，直接结束
    if (pixel_width > header.Width || pixel_height > header.Height){
        free_format(&format);
        free_header(&header);
        free(tiles_data);
        free(other_data);
        for (int i = 0;i<header.Width;i++){
            free(tiles[i]);
        }
        free(image);
        return ;

    }

    int res = pixels_to_tiles_by_bin(image,&pixel_tiles,pixel_width,pixel_height,ctile_array_path,index_array_path);
    if (res !=0){
        printf("图片太大了，超出了地图大小  %d\n",res);
        free_format(&format);
        free_header(&header);
        free(tiles_data);
        free(other_data);
        for (int i = 0;i<header.Width;i++){
            free(tiles[i]);
        }
        free(image);
        return ;
    }


    copy_tiles(tiles,header.Width,header.Height,pixel_tiles,pixel_width,pixel_height,pos_x,pos_y);

    uint8_t *new_tiles_data;
    size_t new_tiles_lenght =  write_tiles_2D_data_to_buffer(tiles, header.Width,header.Height,&new_tiles_data,&format);


    char full_path[256];
    if (is_directory(save_path)) {
        // 如果是目录，将"Terrim.wld"添加到路径后面
        snprintf(full_path, sizeof(full_path), "%s\\Terrim.wld", save_path);
    } else {
        printf("不合法的目录输入：%s",save_path);
        exit(-1);
    }
    full_path[sizeof(full_path) - 1] = '\0';
    FILE *new_fp;
    new_fp = topen(full_path, "wb");
    if (new_fp == NULL) {
        fprintf(stderr, "报错文件的时候出错: %s\n", full_path);
        free_format(&format);
        free_header(&header);
        free(tiles_data);
        free(other_data);
        for (int i = 0;i<header.Width;i++){
            free(tiles[i]);
        }
        free(image);
        free(new_tiles_data);
        for (int i = 0; i < pixel_width; i++) {
            free(pixel_tiles[i]);
        }
        free(pixel_tiles);
        return ;
    }

    write_format(new_fp, &format);
    write_header(new_fp, &header,&format);
    fwrite(new_tiles_data, new_tiles_lenght, 1, new_fp);
    fwrite(other_data, other_data_length, 1, new_fp);
    // write footer
    uint8_t footer_flag = 1;
    fwrite(&footer_flag,sizeof(uint8_t),1,new_fp);
    encode_base128_varint_string(new_fp, header.Name);
    fwrite(&header.WorldId,sizeof(uint32_t),1,new_fp);
    fclose(new_fp);

    free(new_tiles_data);
    for (int i = 0; i < pixel_width; i++) {
        free(pixel_tiles[i]);
    }
    free(pixel_tiles);

    for (int i = 0;i<header.Width;i++){
        free(tiles[i]);
    }
    free(tiles);
    free(tiles_data);
    free(other_data);
    free_format(&format);
    free_header(&header);
    free(image);

}


void copy_tiles(Tile **tiles,int width,int height, Tile **pixel_tiles,int pixel_width,int pixle_height, int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    for (int i = 0; i < pixle_height; i++) {
        for (int j = 0; j < pixel_width; j++) {
            tiles[x + j][y + i] = pixel_tiles[j][i];
        }
    }
}

int is_directory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}