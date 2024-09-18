#pragma warning disable 203 // global never used
#pragma warning disable 204 // never used variable
#pragma warning disable 213 // tag mismatch
#pragma warning disable 217 // loose indentation
#pragma warning disable 229

#define PKT_GENERAL_DATA       1
#define PKT_BALL_TILT_TUT      2
#define PKT_SELECTOR_TILT_TUT  3
#define PKT_MAPPING            4
#define PKT_DOTROAD_TILT_TUT   5

#define CROSS_ANIM_DATA .posX_b_1, .posY_b_1, .posX_e_1, .posY_e_1, .posX_b_2, .posY_b_2, .posX_e_2, .posY_e_2

#define SPRITE_TEXT

#include "math.inc"
#include "wowcore.inc"
#include "topology.inc"
#include "graphics.inc"
#include "string.inc"
#include "log.inc"
#include "sound.inc"
#include "motion_sensor.inc"

const SCREEN_WIDTH = 240;
const SCREEN_HEIGHT = 240;
const BACKGROUND_COLOR = 0xFF1D1027;

const MAX_TANGIBLE_ICONS = 4;
const MAX_ARROW_ICONS = 3;

const MASCOT_CHANGE_TIME = 10000;

const MODULE_GAP = 30;
const HALF_MODULE_GAP = MODULE_GAP >> 1;

const SIMPLE_FONT_SIZE = 7;


new ARROW_TILT          = 0;
new ARROW_TWIST         = 1;
new BALL                = 2;
new COLLECTED_CHECK     = 3;
new COLLECTABLE         = 5;
new COLLECTED           = 6;
new COMPLETE_ICON       = 7;
new COUNT_BAR           = 8;

new DIALOGUE              = 9;
new CIRCLE_QUARTER        = 13;
new SELECTOR              = 14;
new SHAKE_ICON            = 15;
new DIALOGUE_SMALL        = 16;
new TAP_ICON              = 17;
new TILT_ICON             = 18;
new TWIST_ICON            = 19;

new BIG_STAR              = 21;

new CIRCLE_QUARTER_PUSH   = 22;

new FINGER_SPRITE         = 23;
new WRONG_TAP_ICON        = 24;
new RIGHT_TAP_ICON        = 25;

new MASCOT_MAIN_EMPTY_SPRITE       = 27;
new MASCOT_MAIN_EYES_NORMAL_SPRITE = 27;
new MASCOT_MAIN_EYES_CUTE_SPRITE   = 27;
new MASCOT_MAIN_EYES_X_SPRITE      = 27;
new MASCOT_MAIN_MOUNTH_O_SPRITE    = 27;

new MASCOT_SUCCESS_BODY_SPRITE   = 27;
new MASCOT_SUCCESS_EYES_SPRITE   = 27;
new MASCOT_SUCCESS_MOUNTH_SPRITE = 27;

new HI_SPRITE = 28;
new OFF_HAND_SPRITE = 28;
new MAIN_HAND_1_SPRITE = 28;
new MAIN_HAND_2_SPRITE = 28;
new MAIN_HAND_3_SPRITE = 28;
new HE_GREEN_SPRITE  = 28;
new HE_ORANGE_SPRITE = 28;
new HE_PINK_SPRITE   = 28;
new HE_PURPLE_SPRITE = 28;
new HE_WHITE_SPRITE  = 28;
new LLO_GREEN_SPRITE  = 28;
new LLO_ORANGE_SPRITE = 28;
new LLO_PINK_SPRITE   = 28;
new LLO_PURPLE_SPRITE = 28;
new LLO_WHITE_SPRITE  = 28;
new ARC_1_SPRITE = 28;
new ARC_2_SPRITE = 28;
new ARC_3_SPRITE = 28;
new ARC_4_SPRITE = 28;
new ARC_5_SPRITE = 28;

new SIDE_TAP_INDICATOR = 29;
new SILUETTE_SPRITE = 29;

new TWIST_TEXT_ORANGE = 30;
new TWIST_TEXT_PURPLE = 30;
new GOOD_TEXT_GREEN   = 30;
new GOOD_TEXT_ORANGE  = 30;
new MASCOT_SUCCESS_EYEBROWS_SPRITE = 30;
new DOUBLE_PAT_TEXT_ORANGE_SPRITE = 30;
new DOUBLE_PAT_TEXT_GREEN_SPRITE = 30;
new VERY_GOOD_TEXT_GREED_SPRITE = 30;
new countOrangeXL[4];
new countOrangeS[4];
new _03_WHITE_TEXT_SPRITE = 30;
new _03_WHITE_TEXT_SMALL_SPRITE = 30;
new TAP_ICON_GREEN_SPRITE = 30;
new PLUS_ONE_TEXT_SPRITE = 30;
new EXCELLENT_TEXT_SPRITE = 30;
new COLLECTED_CHECK_GREEN = 30;
new SHAKE_L_TEXT_SPRITE = 30;
new SHAKE_XL_TEXT_SPRITE = 30;
new TWIST_ICON_GREEN = 30;
new TWIST_TEXT_ORANGE_L = 30;
new TWIST_TEXT_GREEN_L = 30;
new HIGHTLIGHT_GREEN = 30;
new BALL_EFFECT_ORANGE = 30;
new BALL_EFFECT_GREEN = 30;

new bakeAppStateSpritesFlag = 0;

new ACTION_SOUND = 0;
new GOOD_SOUND = 1;
new EXCELLENT_1_SOUND = 2;
new EXCELLENT_2_SOUND = 3;
new TAPS_1_2_SOUND = 4;
new TAPS_3_4_SOUND = 5;
new TAPS_5_6_SOUND = 6;
new TAP_STAGE_SUCCESS_SOUND = 7;
new PLUS_1_SHAPE_COLLECT_1_SOUND = 8;
new PLUS_1_SHAPE_COLLECT_2_SOUND = 9;
new PLUS_1_SHAPE_COLLECT_3_SOUND = 10;
new SELECTOR_MENU_SOUND = 11;
const SOUND_VOLUME = 255;

new tiltTutCollectableSounds{SCREENS_MAX};

const MASCOT_WAIT_TIME = 1000;
new currentTime = 0;
new previousTime = 0;
new deltaTime = 0;
new changeMascotTimer = 0;

const FSM: {
    start = 1,
    twistTutorial,
    tapTutorial,
    tiltTutorial,
    shakeTutorial,
    successScreen,
    firstLaunch,
}

new FSM:applicationState = 0;
new FSM:previousAppState = 0;

new getStarted_AllScreensData[MODULES_MAX * SCREENS_MAX][.face, .pos] = [
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
    [ORIENTATION_UP, 0], [ORIENTATION_UP, 0], [ORIENTATION_UP, 0],
]

new generalDataPkt = 0;
new mappingPkt = 0;
new alreadyLaunched = 0;
new neighbor[TOPOLOGY_FACELET_INFO];

const crossLineThickness = 6;
const MAX_CROSS_STAGES = 2;
new leftBottom[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [20, 12, 20, 28,  12, 20, 28, 20], // +
    [14, 14, 26, 26,  14, 26, 26, 14]  // x
];
new rightBottom[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [220, 12, 220, 28,  212, 20, 228, 20], // +
    [214, 14, 226, 26,  214, 26, 226, 14]  // x
];
new rightTop[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [220, 212, 220, 228,  212, 220, 228, 220], // +
    [214, 214, 226, 226,  214, 226, 226, 214]  // x
];
new leftTop[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [20, 212, 20, 228,  12, 220, 28, 220], // +
    [14, 214, 26, 226,  14, 226, 26, 214]  // x
];

const CROSS_ANIMATION_CHANGE_TIME = 250;
new crossAnimationTimer = 0;
new firstCrossAnimPairStage = 1;
new secondCrossAnimPairStage = 0;

new prevAccelX = 0;
new prevAccelY = 0;
new prevAccelZ = 0;

new crossColor = 0xFFFFFFFF;
new plusColor  = 0xFFFFFFFF;

new magenta = 0xFFFF00FF;

Swap(&value_1, &value_2) {
    new temp = value_1;
    value_1 = value_2;
    value_2 = temp;
}

InitCrossAnimationVars(m_crossColor, m_plusColor) {
    crossColor = m_crossColor;
    plusColor  = m_plusColor;
}

CheckAcceleration() {
    new tempAccelX = MS_getFaceAccelX(0) / 20;
    new tempAccelY = MS_getFaceAccelY(0) / 20;
    new tempAccelZ = MS_getFaceAccelZ(0) / 20;

    if ((prevAccelX != tempAccelX)
     || (prevAccelY != tempAccelY)
     || (prevAccelZ != tempAccelZ)) {
        prevAccelX = tempAccelX;
        prevAccelY = tempAccelY;
        prevAccelZ = tempAccelZ;

        SetDefaultMascot();
    }
}

SetDefaultMascot() {
    changeMascotTimer = 0;
}

SetFacelet(module, screen) {
    new facelet[TOPOLOGY_FACELET];
    facelet.module = module;
    facelet.screen = screen;
    return facelet;
}

SetPlace(face, position) {
    new place[TOPOLOGY_PLACE];
    place.face = face;
    place.position = position;
    return place;
}

GetRightScreen(screen) {
    return ((screen - 1) % SCREENS_MAX);
}

GetBottomScreen(screen) {
    return ((screen + 1) % SCREENS_MAX);
}

DrawCrossAnimation() {
    new pos[CROSS_ANIM_DATA];

    pos = leftBottom[firstCrossAnimPairStage];
    GFX_drawLineXY(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, crossLineThickness, crossColor);
    GFX_drawLineXY(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, crossLineThickness, crossColor);
    
    pos = rightBottom[secondCrossAnimPairStage];
    GFX_drawLineXY(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, crossLineThickness, plusColor);
    GFX_drawLineXY(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, crossLineThickness, plusColor);

    pos = rightTop[firstCrossAnimPairStage];
    GFX_drawLineXY(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, crossLineThickness, crossColor);
    GFX_drawLineXY(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, crossLineThickness, crossColor);
    
    pos = leftTop[secondCrossAnimPairStage];
    GFX_drawLineXY(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, crossLineThickness, plusColor);
    GFX_drawLineXY(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, crossLineThickness, plusColor);
}

UpdateCrossAnimation(m_deltaTime) {
    if (crossAnimationTimer >= CROSS_ANIMATION_CHANGE_TIME) {
        firstCrossAnimPairStage = ++firstCrossAnimPairStage % MAX_CROSS_STAGES;
        secondCrossAnimPairStage = ++secondCrossAnimPairStage % MAX_CROSS_STAGES;
        crossAnimationTimer = 0;
    } else {
        crossAnimationTimer += m_deltaTime;
    }
}

UpdateMascotAnimation(m_deltaTime) {
    /*if (changeMascotTimer >= MASCOT_CHANGE_TIME) {
        mascotWaitModeFlag = 1;
    } else {
        changeMascotTimer += m_deltaTime;
    }*/
}