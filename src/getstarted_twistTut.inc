#include "getstarted_vars.inc"

const TWISTTUT_MASCOT     = 0;
const TWISTTUT_EMPTY      = 1;
const TWISTTUT_ICON       = 2;
const TWISTTUT_SPEECH_BOX = 3;
const TWISTTUT_TEXT_BOX   = 4;
const TWISTTUT_ARROW      = 5;

new arrowIcons[MAX_ARROW_ICONS];

const MAX_TWIST_TUTORIAL_STAGES = 5;

const ARROW_ARRAY_SHIFT_TIME = 500;
new arrowAnimationTime = 0;

new twistTutorialStage = 0;

new getstarted_twistTutLayout_X{} = {
    TWISTTUT_ARROW, TWISTTUT_MASCOT, TWISTTUT_SPEECH_BOX, TWISTTUT_ARROW,
    TWISTTUT_ARROW, TWISTTUT_MASCOT, TWISTTUT_SPEECH_BOX, TWISTTUT_ARROW,
    TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_ARROW,
    TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_ARROW,
    TWISTTUT_ICON, TWISTTUT_EMPTY, TWISTTUT_TEXT_BOX, TWISTTUT_EMPTY,
    TWISTTUT_EMPTY, TWISTTUT_TEXT_BOX, TWISTTUT_EMPTY, TWISTTUT_ICON,
};
new getstarted_twistTutLayout_Y{} = {
    TWISTTUT_ARROW, TWISTTUT_ARROW,    TWISTTUT_SPEECH_BOX, TWISTTUT_MASCOT,
    TWISTTUT_ARROW, TWISTTUT_ARROW,    TWISTTUT_SPEECH_BOX, TWISTTUT_MASCOT,
    TWISTTUT_ICON,  TWISTTUT_EMPTY,    TWISTTUT_TEXT_BOX,   TWISTTUT_EMPTY,
    TWISTTUT_EMPTY, TWISTTUT_TEXT_BOX, TWISTTUT_EMPTY,      TWISTTUT_ICON,
    TWISTTUT_ARROW, TWISTTUT_ARROW,    TWISTTUT_ARROW,      TWISTTUT_ARROW,
    TWISTTUT_ARROW, TWISTTUT_ARROW,    TWISTTUT_ARROW,      TWISTTUT_ARROW,
};

new mascotTwistAnimTimer = 0;
new mascotTwistAngle = 270;
new mascotTwistAnimFlag = 1;

const BAKED_TWISTTUT_MASCOT_X_SPRITE = 1;

TwistTutorialBakeSprites() {
    GFX_clearCache();
    GFX_bakeImage(BAKED_TWISTTUT_MASCOT_X_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_X_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
}

MascotTwistAnimUpdate(angle) {
    if (((mascotTwistAngle <= (angle + 90)) && (mascotTwistAngle >= angle))
     || ((mascotTwistAngle >= (angle - 90)) && (mascotTwistAngle <= angle))) {
        mascotTwistAngle -= 15;
    } else {
        mascotTwistAngle -= 30;
    }
    mascotTwistAngle %= 360;
    if (mascotTwistAngle == angle) {
        mascotTwistAnimFlag = 0;
    }
}

DrawTwistTutorial(screen) {
    new screenI = SELF_ID * SCREENS_MAX + screen;
    new angle = (4 - getStarted_AllScreensData[screenI].pos) * 90;
    new currentStage = twistTutorialStage % 2;
    new screenLayout = (currentStage) ? 
                       (getstarted_twistTutLayout_Y{getStarted_AllScreensData[screenI].face * TOPOLOGY_POSITIONS_MAX + getStarted_AllScreensData[screenI].pos}) : 
                       (getstarted_twistTutLayout_X{getStarted_AllScreensData[screenI].face * TOPOLOGY_POSITIONS_MAX + getStarted_AllScreensData[screenI].pos});
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);
    switch (screenLayout) {
        case TWISTTUT_MASCOT: {
            // Check if start tutorial after first twist
            if (twistTutorialStage) {
                // what axis we will twist
                if (currentStage) {
                    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                    if (twistTutorialStage == 3) {
                        GFX_drawImageXY(110, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_CUTE_SPRITE);
                        GFX_drawImageXY( 73, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                    } else {
                        GFX_drawImageXY(110, 115, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                        GFX_drawImageXY( 70, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                    }
                    GFX_drawImageXY(120, 31, 0xFF, magenta, 100, 100, 180, MIRROR_BLANK, DIALOGUE);
                } else {
                    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                    if (twistTutorialStage == 4) {
                        GFX_drawImageXY(130, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_X_SPRITE);
                        GFX_drawImageXY(167, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                    } else {
                        GFX_drawImageXY(105, 105, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                        GFX_drawImageXY(150, 110, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                    }
                    GFX_drawImageXY(31, 120, 0xFF, magenta, 100, 100, 90, MIRROR_BLANK, DIALOGUE);
                }
            } else {    
                if (mascotTwistAnimFlag) {
                    MascotTwistAnimUpdate(angle);
                    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, SILUETTE_SPRITE);
                    GFX_drawBakedImageXY(120, 120, 0xFF, 0, 100, 100, mascotTwistAngle, MIRROR_BLANK, BAKED_TWISTTUT_MASCOT_X_SPRITE);
                } else {
                    // If we not rotating
                    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                    GFX_drawImageXY(120, 115, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                    GFX_drawImageXY(170, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                }
                GFX_drawImageXY(31, 120, 0xFF, magenta, 100, 100, 90, MIRROR_BLANK, DIALOGUE);
            }
        }
        case TWISTTUT_ICON: {
            GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TWIST_ICON);
        }
        case TWISTTUT_SPEECH_BOX: {
            GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
            InitCrossAnimationVars(0xFFFF5C00, 0xFFFFB576);
            if (twistTutorialStage > 1) {
                #ifdef SPRITE_TEXT
                GFX_drawImageXY(120, 146, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TWIST_TEXT_PURPLE);
                GFX_drawImageXY(120,  94, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TWIST_TEXT_ORANGE);
                #else
                GFX_drawTextXY(120, 160, 17, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "twist");
                GFX_drawTextXY(120,  70, 17, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFA552, "twist");
                #endif
            } else if (twistTutorialStage == 1) {
                #ifdef SPRITE_TEXT
                GFX_drawImageXY(120, 154, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, GOOD_TEXT_ORANGE);
                GFX_drawTextXY(120,  110, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "one more time!");
                GFX_drawTextXY(120,  85, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "twist another");
                GFX_drawTextXY(120,  60, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "side");
                #else
                GFX_drawTextXY(110, 130, 17, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFA552, "good!");
                GFX_drawTextXY(120,  98, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "one more time!");
                GFX_drawTextXY(120,  78, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "twist another");
                GFX_drawTextXY(120,  58, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "side");
                #endif
            } else {
                #ifdef SPRITE_TEXT
                GFX_drawImageXY(120, 134, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TWIST_TEXT_ORANGE);
                GFX_drawTextXY(120, 90, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "the cube once");
                #else
                GFX_drawTextXY(120, 140, 17, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "twist");
                GFX_drawTextXY(120, 100, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "the cube once");
                #endif
            }
            DrawCrossAnimation();
        }
        case TWISTTUT_TEXT_BOX: {
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
            GFX_drawTextXY(posX_1, posY_1, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "twist the");
            GFX_drawTextXY(posX_2, posY_2, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "cube once");
        }
        case TWISTTUT_ARROW: {
            new posX = 0;
            new posY = 0;
            new isMainAxisX = ((angle % 180) == 0);
            if (isMainAxisX) {
                posX = 120;
                posY = 80;
                angle = 0;
                if (currentStage) {
                    posY = 120;
                    posX = 80;
                    angle = 90;
                    isMainAxisX = 0;
                }
            } else {
                posY = 120;
                posX = 80;
                angle = 90;
                if (currentStage) {
                    posX = 120;
                    posY = 80;
                    angle = 0;
                    isMainAxisX = 1;
                }
            }
            new arrowSprite = ARROW_TWIST;
            if (!isMainAxisX) {
                GFX_drawImageXY(posX, posY, arrowIcons[0], magenta, 100, 100, angle, MIRROR_BLANK, arrowSprite);
                GFX_drawImageXY(posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX, arrowIcons[1], magenta, 100, 100, angle, MIRROR_BLANK, arrowSprite);
                GFX_drawImageXY(posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX, arrowIcons[2], magenta, 100, 100, angle, MIRROR_BLANK, arrowSprite);
            } else {
                GFX_drawImageXY(posX, posY, arrowIcons[2], magenta, 100, 100, angle, MIRROR_BLANK, arrowSprite);
                GFX_drawImageXY(posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX, arrowIcons[1], magenta, 100, 100, angle, MIRROR_BLANK, arrowSprite);
                GFX_drawImageXY(posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX, arrowIcons[0], magenta, 100, 100, angle, MIRROR_BLANK, arrowSprite);
            }
        }
    }
}

UpdateTwistTutorial(m_deltaTime) {
    arrowAnimationTime += m_deltaTime;
    if (arrowAnimationTime >= ARROW_ARRAY_SHIFT_TIME) {
        arrowAnimationTime = 0;
        new lastArrowElement = arrowIcons[MAX_ARROW_ICONS - 1];
        for(new arrowI = MAX_ARROW_ICONS - 1; arrowI >= 1; --arrowI) {
            arrowIcons[arrowI] = arrowIcons[arrowI - 1];
        }
        arrowIcons[0] = lastArrowElement;
    }

    if (!twistTutorialStage && !mascotTwistAnimFlag) {
        if (mascotTwistAnimTimer >= MASCOT_WAIT_TIME) {
            mascotTwistAnimTimer = 0;
            mascotTwistAnimFlag = 1;
        } else {
            mascotTwistAnimTimer += m_deltaTime;
        }
    }

    if (SELF_ID == 0) {
        if (twistTutorialStage >= 5) {
            SetApplicationState(FSM:successScreen);
        }
    }
}