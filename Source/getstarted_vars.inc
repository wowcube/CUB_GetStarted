#define PKT_GENERAL_DATA       P2P_CMD_BASE_SCRIPT_1 + 1
#define PKT_BALL_TILT_TUT      P2P_CMD_BASE_SCRIPT_1 + 2
#define PKT_SELECTOR_TILT_TUT  P2P_CMD_BASE_SCRIPT_1 + 3

#define CROSS_ANIM_DATA .posX_b_1, .posY_b_1, .posX_e_1, .posY_e_1, .posX_b_2, .posY_b_2, .posX_e_2, .posY_e_2

#include "math.inc"
#include "run.inc"

const SCREEN_WIDTH = 240;
const SCREEN_HEIGHT = 240;
const BACKGROUND_COLOR = 0xFF18193F;

const MAX_TANGIBLE_ICONS = 4;
const MAX_ARROW_ICONS = 3;

const MASCOT_CHANGE_TIME = 30000;

const MODULE_GAP = 30;
const HALF_MODULE_GAP = MODULE_GAP >> 1;

const SIMPLE_FONT_SIZE = 7;


new ARROW_TILT          = 0;
new ARROW_TWIST         = 1;
new BALL                = 2;
new COLLECTED_CHECK     = 3;
new COLLECTED_CHECK_RED = 4;
new COLLECTABLE         = 5;
new COLLECTED           = 6;
new COMPLETE_ICON       = 7;
new COUNT_BAR           = 8;

new DIALOGUE              = 9;
new MASCOT_MAIN_SPRITE    = 10;
new MASCOT_SUCCESS_SPRITE = 11;
new MASCOT_WAIT_SPRITE    = 12;
new CIRCLE_QUARTER        = 13;
new SELECTOR              = 14;
new SHAKE_ICON            = 15;
new DIALOGUE_SMALL        = 16;
new TAP_ICON              = 17;
new TILT_ICON             = 18;
new TWIST_ICON            = 19;

new SMALL_STAR            = 20;
new BIG_STAR              = 21;

new CIRCLE_QUARTER_PUSH   = 22;

new FINGER_SPRITE         = 23;
new WRONG_TAP_ICON        = 24;
new WRONG_TAP_RIM         = 26;
new RIGHT_TAP_ICON        = 25;
new RIGHT_TAP_RIM_1       = 26;
new RIGHT_TAP_RIM_2       = 26;
new RIGHT_TAP_RIM_3       = 26;

new SPEECH_BUBBLE_TAP_SPRITE = 26;

new MASCOT_MAIN_EMPTY_SPRITE       = 27;
new MASCOT_MAIN_EYES_NORMAL_SPRITE = 27;
new MASCOT_MAIN_MOUNTH_O_SPRITE    = 27;

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
}

new FSM:applicationState = 0;
new FSM:previousAppState = 0;

new getStarted_screenData[SCREENS_MAX][.sideType, .angle] = [
    [TOPOLOGY_location:LOCATION_UP, 180],
    [TOPOLOGY_location:LOCATION_UP, 180],
    [TOPOLOGY_location:LOCATION_UP, 180],
]

new needNewSideType = 1;
new currentMascotSprite = 0;
new generalDataPkt = 0;
new alreadyLaunched = 0;
new dataSaved = 0;
new neighbor[TOPOLOGY_FACELET_INFO];

const crossLineThickness = 6;
const MAX_CROSS_STAGES = 2;
new leftBottom[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [38, 30, 38, 46,  30, 38, 46, 38], // +
    [32, 32, 43, 43,  32, 43, 43, 32]  // x
];
new rightBottom[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [194, 38, 210, 38,  202, 30, 202, 46], // +
    [196, 32, 207, 43,  196, 43, 207, 32]  // x
];
new rightTop[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [194, 202, 210, 202,  202, 194, 202, 210], // +
    [196, 196, 207, 207,  196, 207, 207, 196]  // x
];
new leftTop[MAX_CROSS_STAGES][CROSS_ANIM_DATA] = [
    [38, 194, 38, 210,  30, 202, 46, 202], // +
    [32, 196, 43, 207,  32, 207, 43, 196]  // x
];

const CROSS_ANIMATION_CHANGE_TIME = 250;
new crossAnimationTimer = 0;
new firstCrossAnimPairStage = 1;
new secondCrossAnimPairStage = 0;

new crossR;
new crossG;
new crossB;
new plusR;
new plusG;
new plusB;

InitCrossAnimationVars(crossColor, plusColor) {
    crossR = crossColor >> 16 & 0xFF;
    crossG = crossColor >>  8 & 0xFF;
    crossB = crossColor       & 0xFF;
    plusR = plusColor >> 16 & 0xFF;
    plusG = plusColor >>  8 & 0xFF;
    plusB = plusColor       & 0xFF;
}

SetDefaultMascot() {
    changeMascotTimer = 0;
    currentMascotSprite = (applicationState != FSM:successScreen) ? (MASCOT_MAIN_SPRITE) : (MASCOT_SUCCESS_SPRITE);
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
    abi_CMD_LINE(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, crossR, crossG, crossB, crossLineThickness, true);
    abi_CMD_LINE(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, crossR, crossG, crossB, crossLineThickness, true);
    
    pos = rightBottom[secondCrossAnimPairStage];
    abi_CMD_LINE(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, plusR, plusG, plusB, crossLineThickness, true);
    abi_CMD_LINE(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, plusR, plusG, plusB, crossLineThickness, true);

    pos = rightTop[firstCrossAnimPairStage];
    abi_CMD_LINE(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, crossR, crossG, crossB, crossLineThickness, true);
    abi_CMD_LINE(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, crossR, crossG, crossB, crossLineThickness, true);
    
    pos = leftTop[secondCrossAnimPairStage];
    abi_CMD_LINE(pos.posX_b_1, pos.posY_b_1, pos.posX_e_1, pos.posY_e_1, plusR, plusG, plusB, crossLineThickness, true);
    abi_CMD_LINE(pos.posX_b_2, pos.posY_b_2, pos.posX_e_2, pos.posY_e_2, plusR, plusG, plusB, crossLineThickness, true);
}

UpdateCrossAnimation(deltaTime) {
    if (crossAnimationTimer >= CROSS_ANIMATION_CHANGE_TIME) {
        firstCrossAnimPairStage = ++firstCrossAnimPairStage % MAX_CROSS_STAGES;
        secondCrossAnimPairStage = ++secondCrossAnimPairStage % MAX_CROSS_STAGES;
        crossAnimationTimer = 0;
    } else {
        crossAnimationTimer += deltaTime;
    }
}

UpdateMascotAnimation(deltaTime) {
    if (changeMascotTimer >= MASCOT_CHANGE_TIME) {
        currentMascotSprite = MASCOT_WAIT_SPRITE;
    } else {
        changeMascotTimer += deltaTime;
    }
    if ((abi_MTD_GetTapsCount() > 0) || (abi_MTD_GetShakesCount() > 0)) {
        SetDefaultMascot();
    }
}