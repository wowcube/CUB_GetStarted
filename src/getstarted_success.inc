#include "getstarted_vars.inc"

const SUCCESS_MASCOT     = 0;
const SUCCESS_EMPTY      = 1;
const SUCCESS_ICON       = 2;
const SUCCESS_SPEECH_BOX = 3;
const SUCCESS_TEXT_BOX   = 4;
const SUCCESS_ICON_2     = 5;

new getstarted_successLayout{} = {
    SUCCESS_TEXT_BOX, SUCCESS_ICON_2, SUCCESS_SPEECH_BOX, SUCCESS_MASCOT,
    SUCCESS_TEXT_BOX, SUCCESS_ICON_2, SUCCESS_SPEECH_BOX, SUCCESS_MASCOT,
    SUCCESS_TEXT_BOX, SUCCESS_EMPTY, SUCCESS_ICON, SUCCESS_EMPTY,
    SUCCESS_TEXT_BOX, SUCCESS_EMPTY, SUCCESS_ICON, SUCCESS_EMPTY,
    SUCCESS_TEXT_BOX, SUCCESS_EMPTY, SUCCESS_ICON, SUCCESS_EMPTY,
    SUCCESS_TEXT_BOX, SUCCESS_EMPTY, SUCCESS_ICON, SUCCESS_EMPTY,
};

const SUCCESS_EFFECT_TIME = 1000;

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

new successAnimationFrame = 0;
new alphaHightlight = 0;
new successRepeatAnimFlag = 0;
new successStars[6][.X, .Y] = [
    [22,  28], [277,  44], [233, 162], 
    [95, 229], [233, 266], [ 40, 268] 
];

SuccessFontPercents() {
    return 1 + ((SIMPLE_FONT_SIZE) * sucAnimEffectTimePercent) / 100;
}

SuccessStarsPercents() {
    /*smallStarX = 120 + ((SMALL_STAR_X_END - 120) * sucAnimEffectTimePercent) / 100;
    smallStarY = 120 + ((SMALL_STAR_Y_END - 120) * sucAnimEffectTimePercent) / 100;
    bigStarX = 120 + ((BIG_STAR_X_END - 120) * sucAnimEffectTimePercent) / 100;
    bigStarY = 120 + ((BIG_STAR_Y_END - 120) * sucAnimEffectTimePercent) / 100;*/
    new alpha = 255;//(255 * (100 - sucAnimEffectTimePercent)) / 100;
    if (alpha < 0) {
        alpha = 0;
    }
    for (new i = 0; i < 6; ++i) {
        new x = 120 + ((successStars[i].X - 120) * sucAnimEffectTimePercent) / 100;
        new y = 120 + ((successStars[i].Y - 120) * sucAnimEffectTimePercent) / 100;
        GFX_drawImageXY(x, y, alpha, magenta, 100, 100, 270, MIRROR_BLANK, BIG_STAR);
    }
}

UpdateSuccessAnimationFrame() {
    ++successAnimationFrame;

    if (successAnimationFrame >= 31) {
        if (!successRepeatAnimFlag) {
            successRepeatAnimFlag = 1;
            alphaHightlight = successAnimationFrame = 0;
        }
        if (successAnimationFrame > 120) {
            successRepeatAnimFlag = 0;
            alphaHightlight = successAnimationFrame = 0;
        }
        return;
    }
    if (successAnimationFrame >= 16) {
        alphaHightlight -= 17;
    } else if (successAnimationFrame >= 0) {
        alphaHightlight += 17;
    }
}

DrawSuccessHighlight(angle) {
    GFX_drawImageXY(120, 120, alphaHightlight, magenta, 100, 100, 270, MIRROR_BLANK, HIGHTLIGHT_GREEN);
}

DrawSuccessScreen(screen) {
    new screenI = SELF_ID * SCREENS_MAX + screen;
    new angle = (4 - getStarted_AllScreensData[screenI].pos) * 90;
    new screenLayout = getstarted_successLayout{
                        getStarted_AllScreensData[screenI].face * TOPOLOGY_POSITIONS_MAX
                      + getStarted_AllScreensData[screenI].pos};

    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);
    switch (screenLayout) {
        case SUCCESS_MASCOT: {
            if (!mascotWaitModeFlag) {
                GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_SUCCESS_BODY_SPRITE);
                GFX_drawImageXY(111, 114, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_SUCCESS_EYES_SPRITE);
                GFX_drawImageXY( 74, 124, 0xFF, magenta, 100, 100, angle - 15, MIRROR_BLANK, MASCOT_SUCCESS_MOUNTH_SPRITE);
                GFX_drawImageXY(151, 110, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_SUCCESS_EYEBROWS_SPRITE);
            } else {
                GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                GFX_drawImageXY(110, 115, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
                GFX_drawImageXY( 70, 120, 0xFF, magenta, 100, 100, angle, MIRROR_Y,     MASCOT_WAIT_MOUNTH_SPRITE);
            }
            GFX_drawImageXY(120, 31, 0xFF, magenta, 100, 100, 180, MIRROR_BLANK, DIALOGUE);
            DrawSuccessHighlight(angle);
        }
        case SUCCESS_ICON: {
            GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, COMPLETE_ICON);
            new posX = 212;
            new posY = 120;
            if (angle != 270) {
                posX = 120;
                posY = 28;
            }
            if (previousAppState == FSM:twistTutorial) {
                GFX_drawTextXY(posX, posY, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "nice job!");
            } else if (previousAppState == FSM:tapTutorial) {
                GFX_drawTextXY(posX, posY, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "you are awesome!");
            }
            DrawSuccessHighlight(angle);
        }
        case SUCCESS_ICON_2: {
            if (!successAnimationEffectFlag) {
                SuccessStarsPercents();
            }
            //GFX_drawImageXY(smallStarX, smallStarY, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, SMALL_STAR);

            GFX_drawImageXY(103, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, COMPLETE_ICON);
            if (previousAppState == FSM:twistTutorial) {
                GFX_drawTextXY(190, 120, SuccessFontPercents(), angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "nice job!");
            } else if (previousAppState == FSM:tapTutorial) {
                GFX_drawTextXY(190, 120, SuccessFontPercents(), angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "you are awesome!");
            }

            DrawSuccessHighlight(angle);
        }
        case SUCCESS_SPEECH_BOX: {
            GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF2DC81B, 0xFFAEE4A8);
            DrawCrossAnimation();
            if (previousAppState == FSM:twistTutorial) {
                #ifdef SPRITE_TEXT
                GFX_drawImageXY(120, 134, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, GOOD_TEXT_GREEN);
                GFX_drawTextXY(120, 90, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "go ahead");
                #else
                GFX_drawTextXY(110, 140, 17, angle, 0, TEXT_ALIGN_CENTER, 0xFFFF5252, "good!");
                GFX_drawTextXY(120, 100, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "go ahead");
                #endif
            } else if (previousAppState == FSM:tapTutorial) {
                #ifdef SPRITE_TEXT
                GFX_drawImageXY(120, 134, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, VERY_GOOD_TEXT_GREED_SPRITE);
                GFX_drawTextXY(120, 80, SIMPLE_FONT_SIZE - 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "next thing");
                GFX_drawTextXY(120, 60, SIMPLE_FONT_SIZE - 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "to learn is tilt");
                #else
                GFX_drawTextXY(120, 150, 10, angle, 0, TEXT_ALIGN_CENTER, 0xFFFF5252, "very good!");
                GFX_drawTextXY(120, 100, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "next thing to");
                GFX_drawTextXY(120,  80, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "learn is tilt");
                #endif
            }
            DrawSuccessHighlight(angle);
        }
        case SUCCESS_TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 105;
            new posX_2 = 120;
            new posY_2 = 135;
            if ((getStarted_AllScreensData[screenI].face == TOPOLOGY_orientation:ORIENTATION_DOWN) ||
                (getStarted_AllScreensData[screenI].face == TOPOLOGY_orientation:ORIENTATION_UP)) {
                InitCrossAnimationVars(0xFF2DC81B, 0xFFAEE4A8);
                DrawCrossAnimation();
            }
            if (previousAppState == FSM:twistTutorial) {
                GFX_drawTextXY(posX_1, posY_1, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "twist it one");
                GFX_drawTextXY(posX_2, posY_2, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "more time");
            } else if (previousAppState == FSM:tapTutorial) {
                GFX_drawTextXY(posX_1, posY_1, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "double slap");
                GFX_drawTextXY(posX_2, posY_2, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "to continue");
            }
            DrawSuccessHighlight(angle);
        }
        case SUCCESS_EMPTY: {
            DrawSuccessHighlight(angle);
        }
    }
}

UpdateSuccessScreen(m_deltaTime) {
    if (!successAnimationEffectFlag) {
        if (successAnimationEffectTimer < SUCCESS_EFFECT_TIME) {
            successAnimationEffectTimer += m_deltaTime;
        } else {
            successAnimationEffectTimer = SUCCESS_EFFECT_TIME;
            successAnimationEffectFlag = 1;
        }
        sucAnimEffectTimePercent = (successAnimationEffectTimer * 100) / SUCCESS_EFFECT_TIME;
    }
    UpdateSuccessAnimationFrame();
}