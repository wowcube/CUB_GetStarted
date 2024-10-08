#include "getstarted_vars.inc"

const MASCOT     = 0;
const EMPTY      = 1;
const ICON       = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const TEXT_BOX_2 = 5;
const MASCOT_2   = 6;

new getstarted_shakeTutorial{} = {
    EMPTY, MASCOT, SPEECH_BOX, EMPTY,
    EMPTY, MASCOT, SPEECH_BOX, EMPTY,
    EMPTY, TEXT_BOX, EMPTY, ICON,
    EMPTY, ICON, EMPTY, TEXT_BOX,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    EMPTY, TEXT_BOX, EMPTY, ICON,
};

new getstarted_shakeTutorialFill{} = {
    EMPTY, TEXT_BOX_2, ICON, MASCOT_2,
    EMPTY, TEXT_BOX_2, ICON, MASCOT_2,
    EMPTY, TEXT_BOX_2, ICON, EMPTY,
    EMPTY, TEXT_BOX_2, ICON, EMPTY,
    EMPTY, TEXT_BOX_2, ICON, EMPTY,
    EMPTY, TEXT_BOX_2, ICON, EMPTY,
};

new shakeTutorialStage = 0;
new beginShakeTutorial = 0;

new mascotShakeAnimFlag = 1;
new mascotShakePosX = 120;
new mascotShakeStep = -10;
new mascotShakeAnimTimer = 0;

const BAKED_SHAKETUT_MASCOT_SPRITE = 0;
const BAKED_SHAKETUT_FILL_SPRITE = 2;
const BAKED_SHAKETUT_END_SPRITE = 3;

new shakeAnimEffectTimePercent = 0;
new shakeStars[6][.X, .Y] = [
    [222,  40], [277, 124], [233, 242], 
    [125, 249], [ 33, 266], [120, 128] 
];
new shakeAnimEffectTime = 0;

ShakeTutorialBakeSprites() {
    GFX_clearCache();
    GFX_bakeImage(BAKED_SHAKETUT_MASCOT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(120,  98, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_CUTE_SPRITE);
    GFX_drawImageXY(120, 133, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_SHAKETUT_FILL_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(200,  44, 0xFF, magenta, 100, 100, 350, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(190,  60, 0xFF, magenta, 100, 100, 350, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
    GFX_drawImageXY(210,  90, 0xFF, magenta, 100, 100, 350, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_SHAKETUT_END_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(200,  44, 0xFF, magenta, 100, 100, 350, MIRROR_BLANK, MASCOT_SUCCESS_BODY_SPRITE);
    GFX_drawImageXY(185,  15, 0xFF, magenta, 100, 100, 350, MIRROR_BLANK, MASCOT_SUCCESS_EYEBROWS_SPRITE);
    GFX_drawImageXY(190,  60, 0xFF, magenta, 100, 100, 350, MIRROR_BLANK, MASCOT_SUCCESS_EYES_SPRITE);
    GFX_drawImageXY(200,  95, 0xFF, magenta, 100, 100, 350, MIRROR_BLANK, MASCOT_SUCCESS_MOUNTH_SPRITE);
    GFX_render();
}

MascotShakeAnimation() {
    mascotShakePosX += mascotShakeStep;
    if ((mascotShakePosX == 90) || (mascotShakePosX == 150)) {
        mascotShakeStep = -mascotShakeStep;
        ++mascotShakeAnimFlag;
    }

    if ((mascotShakeAnimFlag == 6) && (mascotShakePosX == 120)) {
        mascotShakeAnimFlag = 0;
        mascotShakeStep = -mascotShakeStep;
    }
}

RenderStarsShake(angle, pos) {
    shakeAnimEffectTimePercent = (shakeAnimEffectTime * 100) / 1000;
    for (new i = 0; i < 6; ++i) {
        new x = (shakeStars[i].X * shakeAnimEffectTimePercent) / 100;
        new y = (shakeStars[i].Y * shakeAnimEffectTimePercent) / 100;
        GFX_drawImageXY(x, y, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, BIG_STAR);
    }
}

GetShakeFillColor(shakeInPercents) {
    return (0xFF000000
    | 0xFA + ((0xFF - 0xFA) * (100 - shakeInPercents)) / 100 << 16
    | 0x46 + ((0xA5 - 0x46) * (100 - shakeInPercents)) / 100 << 8
    | 0x46 + ((0x52 - 0x46) * (100 - shakeInPercents)) / 100);
}

ShakeFillBackground(angle) {
    if (shakeTutorialStage >= SENSITIVITY_MENU_CHANGE_SCRIPT) {
        GFX_drawRectangle([0, 0, 240, 240], 0xFF3CB22E);
    } else {
        new shakeInPercents = (shakeTutorialStage * 100) / SENSITIVITY_MENU_CHANGE_SCRIPT;
        new rect[GFX_RECTANGLE];
        rect.x = 0;
        rect.y = 0;
        rect.w = 240;
        rect.h = 240;
        switch (angle) {
            case 360: {
                if (shakeInPercents <= 50) {
                    rect.y = rect.h - (480 * shakeInPercents) / 100;
                }
            }
            case 90: {
                if (shakeInPercents >= 50) {
                    rect.w = (480 * shakeInPercents) / 100 - rect.w;
                } else {
                    rect.w = rect.h = 0;
                }
            }
            case 180: {
                if (shakeInPercents >= 50) {
                    rect.h = (480 * shakeInPercents) / 100 - rect.h;
                } else {
                    rect.w = rect.h = 0;
                }
            }
            case 270: {
                if (shakeInPercents <= 50) {
                    rect.x = rect.w - (480 * shakeInPercents) / 100;
                }
            }
        }
        
        GFX_drawRectangle(rect, GetShakeFillColor(shakeInPercents));
    }
}

DrawShakeTutorial(screen) {
    new screenI = SELF_ID * SCREENS_MAX + screen;
    new angle = (4 - getStarted_AllScreensData[screenI].pos) * 90;
    new screenLayout = (beginShakeTutorial) ? 
                       (getstarted_shakeTutorialFill{getStarted_AllScreensData[screenI].face * TOPOLOGY_POSITIONS_MAX + getStarted_AllScreensData[screenI].pos}) : 
                       (getstarted_shakeTutorial{getStarted_AllScreensData[screenI].face * TOPOLOGY_POSITIONS_MAX + getStarted_AllScreensData[screenI].pos});

    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    if (beginShakeTutorial) {
        ShakeFillBackground(angle);
    }
    switch (screenLayout) {
        case MASCOT: {
            if (mascotShakeAnimFlag) {
                MascotShakeAnimation();
            }
            if (!mascotShakeAnimFlag){
                GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                GFX_drawImageXY( 98, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                GFX_drawImageXY(133, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
            } else {
                GFX_drawBakedImageXY(mascotShakePosX, 120, 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_SHAKETUT_MASCOT_SPRITE);
            }
            GFX_drawImageXY(31, 120, 0xFF, magenta, 100, 100, 90, MIRROR_BLANK, DIALOGUE);
        }
        case ICON: {
            if (shakeTutorialStage < SENSITIVITY_MENU_CHANGE_SCRIPT) {
                GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, SHAKE_ICON);
            }
        }
        case SPEECH_BOX: {
            GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
            InitCrossAnimationVars(0xFFFF5C00, 0xFFFFB576);
            DrawCrossAnimation();
            #ifdef SPRITE_TEXT
            GFX_drawImageXY(120, 154, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, SHAKE_XL_TEXT_SPRITE);
            GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "the cube to");
            GFX_drawTextXY(120,  90, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "return to");
            GFX_drawTextXY(120,  60, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "the main menu");
            #else
            GFX_drawTextXY(120, 180, 18, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFA552, "shake");
            GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "the cube to");
            GFX_drawTextXY(120,  90, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "return to");
            GFX_drawTextXY(120,  60, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "the main menu");
            #endif
        }
        case TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            if (angle == 90) {
                posX_1 = 140;
                posY_1 = 120;
                posX_2 = 100;
                posY_2 = 120;
            } else if (angle == 180) {
                posX_1 = 120;
                posY_1 = 140;
                posX_2 = 120;
                posY_2 = 100;
            } else if (angle == 270) {
                posX_1 = 100;
                posY_1 = 120;
                posX_2 = 140;
                posY_2 = 120;
            }
            GFX_drawTextXY(posX_1, posY_1, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "shake");
            GFX_drawTextXY(posX_2, posY_2, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube");
        }
        case TEXT_BOX_2: {
            if (shakeTutorialStage < SENSITIVITY_MENU_CHANGE_SCRIPT) {
                GFX_drawTextXY(100, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "shake the");
                GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "cube to go back");
                GFX_drawTextXY(140, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "to main menu");
            }
        }
        case MASCOT_2: {
            if (shakeTutorialStage < SENSITIVITY_MENU_CHANGE_SCRIPT) {
                GFX_drawBakedImage([120, 120], 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_SHAKETUT_FILL_SPRITE);
                GFX_drawImageXY(72, 120, 0xFF, magenta, 100, 100, 90, MIRROR_BLANK, DIALOGUE_SMALL);
                #ifdef SPRITE_TEXT
                GFX_drawImageXY(55, 120, 0xFF, magenta, 100, 100, 90, MIRROR_BLANK, SHAKE_L_TEXT_SPRITE);
                #else
                GFX_drawTextXY(50, 120, 16, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "shake!");
                #endif
            } else {
                GFX_drawBakedImage([120, 120], 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_SHAKETUT_END_SPRITE);
            }
        }
    }
    if ((shakeTutorialStage >= SENSITIVITY_MENU_CHANGE_SCRIPT) && (tiltAnimEffectTime <= 1000)) {
        RenderStarsShake(angle, getStarted_AllScreensData[screenI].pos);
    }
}

UpdateShakeTutorial(m_deltaTime) {
    if (!beginShakeTutorial && !mascotShakeAnimFlag) {
        if (mascotShakeAnimTimer >= MASCOT_WAIT_TIME) {
            mascotShakeAnimTimer = 0;
            mascotShakeAnimFlag = 1;
        } else {
            mascotShakeAnimTimer += m_deltaTime;
        }
    }
    if (shakeTutorialStage >= SENSITIVITY_MENU_CHANGE_SCRIPT) {
        shakeAnimEffectTime += m_deltaTime;
    }
    if (SELF_ID == 0) {
        if (shakeAnimEffectTime >= 2000) {
            SaveData();
            quit();
        }
    }
}