#include "getstarted_vars.inc"

const MASCOT     = 0;
const EMPTY      = 1;
const ICON       = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const TEXT_BOX_2 = 5;
const MASCOT_2   = 6;

new getstarted_shakeTutorial{} = {
    EMPTY, EMPTY, SPEECH_BOX, MASCOT,
    EMPTY, EMPTY, SPEECH_BOX, MASCOT,
    EMPTY, ICON, EMPTY, TEXT_BOX,
    EMPTY, TEXT_BOX, EMPTY, ICON,
    ICON, EMPTY, TEXT_BOX, EMPTY,
    EMPTY, ICON, EMPTY, TEXT_BOX,
};

new getstarted_shakeTutorialFill{} = {
    EMPTY, MASCOT_2, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
    EMPTY, EMPTY, EMPTY, TEXT_BOX_2,
};

new shakeTutorialStage = 0;
new beginShakeTutorial = 0;

new mascotShakeAnimFlag = 1;
new mascotShakePosX = 120;
new mascotShakeStep = -10;
new mascotShakeAnimTimer = 0;

const BAKED_SHAKETUT_MASCOT_SPRITE = 0;
const BAKED_SHAKETUT_WAIT_SPRITE = 1;

ShakeTutorialBakeSprites() {
    GFX_clearCache();
    GFX_bakeImage(BAKED_SHAKETUT_MASCOT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(130, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_SHAKETUT_WAIT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
    GFX_drawImageXY(130, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
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

GetShakeFillColor(shakeInPercents) {
    return (0xFF000000
    | 0xFA + ((0xFF - 0xFA) * (100 - shakeInPercents)) / 100 << 16
    | 0x46 + ((0xA5 - 0x46) * (100 - shakeInPercents)) / 100 << 8
    | 0x46 + ((0x52 - 0x46) * (100 - shakeInPercents)) / 100);
}

ShakeFillBackground(angle) {
    new shakeInPercents = (shakeTutorialStage * 100) / SENSITIVITY_MENU_CHANGE_SCRIPT;
    new rect[GFX_RECTANGLE];
    rect.x = 0;
    rect.y = 0;
    rect.w = 240;
    rect.h = 240;
    switch (angle) {
        case 0: {
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

DrawShakeTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    new screenLayout = (beginShakeTutorial) ? 
                       (getstarted_shakeTutorialFill{getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90}) : 
                       (getstarted_shakeTutorial{getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90});
    
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    if (beginShakeTutorial) {
        ShakeFillBackground(angle);
    }
    switch (screenLayout) {
        case MASCOT: {
            if (mascotShakeAnimFlag) {
                MascotShakeAnimation();
            }
            if (!mascotWaitModeFlag) {
                GFX_drawBakedImageXY(mascotShakePosX, 120, 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_SHAKETUT_MASCOT_SPRITE);
            } else {
                GFX_drawBakedImageXY(mascotShakePosX, 120, 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_SHAKETUT_WAIT_SPRITE);
            }
            GFX_drawImageXY(31, 120, 0xFF, magenta, 100, 100, 90, MIRROR_BLANK, DIALOGUE);
        }
        case ICON: {
            GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, SHAKE_ICON);
        }
        case SPEECH_BOX: {
            GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            GFX_drawTextXY(120, 180, 18, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFA552, "shake");
            GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "the cube up and");
            GFX_drawTextXY(120,  90, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "down to go back");
            GFX_drawTextXY(120,  60, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to the main menu");
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
            GFX_drawTextXY(posX_1, posY_1, 6, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "shake");
            GFX_drawTextXY(120, 120, 6, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube up");
            GFX_drawTextXY(posX_2, posY_2, 6, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "and down");
        }
        case TEXT_BOX_2: {
            GFX_drawTextXY(100, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "shake the");
            GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "cube to go back");
            GFX_drawTextXY(140, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "to main menu");
        }
        case MASCOT_2: {
            if (!shakeTutorialStage) {
                if (!mascotWaitModeFlag) {
                    GFX_drawBakedImage([210, 210], 0xFF, 0, 100, 100, 75, MIRROR_BLANK, BAKED_SHAKETUT_MASCOT_SPRITE);
                } else {
                    GFX_drawBakedImage([210, 210], 0xFF, 0, 100, 100, 75, MIRROR_BLANK, BAKED_SHAKETUT_WAIT_SPRITE);
                }
                GFX_drawImageXY(91, 120, 0xFF, magenta, 100, 100, 90, MIRROR_BLANK, DIALOGUE_SMALL);
                GFX_drawTextXY(70, 130, 16, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "shake!");
            }
        }
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
}