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
    new screenLayout = getstarted_successLayout{getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90};
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);
    switch (screenLayout) {
        case MASCOT: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, currentMascotSprite);
            GFX_drawImage([120, 31], 0xFF, 0, 180, MIRROR_BLANK, DIALOGUE);
        }
        case ICON: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, COMPLETE_ICON);
            new posX = 212;
            new posY = 120;
            if (angle != 270) {
                posX = 120;
                posY = 28;
            }
            if (previousAppState == FSM:twistTutorial) {
                GFX_drawLabel(GetGFXPoint(posX, posY), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "nice job!");
            } else if (previousAppState == FSM:tapTutorial) {
                GFX_drawLabel(GetGFXPoint(posX, posY), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "you are awesome!");
            }
        }
        case ICON_2: {
            SuccessStarsPercents();
            GFX_drawImage(GetGFXPoint(smallStarX, smallStarY), 0xFF, 0, angle, MIRROR_BLANK, SMALL_STAR);
            GFX_drawImage(GetGFXPoint(bigStarX, bigStarY), 0xFF, 0, angle, MIRROR_BLANK, BIG_STAR);
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, COMPLETE_ICON);
            if (previousAppState == FSM:twistTutorial) {
                GFX_drawLabel([212, 120], SuccessFontPercents(), angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "nice job!");
            } else if (previousAppState == FSM:tapTutorial) {
                GFX_drawLabel([212, 120], SuccessFontPercents(), angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "you are awesome!");
            }

        }
        case SPEECH_BOX: {
            GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF9898, 0xFF5252);
            DrawCrossAnimation();
            if (previousAppState == FSM:twistTutorial) {
                GFX_drawLabel([110, 140], 17, angle, 0, TEXT_ALIGN_CENTER, 0xFFFF5252, "good!");
                GFX_drawLabel([120, 100], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "go ahead");
            } else if (previousAppState == FSM:tapTutorial) {
                GFX_drawLabel([110, 140], 10, angle, 0, TEXT_ALIGN_CENTER, 0xFFFF5252, "very good!");
                GFX_drawLabel([120, 100], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "next thing to");
                GFX_drawLabel([120,  80], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "learn is tilt");
            }
        }
        case TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 105;
            new posX_2 = 120;
            new posY_2 = 135;
            if ((getStarted_screenData[screen].sideType == TOPOLOGY_location:LOCATION_DOWN) ||
                (getStarted_screenData[screen].sideType == TOPOLOGY_location:LOCATION_UP)) {
                InitCrossAnimationVars(0xFF9898, 0xFF5252);
                DrawCrossAnimation();
            }
            if (previousAppState == FSM:twistTutorial) {
                GFX_drawLabel(GetGFXPoint(posX_1, posY_1), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "twist it one");
                GFX_drawLabel(GetGFXPoint(posX_2, posY_2), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "more time");
            } else if (previousAppState == FSM:tapTutorial) {
                GFX_drawLabel(GetGFXPoint(posX_1, posY_1), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "double tap");
                GFX_drawLabel(GetGFXPoint(posX_2, posY_2), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "to continue");
            }
        }
    }
}

UpdateSuccessScreen(deltaTime) {
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