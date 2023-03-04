#include "fs.h"
#include "../lib/string.h"
#include "../drivers/ata.h"
#include "../console.h"
#include "../lib/errno.h"

int stat(const char* name, struct stat *buf) {
    struct dir d;
    read_sectors_ATA_PIO((uint32_t)&d, kernel_size_in_offset, 1);
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
    read_sectors_ATA_PIO((uint32_t)buf, st.offset_sectors + kernel_size_in_offset, sectors);
    return sectors * sector_size;
}
