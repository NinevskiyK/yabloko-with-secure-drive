#include "../fs/fs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

char* basename(char* path) {
    char* c = strrchr(path, '/');
    if (c && *c) {
        return c + 1;
    }
    return path;
}

void encrypt_block(char *sector, uint32_t key, uint32_t *prev) {
    uint32_t *block = (uint32_t *)sector;
    *block ^= *prev;
    *block ^= key;
    *prev = *block;
}

void encrypt_sector(char *sector, uint32_t key, uint32_t *prev) {
    for (int i = 0; i < blocks_in_sector; ++i) {
        encrypt_block(sector + i * block_size, key, prev);
    }
}

int main(int argc, char* argv[]) {
    char sector[sector_size];
    struct dir dir = {{0}};

    if (argc < 5) {
        fprintf(stderr, "Usage: %s IV ENC_KEY OUT.FS KERNEL.BIN [FILES...]\n", argv[0]);
        return 1;
    }

    errno = 0;
    dir.init_vector = strtol(argv[1], NULL, 10);
    uint32_t key = strtol(argv[2], NULL, 10);
    if (errno) {
        perror("strtol: ");
        return 1;
    }

    FILE* image = fopen(argv[3], "wb");
    if (!image) {
        perror(argv[3]);
        return 1;
    }

    if (fwrite(&dir, sizeof(dir), 1, image) < 1) {
        perror("fwrite");
        return 1;
    }
    uint32_t sector_offset = 1;

    for (int i = 4; i < argc; ++i) {
        uint32_t prev_block = dir.init_vector;
        char* name = argv[i];
        struct dirent *dirent = &dir.entries[i-4];
        dirent->offset_sectors = sector_offset;
        dirent->size_bytes = 0;

        FILE* file = fopen(name, "rb");
        if (!file) {
            perror(name);
            return 1;
        }

        size_t read_size;
        while ((read_size = fread(sector, 1, sizeof(sector), file))) {
            if (i != 4) {
                encrypt_sector(sector, key, &prev_block);
            }
            if (fwrite(sector, 1, sizeof(sector), image) != sizeof(sector)) {
                perror(name);
                return 1;
            }
            sector_offset++;
            dirent->size_bytes += read_size;
        }

        if (fclose(file)) {
            perror(name);
            return 1;
        }

        dirent->reserved = 0;
        dirent->name[sizeof(dirent->name) - 1] = '\0';
        strncpy(dirent->name, basename(name), sizeof(dirent->name) - 1);
    }

    fseek(image, 0, SEEK_SET);
    if (fwrite(&dir, sizeof(dir), 1, image) < 1) {
        perror("fwrite");
        return 1;
    }

    if (fclose(image)) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}
