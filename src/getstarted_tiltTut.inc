#define GS_BALL_DATA .posX, .posY, .angle, .pos, .module, .screen, .moduleT, .screenT, .collectables, .spd
#define GS_SELECTOR_DATA .accelX, .accelY, .pos, .module, .screen, .moduleT, .screenT

#include "getstarted_vars.inc"

const TILTTUT_EMPTY       = 0;
const TILTTUT_ICON        = 1;
const TILTTUT_TEXT_BOX    = 2;
const COLLECT_CIRCLE      = 3;
const ARROW_HINT          = 4;
const FINISHED_TOP        = 5;
const FINISHED_DOUBLE_TAP = 6;
const FINISHED_EXCELLENT  = 7;
const SELECTOR_TOP        = 8;
const SELECTOR_TEXT_BOX   = 9;

const SELECTOR_SENSITIVITY = -45;

new getstarted_tiltTutCollect{} = {
    COLLECT_CIRCLE, COLLECT_CIRCLE, COLLECT_CIRCLE, COLLECT_CIRCLE,
    TILTTUT_EMPTY, TILTTUT_TEXT_BOX, TILTTUT_EMPTY, TILTTUT_ICON,
    TILTTUT_EMPTY, TILTTUT_TEXT_BOX, ARROW_HINT, TILTTUT_ICON,
    ARROW_HINT, TILTTUT_ICON, TILTTUT_EMPTY, TILTTUT_TEXT_BOX,
    TILTTUT_ICON, TILTTUT_EMPTY, TILTTUT_TEXT_BOX, ARROW_HINT,
    TILTTUT_TEXT_BOX, ARROW_HINT, TILTTUT_ICON, TILTTUT_EMPTY,
};

new getstarted_tiltTutFinished{} = {
    FINISHED_TOP, FINISHED_TOP, FINISHED_TOP, FINISHED_TOP,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
    FINISHED_DOUBLE_TAP, TILTTUT_EMPTY, FINISHED_EXCELLENT, TILTTUT_EMPTY,
};

new getstarted_selectorTutorial{} = {
    SELECTOR_TOP, SELECTOR_TOP, SELECTOR_TOP, SELECTOR_TOP,
    TILTTUT_EMPTY, TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX,
    TILTTUT_EMPTY, TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX,
    TILTTUT_EMPTY, SELECTOR_TEXT_BOX, TILTTUT_EMPTY, TILTTUT_ICON,
    TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX, TILTTUT_EMPTY,
    TILTTUT_EMPTY, TILTTUT_ICON, TILTTUT_EMPTY, SELECTOR_TEXT_BOX,
};

const BALL_SQUARE_RADIUS = 289;
const BALL_ORBIT = 107;
new tiltTutBall[GS_BALL_DATA];

new tiltTutSelector[GS_SELECTOR_DATA];

new finishTiltTutorial = 0;
new selectorTutorial = 0;
new collectables{SCREENS_MAX} = {1, 1, 1};
new collectablesAngles[SCREENS_MAX] = [0, 3, 1];

new ballPkt = 0;
new selectorPkt = 0;

new selectorOutAnimation = 0;
new selectorInAnimation = 0;
new selectorYOffset = 0; 
new selectorXOffset = 0;

const PLUS_ONE_MAX_TICKS = 15;
new currentPlusOneTick = 0;
new collectedPlusOneEffectStep = 5;
new collectedPlusOneEffect = 0;
new backPlusOneOffset = 5;
new mainOneX = 75;
new mainOneY = 75;

new COLLECTED_MASCOT_ANIM_TIME = 500;
new collectedTokens = 0;
new collectedMascotEyesAnimFlag = 0;
new collectedMascotEyesAnimTimer = 0;
new collectableSprite = 0;

const BAKED_TILTTUT_WAIT_SPRITE = 0;
const BAKED_TILTTUT_MAIN_N_SPRITE = 1;
const BAKED_TILTTUT_MAIN_V_SPRITE = 2;
const BAKED_TILTTUT_SUCCESS_SPRITE = 3;
const BAKED_TILTTUT_WAIT_END_SPRITE = 4;
const BAKED_TILTTUT_MAIN_HEAD_SPRITE = 5;
const BAKED_TILTTUT_WAIT_HEAD_SPRITE = 6;
const BAKED_TILTTUT_SUCC_HEAD_SPRITE = 7;

TiltTutorialBakeSprites() {
    GFX_clearCache();
    GFX_bakeImage(BAKED_TILTTUT_WAIT_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
        GFX_drawImageXY(180,  44, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
        GFX_drawImageXY(180,  50, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
        GFX_drawImageXY(175,  20, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_WAIT_EYEBROWS_SPRITE);
        GFX_drawImageXY(210,  84, 0xFF, magenta, 100, 100, 150, MIRROR_Y,     MASCOT_WAIT_MOUNTH_SPRITE);
        GFX_drawImageXY( 75,  75, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, COLLECTED);
        GFX_drawImageXY(120, 166, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, DIALOGUE_SMALL);
        GFX_drawImageXY(120, 160, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, FOCUS_TEXT_SPRITE);
        GFX_drawTextXY(120, 185, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "collect");
        GFX_drawTextXY(120, 215, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "all shapes");
    GFX_render();

    GFX_bakeImage(BAKED_TILTTUT_MAIN_N_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
        GFX_drawImageXY(180,  44, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
        GFX_drawImageXY(180,  50, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
        GFX_drawImageXY(199,  84, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
        GFX_drawImageXY( 75,  75, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, COLLECTED);
        GFX_drawImageXY(120, 166, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, DIALOGUE_SMALL);
        GFX_drawTextXY(120, 162, SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "collect");
        GFX_drawTextXY(120, 202, SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "all shapes");
    GFX_render();

    GFX_bakeImage(BAKED_TILTTUT_MAIN_V_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
        GFX_drawImageXY(180,  44, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
        GFX_drawImageXY(180,  50, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_MAIN_EYES_CUTE_SPRITE);
        GFX_drawImageXY(199,  84, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
        GFX_drawImageXY( 75,  75, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, COLLECTED);
        GFX_drawImageXY(120, 166, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, DIALOGUE_SMALL);
        GFX_drawTextXY(120, 162, SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "collect");
        GFX_drawTextXY(120, 202, SIMPLE_FONT_SIZE + 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "all shapes");
    GFX_render();

    GFX_bakeImage(BAKED_TILTTUT_SUCCESS_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
        GFX_drawImageXY(180,  44, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_SUCCESS_BODY_SPRITE);
        GFX_drawImageXY(170,  10, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_SUCCESS_EYEBROWS_SPRITE);
        GFX_drawImageXY(180,  50, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_SUCCESS_EYES_SPRITE);
        GFX_drawImageXY(199,  84, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_SUCCESS_MOUNTH_SPRITE);
        GFX_drawImageXY(120, 166, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, DIALOGUE_SMALL);
        #ifdef SPRITE_TEXT
        GFX_drawImageXY(120, 160, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, EXCELLENT_TEXT_SPRITE);
        GFX_drawTextXY(120, 185, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "double slap");
        GFX_drawTextXY(120, 215, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to continue");
        #else
        GFX_drawTextXY(125, 150, 10, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF2DC81B, "excellent!");
        GFX_drawTextXY(120, 185, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "double slap");
        GFX_drawTextXY(120, 215, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to continue");
        #endif
    GFX_render();

    GFX_bakeImage(BAKED_TILTTUT_WAIT_END_SPRITE, 240, 240, GFX_format:FORMAT_RGB565);
        GFX_drawImageXY(180,  44, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
        GFX_drawImageXY(180,  50, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_WAIT_EYES_SPRITE);
        GFX_drawImageXY(175,  20, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_WAIT_EYEBROWS_SPRITE);
        GFX_drawImageXY(210,  84, 0xFF, magenta, 100, 100, 150, MIRROR_Y,     MASCOT_WAIT_MOUNTH_SPRITE);
        GFX_drawImageXY(120, 166, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, DIALOGUE_SMALL);
        #ifdef SPRITE_TEXT
        GFX_drawImageXY(120, 160, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, FOCUS_TEXT_SPRITE);
        GFX_drawTextXY(120, 185, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "double slap");
        GFX_drawTextXY(120, 215, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to continue");
        #else
        GFX_drawTextXY(125, 150, 10, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFF5252, "focus!");
        GFX_drawTextXY(120, 185, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_TOP_CENTER, 0xFF542682, "double slap");
        GFX_drawTextXY(120, 215, SIMPLE_FONT_SIZE - 1, 0, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFF542682, "to continue");
        #endif
    GFX_render();

    GFX_bakeImage(BAKED_TILTTUT_MAIN_HEAD_SPRITE, 130, 30, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(80, 100, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_TILTTUT_WAIT_HEAD_SPRITE, 130, 30, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(80, 100, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
    GFX_render();
    GFX_bakeImage(BAKED_TILTTUT_SUCC_HEAD_SPRITE, 130, 30, GFX_format:FORMAT_RGB565);
    GFX_drawImageXY(80, 100, 0xFF, magenta, 100, 100, 345, MIRROR_BLANK, MASCOT_SUCCESS_BODY_SPRITE);
    GFX_render();
}

MovePlusOneEffect(angle) {
    if ((angle == 0) || (angle == 360)) {
        mainOneY = 100 - (currentPlusOneTick * collectedPlusOneEffectStep);
        mainOneX = 50;
    } else if (angle == 90) {
        mainOneX = 50 + (currentPlusOneTick * collectedPlusOneEffectStep);
        mainOneY = 100;
    } else if (angle == 270) {
        mainOneX = 100 - (currentPlusOneTick * collectedPlusOneEffectStep);
        mainOneY = 100;
    }

    GFX_drawImageXY(mainOneX, mainOneY, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, PLUS_ONE_TEXT_SPRITE);
    
    ++currentPlusOneTick;

    if (currentPlusOneTick > PLUS_ONE_MAX_TICKS) {
        collectedPlusOneEffect = 0;
    }
}

RenderBall(screen) {
    if ((tiltTutBall.module == SELF_ID) && (tiltTutBall.screen == screen)) {
        new ballAlpha = 0x19;//0x19;//0x26
        new traceAngle = -(tiltTutBall.spd >> 2);
        new posX = BALL_ORBIT * FixedCos(tiltTutBall.angle + traceAngle) >> FP;
        new posY = BALL_ORBIT * FixedSin(tiltTutBall.angle + traceAngle) >> FP;
        GFX_drawImageXY(posX, posY, ballAlpha, magenta, 100, 100, 0, MIRROR_BLANK, BALL);
        posX = BALL_ORBIT * FixedCos(tiltTutBall.angle + (traceAngle / 3)) >> FP;
        posY = BALL_ORBIT * FixedSin(tiltTutBall.angle + (traceAngle / 3)) >> FP;
        GFX_drawImageXY(posX, posY, ballAlpha, magenta, 100, 100, 0, MIRROR_BLANK, BALL);
        posX = BALL_ORBIT * FixedCos(tiltTutBall.angle + ((traceAngle * 2) / 3)) >> FP;
        posY = BALL_ORBIT * FixedSin(tiltTutBall.angle + ((traceAngle * 2) / 3)) >> FP;
        GFX_drawImageXY(posX, posY, ballAlpha, magenta, 100, 100, 0, MIRROR_BLANK, BALL);

        GFX_drawImageXY(tiltTutBall.posX, tiltTutBall.posY, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, BALL);
    } else {
        new neighborLeft[TOPOLOGY_FACELET_INFO];
        neighborLeft = TOPOLOGY_getAdjacentFacelet(SetFacelet(tiltTutBall.module, tiltTutBall.screen), TOPOLOGY_neighbor:NEIGHBOR_LEFT);
        new neighborTop[TOPOLOGY_FACELET_INFO];
        neighborTop  = TOPOLOGY_getAdjacentFacelet(SetFacelet(tiltTutBall.module, tiltTutBall.screen), TOPOLOGY_neighbor:NEIGHBOR_TOP);
        tiltTutBall.posX = BALL_ORBIT * FixedCos(tiltTutBall.angle) >> FP;
        tiltTutBall.posY = BALL_ORBIT * FixedSin(tiltTutBall.angle) >> FP;
        // If for ball i am left neighbor and ball for me top neighbor
        if ((SELF_ID == neighborLeft.module) && (screen == neighborLeft.screen)) {
            if (tiltTutBall.posX < 0) {
                Swap(tiltTutBall.posX, tiltTutBall.posY);
                tiltTutBall.posY = -(MODULE_GAP + tiltTutBall.posY);
                GFX_drawImageXY(tiltTutBall.posX, tiltTutBall.posY, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, BALL);
            }
        // If for ball i am top neighbor and ball for me left neighbor
        } else if ((SELF_ID == neighborTop.module) && (screen == neighborTop.screen)) {
            if (tiltTutBall.posY < 0) {
                Swap(tiltTutBall.posX, tiltTutBall.posY);
                tiltTutBall.posX = -(MODULE_GAP + tiltTutBall.posX);
                GFX_drawImageXY(tiltTutBall.posX, tiltTutBall.posY, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, BALL);
            }
        }
    }
}

RenderStars(angle, pos) {
    switch(pos) {
        case 1: {
            GFX_drawImageXY( 22,  28, 0x7F, magenta, 100, 100, angle + 15, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(177,  44, 0xFF, magenta, 100, 100, angle - 15, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(233, 162, 0x33, magenta, 100, 100, angle + 75, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY( 95, 229, 0x33, magenta, 100, 100, angle - 15, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(133, 166, 0xFF, magenta, 100, 100, angle + 15, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY( 40, 168, 0xFF, magenta, 100, 100, angle + 7,  MIRROR_BLANK, BIG_STAR);
        }
        case 2: {
            GFX_drawImageXY(130, 165, 0xFF, magenta, 100, 100, angle + 8, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY( 28, 210, 0x7F, magenta, 100, 100, angle + 15, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY( 44,  54, 0xFF, magenta, 100, 100, angle - 15, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(162,   0, 0x33, magenta, 100, 100, angle - 75, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(229, 136, 0x33, magenta, 100, 100, angle - 15, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(166,  97, 0xFF, magenta, 100, 100, angle + 15, MIRROR_BLANK, BIG_STAR);
        }
        case 3: {
            GFX_drawImageXY( 38, 234, 0x7F, magenta, 100, 100, angle + 90, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY( 20,  73, 0xFF, magenta, 100, 100, angle + 60, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(105,  20, 0x33, magenta, 100, 100, angle + 150, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(206, 105, 0x33, magenta, 100, 100, angle + 60, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(142,  91, 0xFF, magenta, 100, 100, angle + 90, MIRROR_BLANK, BIG_STAR);
            GFX_drawImageXY(165, 181, 0xFF, magenta, 100, 100, angle + 82, MIRROR_BLANK, BIG_STAR);
        }
    }
}

CheckCollectables() {
    new alreadyCollected;
    for (new item = 0; item < SCREENS_MAX; ++item) {
        alreadyCollected |= collectables{item} << item;
    }
    return alreadyCollected;
}

SendBall(pktNumber) {
    new data[MAX_PACKET_SIZE / 4];

    data[0] = (tiltTutBall.module << 8) | (tiltTutBall.screen << 16) | (CheckCollectables() << 24);
    data[1] = tiltTutBall.angle;
    data[2] = tiltTutBall.pos;
    data[3] = pktNumber;
    data[4] = tiltTutBall.spd;

    broadcastPacket(PKT_BALL_TILT_TUT, data);
}

SendSelector(pktNumber) {
    new data[MAX_PACKET_SIZE / 4];

    data[0] = (tiltTutSelector.module << 8) | (tiltTutSelector.screen << 16);
    data[1] = tiltTutSelector.pos;
    data[2] = pktNumber;
    data[3] = (selectorOutAnimation == 1) ? (2) : (1);

    broadcastPacket(PKT_SELECTOR_TILT_TUT, data);
}

FindPosDependOnAngle(angle, &posX_1, &posY_1, &posX_2, &posY_2) {
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
}

FindUpSecondPosBall() {
    for (new screenI = 0; screenI < MODULES_MAX * SCREENS_MAX; ++screenI) {
        if ((getStarted_AllScreensData[screenI].face == ORIENTATION_UP)
         && (getStarted_AllScreensData[screenI].pos == 2)) {
            tiltTutBall.module = screenI / SCREENS_MAX;
            tiltTutBall.screen = screenI % SCREENS_MAX;
            tiltTutBall.angle = 45;
            tiltTutBall.pos = 2;
        }
    }
}

FindUpSecondPosSelector() {
    for (new screenI = 0; screenI < MODULES_MAX * SCREENS_MAX; ++screenI) {
        if ((getStarted_AllScreensData[screenI].face == ORIENTATION_UP)
         && (getStarted_AllScreensData[screenI].pos == 3)) {
            tiltTutSelector.module = screenI / SCREENS_MAX;
            tiltTutSelector.screen = screenI % SCREENS_MAX;
            tiltTutSelector.pos = 3;
        }
    }
}

SelectorLogic() {
    if (tiltTutSelector.module >= MODULES_MAX) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            if (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + screenI].face == ORIENTATION_UP) {
                if (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + screenI].pos == tiltTutSelector.pos) {
                    tiltTutSelector.module = SELF_ID;
                    tiltTutSelector.screen = screenI;
                    break;
                }
            }
        }
    }

    if (tiltTutSelector.module == SELF_ID) {
        if (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutSelector.screen].face == TOPOLOGY_orientation:ORIENTATION_UP) {
            if (tiltTutSelector.pos == getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutSelector.screen].pos) {
                tiltTutSelector.moduleT = SELF_ID;
                tiltTutSelector.screenT = tiltTutSelector.screen;

                new accelX = -MS_getFaceAccelX(tiltTutSelector.screen);
                new accelY = -MS_getFaceAccelY(tiltTutSelector.screen);

                tiltTutSelector.accelX = tiltTutSelector.accelY = 0;

                accelX *= (accelX <= 0);
                accelY *= (accelY <= 0);
                if (ABS(accelX) > ABS(accelY)) {
                    tiltTutSelector.accelX = accelX >> 1;
                } else {
                    tiltTutSelector.accelY = accelY >> 1;
                }
                
                if (accelX < SELECTOR_SENSITIVITY) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, tiltTutSelector.screen), TOPOLOGY_neighbor:NEIGHBOR_LEFT);
                    if (neighbor.connected) {
                        tiltTutSelector.pos = (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutSelector.screen].pos + 1) % 4;
                        tiltTutSelector.module = neighbor.module;
                        tiltTutSelector.screen = neighbor.screen;
                        selectorOutAnimation = 1;
                    }
                } else if (accelY < SELECTOR_SENSITIVITY) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, tiltTutSelector.screen), TOPOLOGY_neighbor:NEIGHBOR_TOP);
                    if (neighbor.connected) {
                        tiltTutSelector.pos = (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutSelector.screen].pos - 1) % 4;
                        tiltTutSelector.module = neighbor.module;
                        tiltTutSelector.screen = neighbor.screen;
                        selectorOutAnimation = 2;
                    }
                }
            } else {
                FindUpSecondPosSelector();
            }
        } else {
            FindUpSecondPosSelector();
        }
        selectorPkt = ++selectorPkt % 0xFFFF;
        SendSelector(selectorPkt);
    } else if (tiltTutSelector.moduleT == SELF_ID) {
        SendSelector(selectorPkt);
    }
}

BallLogic() {
    //LOG_i("ball logic %d %d",tiltTutBall.module, tiltTutBall.pos);
    if ((tiltTutBall.module >= MODULES_MAX)) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            //LOG_i("screen face %d %d",screenI, getStarted_AllScreensData[SELF_ID * SCREENS_MAX + screenI].face);
            if (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + screenI].face == TOPOLOGY_orientation:ORIENTATION_UP) {
                if (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + screenI].pos == tiltTutBall.pos) {
                    tiltTutBall.module = SELF_ID;
                    tiltTutBall.screen = screenI;
                    tiltTutBall.angle = 45;
                    //LOG_i("ball is here");
                    break;
                }
            }
        }
    }

    if (tiltTutBall.module >= MODULES_MAX) {
        return;
    }

    new allCollected = 0;
    for (new item = 0; item < SCREENS_MAX; ++item) {
        allCollected += (collectables{item} == 0);
    }
    if (collectedTokens < allCollected) {
        collectedMascotEyesAnimFlag = 1;
        collectedMascotEyesAnimTimer = 0;
        collectedTokens = allCollected;
    }
    if ((allCollected == SCREENS_MAX) && (finishTiltTutorial == 0)) {
        finishTiltTutorial = 1;
    }

    if (tiltTutBall.module == SELF_ID) {
        if (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutBall.screen].face == TOPOLOGY_orientation:ORIENTATION_UP) {
            if (tiltTutBall.pos == getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutBall.screen].pos) {
                tiltTutBall.moduleT = SELF_ID;
                tiltTutBall.screenT = tiltTutBall.screen;

                new accelX = MS_getFaceAccelX(tiltTutBall.screen) >> 2;
                new accelY = MS_getFaceAccelY(tiltTutBall.screen) >> 2;
                //LOG_i("%d",accelX - accelY);
                if (tiltTutBall.spd >= 0) {
                    tiltTutBall.spd -= 6;
                } else if (tiltTutBall.spd < 0) {
                    tiltTutBall.spd += 6;
                }
                new tempSpd = tiltTutBall.spd += accelX - accelY;
                if ((tiltTutBall.spd > -16) && (tiltTutBall.spd < 0)) {
                    tempSpd *= -1;
                }
                //LOG_i("%d %d", tiltTutBall.spd, tempSpd);
                tiltTutBall.angle += (tempSpd >> 4);
                //tiltTutBall.angle += accelX - accelY;

                if (tiltTutBall.angle < 0) {
                    tiltTutBall.angle = 360 + tiltTutBall.angle;
                }
                tiltTutBall.angle %= 360;
                                    
                tiltTutBall.posX = BALL_ORBIT * FixedCos(tiltTutBall.angle) >> FP;
                tiltTutBall.posY = BALL_ORBIT * FixedSin(tiltTutBall.angle) >> FP;
                //LOG_i("got the ball %d %d %d %d",tiltTutBall.screen, tiltTutBall.angle, tiltTutBall.posX, tiltTutBall.posY);

                for (new item = 0; item < SCREENS_MAX; ++item) {
                    if (collectables{item} && (collectablesAngles[item] == tiltTutBall.pos)) {
                        new distance = CheapDistance(75 - tiltTutBall.posX, 75 - tiltTutBall.posY);
                        if (distance < BALL_SQUARE_RADIUS) {
                            collectables{item} = 0;
                            collectedPlusOneEffect = 1;
                            SND_play(tiltTutCollectableSounds{item}, SOUND_VOLUME);
                        }
                    }
                }

                if (tiltTutBall.posX < -HALF_MODULE_GAP) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, tiltTutBall.screen), TOPOLOGY_neighbor:NEIGHBOR_LEFT);
                    if (neighbor.connected) {
                        //LOG_i("left %d %d",tiltTutBall.pos, getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutBall.screen].pos);
                        tiltTutBall.pos = (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutBall.screen].pos + 1) % 4;
                        tiltTutBall.module = neighbor.module;
                        tiltTutBall.screen = neighbor.screen;
                        tiltTutBall.angle = 352;
                        //LOG_i("left end %d %d",tiltTutBall.pos, getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutBall.screen].pos);
                    }
                } else if (tiltTutBall.posY < -HALF_MODULE_GAP) {
                    neighbor = TOPOLOGY_getAdjacentFacelet(SetFacelet(SELF_ID, tiltTutBall.screen), TOPOLOGY_neighbor:NEIGHBOR_TOP);
                    if (neighbor.connected) {
                        //LOG_i("top %d",tiltTutBall.pos);
                        tiltTutBall.pos = (getStarted_AllScreensData[SELF_ID * SCREENS_MAX + tiltTutBall.screen].pos - 1) % 4;
                        tiltTutBall.module = neighbor.module;
                        tiltTutBall.screen = neighbor.screen;
                        tiltTutBall.angle = 98;
                    }
                }
                //LOG_i("send ball to %d %d %d",tiltTutBall.module, tiltTutBall.screen, tiltTutBall.pos);
            } else {
                FindUpSecondPosBall();
            }
        } else {
            FindUpSecondPosBall();
        }
        ballPkt = ++ballPkt % 0xFFFF;
        SendBall(ballPkt);
    } else if (tiltTutBall.moduleT == SELF_ID) {
        SendBall(ballPkt);
    }
}

DrawTiltTutorial(screen) {
    new screenI = SELF_ID * SCREENS_MAX + screen;
    new angle = (4 - getStarted_AllScreensData[screenI].pos) * 90;
    new layoutElement = getStarted_AllScreensData[screenI].face * TOPOLOGY_POSITIONS_MAX + getStarted_AllScreensData[screenI].pos;

    new arcColor = 0xFFFFC28A;
    collectableSprite = COLLECTABLE;

    new screenLayout = getstarted_tiltTutCollect{layoutElement};
    if (finishTiltTutorial) {
        screenLayout = getstarted_tiltTutFinished{layoutElement};
        arcColor = 0xFFBEFFB7;
    } else if (selectorTutorial) {
        screenLayout = getstarted_selectorTutorial{layoutElement};
    }
    
    if (mascotWaitModeFlag) {
        arcColor = 0xFFFF8585;
        collectableSprite = COLLECTABLE_WAIT;
    }

    GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], BACKGROUND_COLOR);

    switch (screenLayout) {
        case TILTTUT_ICON: {
            if (tiltTutSelector.pos != 1) {
                GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TILT_ICON);
            } else {
                GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TAP_ICON_GREEN_SPRITE);
            }
        }
        case TILTTUT_TEXT_BOX: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            FindPosDependOnAngle(angle, posX_1, posY_1, posX_2, posY_2);
            
            GFX_drawTextXY(posX_1, posY_1, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "tilt the cube");
            GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "and collect");
            GFX_drawTextXY(posX_2, posY_2, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "all shapes");
        }
        case ARROW_HINT: {
            GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, ARROW_TILT);
        }
        case COLLECT_CIRCLE: {
            GFX_drawArc([-15, -15],  92, 4, 0, 90, arcColor);
            GFX_drawArc([-15, -15], 160, 4, 0, 90, arcColor);

            new curMascotSprite = BAKED_TILTTUT_MAIN_N_SPRITE;
            if (mascotWaitModeFlag) {
                curMascotSprite = BAKED_TILTTUT_WAIT_SPRITE;
            } else if (collectedMascotEyesAnimFlag) {
                curMascotSprite = BAKED_TILTTUT_MAIN_V_SPRITE;
            }
            if (getStarted_AllScreensData[screenI].pos == 0) {
                GFX_drawBakedImage([120, 120], 0xFF, 0, 100, 100, 0, MIRROR_BLANK, curMascotSprite);
                if (collectables{0}) {
                    GFX_drawImageXY(75, 75, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, collectableSprite);
                }
            } else if (getStarted_AllScreensData[screenI].pos == 3) {
                if (curMascotSprite == BAKED_TILTTUT_WAIT_SPRITE) {
                    GFX_drawBakedImage([15, 175], 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_TILTTUT_WAIT_HEAD_SPRITE);
                } else {
                    GFX_drawBakedImage([15, 175], 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_TILTTUT_MAIN_HEAD_SPRITE);
                }
                GFX_drawImageXY(75, 75, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, (collectables{1}) ? (collectableSprite) : (COLLECTED));
            } else if (getStarted_AllScreensData[screenI].pos == 2) {
                GFX_drawImageXY(176, 208, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, COUNT_BAR);
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    if (!collectables{item}) {
                        GFX_drawImageXY(200 - (24 * item), 208, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, COLLECTED_CHECK);
                    }
                }
            } else if (getStarted_AllScreensData[screenI].pos == 1) {
                GFX_drawTextXY(210, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "tilt the cube");
                GFX_drawImageXY(75, 75, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, (collectables{2}) ? (collectableSprite) : (COLLECTED));
            }

            if (collectedPlusOneEffect) {
                MovePlusOneEffect(angle);
            }

            RenderBall(screen);
        }
        case FINISHED_TOP: {
            GFX_drawArc([-15, -15],  92, 4, 0, 90, arcColor);
            GFX_drawArc([-15, -15], 160, 4, 0, 90, arcColor);

            new curMascotSprite = BAKED_TILTTUT_SUCCESS_SPRITE;
            if (mascotWaitModeFlag) {
                curMascotSprite = BAKED_TILTTUT_WAIT_END_SPRITE;
            } else {
                RenderStars(angle, getStarted_AllScreensData[screenI].pos);
            }

            if (getStarted_AllScreensData[screenI].pos != 2) {
                GFX_drawImageXY(75, 75, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, COLLECTED);
            }

            if (getStarted_AllScreensData[screenI].pos == 0) {
                GFX_drawBakedImage([120, 120], 0xFF, 0, 100, 100, 0, MIRROR_BLANK, curMascotSprite);
            } else if (getStarted_AllScreensData[screenI].pos == 3) {
                if (curMascotSprite == BAKED_TILTTUT_WAIT_END_SPRITE) {
                    GFX_drawBakedImage([20, 175], 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_TILTTUT_WAIT_HEAD_SPRITE);
                } else {
                    GFX_drawBakedImage([20, 175], 0xFF, 0, 100, 100, angle, MIRROR_BLANK, BAKED_TILTTUT_SUCC_HEAD_SPRITE);
                }
            } else if (getStarted_AllScreensData[screenI].pos == 2) {
                GFX_drawImageXY(176, 208, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, COUNT_BAR);
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    GFX_drawImageXY(200 - (24 * item), 208, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, 
                                   (!mascotWaitModeFlag) ? (COLLECTED_CHECK_GREEN) : (COLLECTED_CHECK_RED));
                }
            }

            if (collectedPlusOneEffect) {
                MovePlusOneEffect(angle);
            }

            RenderBall(screen);
        }
        case FINISHED_DOUBLE_TAP: {
            new posX_1 = 120;
            new posY_1 = 100;
            new posX_2 = 120;
            new posY_2 = 140;
            FindPosDependOnAngle(angle, posX_1, posY_1, posX_2, posY_2);
            GFX_drawTextXY(posX_1, posY_1, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_TOP_CENTER, 0xFFFFFFFF, "double slap");
            GFX_drawTextXY(posX_2, posY_2, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_BOTTOM_CENTER, 0xFFFFFFFF, "to continue");
        }
        case FINISHED_EXCELLENT: {
            GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE + 1, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "excellent!");
        }
        case SELECTOR_TOP: {
            if ((getStarted_AllScreensData[screenI].pos == 0) && (tiltTutSelector.module != SELF_ID)) {
                if (tiltTutSelector.pos != 1) {
                    if (!mascotWaitModeFlag) {
                        GFX_drawRectangle([0, 0, SCREEN_WIDTH, SCREEN_HEIGHT], 0xFFFFFFFF);
                        InitCrossAnimationVars(0xFFFF5C00, 0xFFFFB576);
                        GFX_drawTextXY(120,  80, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "put the white");
                        GFX_drawTextXY(120, 105, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "hightlight on me");
                        GFX_drawTextXY(120, 130, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "and slap the");
                        GFX_drawTextXY(120, 155, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFF542682, "cube 2 times");
                    } else {
                        InitCrossAnimationVars(0xFFFF9898, 0xFFFF5252);
                        GFX_drawTextXY(120,  60, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "put the white");
                        GFX_drawTextXY(120,  90, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "hightlight");
                        GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "and slap");
                        GFX_drawTextXY(120, 150, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube");
                        GFX_drawTextXY(120, 180, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "2 times");
                    }
                    DrawCrossAnimation();
                }
            } else if ((getStarted_AllScreensData[screenI].pos == 2) && (tiltTutSelector.module != SELF_ID)) {
                if (!mascotWaitModeFlag) {
                    if (tiltTutSelector.pos == 1) {
                        InitCrossAnimationVars(0xFF2DC81B, 0xFFAEE4A8);
                        #ifdef SPRITE_TEXT
                        GFX_drawImageXY(120, 136, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, DOUBLE_SLAP_TEXT_GREEN_SPRITE);
                        GFX_drawTextXY(120, 89, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube now!");
                        #else
                        GFX_drawTextXY(120, 130, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "slap the cube");
                        GFX_drawTextXY(120, 110, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "2 times now");
                        #endif
                    } else {
                        InitCrossAnimationVars(0xFFFF5C00, 0xFFFFB576);
                        GFX_drawTextXY(120, 155, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "tilt the cube");
                        GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "for navigation");
                        GFX_drawTextXY(120,  80, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "or selection");
                    }
                } else {
                    InitCrossAnimationVars(0xFFFF9898, 0xFFFF5252);
                    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, ANOTHER_TRY_TEXT_SPRITE);
                }
                DrawCrossAnimation();
            } else if (getStarted_AllScreensData[screenI].pos == 1) {
                if (!mascotWaitModeFlag) {
                    if (tiltTutSelector.module != SELF_ID) {
                        GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EMPTY_SPRITE);
                        if (tiltTutSelector.pos == 2) {
                            GFX_drawImageXY(130, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_X_SPRITE);
                        } else if (tiltTutSelector.pos == 0) {
                            GFX_drawImageXY(130, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_CUTE_SPRITE);
                        } else {
                            GFX_drawImageXY(136, 115, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_EYES_NORMAL_SPRITE);
                        }
                        GFX_drawImageXY(170, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_MAIN_MOUNTH_O_SPRITE);
                        if ((tiltTutSelector.pos == 2) || (tiltTutSelector.pos == 3)) {
                            GFX_drawImageXY(120, 31, 0xFF, magenta, 100, 100, 180, MIRROR_BLANK, DIALOGUE);
                        }
                    } else {
                        GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_SUCCESS_BODY_SPRITE);
                        GFX_drawImageXY( 84, 114, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_SUCCESS_EYEBROWS_SPRITE);
                        GFX_drawImageXY(128, 114, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_SUCCESS_EYES_SPRITE);
                        GFX_drawImageXY(162, 114, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_SUCCESS_MOUNTH_SPRITE);
                    }
                } else {
                    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_WAIT_BODY_SPRITE);
                    GFX_drawImageXY(132, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, MASCOT_WAIT_EYE_2_SPRITE);
                    GFX_drawImageXY(163, 110, 0xFF, magenta, 100, 100, 75, MIRROR_BLANK, MASCOT_WAIT_MOUNTH_SPRITE);
                }
            } else if ((getStarted_AllScreensData[screenI].pos == 3) && (tiltTutSelector.module != SELF_ID)) {
                if (mascotWaitModeFlag) {
                    InitCrossAnimationVars(0xFFFF9898, 0xFFFF5252);
                    DrawCrossAnimation();
                    if (tiltTutSelector.pos == 2) {
                        GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, ANOTHER_TRY_TEXT_SPRITE);
                    } else if (tiltTutSelector.pos == 0) {
                        GFX_drawTextXY(180, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "put the white");
                        GFX_drawTextXY(150, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "hightlight");
                        GFX_drawTextXY(120, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "and slap");
                        GFX_drawTextXY( 90, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the cube");
                        GFX_drawTextXY( 60, 120, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "2 times");
                    }
                } else if (tiltTutSelector.pos == 1) {
                    GFX_drawImageXY(120, 120, 0xFF, magenta, 100, 100, angle, MIRROR_BLANK, TAP_ICON_GREEN_SPRITE);
                }
            }

            if (selectorOutAnimation) {
                if (selectorOutAnimation == 1) {
                    selectorXOffset += 30;
                    selectorYOffset = -35;
                } else {
                    selectorYOffset += 30;
                    selectorXOffset = -35;
                }
                GFX_drawImageXY(85 - selectorXOffset, 85 - selectorYOffset, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, SELECTOR);
                if ((-selectorXOffset < -180) || (-selectorYOffset < -180)) {
                    selectorInAnimation = selectorOutAnimation = selectorYOffset = selectorXOffset = 0;
                }
            } else if (selectorInAnimation) {
                if (selectorInAnimation == 1) {
                    selectorXOffset += 30;
                    selectorYOffset = 210;
                } else {
                    selectorYOffset += 30;
                    selectorXOffset = 210;
                }
                GFX_drawImageXY(-90 + selectorXOffset, -90 + selectorYOffset, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, SELECTOR);
                if ((selectorXOffset > 210) || (selectorYOffset > 210)) {
                    selectorOutAnimation = selectorInAnimation = selectorYOffset = selectorXOffset = 0;
                    SND_play(SELECTOR_MENU_SOUND, SOUND_VOLUME);
                }
            } else if ((tiltTutSelector.module == SELF_ID) && (tiltTutSelector.screen == screen)) {
                GFX_drawImageXY(120 + tiltTutSelector.accelX, 120 + tiltTutSelector.accelY, 0xFF, magenta, 100, 100, 0, MIRROR_BLANK, SELECTOR);
            }
        }
        case SELECTOR_TEXT_BOX: {
            new staticCenterPos = 120;
            new changingPos = 80;
            new posStep = 20;
            new isStaticX = 1;
            new isStaticY = 0;
            if (getStarted_AllScreensData[screenI].pos == 3) {
                changingPos = 140;
                posStep = -posStep;
                isStaticX = 0;
                isStaticY = 1;
            } else if (getStarted_AllScreensData[screenI].pos == 2) {
                changingPos = 140;
                posStep = -posStep;
            } else if (getStarted_AllScreensData[screenI].pos == 1) {
                isStaticX = 0;
                isStaticY = 1;
            }
            GFX_drawTextXY(staticCenterPos * isStaticX + changingPos * isStaticY, staticCenterPos * isStaticY + changingPos * isStaticX, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "put the");
            GFX_drawTextXY(staticCenterPos * isStaticX + (changingPos + posStep * 1) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 1) * isStaticX, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "highlight on");
            GFX_drawTextXY(staticCenterPos * isStaticX + (changingPos + posStep * 2) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 2) * isStaticX, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "the little");
            GFX_drawTextXY(staticCenterPos * isStaticX + (changingPos + posStep * 3) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 3) * isStaticX, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "cube and");
            GFX_drawTextXY(staticCenterPos * isStaticX + (changingPos + posStep * 4) * isStaticY, staticCenterPos * isStaticY + (changingPos + posStep * 4) * isStaticX, SIMPLE_FONT_SIZE, angle, 0, TEXT_ALIGN_CENTER, 0xFFFFFFFF, "double slap");
        }
    }

}

UpdateTiltTutorial(m_deltaTime) {
    if (selectorTutorial) {
        SelectorLogic();
    } else {
        BallLogic();
        if (collectedMascotEyesAnimFlag) {
            if (collectedMascotEyesAnimTimer >= COLLECTED_MASCOT_ANIM_TIME) {
                collectedMascotEyesAnimFlag = 0;
            } else {
                collectedMascotEyesAnimTimer += m_deltaTime;
            }
        }
    }
}