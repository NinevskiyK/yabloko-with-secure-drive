#include "fs.h"
#include "../lib/string.h"
#include "../drivers/ata.h"
#include "../console.h"
#include "../lib/errno.h"

static void read_sectors(void* buf, uint32_t offset, uint8_t sectors_count) {
    read_sectors_ATA_PIO((uint32_t)buf, mbr_size_in_offset + offset, sectors_count);
}

int stat(const char* name, struct stat *buf) {
    struct dir d;
    read_sectors(&d, 0, 1);
    for (int i = 0; i < ents_in_dir; ++i) {
        if (!strcmp(d.entries[i].name, name)) {
            buf->size = d.entries[i].size_bytes;
            buf->offset_sectors = d.entries[i].offset_sectors;
            return 0;
        }
    }
    m_errno = ENTRY_NOT_FOUND_ERR;
    return 1;
}

static void decrypt_block(void *buf, uint32_t *prev) {
    uint32_t *block = buf;
    uint32_t temp = *prev;
    *prev = *block;
    asm("mov %%cr3, %%eax\n\t"
        "xor %%eax, %%ebx\n\t" : "=b"(*block) : "b"(*block) : "eax");
    *block ^= temp;
}

static void decrypt_sector(void *buf, uint32_t *prev) {
    for (int block = 0; block < blocks_in_sector; ++block) {
        decrypt_block(buf + block * block_size, prev);
    }
}

uint32_t get_iv() {
    struct dir d;
    read_sectors(&d, 0, 1);
    return d.init_vector;
}

static void decrypt_sectors(void* buf, uint8_t sectors_count) {
    uint32_t prev = get_iv();
    for (uint8_t sector = 0; sector < sectors_count; ++sector) {
        decrypt_sector(buf + sector * sector_size, &prev);
    }
}

int read_file(const char* name, void* buf, uint32_t bufsize) {
    struct stat st;
    if (stat(name, &st)) {
        return -1;
    }
    uint8_t sectors = (st.size + sector_size - 1) / sector_size;
    if (bufsize < sectors * sector_size) {
        m_errno = SMALL_BUF_ERR;
        return -1;
    }
    read_sectors(buf, st.offset_sectors, sectors);
    decrypt_sectors(buf, sectors);
    return sectors * sector_size;
}
