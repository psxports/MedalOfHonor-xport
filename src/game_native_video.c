#include "psx.h"
#include "game_shell_signatures.h"
#include "psx_press.h"
#include "psx_stream.h"
#include "psx_spu.h"
#include "psx_gpu.h"
#include <stdlib.h>
#include <string.h>

static uint32 video_read32(const uint8 *p)
{
    return p[0] | (uint32)p[1] << 8 | (uint32)p[2] << 16 | (uint32)p[3] << 24;
}

uint32 moh_native_play_movie(uint32 media)
{
    char path[256], local_path[272];
    uint8 raw[2352], prefix[44];
    uint32 last = 0u, i, frame = 0u, skipped = 0u;
    uint32 *ring, *vlc, *pixels, *saved_vram, *data, *header, block[192];
    PSX_RECT video_area = {0, 0, 960, 480};
    DISPENV saved_display, movie_display;
    FILE *file;
    CdlLOC position;
    SpuCommonAttr saved_audio = {0}, movie_audio = {0};
    SpuReverbControl saved_control;
    uint64 deadline;
    uint32 prior_buttons = xport_input_read(0);
    for (i = 0u; i < sizeof(path) - 1u; ++i) {
        path[i] = (char)r_u8(media + i);
        if (!path[i]) break;
    }
    path[sizeof(path) - 1u] = 0;
    snprintf(local_path, sizeof(local_path), "DATA/%s", path);
    file = xport_fopen(local_path, "rb");
    if (!file) moh_draft_fail_fast("Movie file unavailable", media);
    if (fread(prefix, 1u, sizeof(prefix), file) != sizeof(prefix) ||
        memcmp(prefix, "RIFF", 4u) || memcmp(prefix + 8, "CDXA", 4u))
        moh_draft_fail_fast("Movie CD-XA container", media);
    while (fread(raw, 1u, sizeof(raw), file) == sizeof(raw)) {
        if (raw[24] == 0x60u && raw[25] == 1u) {
            uint32 number = video_read32(raw + 32);
            if (number > last) last = number;
        }
    }
    fclose(file);
    if (!last || !cd_mount_stream_file(local_path)) moh_draft_fail_fast("Movie stream resource", media);
    ring = (uint32 *)calloc(256u, 2048u);
    vlc = (uint32 *)calloc(65537u, sizeof(uint32));
    pixels = (uint32 *)calloc(640u * 480u, 3u);
    saved_vram = (uint32 *)calloc(640u * 480u, 3u);
    if (!ring || !vlc || !pixels || !saved_vram) moh_draft_fail_fast("Movie native buffers", media);
    memcpy(&saved_display, moh_draft_guest_pointer(r_u32(0x8007BF4Cu) + 92u, sizeof(saved_display)), sizeof(saved_display));
    StoreImage(&video_area, saved_vram);
    DecDCTReset(0);
    DecDCTvlcSize(0);
    SpuGetCommonAttr(&saved_audio);
    spu_reverb_export(&saved_control);
    saved_audio.mask = SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX;
    saved_audio.cd.mix = (sint32)(saved_control.control & 1u);
    movie_audio.mask = saved_audio.mask;
    movie_audio.cd.volume.left = movie_audio.cd.volume.right =
        (sint16)(r_u32(0x80038654u) >= 4u ? 0x3FFFu : r_u32(0x80038654u) * 4096u);
    movie_audio.cd.mix = 1;
    SpuSetCommonAttr(&movie_audio);
    StSetRing(ring, 256u);
    StSetStream(0u, 0u, last + 1u, NULL, NULL);
    stream_set_read_mode(0xC0u);
    CdIntToPos(0, &position);
    if (!CdControl(CdlSeekL, (uint8 *)&position, NULL)) moh_draft_fail_fast("Movie stream seek", media);
    CdControl(CdlDemute, NULL, NULL);
    SsSetSerialVol(SS_SERIAL_A, 127, 127);
    if (!CdRead2(0xC0)) moh_draft_fail_fast("Movie CD read", media);
    deadline = xport_timer_get() + 5000000u;
    fprintf(stderr, "Movie started: %s, final frame %u\n", path, last);
    while (frame < last && !xport_isquit()) {
        uint32 buttons;
        while (stream_pump() > 0) {}
        if (!StGetNext(&data, &header)) {
            StHEADER *info = (StHEADER *)header;
            uint32 width = info->width, height = info->height, x, y, bx, by;
            frame = info->frameCount;
            deadline = xport_timer_get() + 5000000u;
            if (!width || !height || width > 640u || height > 480u ||
                (width & 15u) || (height & 15u)) moh_draft_fail_fast("Movie frame dimensions", media);
            if (DecDCTvlc(data, vlc)) moh_draft_fail_fast("Movie VLC continuation", media);
            StFreeRing(data);
            DecDCTin(vlc, 1);
            /* MDEC emits macroblocks down each sixteen-pixel column */
            for (bx = 0u; bx < width; bx += 16u)
                for (by = 0u; by < height; by += 16u) {
                    const uint8 *rgb = (const uint8 *)block;
                    DecDCTout(block, 192);
                    for (y = 0u; y < 16u; ++y)
                        for (x = 0u; x < 16u; ++x) {
                            uint32 offset = (y * 16u + x) * 3u;
                            uint8 *destination = (uint8 *)pixels + ((by + y) * width + bx + x) * 3u;
                            memcpy(destination, rgb + offset, 3u);
                        }
                }
            {
                PSX_RECT frame_area = {0, 0, (sint16)(width * 3u / 2u), (sint16)height};
                SetDefDispEnv(&movie_display, 0, 0, (sint32)width, (sint32)height);
                movie_display.isrgb24 = 1u;
                LoadImagePSX(&frame_area, pixels);
                PutDispEnv(&movie_display);
                /* VSync presents the same VRAM frame between decoded frames */
                gpu_present();
            }
        }
        xport_poll();
        buttons = xport_input_read(0);
        if ((buttons & ~prior_buttons) & 0xF840u) { skipped = 1u; break; }
        prior_buttons = buttons;
        if (xport_timer_get() > deadline) moh_draft_fail_fast("Movie stream timeout", media);
        VSync(0);
    }
    CdControl(CdlPause, NULL, NULL);
    SpuSetCommonAttr(&saved_audio);
    StUnSetRing();
    DecDCTReset(0);
    LoadImagePSX(&video_area, saved_vram);
    PutDispEnv(&saved_display);
    free(saved_vram); free(pixels); free(vlc); free(ring);
    fprintf(stderr, "Movie ended: %s, frame %u, skipped %u\n", path, frame, skipped);
    return skipped;
}

uint32 moh_native_movie_screen(uint32 screen)
{
    uint32 group, media, next, skipped;
    const char *name = (const char *)moh_draft_guest_pointer(screen, 64u);
    if (strncmp(name, "m_movie", 7u)) return 0u;
    group = r_u32(screen + 64u);
    media = group ? r_u32(group + 160u) : 0u;
    if (!media) moh_draft_fail_fast("Movie screen media", screen);
    skipped = moh_native_play_movie(media);
    next = r_u32(r_u32(screen + 28u) + 60u);
    if (skipped && (!strcmp(name, "m_movie_ea") || !strcmp(name, "m_movie_dwi") || !strcmp(name, "m_movie_moh")))
        next = shell_title_find_screen(0u, "m_title");
    if (!strncmp(name, "m_movie_b", 9u) && r_u8(0x8007BFD0u)) {
        next = shell_title_find_screen(0u, "m_title"); w_u8(0x8007BFD0u, 0u);
    } else if (!strncmp(name, "m_movie_g", 9u) && r_u8(0x8007BFD1u)) {
        next = shell_title_find_screen(0u, "m_pmeval"); w_u8(0x8007BFD1u, 0u);
    } else if (strncmp(name, "m_movie_b", 9u) && strncmp(name, "m_movie_g", 9u) && r_u8(0x8007C006u) == 0u &&
        (r_u8(0x8007C007u) == 1u || r_u8(0x8007C007u) == 2u)) {
        next = shell_title_find_screen(0u, "m_postmis");
    } else if (strncmp(name, "m_movie_b", 9u) && strncmp(name, "m_movie_g", 9u) && r_u8(0x8007C007u) == 3u) {
        next = shell_title_find_screen(0u, "m_movie_mov_medal"); w_u8(0x8007C007u, 2u);
    }
    if (!next) moh_draft_fail_fast("Movie successor screen", screen);
    w_u32(0x8007BA68u, next);
    return 1u;
}
