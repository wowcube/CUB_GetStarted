forward run(const pkt[], size, const src[]); // public Pawn function seen from C

#include "getstarted_start.inc"
#include "getstarted_twistTut.inc"
#include "getstarted_tapTut.inc"
#include "getstarted_tiltTut.inc"
#include "getstarted_shakeTut.inc"
#include "getstarted_success.inc"

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
            //arrowIcons[0] = ARROW_3;
            //arrowIcons[1] = ARROW_2;
            //arrowIcons[2] = ARROW_1;
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
    //for (new module = 0; module < CUBES_MAX; ++module) {
        for (new screen = 0; screen < FACES_MAX ; ++screen) {
            //if (TopologyGetPlaneProject(abi_cubeN, screen) == topology_location:location_max) {
            //    return;
            //}
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
    //}
    
    needNewSideType = 0;
}

SendGeneralInfo(pktNumber) {
    new data[4];

    new flags = beginTapTutorial | (finishTapTutorial << 1) | (beginTiltTutorial << 2) | (finishTiltTutorial << 3) | (selectorTutorial << 4) | (beginShakeTutorial << 5);
    //data[0] = PKT_GENERAL_DATA | (beginTapTutorial << 8) | (beginTiltTutorial << 16) | (applicationState << 24);
    //data[1] = tapTutorialStage | (finishTapTutorial << 8) | (finishTiltTutorial << 16);
    data[0] = PKT_GENERAL_DATA | (flags << 8) | (previousAppState << 16) | (applicationState << 24);
    data[1] = tapTutorialStage | (shakeTutorialStage << 8) | (twistTutorialStage << 16);
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
        //abi_CMD_TEXT_ITOA(checkTopology, 0, 120, 60, 25, 0, TEXT_ALIGN_CENTER, 0xFF, 0x00, 0x00, true);
        //abi_CMD_TEXT_ITOA(abi_MTD_GetShakesCount(), 0, 120, 180, 25, 0, TEXT_ALIGN_CENTER, 0xFF, 0x00, 0x00, true);
        abi_CMD_G2D_END();
    }
}

ONTICK() {
    currentTime = abi_GetTime();
    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

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
    previousTime = abi_GetTime();

    #ifndef CUBIOS_EMULATOR

    ARROW_1 = getSpriteIdByName("arrow_1.png");
    ARROW_2 = getSpriteIdByName("arrow_2.png");
    ARROW_3 = getSpriteIdByName("arrow_3.png");
    
    ARROW_TILT    = getSpriteIdByName("arrow_tilt.png");
    BALL          = getSpriteIdByName("ball.png");
    COLLECTABLE   = getSpriteIdByName("collectable.png");
    COMPLETE_ICON = getSpriteIdByName("complete.png");
    COUNT_BAR     = getSpriteIdByName("count_bar.png");

    DIALOGUE              = getSpriteIdByName("dialogue.png");
    MASCOT_MAIN_SPRITE    = getSpriteIdByName("mascot_main.png");
    MASCOT_SUCCESS_SPRITE = getSpriteIdByName("mascot_success.png");
    MASCOT_WAIT_SPRITE    = getSpriteIdByName("mascot_wait.png");
    CIRCLE_QUARTER        = getSpriteIdByName("quarter.png");
    SELECTOR              = getSpriteIdByName("selector.png");
    SHAKE_ICON            = getSpriteIdByName("shake_icon.png");
    TAP_ICON              = getSpriteIdByName("tap_icon.png");
    TILT_ICON             = getSpriteIdByName("tilt_icon.png");
    TWIST_ICON            = getSpriteIdByName("twist_icon.png");

    #endif

    SetApplicationState(FSM:start);
}

ON_CHECK_ROTATE() {
    needNewSideType = 1;
    SetDefaultMascot();
    
    if (abi_cubeN == 0) {
        if (applicationState == FSM:twistTutorial) {
            ++twistTutorialStage;
            if (twistTutorialStage >= MAX_TWIST_TUTORIAL_STAGES) {
                SetApplicationState(FSM:successScreen);
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

ON_LOAD_GAME_DATA() {
}

ON_CMD_NET_RX (const pkt[]) {
    switch (abi_ByteN(pkt, 4)) {
        case PKT_GENERAL_DATA: {
            new packetNumberReceived = pkt[4];
            if ((generalDataPkt < packetNumberReceived) || ((generalDataPkt - packetNumberReceived) > (0x7FFFFFFF >> 1))) {
                generalDataPkt = packetNumberReceived;
                SetApplicationState(abi_ByteN(pkt, 7));
                //previousAppState = abi_ByteN(pkt, 6);
                new flags = abi_ByteN(pkt, 5);
                beginTapTutorial = flags & 0x1;
                finishTapTutorial = (flags >> 1) & 0x1;
                beginTiltTutorial = (flags >> 2) & 0x1;
                finishTiltTutorial = (flags >> 3) & 0x1;
                selectorTutorial = (flags >> 4) & 0x1;
                beginShakeTutorial = (flags >> 5) & 0x1;
                if (abi_ByteN(pkt, 8) > tapTutorialStage) {
                    mascotTapReactAnimFlag = 1;
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
                tiltTutSelector.screen = abi_ByteN(pkt, 6);
                tiltTutSelector.screenAngle = pkt[2];
            }
        }
    }
}