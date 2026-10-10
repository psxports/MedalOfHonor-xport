#include "game_draft_signatures.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sint32 asset_index_name(const char *source, char *destination, size_t capacity)
{
    size_t count = 0u;
    unsigned char character;
    while (*source == '/' || *source == '\\') ++source;
    while ((character = (unsigned char)*source++) != 0u && character != ';')
    {
        if (count + 1u >= capacity) return 0;
        if (character == '\\') character = '/';
        if (character >= 'a' && character <= 'z') character -= 'a' - 'A';
        destination[count++] = (char)character;
    }
    destination[count] = 0;
    return count != 0u;
}

uint32 moh_native_asset_lba(const char *name)
{
    char wanted[1024], path[MAX_PATH], line[1400], indexed[1024];
    char *separator, *lba_text, *size_text, *end;
    DWORD length;
    unsigned long lba;
    FILE *index;
    if (!name || !asset_index_name(name, wanted, sizeof(wanted))) return 0u;
    length = GetModuleFileNameA(NULL, path, sizeof(path));
    if (!length || length >= sizeof(path)) return 0u;
    separator = strrchr(path, '\\');
    if (!separator || (size_t)(separator - path) + sizeof("\\DATA\\native-file-index.tsv") > sizeof(path)) return 0u;
    strcpy(separator, "\\DATA\\native-file-index.tsv");
    index = fopen(path, "rb");
    if (!index) return 0u;
    while (fgets(line, sizeof(line), index))
    {
        lba_text = strchr(line, '\t');
        if (!lba_text) continue;
        *lba_text++ = 0;
        size_text = strchr(lba_text, '\t');
        if (!size_text) continue;
        *size_text = 0;
        if (!asset_index_name(line, indexed, sizeof(indexed)) || strcmp(indexed, wanted)) continue;
        lba = strtoul(lba_text, &end, 10);
        fclose(index);
        return *end == 0 && lba != 0ul ? (uint32)lba : 0u;
    }
    fclose(index);
    return 0u;
}
