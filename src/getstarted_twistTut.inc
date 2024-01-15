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
    TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_SPEECH_BOX, TWISTTUT_MASCOT,
    TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_SPEECH_BOX, TWISTTUT_MASCOT,
    TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_ARROW,      TWISTTUT_ARROW,
    TWISTTUT_ARROW, TWISTTUT_ARROW, TWISTTUT_ARROW,      TWISTTUT_ARROW,
    TWISTTUT_ICON,  TWISTTUT_EMPTY, TWISTTUT_TEXT_BOX,   TWISTTUT_EMPTY,
    TWISTTUT_EMPTY, TWISTTUT_ICON,  TWISTTUT_EMPTY,      TWISTTUT_TEXT_BOX,
};
new getstarted_twistTutLayout_Y{} = {
    TWISTTUT_ARROW, TWISTTUT_MASCOT, TWISTTUT_SPEECH_BOX, TWISTTUT_ARROW,
    TWISTTUT_ARROW, TWISTTUT_MASCOT, TWISTTUT_SPEECH_BOX, TWISTTUT_ARROW,
    TWISTTUT_ICON,  TWISTTUT_EMPTY,  TWISTTUT_TEXT_BOX,   TWISTTUT_EMPTY,
    TWISTTUT_EMPTY, TWISTTUT_ICON,   TWISTTUT_EMPTY,      TWISTTUT_TEXT_BOX,
    TWISTTUT_ARROW, TWISTTUT_ARROW,  TWISTTUT_ARROW,      TWISTTUT_ARROW,
    TWISTTUT_ARROW, TWISTTUT_ARROW,  TWISTTUT_ARROW,      TWISTTUT_ARROW,
};

new mascotTwistAnimTimer = 0;
new mascotTwistAngle = 270;
new mascotTwistAnimFlag = 1;

new mascotBakedNormalSprite = 1;

const BAKED_TWISTTUT_WAIT_SPRITE = 0;
const BAKED_TWISTTUT_MASCOT_X_SPRITE = 1;

TwistTutorialBakeSprites() {
    GFX_clearCache();
    GFX_bakeImage(BAKED_TWISTTUT_WAIT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
    GFX_drawImageXY(120, 120, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_TWISTTUT_MASCOT_X_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(120, 120, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_X_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
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
    new angle = getStarted_screenData[screen].angle;
    new currentStage = twistTutorialStage % 2;
    new screenLayout = (currentStage) ? 
                       (getstarted_twistTutLayout_Y{getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90}) : 
                       (getstarted_twistTutLayout_X{getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90});
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);
    switch (screenLayout) {
        case TWISTTUT_MASCOT: {
            if (twistTutorialStage) {
                if (currentStage) {
                    if (!mascotWaitModeFlag) {
                        GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                        GFX_drawImageXY(110, 115, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                        GFX_drawImageXY( 70, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                    } else {
                        GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                        GFX_drawImageXY(110, 115, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
                        GFX_drawImageXY( 70, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
                    }
                    GFX_drawImage([120,  31], 0xFF, 0, 180, MIRROR_BLANK, DIALOGUE);
                } else {
                    if (!mascotWaitModeFlag) {
                        GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                        GFX_drawImageXY(105, 105, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                        GFX_drawImageXY(150, 110, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                    } else {
                        GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                        GFX_drawImageXY(105, 105, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
                        GFX_drawImageXY(150, 110, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
                    }
                    GFX_drawImage([ 31, 120], 0xFF, 0, 90, MIRROR_BLANK, DIALOGUE);
                }
            } else {    
                if (mascotTwistAnimFlag) {
                    MascotTwistAnimUpdate(angle);
                    GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, SILUETTE_SPRITE);

                    if (!mascotWaitModeFlag) {
                        GFX_drawBakedImage([120, 120], 0xFF, 0, mascotTwistAngle, MIRROR_BLANK, BAKED_TWISTTUT_MASCOT_X_SPRITE);
                    } else {
                        GFX_drawBakedImage([120, 120], 0xFF, 0, mascotTwistAngle, MIRROR_BLANK, BAKED_TWISTTUT_WAIT_SPRITE);
                    }
                } else {
                    if (!mascotWaitModeFlag) {
                        GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                        GFX_drawImageXY(120, 115, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                        GFX_drawImageXY(170, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                    } else {
                        GFX_drawImageXY(120, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                        GFX_drawImageXY(120, 115, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
                        GFX_drawImageXY(170, 120, 0xFF, 0, angle, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
                    }
                }
                GFX_drawImage([ 31, 120], 0xFF, 0, 90, MIRROR_BLANK, DIALOGUE);
            }
        }
        case TWISTTUT_ICON: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, TWIST_ICON);
        }
        case TWISTTUT_SPEECH_BOX: {
            GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            if (twistTutorialStage > 1) {
                GFX_drawText([120, 160], 17, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "twist");
                GFX_drawText([120,  70], 17, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFA552, "twist");
            } else if (twistTutorialStage == 1) {
                GFX_drawText([110, 130], 17, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFA552, "good!");
                GFX_drawText([120,  98], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "one more time!");
                GFX_drawText([120,  78], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "twist another");
                GFX_drawText([120,  58], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "side");
            } else {
                GFX_drawText([120, 140], 17, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "twist");
                GFX_drawText([120, 100], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "the cube once");
            }
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
            GFX_drawText(GetGFXPoint(posX_1, posY_1), SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "twist the");
            GFX_drawText(GetGFXPoint(posX_2, posY_2), SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "cube once");
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
            if (!isMainAxisX) {
                GFX_drawImage(GetGFXPoint(posX, posY), arrowIcons[0], 0, angle, MIRROR_BLANK, ARROW_TWIST);
                GFX_drawImage(GetGFXPoint(posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX), arrowIcons[1], 0, angle, MIRROR_BLANK, ARROW_TWIST);
                GFX_drawImage(GetGFXPoint(posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX), arrowIcons[2], 0, angle, MIRROR_BLANK, ARROW_TWIST);
            } else {
                GFX_drawImage(GetGFXPoint(posX, posY), arrowIcons[2], 0, angle, MIRROR_BLANK, ARROW_TWIST);
                GFX_drawImage(GetGFXPoint(posX + 40 * !isMainAxisX, posY + 40 * isMainAxisX), arrowIcons[1], 0, angle, MIRROR_BLANK, ARROW_TWIST);
                GFX_drawImage(GetGFXPoint(posX + 80 * !isMainAxisX, posY + 80 * isMainAxisX), arrowIcons[0], 0, angle, MIRROR_BLANK, ARROW_TWIST);
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