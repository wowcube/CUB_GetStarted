#include "getstarted_vars.inc"

const MASCOT     = 0;
const EMPTY      = 1;
const ICON       = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const ICON_2     = 5;

new getstarted_successLayout{} = {
    TEXT_BOX, MASCOT, SPEECH_BOX, ICON_2,
    TEXT_BOX, MASCOT, SPEECH_BOX, ICON_2,
    TEXT_BOX, EMPTY, ICON, EMPTY,
    TEXT_BOX, EMPTY, ICON, EMPTY,
    TEXT_BOX, EMPTY, ICON, EMPTY,
    TEXT_BOX, EMPTY, ICON, EMPTY,
};

const SUCCESS_EFFECT_TIME = 500;

new successAnimationEffectTimer = 0;
new successAnimationEffectFlag = 0;
new sucAnimEffectTimePercent = 0;

const SMALL_STAR_Y_END = 205;
const SMALL_STAR_X_END = 90;
const BIG_STAR_Y_END = 50;
const BIG_STAR_X_END = 50;

new smallStarX = 120;
new smallStarY = 120;
new bigStarX = 120;
new bigStarY = 120;

SuccessFontPercents() {
    return 1 + ((SIMPLE_FONT_SIZE - 1) * sucAnimEffectTimePercent) / 100;
}

SuccessStarsPercents() {
    smallStarX = 120 + ((SMALL_STAR_X_END - 120) * sucAnimEffectTimePercent) / 100;
    smallStarY = 120 + ((SMALL_STAR_Y_END - 120) * sucAnimEffectTimePercent) / 100;
    bigStarX = 120 + ((BIG_STAR_X_END - 120) * sucAnimEffectTimePercent) / 100;
    bigStarY = 120 + ((BIG_STAR_Y_END - 120) * sucAnimEffectTimePercent) / 100;
}

DrawSuccessScreen(screen) {
    new angle = getStarted_screenData[screen].angle;
    new screenLayout = getstarted_successLayout{getStarted_screenData[screen].sideType * FACES_ON_PLANE_MAX + angle / 90};
    abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);
    switch (screenLayout) {
        case MASCOT: {
            abi_CMD_G2D_ADD_SPRITE(currentMascotSprite, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(DIALOGUE, false, 120, 31, 0xFF, 0, 180, MIRROR_BLANK);
        }
        case ICON: {
            abi_CMD_G2D_ADD_SPRITE(COMPLETE_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            new posX = 212;
            new posY = 120;
            if (angle != 270) {
                posX = 120;
                posY = 28;
            }
            if (previousAppState == FSM:twistTutorial) {
                abi_CMD_TEXT("nice job!", 0, posX, posY, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else if (previousAppState == FSM:tapTutorial) {
                abi_CMD_TEXT("you are awesome!", 0, posX, posY, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
        case ICON_2: {
            SuccessStarsPercents();
            abi_CMD_G2D_ADD_SPRITE(SMALL_STAR, false, smallStarX, smallStarY, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(BIG_STAR, false, bigStarX, bigStarY, 0xFF, 0, angle, MIRROR_BLANK);
            abi_CMD_G2D_ADD_SPRITE(COMPLETE_ICON, false, 120, 120, 0xFF, 0, angle, MIRROR_BLANK);
            if (previousAppState == FSM:twistTutorial) {
                abi_CMD_TEXT("nice job!", 0, 212, 120, SuccessFontPercents(), angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else if (previousAppState == FSM:tapTutorial) {
                abi_CMD_TEXT("you are awesome!", 0, 212, 120, SuccessFontPercents(), angle, TEXT_ALIGN_BOTTOM_CENTER, 0xFF, 0xFF, 0xFF, true);
            }

        }
        case SPEECH_BOX: {
            abi_CMD_G2D_ADD_RECTANGLE(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF9898, 0xFF5252);
            DrawCrossAnimation();
            if (previousAppState == FSM:twistTutorial) {
                abi_CMD_TEXT("good!", 0, 110, 140, 17, angle, TEXT_ALIGN_CENTER, 0xFF, 0x52, 0x52, true);
                abi_CMD_TEXT("go ahead", 0, 120, 100, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            } else if (previousAppState == FSM:tapTutorial) {
                abi_CMD_TEXT("very good!", 0, 110, 140, 10, angle, TEXT_ALIGN_CENTER, 0xFF, 0x52, 0x52, true);
                //abi_CMD_TEXT("moving on", 0, 120, 100, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("next thing to", 0, 120, 100, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
                abi_CMD_TEXT("learn is tilt", 0, 120, 80, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0x54, 0x26, 0x82, true);
            }
        }
        case TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 105;
            new posX_2 = 120;
            new posY_2 = 135;
            if ((getStarted_screenData[screen].sideType == topology_location:location_bottom) ||
                (getStarted_screenData[screen].sideType == topology_location:location_top)) {
                InitCrossAnimationVars(0xFF9898, 0xFF5252);
                DrawCrossAnimation();
            }
            if (previousAppState == FSM:twistTutorial) {
                abi_CMD_TEXT("twist it one", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("more time",    0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                //abi_CMD_TEXT("twist to", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                //abi_CMD_TEXT("continue", 0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            } else if (previousAppState == FSM:tapTutorial) {
                abi_CMD_TEXT("double tap", 0, posX_1, posY_1, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
                abi_CMD_TEXT("to continue", 0, posX_2, posY_2, SIMPLE_FONT_SIZE, angle, TEXT_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, true);
            }
        }
    }
}

UpdateSuccessScreen(deltaTime) {
    if (abi_cubeN == 0) {
        if ((previousAppState == FSM:tapTutorial) && (abi_MTD_GetTapsCount() >= 2)) {
            SetApplicationState(FSM:tiltTutorial);
        }
    }
    if (!successAnimationEffectFlag) {
        if (successAnimationEffectTimer < SUCCESS_EFFECT_TIME) {
            successAnimationEffectTimer += deltaTime;
        } else {
            successAnimationEffectTimer = SUCCESS_EFFECT_TIME;
            successAnimationEffectFlag = 1;
        }
        sucAnimEffectTimePercent = (successAnimationEffectTimer * 100) / SUCCESS_EFFECT_TIME;
    }
}