/**
 * Author: YanHua
 * Contact: 2324281453@qq.com
 * Build Tool: CMake
 */
#ifdef _WIN32
// Windows 平台
#pragma comment(lib, "rpcrt4.lib")
#else
// 非 Windows 平台
#include <uuid/uuid.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <format.h>
#include <header.h>
#include <cstring.h>

void parse_header(FILE *fp, Header *header,uint32_t *version, uint32_t format_end_position, uint32_t header_end_position) {
    // 读取 Name 字段
    fseek(fp, format_end_position, SEEK_SET);
    
    Base128Varint str = parse_base128_varint_string_from_file(fp);
    header->Name = str;

      // 读取 Seed 字段
    if (*version >= 179) {
        if (*version == 179) {
            int32_t seed_int;
            fread(&seed_int, sizeof(int32_t), 1, fp);
            char seed_str[12];
            Base128Varint seed;
            sprintf(seed_str, "%d", seed_int);
            seed.length = strlen(seed_str);
            seed.value = seed_str;
            header->Seed = seed;
        } else {
            Base128Varint seed = parse_base128_varint_string_from_file(fp);
            header->Seed = seed;
        }
        // 读取 Genversion 字段

        fread(&header->Genversion, sizeof(uint64_t), 1, fp);
    }

    if (*version >= 181) {
        uint8_t uuid_char[16];
        // 读取 UUID 字段
        fread(uuid_char, sizeof(uint8_t), 16, fp);
        // 将 UUID 转换成字符串形式
        char uuid_str[37];
        sprintf(uuid_str, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid_char[3], uuid_char[2], uuid_char[1], uuid_char[0],
            uuid_char[5], uuid_char[4], uuid_char[7], uuid_char[6],
            uuid_char[8], uuid_char[9], uuid_char[10], uuid_char[11],
            uuid_char[12], uuid_char[13], uuid_char[14], uuid_char[15]);
        strcpy(header->UUID, uuid_str);
    } else {
        // 生成 UUID
        // uuid_t uuid;
        // #ifdef _WIN32
        // UuidCreate(&uuid);
        // #else
        // uuid_generate(uuid);
        // #endif
        // char uuid_str[37];
        // #ifdef _WIN32
        // RPC_CSTR str;
        // UuidToStringA(&uuid, &str);
        // strcpy(uuid_str, (char*)str);
        // RpcStringFreeA(&str);
        // #else
        // uuid_unparse(uuid, uuid_str);
        // #endif
        srand(time(NULL));

        // 生成随机的 UUID
        char uuid_str[37];
        sprintf(uuid_str, "%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
            rand() % 0xffff, rand() % 0xffff, rand() % 0xffff,
            (rand() % 0x0fff) | 0x4000, rand() % 0x3fff + 0x8000,
            rand() % 0xffff, rand() % 0xffff, rand() % 0xffff);


        // 将 UUID 写入 header 结构体
        strcpy(header->UUID, uuid_str);
    }

    // 读取 WorldId 字段
    fread(&header->WorldId, sizeof(uint32_t), 1, fp);

    // 读取 Rect 字段
    fread(header->Rect, sizeof(uint32_t), 4, fp);

    // 读取 Height 字段
    fread(&header->Height, sizeof(uint32_t), 1, fp);

    // 读取 Width 字段
    fread(&header->Width, sizeof(uint32_t), 1, fp);

    if (*version >= 209){
        // 读取 Gamemode 字段
        fread(&header->Gamemode, sizeof(uint32_t), 1, fp);
        
        if (*version >= 222){
            // 读取 Drunkworld 字段
            fread(&header->Drunkworld, sizeof(uint8_t), 1, fp);
        }
        if (*version >= 227){
            // 读取 Goodworld 字段
            fread(&header->Goodworld, sizeof(uint8_t), 1, fp);
        }
        if (*version >= 238){
            // 读取 Tenthanniversary 字段
            fread(&header->Tenthanniversary, sizeof(uint8_t), 1, fp);
        }
        if (*version >= 239){
            // 读取 Dontstarve 字段
            fread(&header->Dontstarve, sizeof(uint8_t), 1, fp);
        }
        if (*version >= 241){
            // 读取 Notthebees 字段
            fread(&header->Notthebees, sizeof(uint8_t), 1, fp);
        }
        if (*version >= 249){
            // 读取 Remixworld 字段
            fread(&header->Remixworld, sizeof(uint8_t), 1, fp);
        }
        if (*version >= 266){
            // 读取 Notrapsworld 字段
            fread(&header->Notrapsworld, sizeof(uint8_t), 1, fp);
        }
        if (*version >= 267){
            // 读取 Zenithworld 字段
            fread(&header->Zenithworld, sizeof(uint8_t), 1, fp);
        }else{
            header->Zenithworld = header->Remixworld & header->Drunkworld;
        }

    }else if (*version == 208){
        uint8_t b;
        fread(&b, sizeof(uint8_t), 1, fp);
        header->Gamemode = (b == 0) ? 2 : 0;
    }else if (*version >=112){
        uint8_t b;
        fread(&b, sizeof(uint8_t), 1, fp);
        header->Gamemode = (b == 0) ? 1 : 0;
    }

    if (*version >= 141){
        // 读取 Creationtime 字段
        fread(&header->Creationtime, sizeof(int64_t), 1, fp);
    }else{
        // Set Creationtime to current time
        time_t now = time(NULL);
        int64_t creation_time = (int64_t)now;
        header->Creationtime = creation_time;
    }

    // 读取 Moontype 字段
    fread(&header->Moontype, sizeof(uint8_t), 1, fp);

    // 读取 Treex 字段
    fread(header->Treex, sizeof(uint32_t), 3, fp);

    // 读取 TreeStyle 字段
    fread(header->TreeStyle, sizeof(uint32_t), 4, fp);

    // 读取 Caveback 字段
    fread(header->Caveback, sizeof(uint32_t), 3, fp);

    // 读取 Cavebackstyle 字段
    fread(header->Cavebackstyle, sizeof(uint32_t), 4, fp);

    // 读取 Icebackstyle 字段
    fread(&header->Icebackstyle, sizeof(uint32_t), 1, fp);

    // 读取 Junglebackstyle 字段
    fread(&header->Junglebackstyle, sizeof(uint32_t), 1, fp);

    // 读取 Hellbackstyle 字段
    fread(&header->Hellbackstyle, sizeof(uint32_t), 1, fp);

    // 读取 SpawnTileX 字段
    fread(&header->SpawnTileX, sizeof(int32_t), 1, fp);

    // 读取 SpawnTileY 字段
    fread(&header->SpawnTileY, sizeof(int32_t), 1, fp);
    
    // 读取 Worldsurface 字段
    fread(&header->Worldsurface, sizeof(double), 1, fp);

    // 读取 Rocklayer 字段
    fread(&header->Rocklayer, sizeof(double), 1, fp);

    // 读取 Gametime 字段
    fread(&header->Gametime, sizeof(double), 1, fp);

    // 读取 Isdaytime 字段
    fread(&header->Isdaytime, sizeof(uint8_t), 1, fp);

    // 读取 Moonphase 字段
    fread(&header->Moonphase, sizeof(uint32_t), 1, fp);

    // 读取 Bloodmoon 字段
    fread(&header->Bloodmoon, sizeof(uint8_t), 1, fp);

    // 读取 Eclipse 字段
    fread(&header->Eclipse, sizeof(uint8_t), 1, fp);

    // 读取 DungeonX 字段
    fread(&header->DungeonX, sizeof(int32_t), 1, fp);

    // 读取 DungeonY 字段
    fread(&header->DungeonY, sizeof(int32_t), 1, fp);

    // 读取 Crimson 字段
    fread(&header->Crimson, sizeof(uint8_t), 1, fp);

    // 读取 DownedBoss1 字段
    fread(&header->DownedBoss1, sizeof(uint8_t), 1, fp);

    // 读取 DownedBoss2 字段
    fread(&header->DownedBoss2, sizeof(uint8_t), 1, fp);

    // 读取 DownedBoss3 字段
    fread(&header->DownedBoss3, sizeof(uint8_t), 1, fp);

    // 读取 DownedQueenBee 字段
    fread(&header->DownedQueenBee, sizeof(uint8_t), 1, fp);

    // 读取 DownedMechBoss1 字段
    fread(&header->DownedMechBoss1, sizeof(uint8_t), 1, fp);

    // 读取 DownedMechBoss2 字段
    fread(&header->DownedMechBoss2, sizeof(uint8_t), 1, fp);

    // 读取 DownedMechBoss3 字段
    fread(&header->DownedMechBoss3, sizeof(uint8_t), 1, fp);

    // 读取 DownedMechBossAny 字段
    fread(&header->DownedMechBossAny, sizeof(uint8_t), 1, fp);

    // 读取 DownedPlantBoss 字段
    fread(&header->DownedPlantBoss, sizeof(uint8_t), 1, fp);

    // 读取 DownedGolemBoss 字段
    fread(&header->DownedGolemBoss, sizeof(uint8_t), 1, fp);
    
    if (*version >= 118){
        // 读取 DownedSlimeKing 字段
        fread(&header->DownedSlimeKing, sizeof(uint8_t), 1, fp);
    }

    // 读取 SavedGoblin 字段
    fread(&header->SavedGoblin, sizeof(uint8_t), 1, fp);

    // 读取 SavedWizard 字段
    fread(&header->SavedWizard, sizeof(uint8_t), 1, fp);

    // 读取 SavedMech 字段
    fread(&header->SavedMech, sizeof(uint8_t), 1, fp);

    // 读取 DownedGoblins 字段
    fread(&header->DownedGoblins, sizeof(uint8_t), 1, fp);

    // 读取 DownedClown 字段
    fread(&header->DownedClown, sizeof(uint8_t), 1, fp);

    // 读取 DownedFrost 字段
    fread(&header->DownedFrost, sizeof(uint8_t), 1, fp);

    // 读取 DownedPirates 字段
    fread(&header->DownedPirates, sizeof(uint8_t), 1, fp);

    // 读取 ShadowOrbSmashed 字段
    fread(&header->ShadowOrbSmashed, sizeof(uint8_t), 1, fp);

    // 读取 SpawnMeteor 字段
    fread(&header->SpawnMeteor, sizeof(uint8_t), 1, fp);

    // 读取 ShadowOrbCount 字段
    fread(&header->ShadowOrbCount, sizeof(uint8_t), 1, fp);

    // 读取 AltarCount 字段
    fread(&header->AltarCount, sizeof(uint32_t), 1, fp);

    // 读取 HardMode 字段
    fread(&header->HardMode, sizeof(uint8_t), 1, fp);

    if (*version >= 257){
        // 读取 AfterPartyOfDoom 字段
        fread(&header->AfterPartyOfDoom, sizeof(uint8_t), 1, fp);
    }
    // 读取 InvasionDelay 字段
    fread(&header->InvasionDelay, sizeof(uint32_t), 1, fp);

    // 读取 InvasionSize 字段
    fread(&header->InvasionSize, sizeof(uint32_t), 1, fp);

    // 读取 InvasionType 字段
    fread(&header->InvasionType, sizeof(uint32_t), 1, fp);

    // 读取 InvasionX 字段
    fread(&header->InvasionX, sizeof(double), 1, fp);
    
    if (*version >= 118){
        // 读取 SlimeRainTime 字段
        fread(&header->SlimeRainTime, sizeof(double), 1, fp);
    }

    if (*version >= 113){
        // 读取 SundialCooldown 字段
        fread(&header->SundialCooldown, sizeof(uint8_t), 1, fp);
    }

    // 读取 Raining 字段
    fread(&header->Raining, sizeof(uint8_t), 1, fp);

    // 读取 RainTime 字段
    fread(&header->RainTime, sizeof(uint32_t), 1, fp);

    // 读取 MaxRain 字段
    fread(&header->MaxRain, sizeof(float), 1, fp);

    // 读取 SavedOreTiersCobalt 字段
    fread(&header->SavedOreTiersCobalt, sizeof(int32_t), 1, fp);

    // 读取 SavedOreTiersMythril 字段
    fread(&header->SavedOreTiersMythril, sizeof(int32_t), 1, fp);

    // 读取 SavedOreTiersAdamantite 字段
    fread(&header->SavedOreTiersAdamantite, sizeof(int32_t), 1, fp);

    // 读取 BgTree 字段
    fread(&header->BgTree, sizeof(uint8_t), 1, fp);

    // 读取 BgCorruption 字段
    fread(&header->BgCorruption, sizeof(uint8_t), 1, fp);

    // 读取 BgJungle 字段
    fread(&header->BgJungle, sizeof(uint8_t), 1, fp);

    // 读取 BgSnow 字段
    fread(&header->BgSnow, sizeof(uint8_t), 1, fp);

    // 读取 BgHallow 字段
    fread(&header->BgHallow, sizeof(uint8_t), 1, fp);

    // 读取 BgCrimson 字段
    fread(&header->BgCrimson, sizeof(uint8_t), 1, fp);

    // 读取 BgDesert 字段
    fread(&header->BgDesert, sizeof(uint8_t), 1, fp);

    // 读取 BgOcean 字段
    fread(&header->BgOcean, sizeof(uint8_t), 1, fp);

    // 读取 CloudBgActive 字段
    fread(&header->CloudBgActive, sizeof(int32_t), 1, fp);

    // 读取 NumClouds 字段
    fread(&header->NumClouds, sizeof(uint16_t), 1, fp);

    // 读取 WindSpeedSet 字段
    fread(&header->WindSpeedSet, sizeof(float), 1, fp);
    if (*version < 95){
        return  ;
    }
    
    // 读取 AnglerFinishedSize 字段
    fread(&header->AnglerFinishedSize, sizeof(uint32_t), 1, fp);
    
    // 读取 Anglerfinished 字段
    header->Anglerfinished = malloc(sizeof(Base128Varint) * header->AnglerFinishedSize);
    for (int i = 0; i < header->AnglerFinishedSize; i++) {
        Base128Varint str = parse_base128_varint_string_from_file(fp);
        header->Anglerfinished[i].length = str.length;
        header->Anglerfinished[i].value = malloc(str.length + 1);
        memcpy(header->Anglerfinished[i].value, str.value, str.length + 1);
        free(str.value);
    }
    if (*version < 99){
        return  ;
    }
    
    // 读取保存的钓鱼人信息
    fread(&header->SavedAngler, sizeof(uint8_t), 1, fp);

    if (*version < 101){
        return  ;
    }
    
    // 读取钓鱼任务信息
    fread(&header->AnglerQuest, sizeof(uint32_t), 1, fp);
    if (*version < 104){
        return  ;
    }
    
    // 读取保存的发型师信息
    fread(&header->SavedStylist, sizeof(uint8_t), 1, fp);

    if (*version >= 140){
        // 读取保存的税收员信息
        fread(&header->SavedTaxCollector, sizeof(uint8_t), 1, fp);
    }
    
    if (*version >= 201){
        // 读取保存的高尔夫球手信息
        fread(&header->SavedGolfer, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 107){
        // 读取入侵事件开始时间
        fread(&header->InvasionSizeStart, sizeof(uint32_t), 1, fp);
    }
    if (*version >= 108){
        // 读取邪教徒事件延迟时间
        fread(&header->CultistDelay, sizeof(uint32_t), 1, fp);
    }else{
        header->CultistDelay = 86400;
    }

    if (*version < 109){
        return  ;
    }

    // 读取击杀计数
    fread(&header->KillsCount, sizeof(uint16_t), 1, fp);

    // 读取击杀列表
    header->Kills = malloc(sizeof(uint32_t) * header->KillsCount);
    fread(header->Kills, sizeof(uint32_t), header->KillsCount, fp);

    if (*version < 131){
        return  ;
    }

    if (*version >= 140){
        // 读取时间快进标志
        fread(&header->FastForwardTime, sizeof(uint8_t), 1, fp);
    }


    // 读取已击败的鱼人标志
    fread(&header->DownedFishron, sizeof(uint8_t), 1, fp);

    if (*version >= 140){
        // 读取已击败的火星人标志
        fread(&header->DownedMartians, sizeof(uint8_t), 1, fp);

        // 读取已击败的古代邪教徒标志
        fread(&header->DownedAncientCultist, sizeof(uint8_t), 1, fp);

        // 读取已击败的月球领主标志
        fread(&header->DownedMoonlord, sizeof(uint8_t), 1, fp);
    }
    if (*version < 140){
        return  ;
    }

    // 读取已击败的万圣节之王标志
    fread(&header->DownedHalloweenKing, sizeof(uint8_t), 1, fp);

    // 读取已击败的万圣节树标志
    fread(&header->DownedHalloweenTree, sizeof(uint8_t), 1, fp);

    // 读取已击败的圣诞节冰女王标志
    fread(&header->DownedChristmasIceQueen, sizeof(uint8_t), 1, fp);

    // 读取已击败的圣诞节圣坦克标志
    fread(&header->DownedChristmasSantank, sizeof(uint8_t), 1, fp);

    // 读取已击败的圣诞节树标志
    fread(&header->DownedChristmasTree, sizeof(uint8_t), 1, fp);

    // 读取已击败的太阳神塔标志
    fread(&header->DownedTowerSolar, sizeof(uint8_t), 1, fp);

    // 读取已击败的飓风神塔标志
    fread(&header->DownedTowerVortex, sizeof(uint8_t), 1, fp);

    // 读取已击败的星云神塔标志
    fread(&header->DownedTowerNebula, sizeof(uint8_t), 1, fp);

    // 读取已击败的星尘神塔标志
    fread(&header->DownedTowerStardust, sizeof(uint8_t), 1, fp);

    // 读取太阳神塔状态
    fread(&header->TowerActiveSolar, sizeof(uint8_t), 1, fp);

    // 读取飓风神塔状态
    fread(&header->TowerActiveVortex, sizeof(uint8_t), 1, fp);

    // 读取星云神塔状态
    fread(&header->TowerActiveNebula, sizeof(uint8_t), 1, fp);

    // 读取星尘神塔状态
    fread(&header->TowerActiveStardust, sizeof(uint8_t), 1, fp);

    // 读取月球末日标志
    fread(&header->LunarApocalypseIsUp, sizeof(uint8_t), 1, fp);

    if (*version >= 170){
        // 读取手动派对标志
        fread(&header->PartyManual, sizeof(uint8_t), 1, fp);

        // 读取真实派对标志
        fread(&header->PartyGenuine, sizeof(uint8_t), 1, fp);

        // 读取派对冷却时间
        fread(&header->PartyCooldown, sizeof(uint32_t), 1, fp);

        // 读取正在庆祝的NPC数量
        fread(&header->PartyCelebratingNpcSize, sizeof(uint32_t), 1, fp);

        // 读取正在庆祝的NPC列表
        header->PartyCelebratingNpcs = malloc(sizeof(int32_t) * header->PartyCelebratingNpcSize);
        fread(header->PartyCelebratingNpcs, sizeof(int32_t), header->PartyCelebratingNpcSize, fp);
    }

    if (*version >= 174){
        // 读取沙暴标志
        fread(&header->SandstormHappening, sizeof(uint8_t), 1, fp);

        // 读取沙暴剩余时间
        fread(&header->SandstormTimeLeft, sizeof(uint32_t), 1, fp);

        // 读取沙暴严重程度
        fread(&header->SandstormSeverity, sizeof(float), 1, fp);

        // 读取沙暴预期严重程度
        fread(&header->SandstormIntendedSeverity, sizeof(float), 1, fp);
    }

    if (*version >= 178){
        // 读取 SavedBartender 字段
        fread(&header->SavedBartender, sizeof(uint8_t), 1, fp);

        // 读取 DownedInvasionT1 字段
        fread(&header->DownedInvasionT1, sizeof(uint8_t), 1, fp);

        // 读取 DownedInvasionT2 字段
        fread(&header->DownedInvasionT2, sizeof(uint8_t), 1, fp);

        // 读取 DownedInvasionT3 字段
        fread(&header->DownedInvasionT3, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 194){
        // 读取 MushroomBg 字段
        fread(&header->MushroomBg, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 215){
        // 读取 UndergroundDesertBg 字段
        fread(&header->UndergroundDesertBg, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 195){
        // 读取 BgTree2 字段
        fread(&header->BgTree2, sizeof(uint8_t), 1, fp);

        // 读取 BgTree3 字段
        fread(&header->BgTree3, sizeof(uint8_t), 1, fp);

        // 读取 BgTree4 字段
        fread(&header->BgTree4, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 204){
        // 读取 CombatBookUsed 字段
        fread(&header->CombatBookUsed, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 207){
        // 读取 LanternNightCooldown 字段
        fread(&header->LanternNightCooldown, sizeof(uint32_t), 1, fp);

        // 读取 LanternNightGenuine 字段
        fread(&header->LanternNightGenuine, sizeof(uint8_t), 1, fp);

        // 读取 LanternNightManual 字段
        fread(&header->LanternNightManual, sizeof(uint8_t), 1, fp);

        // 读取 LanternNightNextNightIsGenuine 字段
        fread(&header->LanternNightNextNightIsGenuine, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 211){
        // 读取 TreetopSize 字段
        fread(&header->TreetopSize, sizeof(uint32_t), 1, fp);

        // 读取 TreeTopVariations 字段
        header->TreeTopVariations = malloc(sizeof(int32_t) * header->TreetopSize);
        fread(header->TreeTopVariations, sizeof(int32_t), header->TreetopSize, fp);
    }

    if (*version >= 212){
        // 读取 ForceHalloweenForToday 字段
        fread(&header->ForceHalloweenForToday, sizeof(uint8_t), 1, fp);

        // 读取 ForceXMasForToday 字段
        fread(&header->ForceXMasForToday, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 216){
        // 读取 SavedOreTiersCopper 字段
        fread(&header->SavedOreTiersCopper, sizeof(uint32_t), 1, fp);

        // 读取 SavedOreTiersIron 字段
        fread(&header->SavedOreTiersIron, sizeof(uint32_t), 1, fp);

        // 读取 SavedOreTiersSilver 字段
        fread(&header->SavedOreTiersSilver, sizeof(uint32_t), 1, fp);

        // 读取 SavedOreTiersGold 字段
        fread(&header->SavedOreTiersGold, sizeof(uint32_t), 1, fp);
    }

    if (*version >= 217){
        // 读取 BoughtCat 字段
        fread(&header->BoughtCat, sizeof(uint8_t), 1, fp);

        // 读取 BoughtDog 字段
        fread(&header->BoughtDog, sizeof(uint8_t), 1, fp);

        // 读取 BoughtBunny 字段
        fread(&header->BoughtBunny, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 223){
        // 读取 DownedEmpressOfLight 字段
        fread(&header->DownedEmpressOfLight, sizeof(uint8_t), 1, fp);

        // 读取 DownedQueenSlime 字段
        fread(&header->DownedQueenSlime, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 240){
        // 读取 DownedDeerclops 字段
        fread(&header->DownedDeerclops, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 250){
    // 读取 UnlockedSlimeBlueSpawn 字段
    fread(&header->UnlockedSlimeBlueSpawn, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 251){
        // 读取 UnlockedMerchantSpawn 字段
        fread(&header->UnlockedMerchantSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedDemolitionistSpawn 字段
        fread(&header->UnlockedDemolitionistSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedPartyGirlSpawn 字段
        fread(&header->UnlockedPartyGirlSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedDyeTraderSpawn 字段
        fread(&header->UnlockedDyeTraderSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedTruffleSpawn 字段
        fread(&header->UnlockedTruffleSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedArmsDealerSpawn 字段
        fread(&header->UnlockedArmsDealerSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedNurseSpawn 字段
        fread(&header->UnlockedNurseSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedPrincessSpawn 字段
        fread(&header->UnlockedPrincessSpawn, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 259){
        // 读取 CombatBookVolumeTwoWasUsed 字段
        fread(&header->CombatBookVolumeTwoWasUsed, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 260){
        // 读取 PeddlersSatchelWasUsed 字段
        fread(&header->PeddlersSatchelWasUsed, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 261){
        // 读取 UnlockedSlimeGreenSpawn 字段
        fread(&header->UnlockedSlimeGreenSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedSlimeOldSpawn 字段
        fread(&header->UnlockedSlimeOldSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedSlimePurpleSpawn 字段
        fread(&header->UnlockedSlimePurpleSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedSlimeRainbowSpawn 字段
        fread(&header->UnlockedSlimeRainbowSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedSlimeRedSpawn 字段
        fread(&header->UnlockedSlimeRedSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedSlimeYellowSpawn 字段
        fread(&header->UnlockedSlimeYellowSpawn, sizeof(uint8_t), 1, fp);

        // 读取 UnlockedSlimeCopperSpawn 字段
        fread(&header->UnlockedSlimeCopperSpawn, sizeof(uint8_t), 1, fp);
    }

    if (*version >= 264){
        // 读取 FastForwardTimeToDusk 字段
        fread(&header->FastForwardTimeToDusk, sizeof(uint8_t), 1, fp);

        // 读取 MoondialCooldown 字段
        fread(&header->MoondialCooldown, sizeof(uint8_t), 1, fp);
    }
    uint32_t current_position = ftell(fp);
    if (current_position != header_end_position){
        printf("Header length mismatch: %ld != %u\n", current_position, header_end_position-format_end_position);
    }
}

void free_header(Header *header) {
    free(header->Name.value);
    free(header->Seed.value);
    for (int i = 0; i < header->AnglerFinishedSize; i++) {
        free(header->Anglerfinished[i].value);
    }
    free(header->Anglerfinished);
    free(header->Kills);
    free(header->PartyCelebratingNpcs);
    free(header->TreeTopVariations);

}

void print_header(Header *header) {
    printf("Name: %s\n", header->Name.value);
    printf("Seed: %s\n", header->Seed.value);
    printf("Genversion: %lu\n", header->Genversion);
    printf("UUID: %s\n", header->UUID);
    printf("\n");
    printf("WorldId: %u\n", header->WorldId);
    printf("Rect: [%u, %u, %u, %u]\n", header->Rect[0], header->Rect[1], header->Rect[2], header->Rect[3]);
    printf("Height: %u\n", header->Height);
    printf("Width: %u\n", header->Width);
    printf("Gamemode: %u\n", header->Gamemode);
    printf("Drunkworld: %u\n", header->Drunkworld);
    printf("Goodworld: %u\n", header->Goodworld);
    printf("Tenthanniversary: %u\n", header->Tenthanniversary);
    printf("Dontstarve: %u\n", header->Dontstarve);
    printf("Notthebees: %u\n", header->Notthebees);
    printf("Remixworld: %u\n", header->Remixworld);
    printf("Notrapsworld: %u\n", header->Notrapsworld);
    printf("Zenithworld: %u\n", header->Zenithworld);
    printf("Creationtime: %lld\n", header->Creationtime);
    printf("Moontype: %u\n", header->Moontype);
    printf("Treex: [%u, %u, %u]\n", header->Treex[0], header->Treex[1], header->Treex[2]);
    printf("TreeStyle: [%u, %u, %u, %u]\n", header->TreeStyle[0], header->TreeStyle[1], header->TreeStyle[2], header->TreeStyle[3]);
    printf("Caveback: [%u, %u, %u]\n", header->Caveback[0], header->Caveback[1], header->Caveback[2]);
    printf("Cavebackstyle: [%u, %u, %u, %u]\n", header->Cavebackstyle[0], header->Cavebackstyle[1], header->Cavebackstyle[2], header->Cavebackstyle[3]);
    printf("Icebackstyle: %u\n", header->Icebackstyle);
    printf("Junglebackstyle: %u\n", header->Junglebackstyle);
    printf("Hellbackstyle: %u\n", header->Hellbackstyle);
    printf("SpawnTileX: %d\n", header->SpawnTileX);
    printf("SpawnTileY: %d\n", header->SpawnTileY);
    printf("Worldsurface: %f\n", header->Worldsurface);
    printf("Rocklayer: %f\n", header->Rocklayer);
    printf("Gametime: %f\n", header->Gametime);

    printf("Isdaytime: %u\n", header->Isdaytime);
    printf("Moonphase: %u\n", header->Moonphase);
    printf("Bloodmoon: %u\n", header->Bloodmoon);
    printf("Eclipse: %u\n", header->Eclipse);
    printf("DungeonX: %d\n", header->DungeonX);
    printf("DungeonY: %d\n", header->DungeonY);
    printf("Crimson: %u\n", header->Crimson);
    printf("DownedBoss1: %u\n", header->DownedBoss1);
    printf("DownedBoss2: %u\n", header->DownedBoss2);
    printf("DownedBoss3: %u\n", header->DownedBoss3);
    printf("DownedQueenBee: %u\n", header->DownedQueenBee);
    printf("DownedMechBoss1: %u\n", header->DownedMechBoss1);
    printf("DownedMechBoss2: %u\n", header->DownedMechBoss2);
    printf("DownedMechBoss3: %u\n", header->DownedMechBoss3);
    printf("DownedMechBossAny: %u\n", header->DownedMechBossAny);
    printf("DownedPlantBoss: %u\n", header->DownedPlantBoss);
    printf("DownedGolemBoss: %u\n", header->DownedGolemBoss);
    printf("DownedSlimeKing: %u\n", header->DownedSlimeKing);
    printf("SavedGoblin: %u\n", header->SavedGoblin);
    printf("SavedWizard: %u\n", header->SavedWizard);
    printf("SavedMech: %u\n", header->SavedMech);
    printf("DownedGoblins: %u\n", header->DownedGoblins);
    printf("DownedClown: %u\n", header->DownedClown);
    printf("DownedFrost: %u\n", header->DownedFrost);
    printf("DownedPirates: %u\n", header->DownedPirates);
    printf("ShadowOrbSmashed: %u\n", header->ShadowOrbSmashed);
    printf("SpawnMeteor: %u\n", header->SpawnMeteor);
    printf("ShadowOrbCount: %u\n", header->ShadowOrbCount);
    printf("AltarCount: %u\n", header->AltarCount);
    printf("HardMode: %u\n", header->HardMode);
    printf("AfterPartyOfDoom: %u\n", header->AfterPartyOfDoom);
    printf("InvasionDelay: %u\n", header->InvasionDelay);
    printf("InvasionSize: %u\n", header->InvasionSize);
    printf("InvasionType: %u\n", header->InvasionType);
    printf("InvasionX: %f\n", header->InvasionX);
    printf("SlimeRainTime: %f\n", header->SlimeRainTime);
    printf("SundialCooldown: %u\n", header->SundialCooldown);
    printf("Raining: %lu\n", header->Raining);
    printf("RainTime: %u\n", header->RainTime);
    printf("MaxRain: %f\n", header->MaxRain);
    printf("SavedOreTiersCobalt: %d\n", header->SavedOreTiersCobalt);
    printf("SavedOreTiersMythril: %d\n", header->SavedOreTiersMythril);
    printf("SavedOreTiersAdamantite: %d\n", header->SavedOreTiersAdamantite);
    printf("BgTree: %u\n", header->BgTree);
    printf("BgCorruption: %u\n", header->BgCorruption);
    printf("BgJungle: %u\n", header->BgJungle);
    printf("BgSnow: %u\n", header->BgSnow);
    printf("BgHallow: %u\n", header->BgHallow);
    printf("BgCrimson: %u\n", header->BgCrimson);
    printf("BgDesert: %u\n", header->BgDesert);
    printf("BgOcean: %u\n", header->BgOcean);
    printf("CloudBgActive: %d\n", header->CloudBgActive);
    printf("NumClouds: %u\n", header->NumClouds);
    printf("WindSpeedSet: %f\n", header->WindSpeedSet);

    printf("AnglerFinishedSize: %u\n", header->AnglerFinishedSize);
    printf("SavedAngler: %u\n", header->SavedAngler);
    printf("AnglerQuest: %u\n", header->AnglerQuest);
    printf("SavedStylist: %u\n", header->SavedStylist);
    printf("SavedTaxCollector: %u\n", header->SavedTaxCollector);
    printf("SavedGolfer: %u\n", header->SavedGolfer);
    printf("InvasionSizeStart: %u\n", header->InvasionSizeStart);
    printf("CultistDelay: %u\n", header->CultistDelay);
    printf("KillsCount: %u\n", header->KillsCount);
    printf("Kills: ");
    for (int i = 0; i < header->KillsCount; i++) {
        printf("%u ", header->Kills[i]);
    }
    printf("\n");
    printf("FastForwardTime: %u\n", header->FastForwardTime);
    printf("DownedFishron: %u\n", header->DownedFishron);
    printf("DownedMartians: %u\n", header->DownedMartians);
    printf("DownedAncientCultist: %u\n", header->DownedAncientCultist);
    printf("DownedMoonlord: %u\n", header->DownedMoonlord);
    printf("DownedHalloweenKing: %u\n", header->DownedHalloweenKing);
    printf("DownedHalloweenTree: %u\n", header->DownedHalloweenTree);
    printf("DownedChristmasIceQueen: %u\n", header->DownedChristmasIceQueen);
    printf("DownedChristmasSantank: %u\n", header->DownedChristmasSantank);
    printf("DownedChristmasTree: %u\n", header->DownedChristmasTree);
    printf("DownedTowerSolar: %u\n", header->DownedTowerSolar);
    printf("DownedTowerVortex: %u\n", header->DownedTowerVortex);
    printf("DownedTowerNebula: %u\n", header->DownedTowerNebula);
    printf("DownedTowerStardust: %u\n", header->DownedTowerStardust);
    printf("TowerActiveSolar: %u\n", header->TowerActiveSolar);
    printf("TowerActiveVortex: %u\n", header->TowerActiveVortex);
    printf("TowerActiveNebula: %u\n", header->TowerActiveNebula);
    printf("TowerActiveStardust: %u\n", header->TowerActiveStardust);
    printf("LunarApocalypseIsUp: %u\n", header->LunarApocalypseIsUp);
    printf("PartyManual: %u\n", header->PartyManual);
    printf("PartyGenuine: %u\n", header->PartyGenuine);
    printf("PartyCooldown: %u\n", header->PartyCooldown);
    printf("PartyCelebratingNpcSize: %u\n", header->PartyCelebratingNpcSize);
    printf("PartyCelebratingNpcs: ");
    for (int i = 0; i < header->PartyCelebratingNpcSize; i++) {
        printf("%d ", header->PartyCelebratingNpcs[i]);
    }
    printf("\n");
    printf("SandstormHappening: %u\n", header->SandstormHappening);
    printf("SandstormTimeLeft: %u\n", header->SandstormTimeLeft);
    printf("SandstormSeverity: %f\n", header->SandstormSeverity);
    printf("SandstormIntendedSeverity: %f\n", header->SandstormIntendedSeverity);
    printf("SavedBartender: %u\n", header->SavedBartender);
    printf("DownedInvasionT1: %u\n", header->DownedInvasionT1);
    printf("DownedInvasionT2: %u\n", header->DownedInvasionT2);
    printf("DownedInvasionT3: %u\n", header->DownedInvasionT3);
    printf("MushroomBg: %u\n", header->MushroomBg);
    printf("UndergroundDesertBg: %u\n", header->UndergroundDesertBg);
    printf("BgTree2: %u\n", header->BgTree2);
    printf("BgTree3: %u\n", header->BgTree3);
    printf("BgTree4: %u\n", header->BgTree4);
    printf("CombatBookUsed: %u\n", header->CombatBookUsed);
    printf("LanternNightCooldown: %u\n", header->LanternNightCooldown);
    printf("LanternNightGenuine: %u\n", header->LanternNightGenuine);
    printf("LanternNightManual: %u\n", header->LanternNightManual);
    printf("LanternNightNextNightIsGenuine: %u\n", header->LanternNightNextNightIsGenuine);
    printf("TreetopSize: %u\n", header->TreetopSize);
    printf("TreeTopVariations: ");
    for (int i = 0; i < header->TreetopSize; i++) {
        printf("%d ", header->TreeTopVariations[i]);
    }
    printf("\n");
    printf("ForceHalloweenForToday: %u\n", header->ForceHalloweenForToday);
    printf("ForceXMasForToday: %u\n", header->ForceXMasForToday);

    printf("SavedOreTiersCopper: %u\n", header->SavedOreTiersCopper);
    printf("SavedOreTiersIron: %u\n", header->SavedOreTiersIron);
    printf("SavedOreTiersSilver: %u\n", header->SavedOreTiersSilver);
    printf("SavedOreTiersGold: %u\n", header->SavedOreTiersGold);
    printf("BoughtCat: %u\n", header->BoughtCat);
    printf("BoughtDog: %u\n", header->BoughtDog);
    printf("BoughtBunny: %u\n", header->BoughtBunny);
    printf("DownedEmpressOfLight: %u\n", header->DownedEmpressOfLight);
    printf("DownedQueenSlime: %u\n", header->DownedQueenSlime);
    printf("DownedDeerclops: %u\n", header->DownedDeerclops);
    printf("UnlockedSlimeBlueSpawn: %u\n", header->UnlockedSlimeBlueSpawn);
    printf("UnlockedMerchantSpawn: %u\n", header->UnlockedMerchantSpawn);
    printf("UnlockedDemolitionistSpawn: %u\n", header->UnlockedDemolitionistSpawn);
    printf("UnlockedPartyGirlSpawn: %u\n", header->UnlockedPartyGirlSpawn);
    printf("UnlockedDyeTraderSpawn: %u\n", header->UnlockedDyeTraderSpawn);
    printf("UnlockedTruffleSpawn: %u\n", header->UnlockedTruffleSpawn);
    printf("UnlockedArmsDealerSpawn: %u\n", header->UnlockedArmsDealerSpawn);
    printf("UnlockedNurseSpawn: %u\n", header->UnlockedNurseSpawn);
    printf("UnlockedPrincessSpawn: %u\n", header->UnlockedPrincessSpawn);
    printf("CombatBookVolumeTwoWasUsed: %u\n", header->CombatBookVolumeTwoWasUsed);
    printf("PeddlersSatchelWasUsed: %u\n", header->PeddlersSatchelWasUsed);
    printf("UnlockedSlimeGreenSpawn: %u\n", header->UnlockedSlimeGreenSpawn);
    printf("UnlockedSlimeOldSpawn: %u\n", header->UnlockedSlimeOldSpawn);
    printf("UnlockedSlimePurpleSpawn: %u\n", header->UnlockedSlimePurpleSpawn);
    printf("UnlockedSlimeRainbowSpawn: %u\n", header->UnlockedSlimeRainbowSpawn);
    printf("UnlockedSlimeRedSpawn: %u\n", header->UnlockedSlimeRedSpawn);
    printf("UnlockedSlimeYellowSpawn: %u\n", header->UnlockedSlimeYellowSpawn);
    printf("UnlockedSlimeCopperSpawn: %u\n", header->UnlockedSlimeCopperSpawn);
    printf("FastForwardTimeToDusk: %u\n", header->FastForwardTimeToDusk);
    printf("MoondialCooldown: %u\n", header->MoondialCooldown);

    
}

void write_header(FILE *fp, Header *header, Format *format) {
    uint32_t version = format->version;
    encode_base128_varint_string(fp, header->Name);
    if (version >= 179) {
        if (version == 179) {
            int32_t seed_int = atoi(header->Seed.value);
            fwrite(&seed_int, sizeof(int32_t), 1, fp);
        } else {
            encode_base128_varint_string(fp, header->Seed);
        }
        fwrite(&header->Genversion, sizeof(header->Genversion), 1, fp);
    }
    if (version >= 181) {
        uint8_t uuid_char[16];
        sscanf(header->UUID, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            &uuid_char[0], &uuid_char[1], &uuid_char[2], &uuid_char[3],
            &uuid_char[4], &uuid_char[5], &uuid_char[6], &uuid_char[7],
            &uuid_char[8], &uuid_char[9], &uuid_char[10], &uuid_char[11],
            &uuid_char[12], &uuid_char[13], &uuid_char[14], &uuid_char[15]);
        uint8_t uuid_bytes[16];
        uuid_bytes[0] = uuid_char[3];
        uuid_bytes[1] = uuid_char[2]; 
        uuid_bytes[2] = uuid_char[1];
        uuid_bytes[3] = uuid_char[0];

        uuid_bytes[4] = uuid_char[5];
        uuid_bytes[5] = uuid_char[4];

        uuid_bytes[6] = uuid_char[7];  
        uuid_bytes[7] = uuid_char[6];

        uuid_bytes[8] = uuid_char[8];
        uuid_bytes[9] = uuid_char[9];
        uuid_bytes[10] = uuid_char[10];
        uuid_bytes[11] = uuid_char[11];

        uuid_bytes[12] = uuid_char[12];
        uuid_bytes[13] = uuid_char[13];
        uuid_bytes[14] = uuid_char[14];
        uuid_bytes[15] = uuid_char[15];

        fwrite(uuid_bytes, sizeof(uint8_t), 16, fp);
    } else {
        srand(time(NULL));
        uint8_t uuid[16];
        for (int i = 0; i < 16; i++) {
            uuid[i] = rand() % 256;
        }
        fwrite(uuid, sizeof(uuid), 1, fp);
    }
    fwrite(&header->WorldId, sizeof(header->WorldId), 1, fp);
    fwrite(header->Rect, sizeof(uint32_t), 4, fp);
    fwrite(&header->Height, sizeof(header->Height), 1, fp);
    fwrite(&header->Width, sizeof(header->Width), 1, fp);
    if (version >= 209){
        fwrite(&header->Gamemode, sizeof(header->Gamemode), 1, fp);
        if (version >= 222){
            fwrite(&header->Drunkworld, sizeof(header->Drunkworld), 1, fp);
        }
        if (version >= 227){
            fwrite(&header->Goodworld, sizeof(header->Goodworld), 1, fp);
        }
        if (version >= 238){
            fwrite(&header->Tenthanniversary, sizeof(header->Tenthanniversary), 1, fp);
        }
        if (version >= 239){
            fwrite(&header->Dontstarve, sizeof(header->Dontstarve), 1, fp);
        }
        if (version >= 241){
            fwrite(&header->Notthebees, sizeof(header->Notthebees), 1, fp);
        }
        if (version >= 249){
            fwrite(&header->Remixworld, sizeof(header->Remixworld), 1, fp);
        }
        if (version >= 266){
            fwrite(&header->Notrapsworld, sizeof(header->Notrapsworld), 1, fp);
        }
        if (version >= 267){
            fwrite(&header->Zenithworld, sizeof(header->Zenithworld), 1, fp);
        }
    }else if (version == 208){
        uint8_t b = (header->Gamemode == 2) ? 0 : 1;
        fwrite(&b, sizeof(b), 1, fp);
    }else if (version >=112){
        uint8_t b = (header->Gamemode == 1) ? 0 : 1;
        fwrite(&b, sizeof(b), 1, fp);
    }
    if (version >= 141){    
        fwrite(&header->Creationtime, sizeof(int64_t), 1, fp);
    }
    fwrite(&header->Moontype, sizeof(uint8_t), 1, fp);
    fwrite(header->Treex, sizeof(uint32_t), 3, fp);
    fwrite(header->TreeStyle, sizeof(uint32_t), 4, fp);
    fwrite(header->Caveback, sizeof(uint32_t), 3, fp);
    fwrite(header->Cavebackstyle, sizeof(uint32_t), 4, fp);
    fwrite(&header->Icebackstyle, sizeof(uint32_t), 1, fp);
    fwrite(&header->Junglebackstyle, sizeof(uint32_t), 1, fp);
    fwrite(&header->Hellbackstyle, sizeof(uint32_t), 1, fp);
    fwrite(&header->SpawnTileX, sizeof(int32_t), 1, fp);
    fwrite(&header->SpawnTileY, sizeof(int32_t), 1, fp);
    fwrite(&header->Worldsurface, sizeof(double), 1, fp);
    fwrite(&header->Rocklayer, sizeof(double), 1, fp);
    fwrite(&header->Gametime, sizeof(double), 1, fp);
    fwrite(&header->Isdaytime, sizeof(uint8_t), 1, fp);
    fwrite(&header->Moonphase, sizeof(uint32_t), 1, fp);
    fwrite(&header->Bloodmoon, sizeof(uint8_t), 1, fp);
    fwrite(&header->Eclipse, sizeof(uint8_t), 1, fp);
    fwrite(&header->DungeonX, sizeof(int32_t), 1, fp);
    fwrite(&header->DungeonY, sizeof(int32_t), 1, fp);
    fwrite(&header->Crimson, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedBoss1, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedBoss2, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedBoss3, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedQueenBee, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedMechBoss1, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedMechBoss2, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedMechBoss3, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedMechBossAny, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedPlantBoss, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedGolemBoss, sizeof(uint8_t), 1, fp);
    if (version >= 118){
        fwrite(&header->DownedSlimeKing, sizeof(uint8_t), 1, fp);
    }
    fwrite(&header->SavedGoblin, sizeof(uint8_t), 1, fp);
    fwrite(&header->SavedWizard, sizeof(uint8_t), 1, fp);
    fwrite(&header->SavedMech, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedGoblins, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedClown, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedFrost, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedPirates, sizeof(uint8_t), 1, fp);
    fwrite(&header->ShadowOrbSmashed, sizeof(uint8_t), 1, fp);
    fwrite(&header->SpawnMeteor, sizeof(uint8_t), 1, fp);
    fwrite(&header->ShadowOrbCount, sizeof(uint8_t), 1, fp);
    fwrite(&header->AltarCount, sizeof(uint32_t), 1, fp);
    fwrite(&header->HardMode, sizeof(uint8_t), 1, fp);
    if (version >= 257) {
        fwrite(&header->AfterPartyOfDoom, sizeof(uint8_t), 1, fp);
    }
    fwrite(&header->InvasionDelay, sizeof(uint32_t), 1, fp);
    fwrite(&header->InvasionSize, sizeof(uint32_t), 1, fp);
    fwrite(&header->InvasionType, sizeof(uint32_t), 1, fp);
    fwrite(&header->InvasionX, sizeof(double), 1, fp);
    if (version >= 118) {
        fwrite(&header->SlimeRainTime, sizeof(double), 1, fp);
    }
    if (version >= 113) {
        fwrite(&header->SundialCooldown, sizeof(uint8_t), 1, fp);
    }
    fwrite(&header->Raining, sizeof(uint8_t), 1, fp);
    fwrite(&header->RainTime, sizeof(uint32_t), 1, fp);
    fwrite(&header->MaxRain, sizeof(float), 1, fp);
    fwrite(&header->SavedOreTiersCobalt, sizeof(int32_t), 1, fp);
    fwrite(&header->SavedOreTiersMythril, sizeof(int32_t), 1, fp);
    fwrite(&header->SavedOreTiersAdamantite, sizeof(int32_t), 1, fp);
    fwrite(&header->BgTree, sizeof(uint8_t), 1, fp);
    fwrite(&header->BgCorruption, sizeof(uint8_t), 1, fp);
    fwrite(&header->BgJungle, sizeof(uint8_t), 1, fp);
    fwrite(&header->BgSnow, sizeof(uint8_t), 1, fp);
    fwrite(&header->BgHallow, sizeof(uint8_t), 1, fp);
    fwrite(&header->BgCrimson, sizeof(uint8_t), 1, fp);
    fwrite(&header->BgDesert, sizeof(uint8_t), 1, fp);
    fwrite(&header->BgOcean, sizeof(uint8_t), 1, fp);
    fwrite(&header->CloudBgActive, sizeof(int32_t), 1, fp);
    fwrite(&header->NumClouds, sizeof(uint16_t), 1, fp);
    fwrite(&header->WindSpeedSet, sizeof(float), 1, fp);
    if (version < 95) {
        return;
    }
    fwrite(&header->AnglerFinishedSize, sizeof(uint32_t), 1, fp);
    for (int i = 0; i < header->AnglerFinishedSize; i++) {
        encode_base128_varint_string(fp, header->Anglerfinished[i]);
    }
    if (version < 99) {
        return;
    }
    fwrite(&header->SavedAngler, sizeof(uint8_t), 1, fp);
    if (version < 101) {
        return;
    }
    fwrite(&header->AnglerQuest, sizeof(uint32_t), 1, fp);
    if (version < 104) {
        return;
    }
    fwrite(&header->SavedStylist, sizeof(uint8_t), 1, fp);
    if (version < 140) {
        return;
    }
    fwrite(&header->SavedTaxCollector, sizeof(uint8_t), 1, fp);
    if (version < 201) {
        return;
    }
    fwrite(&header->SavedGolfer, sizeof(uint8_t), 1, fp);
    if (version < 107) {
        return;
    }
    fwrite(&header->InvasionSizeStart, sizeof(uint32_t), 1, fp);
    if (version < 108) {
        return;
    }
    fwrite(&header->CultistDelay, sizeof(uint32_t), 1, fp);
    if (version < 109) {
        return;
    }

    fwrite(&header->KillsCount, sizeof(uint16_t), 1, fp);
    fwrite(header->Kills, sizeof(uint32_t), header->KillsCount, fp);
    if (version < 131){
        return;
    }
    if (version >= 140){
        fwrite(&header->FastForwardTime, sizeof(uint8_t), 1, fp);
    }
    fwrite(&header->DownedFishron, sizeof(uint8_t), 1, fp);
    if (version >= 140){
        fwrite(&header->DownedMartians, sizeof(uint8_t), 1, fp);
        fwrite(&header->DownedAncientCultist, sizeof(uint8_t), 1, fp);
        fwrite(&header->DownedMoonlord, sizeof(uint8_t), 1, fp);
    }
    if ( version < 140){
        return;
    }
    fwrite(&header->DownedHalloweenKing, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedHalloweenTree, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedChristmasIceQueen, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedChristmasSantank, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedChristmasTree, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedTowerSolar, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedTowerVortex, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedTowerNebula, sizeof(uint8_t), 1, fp);
    fwrite(&header->DownedTowerStardust, sizeof(uint8_t), 1, fp);
    fwrite(&header->TowerActiveSolar, sizeof(uint8_t), 1, fp);
    fwrite(&header->TowerActiveVortex, sizeof(uint8_t), 1, fp);
    fwrite(&header->TowerActiveNebula, sizeof(uint8_t), 1, fp);
    fwrite(&header->TowerActiveStardust, sizeof(uint8_t), 1, fp);
    fwrite(&header->LunarApocalypseIsUp, sizeof(uint8_t), 1, fp);
    if (version >= 170){
        fwrite(&header->PartyManual, sizeof(uint8_t), 1, fp);
        fwrite(&header->PartyGenuine, sizeof(uint8_t), 1, fp);
        fwrite(&header->PartyCooldown, sizeof(uint32_t), 1, fp);
        fwrite(&header->PartyCelebratingNpcSize, sizeof(uint32_t), 1, fp);
        fwrite(header->PartyCelebratingNpcs, sizeof(int32_t), header->PartyCelebratingNpcSize, fp);
    }
    if (version >= 174) {
        fwrite(&header->SandstormHappening, sizeof(uint8_t), 1, fp);
        fwrite(&header->SandstormTimeLeft, sizeof(uint32_t), 1, fp);
        fwrite(&header->SandstormSeverity, sizeof(float), 1, fp);
        fwrite(&header->SandstormIntendedSeverity, sizeof(float), 1, fp);

    }

    if (version >= 178) {
        fwrite(&header->SavedBartender, sizeof(uint8_t), 1, fp);
        fwrite(&header->DownedInvasionT1, sizeof(uint8_t), 1, fp);
        fwrite(&header->DownedInvasionT2, sizeof(uint8_t), 1, fp);
        fwrite(&header->DownedInvasionT3, sizeof(uint8_t), 1, fp);
    }

    if (version >= 194) {
        fwrite(&header->MushroomBg, sizeof(uint8_t), 1, fp);
    }

    if (version >= 215) {
        fwrite(&header->UndergroundDesertBg, sizeof(uint8_t), 1, fp);
    }

    if (version >= 195) {
        fwrite(&header->BgTree2, sizeof(uint8_t), 1, fp);
        fwrite(&header->BgTree4, sizeof(uint8_t), 1, fp);
        fwrite(&header->BgTree3, sizeof(uint8_t), 1, fp);
    }

    if (version >= 204) {
        fwrite(&header->CombatBookUsed, sizeof(uint8_t), 1, fp);
    }

    if (version >= 207) {
        fwrite(&header->LanternNightCooldown, sizeof(uint32_t), 1, fp);
        fwrite(&header->LanternNightGenuine, sizeof(uint8_t), 1, fp);
        fwrite(&header->LanternNightManual, sizeof(uint8_t), 1, fp);
        fwrite(&header->LanternNightNextNightIsGenuine, sizeof(uint8_t), 1, fp);
    }
    if (version >= 211) {
        fwrite(&header->TreetopSize, sizeof(uint32_t), 1, fp);
        fwrite(header->TreeTopVariations, sizeof(int32_t), header->TreetopSize, fp);
    }

    if (version >= 212) {
        fwrite(&header->ForceHalloweenForToday, sizeof(uint8_t), 1, fp);
        fwrite(&header->ForceXMasForToday, sizeof(uint8_t), 1, fp);
    }

    if (version >= 216) {
        fwrite(&header->SavedOreTiersCopper, sizeof(uint32_t), 1, fp);
        fwrite(&header->SavedOreTiersIron, sizeof(uint32_t), 1, fp);
        fwrite(&header->SavedOreTiersSilver, sizeof(uint32_t), 1, fp);
        fwrite(&header->SavedOreTiersGold, sizeof(uint32_t), 1, fp);
    }

    if (version >= 217) {
        fwrite(&header->BoughtCat, sizeof(uint8_t), 1, fp);
        fwrite(&header->BoughtDog, sizeof(uint8_t), 1, fp);
        fwrite(&header->BoughtBunny, sizeof(uint8_t), 1, fp);
    }

    if (version >= 223) {
        fwrite(&header->DownedEmpressOfLight, sizeof(uint8_t), 1, fp);
        fwrite(&header->DownedQueenSlime, sizeof(uint8_t), 1, fp);
    }

    if (version >= 240) {
        fwrite(&header->DownedDeerclops, sizeof(uint8_t), 1, fp);
    }

    if (version >= 250) {
        fwrite(&header->UnlockedSlimeBlueSpawn, sizeof(uint8_t), 1, fp);
    }

    if (version >= 251) {
        fwrite(&header->UnlockedMerchantSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedDemolitionistSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedPartyGirlSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedDyeTraderSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedTruffleSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedArmsDealerSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedNurseSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedPrincessSpawn, sizeof(uint8_t), 1, fp);
    }
    if (version >= 259){
        fwrite(&header->CombatBookVolumeTwoWasUsed, sizeof(uint8_t), 1, fp);
    }

    if (version >= 260){
        fwrite(&header->PeddlersSatchelWasUsed, sizeof(uint8_t), 1, fp);
    }

    if (version >= 261){
        fwrite(&header->UnlockedSlimeGreenSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedSlimeOldSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedSlimePurpleSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedSlimeRainbowSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedSlimeRedSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedSlimeYellowSpawn, sizeof(uint8_t), 1, fp);
        fwrite(&header->UnlockedSlimeCopperSpawn, sizeof(uint8_t), 1, fp);
    }

    if (version >= 264){
        fwrite(&header->FastForwardTimeToDusk, sizeof(uint8_t), 1, fp);
        fwrite(&header->MoondialCooldown, sizeof(uint8_t), 1, fp);
    }
    format->pointers[1] = ftell(fp);
}