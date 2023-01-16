#include "getstarted_vars.inc"

const FL_MASCOT_SPRITE = 0;

const FL_EMPTY        = 0;
const FL_MASCOT       = 1;
const LLO             = 2;
const HE              = 3;
const FL_WELCOME_TEXT = 4;
const HI              = 5;
const FL_QUESTION     = 6;
const FL_TWIST_FIRST_TEXT = 7;
const FL_TWIST_FIRST_ICON = 8;

new getstarted_firstLaunch_hello{} = {
    LLO, FL_WELCOME_TEXT, FL_MASCOT, HE,
    LLO, FL_WELCOME_TEXT, FL_MASCOT, HE,
    FL_WELCOME_TEXT, FL_EMPTY, HI, FL_EMPTY,
    FL_WELCOME_TEXT, FL_EMPTY, HI, FL_EMPTY,
    FL_WELCOME_TEXT, FL_EMPTY, HI, FL_EMPTY,
    FL_WELCOME_TEXT, FL_EMPTY, HI, FL_EMPTY,
};

new getstarted_firstLaunch_first{} = {
    FL_TWIST_FIRST_TEXT, FL_WELCOME_TEXT, FL_MASCOT, FL_TWIST_FIRST_ICON,
    FL_TWIST_FIRST_TEXT, FL_WELCOME_TEXT, FL_MASCOT, FL_TWIST_FIRST_ICON,
    FL_TWIST_FIRST_TEXT, FL_QUESTION, HI, FL_TWIST_FIRST_ICON,
    FL_TWIST_FIRST_TEXT, FL_QUESTION, HI, FL_TWIST_FIRST_ICON,
    FL_TWIST_FIRST_TEXT, FL_QUESTION, HI, FL_TWIST_FIRST_ICON,
    FL_TWIST_FIRST_TEXT, FL_QUESTION, HI, FL_TWIST_FIRST_ICON,
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

const FL_APPEAR_TIME = 1000;
new fl_ChoiceAppearAnimTimer = 0;
new fl_purpleTwistIconTimer = 0;

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

FirstLaunchBakeSprites() {
    GFX_bakeImage(FL_MASCOT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(125, 130, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
}

FirstLaunchFontPercents() {
    return (((100 * fl_ChoiceAppearAnimTimer) / FL_APPEAR_TIME) * 0xFF) / 100;
}

FirstLaunchPercents(maxValue, minValue) {
    new percent = ((100 * fl_ChoiceAppearAnimTimer) / FL_APPEAR_TIME);
    return ((maxValue - minValue) * (100 - percent)) / 100;
}

DrawFirstLaunch(screen) {
    new angle = getStarted_screenData[screen].angle;
    
    new layoutElement = getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90;
    new screenLayout = (changeLayoutFlag) ? (getstarted_firstLaunch_first{layoutElement}) : (getstarted_firstLaunch_hello{layoutElement});
    
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    switch (screenLayout) {
        case FL_MASCOT: {
            GFX_drawImage([217,  85], 0xFF, 0, angle, MIRROR_BLANK, OFF_HAND_SPRITE);
            GFX_drawImage([ 40, 160], 0xFF, 0, angle, MIRROR_BLANK, mascotHandWaveAnimFrames{mascotHandWaveAnimFrame});
            GFX_drawBakedImage([135, 120], 0xFF, 0, 170, MIRROR_BLANK, FL_MASCOT_SPRITE);
        }
        case HI: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, HI_SPRITE);
        }
        case FL_WELCOME_TEXT: {
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
        case FL_QUESTION: {
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            
            GFX_drawText([140, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "want to learn");
            GFX_drawText([120, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "how to use the");
            GFX_drawText([100, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "cube?");
        }
        case FL_TWIST_FIRST_TEXT: {
            new alpha = 0xFF;
            if (fl_ChoiceAppearAnimTimer < FL_APPEAR_TIME) {
                alpha = FirstLaunchFontPercents();
            }
            GFX_drawText([120, 100], SIMPLE_FONT_SIZE + 2, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0x8E36FF), "twist");
            GFX_drawText([120, 140], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, ((alpha << 24) | 0xFFFFFF), "to continue");
        }
        case FL_TWIST_FIRST_ICON: {
            new alpha = 0xFF;
            if (fl_ChoiceAppearAnimTimer < FL_APPEAR_TIME) {
                alpha = FirstLaunchFontPercents();
            }
            GFX_drawImageXY(120, 120, alpha, 0, angle, MIRROR_BLANK,
                           (fl_purpleTwistIconTimer > 500) ? (TWIST_PURPLE_ICON_1_SPRITE) : (TWIST_PURPLE_ICON_2_SPRITE));
        }
    }
    for (new arcI = radianCurtainFlag; arcI < MAX_ARCS; ++arcI) {
        GFX_drawImageXY(arcsInCurtain[arcI].x, arcsInCurtain[arcI].y, 0xFF, 0, 0, MIRROR_BLANK, arcsInCurtain[arcI].sprite);
    }
}

UpdateFirstLaunch(m_deltaTime) {
    if (flStartFlag) {
        if (radianCurtainFlag < MAX_ARCS) {
            radianCurtainTimer += m_deltaTime;
            radianCurtainFlag = radianCurtainTimer / VANISH_TIME_PER_ARC;
        } else {
            if (changeLayoutFlag) {
                fl_ChoiceAppearAnimTimer += m_deltaTime;
                fl_purpleTwistIconTimer += m_deltaTime;
                if (fl_purpleTwistIconTimer > FL_APPEAR_TIME) {
                    fl_purpleTwistIconTimer = 0;
                }
            }

            if (!changeLayoutFlag) {
                if (helloVanishFlag) {
                    helloVanishTimer += m_deltaTime;
                    helloVanishFlag = 1 + helloVanishTimer / HELLO_VANISH_ONE_STEP_TIME;
                    if (helloVanishTimer >= HELLO_VANISH_TIME) {
                        changeLayoutFlag = 1;
                    }
                } else {
                    changeLayoutTimer += m_deltaTime;
                    if (changeLayoutTimer >= CHANGE_LAYOUT_TIME) {
                        helloVanishFlag = 1;
                    }
                }
            }

            mascotHandWaveAnimTimer += m_deltaTime;
            if (mascotHandWaveAnimTimer >= HAND_WAVE_ONE_FRAME_TIME) {
                mascotHandWaveAnimFrame = ++mascotHandWaveAnimFrame % MAX_HAND_WAVE_ANIM_FRAMES;
                mascotHandWaveAnimTimer -= HAND_WAVE_ONE_FRAME_TIME;
            }
        }
    }
    if ((SELF_ID == 0) && (!flStartFlag)) {
        flStartTimer += m_deltaTime;
        if (flStartTimer > 1000) {
            flStartFlag = 1;
        }
    }
}