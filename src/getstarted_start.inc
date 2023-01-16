#include "getstarted_vars.inc"

const MASCOT     = 0;
const GREETING   = 1;
const ICONS      = 2;
const SPEECH_BOX = 3;
const TEXT_BOX   = 4;
const COUNTDOWN  = 5;

const TUTORIAL_COUNTDOWN_TIME = 5000;
const DISPLAY_ONE_ICON_TIME = 1250;

new tutorialStartTimer = TUTORIAL_COUNTDOWN_TIME - 1;

new getstarted_startLayout{} = {
    SPEECH_BOX, MASCOT, GREETING, ICONS,
    SPEECH_BOX, MASCOT, GREETING, ICONS,
    COUNTDOWN, COUNTDOWN, TEXT_BOX, COUNTDOWN,
    TEXT_BOX, COUNTDOWN, COUNTDOWN, COUNTDOWN,
    COUNTDOWN, TEXT_BOX, COUNTDOWN, COUNTDOWN,
    COUNTDOWN, COUNTDOWN, TEXT_BOX, COUNTDOWN,
};

new tangibleIcons[MAX_TANGIBLE_ICONS];
new currentTangibleIcon = 0;

const GREETING_TIME = 1000;

new getstartedGreetingFlag = 1;
new getstartedGreetingTimer = GREETING_TIME;

const BAKED_START_MASCOT_SPRITE = 0;

StartBakeSprites() {
    GFX_bakeImage(BAKED_START_MASCOT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(120, 120, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_drawImageXY(115, 135, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
    GFX_drawImageXY(120, 170, 0xFF, 0, 0, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
    GFX_render();
}

DrawStartCountdown(screen) {
    new angle = getStarted_screenData[screen].angle;
    new screenLayout = getstarted_startLayout{getStarted_screenData[screen].sideType * TOPOLOGY_POSITIONS_MAX + angle / 90};
    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);
    switch (screenLayout) {
        case MASCOT: {
            GFX_drawBakedImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, BAKED_START_MASCOT_SPRITE);
            GFX_drawImage([ 31, 120], 0xFF, 0, angle, MIRROR_BLANK, DIALOGUE);
        }
        case GREETING: {
            InitCrossAnimationVars(0xFF5C00, 0x513469);
            DrawCrossAnimation();
            if (getstartedGreetingFlag) {
                GFX_drawText([120, 120], 15, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "hello");
            } else {
                GFX_drawText([120, 170], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "we will");
                GFX_drawText([120, 130], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "start in");
                GFX_drawText([120,  70], 20, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFA552, "%d", tutorialStartTimer / 1000);
            }
        }
        case ICONS: {
            GFX_drawImage([120, 120], 0xFF, 0, angle, MIRROR_BLANK, tangibleIcons[currentTangibleIcon]);
        }
        case SPEECH_BOX: {
            GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
            InitCrossAnimationVars(0xFF5C00, 0xFFB576);
            DrawCrossAnimation();
            GFX_drawText([120,  90], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "here you can");
            GFX_drawText([120, 120], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "learn how to");
            GFX_drawText([120, 150], SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "use wowcube");
        }
        case TEXT_BOX: {
            if (!getstartedGreetingFlag) {
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
                GFX_drawText(GetGFXPoint(posX_1, posY_1), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "we will");
                GFX_drawText(GetGFXPoint(posX_2, posY_2), SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "start in");
            }
        }
        case COUNTDOWN: {
            if (!getstartedGreetingFlag) {
                GFX_drawText([120, 120], 25, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFA552, "%d", tutorialStartTimer / 1000);
            }
        }
    }
}

UpdateStartCountdown(m_deltaTime) {    
    if (SELF_ID == 0) {
        if (getstartedGreetingFlag) {
            getstartedGreetingTimer -= m_deltaTime;
        } else {
            neighbor = TOPOLOGY_getOppositeFacelet(SetFacelet(SELF_ID, 0));
            if (neighbor.connected) {
                tutorialStartTimer -= m_deltaTime;
            }
            if (tutorialStartTimer <= 0) {
                SetApplicationState(FSM:twistTutorial);
                SND_play(ACTION_SOUND, SOUND_VOLUME);
            }
        }
        if (getstartedGreetingTimer <= 0) {
            getstartedGreetingFlag = 0;
        }
    }
    currentTangibleIcon = (tutorialStartTimer / DISPLAY_ONE_ICON_TIME) % MAX_TANGIBLE_ICONS;
}