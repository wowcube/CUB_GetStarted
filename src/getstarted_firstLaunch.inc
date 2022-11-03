#include "getstarted_vars.inc"

const EMPTY        = 0;
const MASCOT       = 1;
const LLO          = 2;
const HE           = 3;
const WELCOME_TEXT = 4;
const HI           = 5;
const FL_TWIST     = 6;
const FL_TAP       = 7;
const FL_QUESTION  = 8;

new getstarted_firstLaunch_hello{} = {
    LLO, WELCOME_TEXT, MASCOT, HE,
    LLO, WELCOME_TEXT, MASCOT, HE,
    WELCOME_TEXT, EMPTY, HI, EMPTY,
    WELCOME_TEXT, EMPTY, HI, EMPTY,
    WELCOME_TEXT, EMPTY, HI, EMPTY,
    WELCOME_TEXT, EMPTY, HI, EMPTY,
};

new getstarted_firstLaunch_choice{} = {
    FL_TAP, WELCOME_TEXT, MASCOT, FL_TWIST,
    FL_TAP, WELCOME_TEXT, MASCOT, FL_TWIST,
    FL_TAP, FL_QUESTION, FL_TWIST, EMPTY,
    FL_TAP, FL_QUESTION, FL_TWIST, EMPTY,
    FL_TAP, FL_QUESTION, FL_TWIST, EMPTY,
    FL_TAP, FL_QUESTION, FL_TWIST, EMPTY,
};

const MAX_HAND_WAVE_ANIM_FRAMES = 4;
const HAND_WAVE_ONE_FRAME_TIME = 200;
new mascotHandWaveAnimFrames{4};
new mascotHandWaveAnimTimer = 0;
new mascotHandWaveAnimFrame = 0;

const CHANGE_LAYOUT_TIME = 3000;
const HELLO_VANISH_ONE_STEP_TIME = 200;
const HELLO_VANISH_TIME = HELLO_VANISH_ONE_STEP_TIME * 5;
new changeLayoutTimer = 0;
new changeLayoutFlag = 0;
new helloVanishTimer = 0;
new helloVanishFlag = 0;

const CHANGE_LAYOUT_TIME = 1000;
new fl_ChoiceAppearAnimTimer = 0;

const FL_RIM_ANIM_REPEAT = 2000;
new twistRimFlag = 1;
new twistRimAlpha_1 = 0;
new twistRimAlpha_2 = 0;
new twistRimTimer_1 = 180;
new twistRimTimer_2 = 180;
new twistRimResetTimer = 0;
new tapRimFlag = 1;
new tapRimAlpha = 0;
new tapRimTimer = 180;
new tapRimResetTimer = 0;

const MAX_ARCS = 5;
const VANISH_TIME_PER_ARC = 300;
new flStartTimer = 0;
new flStartFlag = 0;
new radianCurtainTimer = 0;
new radianCurtainFlag = 0;
new arcsInCurtain[MAX_ARCS][.x, .y, .sprite] = [
    [ 63,  63, 1],
    [ 92,  92, 1],
    [120, 120, 1],
    [120, 120, 1],
    [120, 120, 1],
]

FirstLaunchFontPercents() {
    return (((100 * fl_ChoiceAppearAnimTimer) / CHANGE_LAYOUT_TIME) * 0xFF) / 100;
}

FirstLaunchPercents(maxValue, minValue) {
    new percent = ((100 * fl_ChoiceAppearAnimTimer) / CHANGE_LAYOUT_TIME);
    return ((maxValue - minValue) * (100 - percent)) / 100;
}

DrawFirstLaunch(screen) {
    new angle = getStarted_screenData[screen].angle;
    
    new layoutElement = getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90;
    new screenLayout = (changeLayoutFlag) ? (getstarted_firstLaunch_choice{layoutElement}) : (getstarted_firstLaunch_hello{layoutElement});
    
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    switch (screenLayout) {
        case MASCOT: {
            GFX_drawImage([217,  85], 0xFF, 0, angle, MIRROR_BLANK, OFF_HAND_SPRITE);
            GFX_drawImage([ 40, 160], 0xFF, 0, angle, MIRROR_BLANK, mascotHandWaveAnimFrames{mascotHandWaveAnimFrame});
            GFX_drawImage([135, 120], 0xFF, 0, 170, MIRROR_BLANK, currentMascotSprite);
        }
        case HI: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, HI_SPRITE);
        }
        case WELCOME_TEXT: {
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();

            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;

            if (angle == 90) {
                posX_1 = 140;
                posY_1 = 120;
                posX_2 = 100;
                posY_2 = 120;
            }
            
            GFX_drawText(GetGFXPoint(posX_1, posY_1), SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "welcome");
            GFX_drawText([120, 120], SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "to wowcube");
            GFX_drawText(GetGFXPoint(posX_2, posY_2), SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "world");
        }
        case HE: {
            if (helloVanishFlag <= 5) {
                GFX_drawImage([185, 120], 0xFF, 0, angle, MIRROR_BLANK, HE_GREEN_SPRITE);
            }
            if (helloVanishFlag <= 4) {
                GFX_drawImage([174, 120], 0xFF, 0, angle, MIRROR_BLANK, HE_ORANGE_SPRITE);
            }
            if (helloVanishFlag <= 3) {
                GFX_drawImage([153, 120], 0xFF, 0, angle, MIRROR_BLANK, HE_PINK_SPRITE);
            }
            if (helloVanishFlag <= 2) {
                GFX_drawImage([140, 120], 0xFF, 0, angle, MIRROR_BLANK, HE_PURPLE_SPRITE);
            }
            if (helloVanishFlag <= 1) {
                GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, HE_WHITE_SPRITE);
            }
        }
        case LLO: {
            if (helloVanishFlag <= 5) {
                GFX_drawImage([120, 185], 0xFF, 0, angle, MIRROR_BLANK, LLO_GREEN_SPRITE);
            }
            if (helloVanishFlag <= 4) {
                GFX_drawImage([120, 174], 0xFF, 0, angle, MIRROR_BLANK, LLO_ORANGE_SPRITE);
            }
            if (helloVanishFlag <= 3) {
                GFX_drawImage([120, 153], 0xFF, 0, angle, MIRROR_BLANK, LLO_PINK_SPRITE);
            }
            if (helloVanishFlag <= 2) {
                GFX_drawImage([120, 140], 0xFF, 0, angle, MIRROR_BLANK, LLO_PURPLE_SPRITE);
            }
            if (helloVanishFlag <= 1) {
                GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, LLO_WHITE_SPRITE);
            }
        }
        case FL_TAP: {
            new alpha = 0xFF;
            new offsetY = 0;
            if (fl_ChoiceAppearAnimTimer < CHANGE_LAYOUT_TIME) {
                offsetY = FirstLaunchPercents(100, 80);
                alpha = FirstLaunchFontPercents();
            }
            GFX_drawImageXY(120, 80 + offsetY, alpha, 0, angle, MIRROR_BLANK, SHAKE_ORANGE_ICON_SPRITE);
            GFX_drawText([120, 180], SIMPLE_FONT_SIZE + 2, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0xF19752), "shake");
            GFX_drawText([120, 210], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0xFFFFFF), "to exit");
            if (fl_ChoiceAppearAnimTimer > CHANGE_LAYOUT_TIME) {
                GFX_drawImage([120, 80], tapRimAlpha, 0, angle, MIRROR_BLANK, FL_TAP_RIM_2);
            }
        }
        case FL_TWIST: {
            new alpha = 0xFF;
            new offset = 0;
            if (fl_ChoiceAppearAnimTimer < CHANGE_LAYOUT_TIME) {
                offset = FirstLaunchPercents(100, 80);
                alpha = FirstLaunchFontPercents();
            }
            if (angle == 180) {
                GFX_drawImageXY(120, 160 - offset, alpha, 0, angle, MIRROR_BLANK, TWIST_GREEN_ICON_SPRITE);
                GFX_drawText([120, 60], SIMPLE_FONT_SIZE + 2, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0x07C995), "twist");
                GFX_drawText([120, 30], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0xFFFFFFFF), "to continue");
                if (fl_ChoiceAppearAnimTimer > CHANGE_LAYOUT_TIME) {
                    GFX_drawImage([120, 160], twistRimAlpha_1, 0, angle, MIRROR_BLANK, FL_TWIST_RIM_1);
                    GFX_drawImage([120, 160], twistRimAlpha_2, 0, angle, MIRROR_BLANK, FL_TWIST_RIM_2);
                }
            } else {
                GFX_drawImageXY(80 + offset, 120, alpha, 0, angle, MIRROR_BLANK, TWIST_GREEN_ICON_SPRITE);
                GFX_drawText([180, 120], SIMPLE_FONT_SIZE + 2, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0xFF07C995), "twist");
                GFX_drawText([210, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0xFFFFFFFF), "to continue");
                if (fl_ChoiceAppearAnimTimer > CHANGE_LAYOUT_TIME) {
                    GFX_drawImage([80, 120], twistRimAlpha_1, 0, angle, MIRROR_BLANK, FL_TWIST_RIM_1);
                    GFX_drawImage([80, 120], twistRimAlpha_2, 0, angle, MIRROR_BLANK, FL_TWIST_RIM_2);
                }
            }
        }
        case FL_QUESTION: {
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            
            GFX_drawText([140, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "want to learn");
            GFX_drawText([120, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "how to use the");
            GFX_drawText([100, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "cube?");
        }
    }
    for (new arcI = radianCurtainFlag; arcI < MAX_ARCS; ++arcI) {
        GFX_drawImageXY(arcsInCurtain[arcI].x, arcsInCurtain[arcI].y, 0xFF, 0, 0, MIRROR_BLANK, arcsInCurtain[arcI].sprite);
    }
}

UpdateFirstLaunch(deltaTime) {
    if (flStartFlag) {
        if (radianCurtainFlag < MAX_ARCS) {
            radianCurtainTimer += deltaTime;
            radianCurtainFlag = radianCurtainTimer / VANISH_TIME_PER_ARC;
        } else {
            if (changeLayoutFlag) {
                fl_ChoiceAppearAnimTimer += deltaTime;

                // Twist
                if (twistRimFlag == 1) {
                    twistRimAlpha_1 = (FixedCos(((twistRimTimer_1 += 15)) % 359) + 256) >> 1;
                    if ((twistRimTimer_1 % 360) == 180) {
                        --twistRimFlag;
                    } else if ((twistRimTimer_1 % 360) == 0) {
                        ++twistRimFlag;
                    }
                }
                if (twistRimFlag == 2) {
                    twistRimAlpha_2 = (FixedCos(((twistRimTimer_2 += 15)) % 359) + 256) >> 1;
                    if ((twistRimTimer_2 % 360) == 180) {
                        --twistRimFlag;
                    }
                }
                if (!twistRimFlag) {
                    twistRimResetTimer += deltaTime;
                }
                if (twistRimResetTimer >= FL_RIM_ANIM_REPEAT) {
                    twistRimResetTimer = 0;
                    twistRimFlag = 1;
                }
                // Tap
                if (tapRimFlag) {
                    tapRimAlpha = (FixedCos(((tapRimTimer += 10)) % 359) + 256) >> 1;
                } else {
                    tapRimResetTimer += deltaTime;
                }
                if ((tapRimTimer % 360) == 180) {
                    tapRimFlag = 0;
                }
                if (tapRimResetTimer >= FL_RIM_ANIM_REPEAT) {
                    tapRimResetTimer = 0;
                    tapRimFlag = 1;
                }
            }

            if (!changeLayoutFlag) {
                if (helloVanishFlag) {
                    helloVanishTimer += deltaTime;
                    helloVanishFlag = 1 + helloVanishTimer / HELLO_VANISH_ONE_STEP_TIME;
                    if (helloVanishTimer >= HELLO_VANISH_TIME) {
                        changeLayoutFlag = 1;
                    }
                } else {
                    changeLayoutTimer += deltaTime;
                    if (changeLayoutTimer >= CHANGE_LAYOUT_TIME) {
                        helloVanishFlag = 1;
                    }
                }
            }

            mascotHandWaveAnimTimer += deltaTime;
            if (mascotHandWaveAnimTimer >= HAND_WAVE_ONE_FRAME_TIME) {
                mascotHandWaveAnimFrame = ++mascotHandWaveAnimFrame % MAX_HAND_WAVE_ANIM_FRAMES;
                mascotHandWaveAnimTimer -= HAND_WAVE_ONE_FRAME_TIME;
            }
        }
    }
    if ((SELF_ID == 0) && (!flStartFlag)) {
        flStartTimer += deltaTime;
        if (flStartTimer > 1000) {
            flStartFlag = 1;
        }
    }
}