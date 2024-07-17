#include "getstarted_vars.inc"

const TAPTUT_MASCOT      = 0;
const TAPTUT_EMPTY       = 1;
const TAPTUT_ICON        = 2;
const TAPTUT_TEXT_BOX    = 3;
const RIGHT_WAY          = 4;
const WRONG_WAY          = 5;
const FINGER_ANIM        = 6;
const BUBBLE_TEXT        = 7;
const TAPTUT_BIG_TAP_COUNT   = 8;
const TAPTUT_SMALL_TAP_COUNT = 9;
const TAPTUT_TAPSIDE_ICON    = 10;
const TAPTUT_TEXT_BOX_2      = 11;

new getstarted_tapTutorial{} = {
    TAPTUT_TAPSIDE_ICON, TAPTUT_MASCOT, TAPTUT_BIG_TAP_COUNT, TAPTUT_TEXT_BOX_2,
    TAPTUT_TAPSIDE_ICON, TAPTUT_MASCOT, TAPTUT_BIG_TAP_COUNT, TAPTUT_TEXT_BOX_2,
    TAPTUT_TEXT_BOX, TAPTUT_EMPTY, TAPTUT_SMALL_TAP_COUNT, TAPTUT_EMPTY,
    TAPTUT_TEXT_BOX, TAPTUT_EMPTY, TAPTUT_SMALL_TAP_COUNT, TAPTUT_EMPTY,
    TAPTUT_TEXT_BOX, TAPTUT_EMPTY, TAPTUT_SMALL_TAP_COUNT, TAPTUT_EMPTY,
    TAPTUT_TEXT_BOX, TAPTUT_EMPTY, TAPTUT_SMALL_TAP_COUNT, TAPTUT_EMPTY,
};

new getstarted_tapStartTutorial{} = {
    RIGHT_WAY, FINGER_ANIM, BUBBLE_TEXT, WRONG_WAY,
    RIGHT_WAY, FINGER_ANIM, BUBBLE_TEXT, WRONG_WAY,
    TAPTUT_ICON, TAPTUT_EMPTY, TAPTUT_TEXT_BOX, TAPTUT_EMPTY,
    TAPTUT_ICON, TAPTUT_EMPTY, TAPTUT_TEXT_BOX, TAPTUT_EMPTY,
    TAPTUT_ICON, TAPTUT_EMPTY, TAPTUT_TEXT_BOX, TAPTUT_EMPTY,
    TAPTUT_ICON, TAPTUT_EMPTY, TAPTUT_TEXT_BOX, TAPTUT_EMPTY,
};

const TAPS_TO_FINISH_TUTORIAL = 3;
new tapTutorialStage = 0;
new beginTapTutorial = 0;
new fillTapTutorial  = 0;

new alreadyCheckTapReaction = 0;

new mascotTapReactAnimFlag = 0;
new mascotTapReactAngle = 75;
new tapAngleAnimChange = 4;

new tapAnimationFrame = 0;
new repeatAnimFlag = 0;
new fingerAngle = 0;
new mascotPosY  = 0;
new sideTapIndicatorPos  = 0;

const TAP_RIM_ANIM_REPEAT = 3000;
new wrongTapRimTimer = 180;
new wrongTapRimFlag = 1;
new wrongTapRimResetTimer = 0;

new rightTapRimFlag = 1;
new rightTapRimAlpha_1 = 0;
new rightTapRimAlpha_2 = 0;
new rightTapRimAlpha_3 = 0;
new rightTapRimTimer_1 = 180;
new rightTapRimTimer_2 = 180;
new rightTapRimTimer_3 = 180;
new rightTapRimResetTimer = 0;

const TAPTUT_TRANSITION_STATE_TIME = 750;
new taptutTransitionTimer = 0;

const BAKED_TAPTUT_MASCOT_SPRITE = 0;
const BAKED_TAPTUT_MAS_TAPED_SPRITE = 1;
const BAKED_TAPTUT_WAIT_SPRITE = 2;

TapTutorialBakeSprites() {
    GFX_clearCache();
    GFX_bakeImage(BAKED_TAPTUT_MASCOT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(115, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_TAPTUT_MAS_TAPED_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(115, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_CUTE_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_TAPTUT_WAIT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
    GFX_drawImageXY(115, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
    GFX_render();
}

MascotTapReaction() {
    if (!alreadyCheckTapReaction) {
        mascotTapReactAngle += tapAngleAnimChange;
        if ((mascotTapReactAngle == 87) || (mascotTapReactAngle == 75)) {
            tapAngleAnimChange = -tapAngleAnimChange;
        }
        if (mascotTapReactAngle == 75) {
            mascotTapReactAnimFlag = 0;
        }
        alreadyCheckTapReaction = 1;
    }
}

GetTapFillColor(tapsInPercents) {
    return (0xFF000000
    | 0x68 + ((0xB5 - 0x68) * tapsInPercents) / 100 << 16
    | 0x39 + ((0x2A - 0x39) * tapsInPercents) / 100 << 8
    | 0x8D + ((0xAD - 0x8D) * tapsInPercents) / 100);
}

TapFillBackground(angle) {
    switch (angle) {
        case 0: {
            if (tapTutorialStage >= 2) {
                GFX_clear(0xFF8B329F);
            } else if (tapTutorialStage == 1) {
                GFX_drawRectangle([0, 180, 240, 60], 0xFF8B329F);
                GFX_drawRectangle([0, 120, 240, 60], 0x7F8B329F);
                GFX_drawRectangle([0,  60, 240, 60], 0x198B329F);
            }
        }
        case 270: {
            if (tapTutorialStage >= 2) {
                GFX_clear(0xFF8B329F);
            } else if (tapTutorialStage == 1) {
                GFX_drawRectangle([180, 0, 60, 240], 0xFF8B329F);
                GFX_drawRectangle([120, 0, 60, 240], 0x7F8B329F);
                GFX_drawRectangle([ 60, 0, 60, 240], 0x198B329F);
            }
        }
        case 90: {
            if (tapTutorialStage >= TAPS_TO_FINISH_TUTORIAL) {
                GFX_clear(0xFF8B329F);
            } else if (tapTutorialStage == 2) {
                GFX_drawRectangle([  0, 0, 60, 240], 0xFF8B329F);
                GFX_drawRectangle([ 60, 0, 60, 240], 0x7F8B329F);
                GFX_drawRectangle([120, 0, 60, 240], 0x198B329F);
            }
        }
        case 180: {
            if (tapTutorialStage >= TAPS_TO_FINISH_TUTORIAL) {
                GFX_clear(0xFF8B329F);
            } else if (tapTutorialStage == 2) {
                GFX_drawRectangle([0,   0, 240, 60], 0xFF8B329F);
                GFX_drawRectangle([0,  60, 240, 60], 0x7F8B329F);
                GFX_drawRectangle([0, 120, 240, 60], 0x198B329F);
            }
        }
    }
}

FingerTapAnimation() {
    ++tapAnimationFrame;
    if (tapAnimationFrame >= 11) {
        if (!repeatAnimFlag) {
            repeatAnimFlag = 1;
            fingerAngle = mascotPosY = sideTapIndicatorPos = tapAnimationFrame = 0;
        }
        if (tapAnimationFrame > 33) {
            repeatAnimFlag = 0;
            fingerAngle = mascotPosY = sideTapIndicatorPos = tapAnimationFrame = 0;
        }
        return;
    }
    if (tapAnimationFrame >= 6) {
        fingerAngle -= 8;
        --sideTapIndicatorPos;
    } else if (tapAnimationFrame >= 0) {
        fingerAngle += 8;
        ++sideTapIndicatorPos;
    }
    
    if ((tapAnimationFrame >= 8) && (tapAnimationFrame <= 9)) {
        mascotPosY -= 10;
    } else if ((tapAnimationFrame >= 6) && (tapAnimationFrame <= 7)) {
        mascotPosY += 10;
    }
}

DrawTapTutorial(screen) {
    new angle = getStarted_screenData[screen].angle;
    
    new layoutElement = getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90;
    new screenLayout = (getstarted_tapStartTutorial{layoutElement});
    if (fillTapTutorial) {
        screenLayout = (getstarted_tapTutorial{layoutElement});
    }

    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    if (fillTapTutorial) {
        if ((SELF_ID == 0) && (screen == 0)) {
            // Try to sync side tap indicator movement
            FingerTapAnimation();
        }
        TapFillBackground(TOPOLOGY_getAngle(SetFacelet(SELF_ID, screen), TOPOLOGY_orientation_mode:ORIENTATION_MODE_GRAVITY));
    }
    switch (screenLayout) {
        case TAPTUT_MASCOT: {
            GFX_drawImageXY(120, 232 + sideTapIndicatorPos, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, SIDE_TAP_INDICATOR);
            if (!mascotWaitModeFlag) {
                GFX_drawBakedImageXY(210, 210, 0xFF, 0, 100, 100, 75, MIRROR_BLANK, BAKED_TAPTUT_MASCOT_SPRITE);
            } else {
                GFX_drawBakedImageXY(210, 210, 0xFF, 0, 100, 100, 75, MIRROR_BLANK, BAKED_TAPTUT_WAIT_SPRITE);
            }
            GFX_drawImageXY(91, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, DIALOGUE_SMALL);
            if (beginTapTutorial) {
                if (tapTutorialStage == 0) {
                    GFX_drawTextXY(100, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "repeat");
                    GFX_drawTextXY( 70, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "double tap");
                    GFX_drawTextXY( 40, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "three times");
                } else if (tapTutorialStage == 1) {
                    GFX_drawTextXY(100, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "awesome!");
                    GFX_drawTextXY( 70, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "two more");
                    GFX_drawTextXY( 40, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "to go");
                } else if (tapTutorialStage == 2) {
                    GFX_drawTextXY(100, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "nicely done!");
                    GFX_drawTextXY( 70, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "now the");
                    GFX_drawTextXY( 40, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "last one");
                } else if (tapTutorialStage == TAPS_TO_FINISH_TUTORIAL) {
                    GFX_drawTextXY(80, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "yay");
                    GFX_drawTextXY(60, 125, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "!!!");
                }
            } else {
                GFX_drawTextXY(90, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "moving on");
                GFX_drawTextXY(45, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to the tap");
            }
        }
        case TAPTUT_TEXT_BOX: {
            if (fillTapTutorial) {
                GFX_drawImageXY(60, 60, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, CIRCLE_QUARTER);
                
                if (mascotTapReactAnimFlag) {
                    MascotTapReaction();
                    GFX_drawImageXY(60, 60, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, CIRCLE_QUARTER_PUSH);
                }

                GFX_drawTextXY(120, 160, 9, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "double");
                GFX_drawTextXY(120, 210, 9, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "tap");
            } else {
                GFX_drawTextXY(120, 170, 13, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "double");
                GFX_drawTextXY(120, 135, 13, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "tap");
                GFX_drawTextXY(120,  90, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the side of");
                GFX_drawTextXY(120,  65, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube");
            }
        }
        case TAPTUT_TEXT_BOX_2: {
            GFX_drawTextXY( 70, 120, 13, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "double");
            GFX_drawTextXY(105, 120, 13, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "tap");
            GFX_drawTextXY(150, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the side of");
            GFX_drawTextXY(175, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube");
        }
        case TAPTUT_EMPTY: {
            if (fillTapTutorial) {
                GFX_drawImageXY(60, 60, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, CIRCLE_QUARTER);

                if (mascotTapReactAnimFlag) {
                    MascotTapReaction();
                    if ((angle % 180) == 0) {
                        GFX_drawLineXY(165,  86, 177,  86, 6, 0xFFFFFFFF);
                        GFX_drawLineXY( 86, 165,  86, 177, 6, 0xFFFFFFFF);
                        GFX_drawLineXY(146, 146, 156, 156, 6, 0xFFFFFFFF);
                    }
                    GFX_drawImageXY(60, 60, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, CIRCLE_QUARTER_PUSH);
                }
            }
        }
        case TAPTUT_ICON: {
            GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TAP_ICON);
        }
        case WRONG_WAY: {
            GFX_drawImageXY(90, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, WRONG_TAP_ICON);
            if (wrongTapRimFlag) {
                new step = (FixedCos(((wrongTapRimTimer += 10)) % 359) + 256) >> 1;
                GFX_drawImageXY(90, 120, step, magenta, 100, 100, angle, MIRROR_BLANK, WRONG_TAP_RIM);
            } else {
                wrongTapRimResetTimer += deltaTime;
            }
            if ((wrongTapRimTimer % 360) == 180) {
                wrongTapRimFlag = 0;
            }
            if (wrongTapRimResetTimer >= TAP_RIM_ANIM_REPEAT) {
                wrongTapRimResetTimer = 0;
                wrongTapRimFlag = 1;
            }
            GFX_drawTextXY(170, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "not a soft");
            GFX_drawTextXY(210, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "touch");
        }
        case RIGHT_WAY: {
            if (rightTapRimFlag == 1) {
                rightTapRimAlpha_1 = (FixedCos(((rightTapRimTimer_1 += 15)) % 359) + 256) >> 1;
                if ((rightTapRimTimer_1 % 360) == 180) {
                    --rightTapRimFlag;
                } else if ((rightTapRimTimer_1 % 360) == 0) {
                    ++rightTapRimFlag;
                }
            }
            if (rightTapRimFlag == 2) {
                rightTapRimAlpha_2 = (FixedCos(((rightTapRimTimer_2 += 15)) % 359) + 256) >> 1;
                if ((rightTapRimTimer_2 % 360) == 180) {
                    --rightTapRimFlag;
                } else if ((rightTapRimTimer_2 % 360) == 0) {
                    ++rightTapRimFlag;
                }
            }
            if (rightTapRimFlag == 3) {
                rightTapRimAlpha_3 = (FixedCos(((rightTapRimTimer_3 += 15)) % 359) + 256) >> 1;
                if ((rightTapRimTimer_3 % 360) == 180) {
                    --rightTapRimFlag;
                }
            }
            if (!rightTapRimFlag) {
                rightTapRimResetTimer += deltaTime;
            }
            if (rightTapRimResetTimer >= TAP_RIM_ANIM_REPEAT) {
                rightTapRimResetTimer = 0;
                rightTapRimFlag = 1;
            }
            GFX_drawImageXY(120, 90, rightTapRimAlpha_1, magenta, 100, 100, angle, MIRROR_BLANK, RIGHT_TAP_RIM_1);
            GFX_drawImageXY(120, 90, rightTapRimAlpha_2, magenta, 100, 100, angle, MIRROR_BLANK, RIGHT_TAP_RIM_2);
            GFX_drawImageXY(120, 90, rightTapRimAlpha_3, magenta, 100, 100, angle, MIRROR_BLANK, RIGHT_TAP_RIM_3);
            GFX_drawImageXY(120, 90, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, RIGHT_TAP_ICON);
            GFX_drawTextXY(120, 170, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "but a light");
            GFX_drawTextXY(120, 210, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "knock");
        }
        case FINGER_ANIM: {
            FingerTapAnimation();
            GFX_drawImageXY(60, 227, 0xFF, magenta, 100, 100, angle - 10 - fingerAngle, MIRROR_BLANK, FINGER_SPRITE);
            new curMascotSprite = BAKED_TAPTUT_MASCOT_SPRITE;
            if ((tapAnimationFrame >= 5) && tapAnimationFrame <= 7) {
                curMascotSprite = BAKED_TAPTUT_MAS_TAPED_SPRITE;
                GFX_drawLineXY( 60, 183,  48, 191, 8, 0xFFFFFFFF);
                GFX_drawLineXY(170, 180, 185, 190, 8, 0xFFFFFFFF);
                GFX_drawLineXY(150, 195, 154, 215, 8, 0xFFFFFFFF);
            }
            if (mascotWaitModeFlag) {
                curMascotSprite = BAKED_TAPTUT_WAIT_SPRITE;
            }
            GFX_drawBakedImageXY(120, 90 - mascotPosY, 0xFF, 0, 100, 100, angle, MIRROR_BLANK, curMascotSprite);
        }
        case BUBBLE_TEXT: {
            GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_X, SPEECH_BUBBLE_TAP_SPRITE);
            GFX_drawTextXY(140, 170, 12, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "double");
            GFX_drawTextXY(140, 135, 12, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "tap");
            GFX_drawTextXY(140, 100, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "the side of");
            GFX_drawTextXY(140,  75, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "the cube!");
        }
        case TAPTUT_BIG_TAP_COUNT: {
            GFX_drawTextXY(120, 170, 35, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "%02d", tapTutorialStage);
            GFX_drawTextXY(120,  70, 35, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "03");
        }
        case TAPTUT_SMALL_TAP_COUNT: {
            GFX_drawImageXY(60, 60, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, CIRCLE_QUARTER);

            if (mascotTapReactAnimFlag) {
                MascotTapReaction();
                if ((angle % 180) == 0) {
                    GFX_drawLineXY(165,  86, 177,  86, 6, 0xFFFFFFFF);
                    GFX_drawLineXY( 86, 165,  86, 177, 6, 0xFFFFFFFF);
                    GFX_drawLineXY(146, 146, 156, 156, 6, 0xFFFFFFFF);
                }
                GFX_drawImageXY(60, 60, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, CIRCLE_QUARTER_PUSH);
            }
            GFX_drawTextXY(200, 205, 12, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "%02d", tapTutorialStage);
            GFX_drawTextXY(200, 175, 12, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "03");
        }
        case TAPTUT_TAPSIDE_ICON: {
            GFX_drawImageXY(232 + sideTapIndicatorPos, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, SIDE_TAP_INDICATOR);
            GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TAP_SIDE_ICON);
        }
    }

    if (screen == 2) {
        alreadyCheckTapReaction = 0;
    }
}

UpdateTapTutorial(m_deltaTime) {
    if (SELF_ID == 0) {
        if (fillTapTutorial) {
            if (tapTutorialStage >= TAPS_TO_FINISH_TUTORIAL) {
                taptutTransitionTimer += m_deltaTime;
                if (taptutTransitionTimer >= TAPTUT_TRANSITION_STATE_TIME) {
                    SetApplicationState(FSM:successScreen);
                    SND_play(TAP_STAGE_SUCCESS_SOUND, SOUND_VOLUME);
                }
            }
        }
    }
}