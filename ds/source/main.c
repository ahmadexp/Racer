// SPDX-License-Identifier: CC0-1.0

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <nds.h>
#include <fat.h>
#include <filesystem.h>

#include "../../source/generic/lodepng.h"

#include "../../source/projectspecific/meshes/track.h"
#include "../../source/projectspecific/meshes/trackturnl.h"
#include "../../source/projectspecific/meshes/trackturnr.h"

#define PI_F 3.14159265358979323846f

#define MAX_TRACK_X 10
#define MAX_TRACK_Z 10
#define RENDER_RADIUS 2
#define TRACK_TEX_SIZE 256

#define ENABLE_TRACK_LIGHTING 0
#define ENABLE_FAR_TILE_SKIP 0
#define FAR_SKIP_DISTANCE 2

static int track[MAX_TRACK_X][MAX_TRACK_Z] = {
    { 9, 2, 2, 2, 2, 2, 2, 10, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 3, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 3, 0, 0 },
    { 1, 0, 6, 4, 4, 4, 4, 11, 0, 0 },
    { 1, 0, 3, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 0, 5, 2, 2, 2, 2, 2, 2, 10 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
    { 12, 4, 4, 4, 4, 4, 4, 4, 4, 11 },
};

static int track_progress[MAX_TRACK_X][MAX_TRACK_Z];
static int track_length = 0;

static float player_x = 0.0f;
static float player_y = 0.04f;
static float player_z = 0.0f;
static float player_heading = 0.0f;
static float player_speed = 0.0f;
static float player_turn_velocity = 0.0f;
static float player_tilt = 0.0f;

static float camera_heading = 0.0f;
static float camera_roll = 0.0f;

static int lap_count = 0;
static float lap_time = 0.0f;
static float best_lap_time = 3600.0f;

static int frame_counter = 0;
static float hud_fps = 0.0f;
static float hud_fps_accum = 0.0f;
static int hud_fps_frames = 0;
static int track_texture = 0;
static int texture_ready = 0;
static int texture_from_png = 0;
static int filesystem_ready = 0;
static int using_nitrofs = 0;
static unsigned png_error_code = 0;
static int png_path_index = -1;
static u16 track_texture_data[TRACK_TEX_SIZE * TRACK_TEX_SIZE];

static int ds_hud_bg = -1;
static int ds_hud_ready = 0;
static u16 *ds_hud_map = NULL;
static u8 *ds_hud_tiles = NULL;
static PrintConsole ds_hud_console;

static u16 *speedbar_gfx = NULL;
static int speedbar_ready = 0;

#define DS_HUD_TILE_PANEL 240
#define DS_HUD_TILE_BORDER 241
#define DS_HUD_TILE_ROAD 242
#define DS_HUD_TILE_PLAYER 243
#define DS_HUD_TILE_FINISH 244

#define DS_HUD_MAP_X 20
#define DS_HUD_MAP_Y 6

static int player_track_position(void);
static void ds_hud_init(void);
static void ds_hud_draw_minimap(void);
static void ds_hud_draw_stats(void);
static void init_speedbar_overlay(void);
static void render_speedbar_overlay(void);

static int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

static int min_int(int a, int b)
{
    return (a < b) ? a : b;
}

static int tile_index(float v)
{
    return (int)floorf(v + 0.5f);
}

static int tile_rotation_steps(int tile)
{
    if (tile >= 1 && tile <= 4)
        return tile % 4;
    if (tile >= 5 && tile <= 8)
        return (tile - 5 + 2) % 4;
    if (tile >= 9 && tile <= 12)
        return (tile - 9 + 2) % 4;
    return 0;
}

static void ds_hud_fill_tile_4bpp(int tile_index, u8 color_index)
{
    u8 packed = (u8)(color_index | (color_index << 4));

    if (ds_hud_tiles == NULL)
        return;

    memset(ds_hud_tiles + tile_index * 32, packed, 32);
}

static void ds_hud_map_put(int x, int y, int tile)
{
    if (x < 0 || x >= 32 || y < 0 || y >= 24 || ds_hud_map == NULL)
        return;
    ds_hud_map[y * 32 + x] = (u16)(tile & 0x03FF);
}

static void ds_hud_printf_at(int x, int y, const char *fmt, ...)
{
    char line[64];
    va_list args;

    va_start(args, fmt);
    vsnprintf(line, sizeof(line), fmt, args);
    va_end(args);

    printf("\x1b[%d;%dH%-19s", y, x, line);
}

static void ds_hud_init(void)
{
    int x;
    int y;

    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&ds_hud_console, 3, BgType_Text4bpp, BgSize_T_256x256,
                2, 0, false, true);
    consoleSelect(&ds_hud_console);

    ds_hud_bg = ds_hud_console.bgId;
    ds_hud_map = bgGetMapPtr(ds_hud_bg);
    ds_hud_tiles = (u8 *)bgGetGfxPtr(ds_hud_bg);

    BG_PALETTE_SUB[0] = RGB15(3, 4, 5);
    BG_PALETTE_SUB[1] = RGB15(8, 9, 10);
    BG_PALETTE_SUB[2] = RGB15(15, 16, 18);
    BG_PALETTE_SUB[3] = RGB15(25, 25, 25);
    BG_PALETTE_SUB[4] = RGB15(31, 5, 5);
    BG_PALETTE_SUB[5] = RGB15(31, 29, 6);
    BG_PALETTE_SUB[15] = RGB15(31, 31, 31);

    ds_hud_fill_tile_4bpp(DS_HUD_TILE_PANEL, 1);
    ds_hud_fill_tile_4bpp(DS_HUD_TILE_BORDER, 2);
    ds_hud_fill_tile_4bpp(DS_HUD_TILE_ROAD, 3);
    ds_hud_fill_tile_4bpp(DS_HUD_TILE_PLAYER, 4);
    ds_hud_fill_tile_4bpp(DS_HUD_TILE_FINISH, 5);

    for (y = 0; y < 24; ++y) {
        for (x = 0; x < 32; ++x)
            ds_hud_map_put(x, y, DS_HUD_TILE_PANEL);
    }

    for (y = 0; y < 24; ++y) {
        ds_hud_map_put(19, y, DS_HUD_TILE_BORDER);
        ds_hud_map_put(31, y, DS_HUD_TILE_BORDER);
    }
    for (x = 19; x < 32; ++x) {
        ds_hud_map_put(x, 4, DS_HUD_TILE_BORDER);
        ds_hud_map_put(x, 18, DS_HUD_TILE_BORDER);
    }

    bgSetPriority(ds_hud_bg, 0);
    bgShow(ds_hud_bg);

    printf("\x1b[2J");
    ds_hud_ready = 1;
}

static void ds_hud_draw_minimap(void)
{
    int gx;
    int gz;
    int tx = tile_index(player_x);
    int tz = tile_index(player_z);

    if (!ds_hud_ready)
        return;

    for (gx = 0; gx < MAX_TRACK_X + 2; ++gx) {
        for (gz = 0; gz < MAX_TRACK_Z + 2; ++gz) {
            int map_x = DS_HUD_MAP_X + gz;
            int map_y = DS_HUD_MAP_Y + gx;
            int tile = DS_HUD_TILE_BORDER;

            if (gx > 0 && gx <= MAX_TRACK_X && gz > 0 && gz <= MAX_TRACK_Z) {
                int wx = gx - 1;
                int wz = gz - 1;
                int tx = MAX_TRACK_X - 1 - wx;

                tile = DS_HUD_TILE_PANEL;
                if (track[tx][wz] != 0) {
                    tile = DS_HUD_TILE_ROAD;
                    if (track_progress[tx][wz] == 0)
                        tile = DS_HUD_TILE_FINISH;
                }
            }

            ds_hud_map_put(map_x, map_y, tile);
        }
    }

    if (tx >= 0 && tx < MAX_TRACK_X && tz >= 0 && tz < MAX_TRACK_Z) {
        int px = DS_HUD_MAP_X + tz + 1;
        int py = DS_HUD_MAP_Y + (MAX_TRACK_X - 1 - tx) + 1;
        ds_hud_map_put(px, py, DS_HUD_TILE_PLAYER);
    }
}

static void ds_hud_draw_stats(void)
{
    int pos;
    char best_line[32];
    char fps_line[32];

    if (!ds_hud_ready)
        return;

    pos = player_track_position();

    if (best_lap_time >= 3599.0f) {
        snprintf(best_line, sizeof(best_line), "best: --.-- sec");
    } else {
        snprintf(best_line, sizeof(best_line), "best: %.2f sec", best_lap_time);
    }

    snprintf(fps_line, sizeof(fps_line), "fps: %.1f", hud_fps);

    ds_hud_printf_at(1, 2, "Racer [DS]");
    ds_hud_printf_at(1, 5, "track position: %d/%d", pos, track_length);
    ds_hud_printf_at(1, 7, "laps completed: %d", lap_count);
    ds_hud_printf_at(1, 9, "time: %.2f sec", lap_time);
    ds_hud_printf_at(1, 11, "%s", best_line);
    ds_hud_printf_at(1, 13, "%s", fps_line);
    ds_hud_printf_at(1, 15, "minimap ->");
}

static void reset_game(void)
{
    player_x = 0.0f;
    player_y = 0.04f;
    player_z = 0.0f;
    player_heading = 0.0f;
    player_speed = 0.0f;
    player_turn_velocity = 0.0f;
    player_tilt = 0.0f;

    camera_heading = player_heading;
    camera_roll = 0.0f;

    lap_count = 0;
    lap_time = 0.0f;
    best_lap_time = 3600.0f;
}

static void push_outside_void_side(float *pos_x, float *pos_z, float radius,
                                   float block_x, float block_z,
                                   float block_w, float block_h)
{
    if (*pos_x + radius <= block_x || *pos_x - radius >= block_x + block_w ||
        *pos_z + radius <= block_z || *pos_z - radius >= block_z + block_h) {
        return;
    }

    if (*pos_x < block_x || *pos_x > block_x + block_w ||
        *pos_z < block_z || *pos_z > block_z + block_h) {
        if (*pos_z > block_z && *pos_z < block_z + block_h) {
            if (*pos_x < block_x + block_w * 0.5f)
                *pos_x = block_x - radius;
            else
                *pos_x = block_x + block_w + radius;
        } else if (*pos_x > block_x && *pos_x < block_x + block_w) {
            if (*pos_z < block_z + block_h * 0.5f)
                *pos_z = block_z - radius;
            else
                *pos_z = block_z + block_h + radius;
        }
    }
}

static void push_outside_void_corner(float *pos_x, float *pos_z, float radius,
                                     float block_x, float block_z,
                                     float block_w, float block_h)
{
    float impact_x;
    float impact_z;
    float delta_x;
    float delta_z;
    float distance_sq;

    if (*pos_x + radius <= block_x || *pos_x - radius >= block_x + block_w ||
        *pos_z + radius <= block_z || *pos_z - radius >= block_z + block_h) {
        return;
    }

    if (!(*pos_x < block_x || *pos_x > block_x + block_w ||
          *pos_z < block_z || *pos_z > block_z + block_h)) {
        return;
    }

    if (*pos_x > block_x + block_w)
        impact_x = block_x + block_w;
    else if (*pos_x < block_x)
        impact_x = block_x;
    else
        impact_x = *pos_x;

    if (*pos_z > block_z + block_h)
        impact_z = block_z + block_h;
    else if (*pos_z < block_z)
        impact_z = block_z;
    else
        impact_z = *pos_z;

    delta_x = *pos_x - impact_x;
    delta_z = *pos_z - impact_z;
    distance_sq = delta_x * delta_x + delta_z * delta_z;

    if (distance_sq < radius * radius) {
        if (distance_sq > 0.000001f) {
            float distance = sqrtf(distance_sq);
            float scale = radius / distance;
            *pos_x = impact_x + delta_x * scale;
            *pos_z = impact_z + delta_z * scale;
        } else {
            float to_left = fabsf(*pos_x - block_x);
            float to_right = fabsf((block_x + block_w) - *pos_x);
            float to_top = fabsf(*pos_z - block_z);
            float to_bottom = fabsf((block_z + block_h) - *pos_z);

            if (to_left <= to_right && to_left <= to_top && to_left <= to_bottom)
                *pos_x = block_x - radius;
            else if (to_right <= to_top && to_right <= to_bottom)
                *pos_x = block_x + block_w + radius;
            else if (to_top <= to_bottom)
                *pos_z = block_z - radius;
            else
                *pos_z = block_z + block_h + radius;
        }
    }
}

static void clip_camera_to_track(float *cam_x, float *cam_z, float radius)
{
    int grid_x = (int)floorf(*cam_x);
    int grid_z = (int)floorf(*cam_z);
    int range = (int)floorf(radius) + 1;
    int x;
    int z;

    for (x = max_int(grid_x - range, -1); x < min_int(grid_x + range + 1, MAX_TRACK_X + 1); ++x) {
        for (z = max_int(grid_z - range, -1); z < min_int(grid_z + range + 1, MAX_TRACK_Z + 1); ++z) {
            if (x < 0 || x >= MAX_TRACK_X || z < 0 || z >= MAX_TRACK_Z || track[x][z] == 0) {
                push_outside_void_side(cam_x, cam_z, radius,
                                       (float)x - 0.6f, (float)z - 0.6f,
                                       1.2f, 1.2f);
                push_outside_void_corner(cam_x, cam_z, radius,
                                         (float)x - 0.6f, (float)z - 0.6f,
                                         1.2f, 1.2f);
            }
        }
    }
}

static void build_track_texture(void)
{
    int x, y;

    /* Build a deliberately high-contrast test texture for easy visibility. */
    for (y = 0; y < TRACK_TEX_SIZE; ++y) {
        for (x = 0; x < TRACK_TEX_SIZE; ++x) {
            int r = 7;
            int g = 7;
            int b = 7;
            int checker = ((x >> 3) ^ (y >> 3)) & 1;
            int idx = y * TRACK_TEX_SIZE + x;

            if (checker) {
                r = 14;
                g = 14;
                b = 14;
            }

            if ((x >= 30 && x <= 33) && ((y & 15) < 10)) {
                r = 25;
                g = 22;
                b = 4;
            }

            if (x < 4 || x >= (TRACK_TEX_SIZE - 4)) {
                if (((y >> 2) & 1) == 0) {
                    r = 29;
                    g = 29;
                    b = 29;
                } else {
                    r = 31;
                    g = 2;
                    b = 2;
                }
            }

            track_texture_data[idx] = RGB15(r, g, b) | BIT(15);
        }
    }
}

static int load_track_texture_from_png(void)
{
    static const char *paths[] = {
        "nitro:/tracktexture_ds.png",
        "nitro:/Media/tracktexture_ds.png",
        "nitro:/tracktexture.png",
        "nitro:/Media/tracktexture.png",
        "/tracktexture_ds.png",
        "/tracktexture.png",
        "Media/tracktexture_ds.png",
        "Media/tracktexture.png",
        "tracktexture_ds.png",
        "tracktexture.png",
        "fat:/Media/tracktexture_ds.png",
        "fat:/Media/tracktexture.png",
        "fat:/tracktexture_ds.png",
        "fat:/tracktexture.png",
    };
    unsigned char *png_data = NULL;
    size_t png_size = 0;
    unsigned char *rgb = NULL;
    unsigned w = 0;
    unsigned h = 0;
    unsigned err = 78;
    int p;
    int x, y;

    for (p = 0; p < (int)(sizeof(paths) / sizeof(paths[0])); ++p) {
        FILE *f = fopen(paths[p], "rb");
        long size_l;

        if (f == NULL)
            continue;

        if (fseek(f, 0, SEEK_END) != 0) {
            fclose(f);
            continue;
        }

        size_l = ftell(f);
        if (size_l <= 0) {
            fclose(f);
            continue;
        }

        rewind(f);
        png_size = (size_t)size_l;
        png_data = (unsigned char *)malloc(png_size);
        if (png_data == NULL) {
            fclose(f);
            err = 83;
            break;
        }

        if (fread(png_data, 1, png_size, f) != png_size) {
            fclose(f);
            free(png_data);
            png_data = NULL;
            png_size = 0;
            continue;
        }

        fclose(f);
        err = LodePNG_decode24(&rgb, &w, &h, png_data, png_size);
        free(png_data);
        png_data = NULL;
        png_size = 0;

        if (err == 0) {
            png_path_index = p;
            break;
        }
    }

    png_error_code = err;

    if (err != 0 || rgb == NULL || w == 0 || h == 0)
        return 0;

    for (y = 0; y < TRACK_TEX_SIZE; ++y) {
        unsigned sy = (unsigned)(((unsigned)y * h) / TRACK_TEX_SIZE);
        for (x = 0; x < TRACK_TEX_SIZE; ++x) {
            unsigned sx = (unsigned)(((unsigned)x * w) / TRACK_TEX_SIZE);
            unsigned src = (sy * w + sx) * 3;
            unsigned char r = rgb[src + 0];
            unsigned char g = rgb[src + 1];
            unsigned char b = rgb[src + 2];

            track_texture_data[y * TRACK_TEX_SIZE + x] =
                RGB15(r >> 3, g >> 3, b >> 3) | BIT(15);
        }
    }

    free(rgb);
    return 1;
}

static void setup_track_texture(void)
{
    int ok;
    int tex_param = TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T;

    texture_from_png = load_track_texture_from_png();
    if (!texture_from_png)
        build_track_texture();

    ok = glGenTextures(1, &track_texture);
    if (!ok || track_texture == 0) {
        texture_ready = 0;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, track_texture);
    texture_ready = glTexImageNtr2D(GL_RGBA,
                                    TRACK_TEX_SIZE, TRACK_TEX_SIZE,
                                    tex_param,
                                    track_texture_data, NULL);
}

static void setup_dashboard_texture(void)
{
    ds_hud_init();
}

static void build_track_progress(void)
{
    int x, z;

    for (x = 0; x < MAX_TRACK_X; ++x) {
        for (z = 0; z < MAX_TRACK_Z; ++z)
            track_progress[x][z] = 0;
    }

    track_length = 0;
    x = 0;
    z = 0;
    {
        int start_x = 0;
        int start_z = 0;
        int progress = 0;

        while (progress < MAX_TRACK_X * MAX_TRACK_Z) {
            track_progress[x][z] = progress;
            track_length = progress;
            progress++;

            if (x == start_x && z == start_z && progress > 1) {
                track_progress[x][z] = 0;
                track_length -= 1;
                break;
            }

            if (track[x][z] == 1)
                x++;
            else if (track[x][z] == 2)
                z--;
            else if (track[x][z] == 3)
                x--;
            else if (track[x][z] == 4)
                z++;
            else if (track[x][z] == 5)
                x--;
            else if (track[x][z] == 6)
                z++;
            else if (track[x][z] == 9)
                x++;
            else if (track[x][z] == 10)
                z--;
            else if (track[x][z] == 11)
                x--;
            else if (track[x][z] == 12)
                z++;

            if (x < 0 || x >= MAX_TRACK_X || z < 0 || z >= MAX_TRACK_Z)
                break;
        }
    }
}

static void setup_game(void)
{
    int i;

    for (i = 0; i < trackvertexcount; ++i)
        tracktexturearray[i * 2 + 1] = 1.0f - tracktexturearray[i * 2 + 1];
    for (i = 0; i < trackturnlvertexcount; ++i)
        trackturnltexturearray[i * 2 + 1] = 1.0f - trackturnltexturearray[i * 2 + 1];
    for (i = 0; i < trackturnrvertexcount; ++i)
        trackturnrtexturearray[i * 2 + 1] = 1.0f - trackturnrtexturearray[i * 2 + 1];

    for (i = 0; i < trackvertexcount * 3; ++i)
        trackvertexarray[i] /= 8.0f;
    for (i = 0; i < trackturnlvertexcount * 3; ++i)
        trackturnlvertexarray[i] /= 8.0f;
    for (i = 0; i < trackturnrvertexcount * 3; ++i)
        trackturnrvertexarray[i] /= 8.0f;

    build_track_progress();
    reset_game();
}

static int player_track_position(void)
{
    int tx = tile_index(player_x);
    int tz = tile_index(player_z);

    if (tx < 0 || tx >= MAX_TRACK_X || tz < 0 || tz >= MAX_TRACK_Z)
        return 0;
    return track_progress[tx][tz];
}

static int update_gameplay(float dt)
{
    int old_tile;
    int new_tile;
    float old_x = player_x;
    float old_z = player_z;
    int turning_left = 0;
    int turning_right = 0;
    float frame_scale;
    float speed_decay;
    float turn_decay;
    float camera_follow;

    if (dt <= 0.0f)
        dt = 1.0f / 60.0f;

    /* Clamp big stalls so a single hitch doesn't explode simulation state. */
    if (dt > 0.10f)
        dt = 0.10f;

    frame_scale = dt * 60.0f;
    speed_decay = powf(0.985f, frame_scale);
    turn_decay = powf(0.90f, frame_scale);
    camera_follow = 1.0f - powf(0.90f, frame_scale);

    hud_fps_accum += dt;
    hud_fps_frames++;
    if (hud_fps_accum >= 0.25f) {
        hud_fps = (float)hud_fps_frames / hud_fps_accum;
        hud_fps_accum = 0.0f;
        hud_fps_frames = 0;
    }

    scanKeys();
    {
        u16 held = keysHeld();
        u16 down = keysDown();
        int accelerating = 0;
        int braking = 0;

        if (down & KEY_START)
            return 1;

        if (held & (KEY_A | KEY_UP))
            accelerating = 1;
        if (held & (KEY_B | KEY_DOWN))
            braking = 1;

        if (accelerating)
            player_speed += 0.0003f * frame_scale;
        if (braking)
            player_speed -= 0.0012f * frame_scale;

        if (!accelerating && !braking)
            player_speed *= speed_decay;

        if (player_speed > 0.030f)
            player_speed = 0.030f;
        if (player_speed < -0.015f)
            player_speed = -0.015f;

        if (held & KEY_LEFT)
            turning_left = 1;
        if (held & KEY_RIGHT)
            turning_right = 1;
    }

    {
        float turn_input = (float)turning_right - (float)turning_left;
        if (turn_input != 0.0f)
            player_turn_velocity += turn_input * 0.00475f * frame_scale;

        player_heading += player_turn_velocity * frame_scale;
        player_turn_velocity *= turn_decay;
        player_tilt = player_turn_velocity * 15.0f;
    }

    /* mild active-steering speed scrub; avoids hard zeroing on small corrections */
    {
        float steering_amount = fabsf((float)turning_right - (float)turning_left);
        float turn_drag = fabsf(player_turn_velocity) * 0.0080f * frame_scale;
        if (steering_amount > 0.0f)
            turn_drag += 0.0000060f * frame_scale;
        if (turn_drag > 0.00035f)
            turn_drag = 0.00035f;
        if (player_speed > 0.0f) {
            player_speed -= turn_drag;
            if (player_speed < 0.0f) player_speed = 0.0f;
        }
    }

    player_x += cosf(player_heading) * player_speed * frame_scale;

    player_z += sinf(player_heading) * player_speed * frame_scale;

    clip_camera_to_track(&player_x, &player_z, 0.05f);

    {
        int tx = tile_index(player_x);
        int tz = tile_index(player_z);

        if (tx < 0 || tx >= MAX_TRACK_X || tz < 0 || tz >= MAX_TRACK_Z || track[tx][tz] == 0) {
            player_x = old_x;
            player_z = old_z;
            player_speed = 0.0f;
        }
    }

    {
        int old_tx = tile_index(old_x);
        int old_tz = tile_index(old_z);
        int new_tx = tile_index(player_x);
        int new_tz = tile_index(player_z);

        old_tile = track_progress[old_tx][old_tz];
        new_tile = track_progress[new_tx][new_tz];
    }

    lap_time += dt;

    camera_heading += (player_heading - camera_heading) * camera_follow;
    camera_roll += ((player_tilt * 0.85f) - camera_roll) * camera_follow;

    if (old_tile == track_length && new_tile == 0) {
        lap_count++;
        if (lap_time < best_lap_time)
            best_lap_time = lap_time;
        lap_time = 0.0f;
    } else if (old_tile == 0 && new_tile == track_length) {
        lap_count--;
        lap_time = best_lap_time;
    }

    return 0;
}

static void draw_mesh_local(const float *vertices, const float *texcoords,
                            const float *normals, int vertex_count)
{
    int v;

    glBegin(GL_TRIANGLES);
    for (v = 0; v < vertex_count; ++v) {
        int vi = v * 3;
        int ti = v * 2;

        if (texcoords) {
            glTexCoord2f(texcoords[ti + 0], texcoords[ti + 1]);
        }
        if (ENABLE_TRACK_LIGHTING)
            glNormal3f(normals[vi + 0], normals[vi + 1], normals[vi + 2]);
        glVertex3f(vertices[vi + 0], vertices[vi + 1], vertices[vi + 2]);
    }

    glEnd();
}

static void render_scene(void)
{
    int cx = tile_index(player_x);
    int cz = tile_index(player_z);
    int min_x = cx - RENDER_RADIUS;
    int max_x = cx + RENDER_RADIUS;
    int min_z = cz - RENDER_RADIUS;
    int max_z = cz + RENDER_RADIUS;
    int x, z;
    float view_dir_x;
    float view_dir_z;

    if (min_x < 0)
        min_x = 0;
    if (min_z < 0)
        min_z = 0;
    if (max_x >= MAX_TRACK_X)
        max_x = MAX_TRACK_X - 1;
    if (max_z >= MAX_TRACK_Z)
        max_z = MAX_TRACK_Z - 1;

    glLoadIdentity();

    view_dir_x = cosf(camera_heading);
    view_dir_z = sinf(camera_heading);

    {
        float cam_back = 0.15f;
        float cam_up = 0.04f;
        float look_ahead = 0.50f;
        float roll_s;
        float roll_c;
        float up_x;
        float up_y;
        float up_z;

        float cam_x = player_x - cosf(camera_heading) * cam_back;
        float cam_y = player_y + cam_up;
        float cam_z = player_z - sinf(camera_heading) * cam_back;
        float look_x = player_x + cosf(camera_heading) * look_ahead;
        float look_z = player_z + sinf(camera_heading) * look_ahead;

        clip_camera_to_track(&cam_x, &cam_z, 0.08f);

        roll_s = sinf(camera_roll);
        roll_c = cosf(camera_roll);

        up_x = -sinf(camera_heading) * roll_s;
        up_y = roll_c;
        up_z = cosf(camera_heading) * roll_s;

        gluLookAt(cam_x, cam_y, cam_z,
                  look_x, player_y + 0.01f, look_z,
                  up_x, up_y, up_z);
    }

    glColor(RGB15(31, 31, 31));
    if (texture_ready) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, track_texture);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
#if ENABLE_TRACK_LIGHTING
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK |
              POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1 | POLY_MODULATION);
#else
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_MODULATION);
#endif

    for (x = min_x; x <= max_x; ++x) {
        for (z = min_z; z <= max_z; ++z) {
            int t = track[x][z];
            int rot = tile_rotation_steps(t);
#if ENABLE_FAR_TILE_SKIP
            int dx = x - cx;
            int dz = z - cz;

            if ((dx * dx + dz * dz) >= (FAR_SKIP_DISTANCE * FAR_SKIP_DISTANCE)) {
                if (((x + z + frame_counter) & 1) != 0)
                    continue;
            }
#endif

            if (t == 0)
                continue;

            {
                float rel_x = ((float)x + 0.5f) - player_x;
                float rel_z = ((float)z + 0.5f) - player_z;
                float dist_sq = rel_x * rel_x + rel_z * rel_z;
                float forward_dot = rel_x * view_dir_x + rel_z * view_dir_z;

                /* Cull distant tiles that are behind the camera. */
                if (dist_sq > 2.25f && forward_dot < -0.15f)
                    continue;
            }

            glPushMatrix();
            glTranslatef((float)x, 0.0f, (float)z);
            glRotateY(rot * 90);

            if (t >= 1 && t <= 4) {
                draw_mesh_local(trackvertexarray, tracktexturearray,
                                tracknormalarray, trackvertexcount);
            } else if (t >= 5 && t <= 8) {
                draw_mesh_local(trackturnlvertexarray, trackturnltexturearray,
                                trackturnlnormalarray,
                                trackturnlvertexcount);
            } else if (t >= 9 && t <= 12) {
                draw_mesh_local(trackturnrvertexarray, trackturnrtexturearray,
                                trackturnrnormalarray,
                                trackturnrvertexcount);
            }

            glPopMatrix(1);
        }
    }

    //glDisable(GL_TEXTURE_2D);
}

static void render_hud_subscreen(void)
{
    if ((frame_counter & 1) == 0)
        ds_hud_draw_minimap();
    if ((frame_counter & 3) == 0)
        ds_hud_draw_stats();

    bgUpdate();
}

static void init_speedbar_overlay(void)
{
    int i;
    u16 packed;

    oamInit(&oamMain, SpriteMapping_1D_32, false);
    speedbar_gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_16Color);
    if (speedbar_gfx == NULL) {
        speedbar_ready = 0;
        return;
    }

    packed = (u16)(1 | (1 << 4) | (1 << 8) | (1 << 12));
    for (i = 0; i < 16; ++i)
        speedbar_gfx[i] = packed;

    /* Palette bank 0: dim, 1: green, 2: orange/red. */
    SPRITE_PALETTE[0 * 16 + 0] = RGB15(0, 0, 0);
    SPRITE_PALETTE[0 * 16 + 1] = RGB15(8, 8, 8);
    SPRITE_PALETTE[1 * 16 + 0] = RGB15(0, 0, 0);
    SPRITE_PALETTE[1 * 16 + 1] = RGB15(8, 24, 8);
    SPRITE_PALETTE[2 * 16 + 0] = RGB15(0, 0, 0);
    SPRITE_PALETTE[2 * 16 + 1] = RGB15(30, 12, 6);

    speedbar_ready = 1;
}

static void render_speedbar_overlay(void)
{
    const int base_x = 152;
    const int base_y = 184;
    static const u8 bar_blocks[10] = { 1, 1, 1, 1, 2, 2, 2, 3, 3, 3 };
    float speed_norm;
    int i;
    int sprite_id = 0;

    if (!speedbar_ready)
        return;

    speed_norm = fabsf(player_speed) / 0.030f;
    if (speed_norm > 1.0f)
        speed_norm = 1.0f;

    for (i = 0; i < 10; ++i) {
        int x = base_x + i * 10;
        int lit = ((float)(i + 1) / 10.0f) <= speed_norm;
        int pal = 0;
        int b;

        if (lit)
            pal = (i < 7) ? 1 : 2;

        for (b = 0; b < bar_blocks[i]; ++b) {
            int y = base_y - 7 - b * 8;

            oamSet(&oamMain, sprite_id,
                   x, y,
                   0,
                   pal,
                   SpriteSize_8x8, SpriteColorFormat_16Color,
                   speedbar_gfx,
                   -1,
                   false,
                   false,
                   false, false,
                   false);
            sprite_id++;
        }
    }

    while (sprite_id < 32) {
        oamSet(&oamMain, sprite_id,
               0, 0,
               0,
               0,
               SpriteSize_8x8, SpriteColorFormat_16Color,
               speedbar_gfx,
               -1,
               false,
               true,
               false, false,
               false);
        sprite_id++;
    }

    oamUpdate(&oamMain);
}

int main(int argc, char **argv)
{
    unsigned frame_ticks;
    float dt;

    (void)argc;
    (void)argv;

    using_nitrofs = nitroFSInit((argc > 0) ? argv[0] : NULL);
    if (using_nitrofs) {
        filesystem_ready = 1;
    } else {
        filesystem_ready = fatInitDefault();
    }

    videoSetMode(MODE_0_3D);

    /* Reserve VRAM banks A/B for 3D textures before texture upload. */
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankG(VRAM_G_MAIN_SPRITE);

    glInit();

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);
    glViewport(0, 0, 255, 191);

    glMaterialf(GL_AMBIENT, RGB15(7, 7, 7));
    glMaterialf(GL_DIFFUSE, RGB15(23, 23, 23));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

#if ENABLE_TRACK_LIGHTING
    glLight(0, RGB15(31, 28, 24), floattov10(-0.65f), floattov10(0.75f), floattov10(-0.1f));
    glLight(1, RGB15(7, 9, 12), floattov10(0.30f), floattov10(0.2f), floattov10(0.9f));
#endif

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.05f, 48.0f);

    glMatrixMode(GL_MODELVIEW);

    setup_track_texture();
    setup_dashboard_texture();
    setup_game();
    init_speedbar_overlay();

    cpuStartTiming(0);

    while (1) {
        frame_ticks = cpuEndTiming();
        dt = (float)timerTicks2usec(frame_ticks) * 0.000001f;
        cpuStartTiming(0);

        if (update_gameplay(dt))
            break;

        render_scene();
        render_speedbar_overlay();
        glFlush(GL_ZBUFFERING);

        frame_counter++;
        render_hud_subscreen();
        swiWaitForVBlank();
    }

    return 0;
}
