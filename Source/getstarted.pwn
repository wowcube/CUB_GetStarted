forward run(const pkt[], size, const src[]); // public Pawn function seen from C

#include "getstarted_start.inc"
#include "getstarted_twistTut.inc"
#include "getstarted_tapTut.inc"
#include "getstarted_tiltTut.inc"
#include "getstarted_shakeTut.inc"
#include "getstarted_success.inc"

SaveData() {
    new saveData [2] = [0,...];
    saveData[0] = alreadyLaunched + 1;
    abi_CMD_SAVE_STATE(saveData);
    dataSaved = 1;
}

SetApplicationState(newState) {
    if (applicationState != newState) {
        if (newState == FSM:start) {
            tutorialStartTimer = TUTORIAL_COUNTDOWN_TIME;
            currentTangibleIcon = tutorialStartTimer / DISPLAY_ONE_ICON_TIME;
            tangibleIcons[0] = SHAKE_ICON;
            tangibleIcons[1] = TILT_ICON;
            tangibleIcons[2] = TAP_ICON;
            tangibleIcons[3] = TWIST_ICON;
        } else if (newState == FSM:twistTutorial) {
            arrowIcons[0] = 0x19;
            arrowIcons[1] = 0x4C;
            arrowIcons[2] = 0xFF;
        } else if (newState == FSM:tapTutorial) {
            tapTutorialStage = 0;
            beginTapTutorial = 0;
        } else if (newState == FSM:tiltTutorial) {
            tiltTutBall.angle = 45;
            tiltTutBall.screenAngle = 180;
            tiltTutBall.moduleT = tiltTutBall.module = CUBES_MAX;
            tiltTutBall.screenT = tiltTutBall.screen = FACES_MAX;

            tiltTutSelector.screenAngle = 90;
            tiltTutSelector.moduleT = tiltTutSelector.module = CUBES_MAX;
            tiltTutSelector.screenT = tiltTutSelector.screen = FACES_MAX;
        } else if (newState == FSM:successScreen) {
            currentMascotSprite = MASCOT_SUCCESS_SPRITE;
            
            successAnimationEffectTimer = 0;
            successAnimationEffectFlag = 0;
            sucAnimEffectTimePercent = 0;

            smallStarX = 120;
            smallStarY = 120;
            bigStarX = 120;
            bigStarY = 120;
        }
        previousAppState = applicationState;
        applicationState = newState;
        SetDefaultMascot();
    }
}

SetScreenData(screen) {
    getStarted_screenData[screen].angle = TopologyGetAngle(abi_cubeN, screen, topology_projection:projection_splash);
            
    new neighScreen = abi_rightFaceN(abi_cubeN, screen);
    getStarted_screenData[neighScreen].sideType = TopologyGetPlaneProject(abi_cubeN, neighScreen);
    getStarted_screenData[neighScreen].angle = TopologyGetAngle(abi_cubeN, neighScreen, topology_projection:projection_splash);
    
    neighScreen = abi_bottomFaceN(abi_cubeN, screen);
    getStarted_screenData[neighScreen].sideType = TopologyGetPlaneProject(abi_cubeN, neighScreen);
    getStarted_screenData[neighScreen].angle = TopologyGetAngle(abi_cubeN, neighScreen, topology_projection:projection_splash);
}

GetNewSideType() {
    for (new side = 0; side < PLANES_MAX; ++side) {
        new mySideType = TopologyGetPlaneProject(topologyByPlane[side][0].module, topologyByPlane[side][0].screen);
        for (new screen = 1; screen < FACES_ON_PLANE_MAX; ++screen) {
            new neighbourSideType = TopologyGetPlaneProject(topologyByPlane[side][screen].module, topologyByPlane[side][screen].screen);
            if ((mySideType != neighbourSideType)
            || (mySideType == topology_location:location_max)
            || (neighbourSideType == topology_location:location_max)) {
                return;
            }
        }
    }
    
    for (new screen = 0; screen < FACES_MAX ; ++screen) {
        if (TopologyGetPlaneProject(abi_cubeN, screen) == topology_location:location_top) {
            getStarted_screenData[screen].sideType = topology_location:location_top;
            SetScreenData(screen);
            break;
        } else if (TopologyGetPlaneProject(abi_cubeN, screen) == topology_location:location_bottom) {
            getStarted_screenData[screen].sideType = topology_location:location_bottom;
            SetScreenData(screen);
            break;
        }
    }
    
    needNewSideType = 0;
}

SendGeneralInfo(pktNumber) {
    new data[4];

    new flags = beginTapTutorial
    | (finishTiltTutorial << 3) 
    | (selectorTutorial << 4) 
    | (beginShakeTutorial << 5)
    | (getstartedGreetingFlag << 6);

    new randomSoundOrder = 0;
    for (new soundI = 0; soundI < FACES_MAX; ++soundI) {
        randomSoundOrder |= (tiltTutCollectableSounds{soundI} << (soundI * 2));
    }
    data[0] = PKT_GENERAL_DATA | (flags << 8) | (previousAppState << 16) | (applicationState << 24);
    data[1] = tapTutorialStage | (shakeTutorialStage << 8) | (twistTutorialStage << 16) | (randomSoundOrder << 24);
    data[2] = tutorialStartTimer;
    data[3] = pktNumber;

    abi_CMD_NET_TX(0, NET_BROADCAST_TTL_MAX, data);
    abi_CMD_NET_TX(1, NET_BROADCAST_TTL_MAX, data);
    abi_CMD_NET_TX(2, NET_BROADCAST_TTL_MAX, data);
}

ON_PHYSICS_TICK() {
}

RENDER() {
    for (new screenI = 0; screenI < FACES_MAX; ++screenI) {
        abi_CMD_G2D_BEGIN_DISPLAY(screenI, true);
        
        switch (applicationState) {
            case start: {
                DrawStartCountdown(screenI);
            }
            case twistTutorial: {
                DrawTwistTutorial(screenI);
            }
            case tapTutorial: {
                DrawTapTutorial(screenI);
            }
            case tiltTutorial: {
                DrawTiltTutorial(screenI);
            }
            case shakeTutorial: {
                DrawShakeTutorial(screenI);
            }
            case successScreen: {
                DrawSuccessScreen(screenI);
            }
        }
        abi_CMD_G2D_END();
    }
}

ONTICK() {
    currentTime = abi_GetTime();
    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    if ((alreadyLaunched) && (abi_cubeN == 0)) {
        abi_checkShake();
    }

    if (needNewSideType) {
        GetNewSideType();
    }

    UpdateCrossAnimation(deltaTime);
    UpdateMascotAnimation(deltaTime);

    switch(applicationState) {
        case start: {
            UpdateStartCountdown(deltaTime);
        }
        case twistTutorial: {
            UpdateTwistTutorial(deltaTime);
        }
        case tapTutorial: {
            UpdateTapTutorial(deltaTime);
        }
        case tiltTutorial: {
            UpdateTiltTutorial(deltaTime);
        }
        case shakeTutorial: {
            UpdateShakeTutorial(deltaTime);
        }
        case successScreen: {
            UpdateSuccessScreen(deltaTime);
        }
    }

    if (abi_cubeN == 0) {
        generalDataPkt = ++generalDataPkt % 0x7FFFFFFF;
        SendGeneralInfo(generalDataPkt);
    }
}

ON_INIT() {
    abi_CMD_LOAD_STATE();

    previousTime = abi_GetTime();

    ARROW_TWIST = getSpriteIdByName("arrow_twist.png");
    
    ARROW_TILT    = getSpriteIdByName("arrow_tilt.png");
    BALL          = getSpriteIdByName("ball.png");
    COLLECTABLE   = getSpriteIdByName("collectable.png");
    COMPLETE_ICON = getSpriteIdByName("complete.png");
    COUNT_BAR     = getSpriteIdByName("count_bar.png");

    DIALOGUE              = getSpriteIdByName("dialogue.png");
    DIALOGUE_SMALL        = getSpriteIdByName("speechSmall.png");

    COLLECTED_CHECK       = getSpriteIdByName("checked.png");
    COLLECTED_CHECK_RED   = getSpriteIdByName("checked_red.png");

    MASCOT_MAIN_SPRITE    = getSpriteIdByName("mas_main.png");
    MASCOT_SUCCESS_SPRITE = getSpriteIdByName("mas_success.png");
    MASCOT_WAIT_SPRITE    = getSpriteIdByName("mas_wait.png");
    CIRCLE_QUARTER        = getSpriteIdByName("quarter.png");
    SELECTOR              = getSpriteIdByName("selector.png");
    SHAKE_ICON            = getSpriteIdByName("shake_icon.png");
    TAP_ICON              = getSpriteIdByName("tap_icon.png");
    TILT_ICON             = getSpriteIdByName("tilt_icon.png");
    TWIST_ICON            = getSpriteIdByName("twist_icon.png");

    SMALL_STAR            = getSpriteIdByName("star_small.png");
    BIG_STAR              = getSpriteIdByName("star_big.png");

    CIRCLE_QUARTER_PUSH         = getSpriteIdByName("quarterPush.png");

    // Sounds
    ACTION_SOUND      = getSoundIdByName("action.wav");
    GOOD_SOUND        = getSoundIdByName("good.wav");
    EXCELLENT_1_SOUND = getSoundIdByName("excellent_1.wav");
    EXCELLENT_2_SOUND = getSoundIdByName("excellent_2.wav");
    
    SELECTOR_MENU_SOUND = getSoundIdByName("selec_menu.wav");
    
    TAPS_1_2_SOUND = getSoundIdByName("taps_1-2.wav");
    TAPS_3_4_SOUND = getSoundIdByName("taps_3-4.wav");
    TAPS_5_6_SOUND = getSoundIdByName("taps_5-6.wav");
    TAP_STAGE_SUCCESS_SOUND = getSoundIdByName("tapsSuccess.wav");

    PLUS_1_SHAPE_COLLECT_1_SOUND = getSoundIdByName("collect_1.wav");
    PLUS_1_SHAPE_COLLECT_2_SOUND = getSoundIdByName("collect_2.wav");
    PLUS_1_SHAPE_COLLECT_3_SOUND = getSoundIdByName("collect_3.wav");

    tiltTutCollectableSounds{0} = PLUS_1_SHAPE_COLLECT_1_SOUND;
    tiltTutCollectableSounds{1} = PLUS_1_SHAPE_COLLECT_2_SOUND;
    tiltTutCollectableSounds{2} = PLUS_1_SHAPE_COLLECT_3_SOUND;

    if (abi_cubeN == 0) {
        for (new soundI = 0; soundI < FACES_MAX; ++soundI) {
            new j = Random (0, FACES_MAX - 1);
            new temp = tiltTutCollectableSounds{j};
            tiltTutCollectableSounds{j} = tiltTutCollectableSounds{soundI};
            tiltTutCollectableSounds{soundI} = temp;
        }
    }

    SetApplicationState(FSM:start);
}

ON_CHECK_ROTATE() {
    needNewSideType = 1;
    
    if (applicationState == FSM:twistTutorial) {
        SetDefaultMascot();
    }
    
    if (abi_cubeN == 0) {
        if (applicationState == FSM:twistTutorial) {
            ++twistTutorialStage;
            if (twistTutorialStage == 1) {
                abi_CMD_PLAYSND(GOOD_SOUND, SOUND_VOLUME);
            }
            if (twistTutorialStage >= MAX_TWIST_TUTORIAL_STAGES) {
                SetApplicationState(FSM:successScreen);
                abi_CMD_PLAYSND(EXCELLENT_1_SOUND, SOUND_VOLUME);
            }
        } else if ((applicationState == FSM:successScreen) && (previousAppState == FSM:twistTutorial)) {
            SetApplicationState(FSM:tapTutorial);
        }
    }

    if (applicationState == FSM:tiltTutorial) {
        tiltTutBall.module = CUBES_MAX;
        tiltTutSelector.module = CUBES_MAX;
    }
}

ON_LOAD_GAME_DATA(const pkt[]) {
    if (abi_ByteN(pkt, 1) == 0) {
        return;
    }
    alreadyLaunched = pkt[1];
}

ON_CMD_NET_RX (const pkt[]) {
    switch (abi_ByteN(pkt, 4)) {
        case PKT_GENERAL_DATA: {
            new packetNumberReceived = pkt[4];
            if ((generalDataPkt < packetNumberReceived) || ((generalDataPkt - packetNumberReceived) > (0x7FFFFFFF >> 1))) {
                generalDataPkt = packetNumberReceived;
                SetApplicationState(abi_ByteN(pkt, 7));
                new flags = abi_ByteN(pkt, 5);
                beginTapTutorial = flags & 0x1;
                finishTiltTutorial = (flags >> 3) & 0x1;
                selectorTutorial = (flags >> 4) & 0x1;
                beginShakeTutorial = (flags >> 5) & 0x1;
                getstartedGreetingFlag = (flags >> 6) & 0x1;
                if (abi_ByteN(pkt, 8) > tapTutorialStage) {
                    mascotTapReactAnimFlag = 1;
                }
                for (new soundI = 0; soundI < FACES_MAX; ++soundI) {
                    tiltTutCollectableSounds{soundI} = (abi_ByteN(pkt, 11) >> (soundI * 2)) & 0x3;
                }
                tapTutorialStage = abi_ByteN(pkt, 8);
                shakeTutorialStage = abi_ByteN(pkt, 9);
                twistTutorialStage = abi_ByteN(pkt, 10);
                tutorialStartTimer = pkt[3];
            }
        }
        case PKT_BALL_TILT_TUT: {
            new packetNumberReceived = pkt[4];
            if ((ballPkt < packetNumberReceived) || ((ballPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                ballPkt = packetNumberReceived;
                tiltTutBall.module = abi_ByteN(pkt, 5);
                tiltTutBall.screen = abi_ByteN(pkt, 6);
                tiltTutBall.angle = pkt[2];
                tiltTutBall.screenAngle = pkt[3];
                for (new item = 0; item < FACES_MAX; ++item) {
                    collectables{item} = (abi_ByteN(pkt, 7) >> item) & 0x1;
                }
            }
        }
        case PKT_SELECTOR_TILT_TUT: {
            new packetNumberReceived = pkt[3];
            if ((selectorPkt < packetNumberReceived) || ((selectorPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                selectorPkt = packetNumberReceived;
                tiltTutSelector.module = abi_ByteN(pkt, 5);
                if (tiltTutSelector.module == abi_cubeN) {
                    selectorInAnimation = pkt[4];
                }
                tiltTutSelector.screen = abi_ByteN(pkt, 6);
                tiltTutSelector.screenAngle = pkt[2];
            }
        }
    }
}