#include "platform.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "gamebuino.h"
#include "esp_heap_caps.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// the keys without the library's poll, and its power off, see PollKeys
#include "gb_ll_expander.h"
#include "gb_ll_i2c.h"

gb_core core;

static uint32_t NextFrameTime = 0;

void PlatformInit()
{
    core.init();
}

uint32_t GetTicks()
{
    return core.get_millis();
}

void Delay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void FramerateDelay()
{
    const uint32_t FrameMs = 1000 / 60;
    uint32_t Now = GetTicks();
    if ((NextFrameTime == 0) || (Now > NextFrameTime + FrameMs))
        NextFrameTime = Now;
    NextFrameTime += FrameMs;
    if (NextFrameTime > Now)
        Delay(NextFrameTime - Now);
    else
        Delay(1);
}

// RUN and MENU held together for half a second go back to the launcher, as in the other AKA
// games; RUN on its own switches the AKA off when it comes up, as the library does
#define EXIT_HOLD_MS 500

uint16_t PollKeys()
{
    // the keys the game saw last, what it keeps seeing while RUN is down
    static uint16_t Keys = 0;
    // RUN is down, and whether it has been on its own all the while (no MENU)
    static bool RunDown = false, RunAlone = false;
    // when RUN and MENU both went down, 0 while they are not
    static uint32_t ExitHeldSince = 0;

    // The library's poll (core.pool) switches the AKA off as soon as it sees RUN, so RUN held
    // with MENU would never get here: while RUN is down the keys are read straight from the
    // expander, the read the poll makes itself, and the joystick on its own
    const uint16_t Raw = gb_ll_expander_read() & EXPANDER_KEY;
    if (Raw & GB_KEY_RUN)
    {
        core.joystick.update();
        if (!RunDown)
        {
            RunDown = true;
            RunAlone = true;
        }
        if (Raw & GB_KEY_MENU)
        {
            RunAlone = false;
            const uint32_t Now = GetTicks();
            if (!ExitHeldSince)
                ExitHeldSince = Now | 1;
            else if (Now - ExitHeldSince >= EXIT_HOLD_MS)
                ReturnToLoader();
        }
        else
            ExitHeldSince = 0;
        // the combination is not the game's: it keeps the keys it had when RUN went down
        return Keys;
    }
    ExitHeldSince = 0;
    if (RunDown)
    {
        RunDown = false;
        if (RunAlone)
            gb_ll_expander_power_off();
    }
    core.pool();
    Keys = core.buttons.state() | core.joystick.state();
    return Keys;
}

void* AllocLarge(size_t size)
{
    void* p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p)
        p = malloc(size);
    return p;
}

static void MakeDir(const char* Path)
{
    if ((mkdir(Path, 0777) != 0) && (errno != EEXIST))
        printf("cannot make %s\n", Path);
}

void MakeDataDirs()
{
    MakeDir(DATA_DIR);
    MakeDir(DATA_DIR "/music");
}

void ReturnToLoader()
{
    const esp_partition_t* Loader = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
    if (Loader)
    {
        esp_ota_set_boot_partition(Loader);
        esp_restart();
    }
    // no launcher on this AKA: there is nothing to go back to, so start the game over
    esp_restart();
}
