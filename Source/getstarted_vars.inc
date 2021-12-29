#define PKT_GENERAL_DATA       P2P_CMD_BASE_SCRIPT_1 + 1
#define PKT_BALL_TILT_TUT      P2P_CMD_BASE_SCRIPT_1 + 2
#define PKT_SELECTOR_TILT_TUT  P2P_CMD_BASE_SCRIPT_1 + 3

#define CROSS_ANIM_DATA .posX_b_1, .posY_b_1, .posX_e_1, .posY_e_1, .posX_b_2, .posY_b_2, .posX_e_2, .posY_e_2

#include "physics.inc"
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

new ARROW_1 = 0;
new ARROW_2 = 1;
new ARROW_3 = 2;

new ARROW_TILT      = 3;
new BALL            = 4;
new COLLECTED_CHECK = 5;
new COLLECTABLE     = 6;
new COLLECTED       = 7;
new COMPLETE_ICON   = 8;
new COUNT_BAR       = 9;

new DIALOGUE              = 10;
new MASCOT_MAIN_SPRITE    = 11;
new MASCOT_SUCCESS_SPRITE = 12;
new MASCOT_WAIT_SPRITE    = 13;
new CIRCLE_QUARTER        = 14;
new SELECTOR              = 15;
new SHAKE_ICON            = 16;
new TAP_ICON              = 17;
new TILT_ICON             = 18;
new TWIST_ICON            = 19;

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

new getStarted_screenData[FACES_MAX][.sideType, .angle] = [
    [topology_location:location_top, 180],
    [topology_location:location_top, 180],
    [topology_location:location_top, 180],
]

new needNewSideType = 1;
new currentMascotSprite = 0;
new generalDataPkt = 0;



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