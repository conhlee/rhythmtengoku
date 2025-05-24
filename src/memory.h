#pragma once

#include "global.h"
#include "levels.h"

#define SAVE_BUFFER_SIZE_ORIG (0x3B04) // CONHLEE
#define SAVE_BUFFER_SIZE (sizeof(struct SaveBuffer))

#define EXTR_MAGIC (0x42000069)

extern struct SaveBuffer {
    /* [0x000] Header */
    struct SaveBufferHeader {
        char RIQ[4]; // "RIQ"
        u32 bufferSize; // CONHLEE: this isn't even used :(
        u32 checksum; // CONHLEE: checksum will be generated on original data ONLY
        u32 unkC; // 0x26040000
    } header;
    /* [0x010] Game Save Data */
    struct TengokuSaveData {
        s8 gsCursorX, gsCursorY;
        s8 recentLevelX, recentLevelY;
        s8 recentLevelState;
        u8 recentLevelClearedByBarista;
        u8 levelStates[TOTAL_LEVELS_ORIG]; // CONHLEE
        u16 recentLevelScore;
        u16 levelScores[TOTAL_LEVELS_ORIG]; // CONHLEE
        u16 currentFlow;
        u16 unkB0;
        u8 totalSongs;
        u8 unkB3; // above
        struct StudioSongData {
            u8 songID;
            s8 replayID;
            u8 drumKitID;
            u8 unk3;
        } studioSongs[45 + 10];
        u8 levelTotalPlays[TOTAL_LEVELS_ORIG]; // CONHLEE
        u8 levelFirstOK[TOTAL_LEVELS_ORIG]; // CONHLEE
        u8 levelFirstSuperb[TOTAL_LEVELS_ORIG]; // CONHLEE
        u8 totalPerfects;
        u8 campaignsCleared[TOTAL_PERFECT_CAMPAIGNS_ORIG]; // CONHLEE
        u8 campaignState;
        u8 campaignAttemptsLeft;
        u8 playsUntilNextCampaign;
        u8 currentCampaign;
        u8 unk26A;
        u8 readingMaterialUnlocked[20];
        u8 drumKitsUnlocked[15];
        u8 totalMedals;
        u8 unk28F;
        u8 minFailsForBaristaHelp;
        u8 unk291;
        u32 unk294[16];
        /*
            u32 highScoreMrUpbeat;
            u32 highScoreMannequinFactory;
            u32 highScoreSickBeatsSP;
            u32 highScoreQuizShowEX;
            u32 unk2A4;
            u32 unk2A8;
            u32 unk2AC;
            u32 unk2B0;
            u32 soundMode;
            u32 perfectClearCafeDialoguePending;
            u32 unk2BC;
            u32 unk2C0;
            u32 unk2C4;
            u32 unk2C8;
            u32 unk2CC;
            u32 unk2D0;
        */
        struct DrumReplaySaveAlloc {
            u8 nextAvailableBlock;
            u8 replayStartBlocks[10];
            u16 replaySizes[10];
            u8 saveMemory[0x38][0x100];
        } drumReplaysAlloc;

        u32 extrMagic; // CONHLEE: compare to EXTR_MAGIC
        u16 extrReserved; // CONHLEE: for future use ..

        u8 additionalLevelCount; // CONHLEE: will equ TOTAL_LEVELS_ADDI
        u8 additionalCampaignCount; // CONHLEE: will equ TOTAL_PERFECT_CAMPAIGNS_ADDI

        struct __attribute__((__packed__, __aligned__(1))) AdditionalLevel {
            // reading score as U16 could trigger unaligned read
            union {
                u8 asU8[2];
                // u16 asU16;
            } uScore;
            u8 state;
            u8 totalPlays;
            u8 firstOK;
            u8 firstSuperb;
            u8 myCampaignCleared;
        } additionalLevel[TOTAL_LEVELS_ADDI];

        // CONHLEE: don't actually read from this; use additionalLevelCount
        //          to calculate the addr of this field like so:
        //          (u8*)(savedata->additionalLevel + savedata->additionalLevelCount)
        u8 additionalCampaignsCleared[TOTAL_PERFECT_CAMPAIGNS_ADDI];
    } data;
} *D_030046a8;

// CONHLEE
static inline u8 savedata_get_levelstate(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        return savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].state;
    }
    return savedata->levelStates[levelIdx];
}

// CONHLEE
static inline void savedata_set_levelstate(struct TengokuSaveData *savedata, u8 levelIdx, u8 state) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].state = state;
        return;
    }
    savedata->levelStates[levelIdx] = state;
}

// CONHLEE
static inline u16 savedata_get_levelscore(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        u8* scoreBytes = savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].uScore.asU8;

        u16 score = ((u16)scoreBytes[1] << 8) | scoreBytes[0];
        return score;
    }
    return savedata->levelScores[levelIdx];
}

// CONHLEE
static inline void savedata_set_levelscore(struct TengokuSaveData *savedata, u8 levelIdx, u16 score) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        u8* scoreBytes = savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].uScore.asU8;

        scoreBytes[0] = (u8)(score & 0xFF);
        scoreBytes[1] = (u8)((score >> 8) & 0xFF);
        return;
    }
    savedata->levelScores[levelIdx] = score;
}

// CONHLEE
static inline u8 savedata_get_level_totalplays(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        return savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].totalPlays;
    }
    return savedata->levelTotalPlays[levelIdx];
}
static inline u8 savedata_get_level_firstok(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        return savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].firstOK;
    }
    return savedata->levelFirstOK[levelIdx];
}
static inline u8 savedata_get_level_firstsuperb(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        return savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].firstSuperb;
    }
    return savedata->levelFirstSuperb[levelIdx];
}

// CONHLEE
static inline void savedata_incr_level_totalplays(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].totalPlays++;
        return;
    }
    savedata->levelTotalPlays[levelIdx]++;
}

// CONHLEE
static inline void savedata_update_level_firstok(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        u8 totalPlays = savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].totalPlays;
        savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].firstOK = totalPlays;
        return;
    }
    savedata->levelFirstOK[levelIdx] = savedata->levelTotalPlays[levelIdx];
}
static inline void savedata_update_level_firstsuperb(struct TengokuSaveData *savedata, u8 levelIdx) {
    if (levelIdx >= TOTAL_LEVELS_ORIG) {
        u8 totalPlays = savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].totalPlays;
        savedata->additionalLevel[levelIdx - TOTAL_LEVELS_ORIG].firstSuperb = totalPlays;
        return;
    }
    savedata->levelFirstSuperb[levelIdx] = savedata->levelTotalPlays[levelIdx];
}

// CONHLEE
static inline u8 savedata_get_campaign_cleared(struct TengokuSaveData *savedata, u8 campaignIdx) {
    if (campaignIdx >= TOTAL_LEVELS_ORIG) {
        u8* additionalCampaignsCleared = (u8*)(savedata->additionalLevel + savedata->additionalLevelCount);
        return additionalCampaignsCleared[campaignIdx - TOTAL_LEVELS_ORIG];
    }
    return savedata->campaignsCleared[campaignIdx];
}
static inline void savedata_set_campaign_cleared(struct TengokuSaveData *savedata, u8 campaignIdx, u8 cleared) {
    if (campaignIdx >= TOTAL_LEVELS_ORIG) {
        u8* additionalCampaignsCleared = (u8*)(savedata->additionalLevel + savedata->additionalLevelCount);
        additionalCampaignsCleared[campaignIdx - TOTAL_LEVELS_ORIG] = cleared;
        return;
    }
    savedata->campaignsCleared[campaignIdx] = cleared;
}

extern void init_ewram(void);
extern void *get_save_buffer_start(void);
extern void *get_save_buffer_end(void);
extern void *get_memory_heap_start(void);
extern u32 get_memory_heap_length(void);
extern s32 generate_save_buffer_checksum(s32 *buffer, u32 size);
extern void init_save_buffer(void);
extern void clear_save_data(void);
extern s32 copy_to_save_buffer(u8 *cartRAM);
extern s32 copy_sram_to_save_buffer(void);
extern s32 copy_sram_backup_to_save_buffer(void);
extern void flush_save_buffer(u8 *cartRAM);
extern s32 get_offset_from_save_buffer(void *buffer);
extern void write_save_buffer_header_to_sram(u8 *cartRAM);
extern void write_save_buffer_data_to_sram(u8 *buffer, u32 size);
extern void flush_save_buffer_to_sram(void);
extern void flush_save_buffer_to_sram_backup(void);
extern void func_080009c8_stub(void);
extern void func_080009cc_stub(void);
extern s32 func_080009d0(s16 *);
extern s32 func_080009fc(void);
