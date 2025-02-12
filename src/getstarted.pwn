forward run(const pkt[], size, const src[]); // public Pawn function seen from C

#include "getstarted_start.inc"
#include "getstarted_twistTut.inc"
#include "getstarted_tapTut.inc"
#include "getstarted_tiltTut.inc"
#include "getstarted_shakeTut.inc"
#include "getstarted_success.inc"
#include "getstarted_firstLaunch.inc"

SaveData() {
    if (dataSaved) {
        return;
    }
    new saveData [2] = [0,...];
    saveData[0] = alreadyLaunched + 1;
    saveState(saveData);
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
            beginTapTutorial = 1;
            fillTapTutorial  = 0;
        } else if (newState == FSM:tiltTutorial) {
            tiltTutBall.angle = 45;
            tiltTutBall.pos = 2;
            tiltTutBall.moduleT = tiltTutBall.module = MODULES_MAX;
            tiltTutBall.screenT = tiltTutBall.screen = SCREENS_MAX;

            tiltTutSelector.pos = 3;
            tiltTutSelector.moduleT = tiltTutSelector.module = MODULES_MAX;
            tiltTutSelector.screenT = tiltTutSelector.screen = SCREENS_MAX;

            DotRoad.pos = 3;
            DotRoad.number = 4;
            DotRoad.moduleT = DotRoad.module = MODULES_MAX;
            DotRoad.screenT = DotRoad.screen = SCREENS_MAX;
        } else if (newState == FSM:successScreen) {
            successAnimationEffectTimer = 0;
            successAnimationEffectFlag = 0;
            sucAnimEffectTimePercent = 0;

            successAnimationFrame = 0;
            alphaHightlight = 0;
            successRepeatAnimFlag = 0;
        } else if (newState == FSM:firstLaunch) {
            mascotHandWaveAnimFrames{0} = MAIN_HAND_1_SPRITE;
            mascotHandWaveAnimFrames{1} = MAIN_HAND_2_SPRITE;
            mascotHandWaveAnimFrames{2} = MAIN_HAND_3_SPRITE;
            mascotHandWaveAnimFrames{3} = MAIN_HAND_2_SPRITE;
                
            arcsInCurtain[0].sprite = ARC_1_SPRITE;
            arcsInCurtain[1].sprite = ARC_2_SPRITE;
            arcsInCurtain[2].sprite = ARC_3_SPRITE;
            arcsInCurtain[3].sprite = ARC_4_SPRITE;
            arcsInCurtain[4].sprite = ARC_5_SPRITE;
        } else if (newState == FSM:shakeTutorial) {
            shakeTutorialStage = 0;
        }
        previousAppState = applicationState;
        applicationState = newState;
        SetDefaultMascot();
        bakeAppStateSpritesFlag = 1;
    }
}

BakeSpritesForCurrentAppState(currentAppState) {
    GFX_clearCache();
    switch (currentAppState) {
        case start: {
            StartBakeSprites();
        }
        case twistTutorial: {
            TwistTutorialBakeSprites();
        }
        case tapTutorial: {
            TapTutorialBakeSprites();
        }
        case tiltTutorial: {
            TiltTutorialBakeSprites();
        }
        case shakeTutorial: {
            ShakeTutorialBakeSprites();
        }
        case successScreen: {
        }
        case firstLaunch: {
            FirstLaunchBakeSprites();
        }
    }
    bakeAppStateSpritesFlag = 0;
}

GetMapping() {
    for (new moduleI = 0; moduleI < MODULES_MAX; ++moduleI) {
        for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
            new curPlace[TOPOLOGY_PLACE];
            curPlace = TOPOLOGY_getPlace(SetFacelet(moduleI, screenI), ORIENTATION_MODE_SPLASH);
            getStarted_AllScreensData[moduleI * SCREENS_MAX + screenI].face = TOPOLOGY_getFaceletOrientation(SetFacelet(moduleI, screenI));
            getStarted_AllScreensData[moduleI * SCREENS_MAX + screenI].pos = curPlace.position;
        }
    }
}

SendGeneralInfo(pktNumber) {
    new data[MAX_PACKET_SIZE / 4];

    new flags = beginTapTutorial
    | (finishTiltTutorial << 3) 
    | (selectorTutorial << 4) 
    | (beginShakeTutorial << 5)
    | (getstartedGreetingFlag << 6)
    | (fillTapTutorial << 7)
    | (flStartFlag << 8)
    | (tiltTutEndFlag << 9);

    new randomSoundOrder = 0;
    for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
        randomSoundOrder |= (tiltTutCollectableSounds{soundI} << (soundI * 2));
    }
    data[0] = (shakeTutorialStage << 8) | (previousAppState << 16) | (applicationState << 24);
    data[1] = tapTutorialStage | (sideTapIndicatorPos << 8) | (twistTutorialStage << 16) | (randomSoundOrder << 24);
    data[2] = tutorialStartTimer;
    data[3] = alreadyLaunched;
    data[4] = flags;

    broadcastPacket(PKT_GENERAL_DATA, data);
}

SendMapping() {
    new data[MAX_PACKET_SIZE / 4];

    new dataI = 0;
    for (new screenI = 3, offset = 0; screenI < MODULES_MAX * SCREENS_MAX; ++screenI, ++offset) {
        if (offset >= 6) {
            offset = 0;
            ++dataI;
        }
        data[dataI] |= (getStarted_AllScreensData[screenI].face << (5 * offset))
                     | (getStarted_AllScreensData[screenI].pos << (3 + (5 * offset)));
    }
    
    data[4] = mappingPkt;

    broadcastPacket(PKT_MAPPING, data);
}

CheckTwists(twist[TOPOLOGY_TWIST_INFO]) {
    // While twisting uart == screen
    new uartNumber = twist.screen;
    if ((twist.direction < TOPOLOGY_twist:TWIST_LEFT) && (twist.direction > TOPOLOGY_twist:TWIST_RIGHT)) {
        commonRotationCount = 0;
        rotationSequence = [0, 0, 0];
        return false;
    }
    rotationSequence[uartNumber]++;
    commonRotationCount++;
    
    if (commonRotationCount < TWISTS_COUNT_FOR_SPECIAL_EXIT) {
        new singleRotationCount = 0;
        for (new uarti = 0; uarti < SCREENS_MAX; ++uarti) {
            if (rotationSequence[uarti] > 0 && rotationSequence[uarti] < TWISTS_BY_UART) {
                ++singleRotationCount;
            }
        }
        // Sequence is incorrect
        if (singleRotationCount > 1) {
            return false;
        }
    // Check 6 twists sequence
    } else if (commonRotationCount == TWISTS_COUNT_FOR_SPECIAL_EXIT) {
        new res = true;
        for (new uartI = 0; uartI < SCREENS_MAX; ++uartI) {
            if (rotationSequence[uartI] != TWISTS_BY_UART) {
                res = false;
                break;
            }
        }
        commonRotationCount = 0;
        rotationSequence = [0, 0, 0];
        return res;
    } else {
        commonRotationCount = 0;
        rotationSequence = [0, 0, 0];
        return false;
    }
    return false;
}

public ON_PhysicsTick() {
}

public ON_Render() {
    if (bakeAppStateSpritesFlag) {
        BakeSpritesForCurrentAppState(applicationState);
    }

    for (new screenI = 0; screenI < SCREENS_MAX; ++screenI) {
        GFX_setRenderTarget(screenI);
        
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
            case firstLaunch: {
                DrawFirstLaunch(screenI);
            }
        }
        GFX_render();
    }
}

public ON_Tick() {
    currentTime = getTime();
    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    CheckAcceleration();
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
        case firstLaunch: {
            UpdateFirstLaunch(deltaTime);
        }
    }

    if (SELF_ID == 0) {
        generalDataPkt = ++generalDataPkt % 0x7FFFFFFF;
        SendGeneralInfo(generalDataPkt);
        SendMapping();
    }
}

public ON_Init(id, size, const pkt[]) {
    loadState();

    previousTime = getTime();

    ARROW_TWIST = GFX_getAssetId("arrow_twist.png");
    
    ARROW_TILT    = GFX_getAssetId("arrow_tilt.png");
    BALL          = GFX_getAssetId("ball.png");
    COLLECTED     = GFX_getAssetId("collected.png");
    COLLECTABLE   = GFX_getAssetId("collectable.png");
    COMPLETE_ICON = GFX_getAssetId("complete.png");
    COUNT_BAR     = GFX_getAssetId("count_bar.png");

    DIALOGUE              = GFX_getAssetId("dialogue.png");
    DIALOGUE_SMALL        = GFX_getAssetId("speechSmall.png");

    COLLECTED_CHECK       = GFX_getAssetId("checked.png");

    CIRCLE_QUARTER        = GFX_getAssetId("quarter.png");
    SELECTOR              = GFX_getAssetId("selector.png");
    SHAKE_ICON            = GFX_getAssetId("shake_icon.png");
    TAP_ICON              = GFX_getAssetId("tap_icon.png");
    TILT_ICON             = GFX_getAssetId("tilt_icon.png");
    TWIST_ICON            = GFX_getAssetId("twist_icon.png");

    BIG_STAR              = GFX_getAssetId("star_big.png");

    CIRCLE_QUARTER_PUSH   = GFX_getAssetId("quarterPush.png");

    FINGER_SPRITE         = GFX_getAssetId("finger.png");
    WRONG_TAP_ICON        = GFX_getAssetId("wrongTap.png");
    RIGHT_TAP_ICON        = GFX_getAssetId("rightTap.png");
    
    MASCOT_MAIN_EMPTY_SPRITE       = GFX_getAssetId("masMainE.png");
    MASCOT_MAIN_EYES_NORMAL_SPRITE = GFX_getAssetId("masEyesN.png");
    MASCOT_MAIN_EYES_CUTE_SPRITE   = GFX_getAssetId("masEyesV.png");
    MASCOT_MAIN_EYES_X_SPRITE      = GFX_getAssetId("masEyesX.png");
    MASCOT_MAIN_MOUNTH_O_SPRITE    = GFX_getAssetId("masMouthO.png");

    MASCOT_SUCCESS_BODY_SPRITE   = GFX_getAssetId("masSuccess.png");
    MASCOT_SUCCESS_EYES_SPRITE   = GFX_getAssetId("masSucEyes.png");
    MASCOT_SUCCESS_MOUNTH_SPRITE = GFX_getAssetId("mSucMouth.png");

    HI_SPRITE          = GFX_getAssetId("hi.png");
    OFF_HAND_SPRITE    = GFX_getAssetId("off_hand.png");
    MAIN_HAND_1_SPRITE = GFX_getAssetId("hand_main1.png");
    MAIN_HAND_2_SPRITE = GFX_getAssetId("hand_main2.png");
    MAIN_HAND_3_SPRITE = GFX_getAssetId("hand_main3.png");

    HE_GREEN_SPRITE  = GFX_getAssetId("heGreen.png");
    HE_ORANGE_SPRITE = GFX_getAssetId("heOrange.png");
    HE_PINK_SPRITE   = GFX_getAssetId("hePink.png");
    HE_PURPLE_SPRITE = GFX_getAssetId("hePurple.png");
    HE_WHITE_SPRITE  = GFX_getAssetId("heWhite.png");
    LLO_GREEN_SPRITE  = GFX_getAssetId("lloGreen.png");
    LLO_ORANGE_SPRITE = GFX_getAssetId("lloOrange.png");
    LLO_PINK_SPRITE   = GFX_getAssetId("lloPink.png");
    LLO_PURPLE_SPRITE = GFX_getAssetId("lloPurple.png");
    LLO_WHITE_SPRITE  = GFX_getAssetId("lloWhite.png");

    ARC_1_SPRITE = GFX_getAssetId("arc1.png");
    ARC_2_SPRITE = GFX_getAssetId("arc2.png");
    ARC_3_SPRITE = GFX_getAssetId("arc3.png");
    ARC_4_SPRITE = GFX_getAssetId("arc4.png");
    ARC_5_SPRITE = GFX_getAssetId("arc5.png");

    SIDE_TAP_INDICATOR = GFX_getAssetId("sideTapInd.png");
    SILUETTE_SPRITE = GFX_getAssetId("siluette.png");

    TWIST_TEXT_ORANGE = GFX_getAssetId("TXtwistOXL.png");
    TWIST_TEXT_PURPLE = GFX_getAssetId("TXtwistPXL.png");
    GOOD_TEXT_GREEN   = GFX_getAssetId("TXgoodGXL.png");
    GOOD_TEXT_ORANGE  = GFX_getAssetId("TXgoodOXL.png");
    MASCOT_SUCCESS_EYEBROWS_SPRITE = GFX_getAssetId("masSucBrow.png");
    DOUBLE_PAT_TEXT_ORANGE_SPRITE = GFX_getAssetId("TX_D_PatO.png");
    DOUBLE_PAT_TEXT_GREEN_SPRITE = GFX_getAssetId("TX_D_PatG.png");
    VERY_GOOD_TEXT_GREED_SPRITE = GFX_getAssetId("TXvgoodGXL.png");

    countOrangeXL[0] = GFX_getAssetId("TX_00_O.png");
    countOrangeXL[1] = GFX_getAssetId("TX_01_O.png");
    countOrangeXL[2] = GFX_getAssetId("TX_02_O.png");
    countOrangeXL[3] = GFX_getAssetId("TX_03_O.png");
    countOrangeS[0] = GFX_getAssetId("TX_00_OS.png");
    countOrangeS[1] = GFX_getAssetId("TX_01_OS.png");
    countOrangeS[2] = GFX_getAssetId("TX_02_OS.png");
    countOrangeS[3] = GFX_getAssetId("TX_03_OS.png");
    _03_WHITE_TEXT_SPRITE = GFX_getAssetId("TX_03_W.png");
    _03_WHITE_TEXT_SMALL_SPRITE = GFX_getAssetId("TX_03_WS.png");
    TAP_ICON_GREEN_SPRITE = GFX_getAssetId("tap_iconG.png");
    PLUS_ONE_TEXT_SPRITE = GFX_getAssetId("plusOne.png");
    EXCELLENT_TEXT_SPRITE = GFX_getAssetId("TXexcellent.png");
    COLLECTED_CHECK_GREEN = GFX_getAssetId("checked_grn.png");
    SHAKE_L_TEXT_SPRITE = GFX_getAssetId("TXshakeL.png");
    SHAKE_XL_TEXT_SPRITE = GFX_getAssetId("TXshakeXL.png");
    TWIST_TEXT_ORANGE_L = GFX_getAssetId("TXtwistOL.png");
    TWIST_TEXT_GREEN_L = GFX_getAssetId("TXtwistGL.png");
    TWIST_ICON_GREEN = GFX_getAssetId("twist_iconG.png");
    HIGHTLIGHT_GREEN = GFX_getAssetId("lightGreen.png");
    BALL_EFFECT_ORANGE = GFX_getAssetId("effOrange.png");
    BALL_EFFECT_GREEN = GFX_getAssetId("effGreen.png");

    // Sounds
    ACTION_SOUND      = SND_getAssetId("action.mp3");
    GOOD_SOUND        = SND_getAssetId("good.mp3");
    EXCELLENT_1_SOUND = SND_getAssetId("excellent_1.mp3");
    EXCELLENT_2_SOUND = SND_getAssetId("excellent_2.mp3");
    
    SELECTOR_MENU_SOUND = SND_getAssetId("selec_menu.mp3");
    
    TAPS_1_2_SOUND = SND_getAssetId("taps_1-2.wav");
    TAPS_3_4_SOUND = SND_getAssetId("taps_3-4.wav");
    TAPS_5_6_SOUND = SND_getAssetId("taps_5-6.wav");
    TAP_STAGE_SUCCESS_SOUND = SND_getAssetId("tapsSuccess.mp3");

    PLUS_1_SHAPE_COLLECT_1_SOUND = SND_getAssetId("collect_1.mp3");
    PLUS_1_SHAPE_COLLECT_2_SOUND = SND_getAssetId("collect_2.mp3");
    PLUS_1_SHAPE_COLLECT_3_SOUND = SND_getAssetId("collect_3.mp3");

    tiltTutCollectableSounds{0} = PLUS_1_SHAPE_COLLECT_1_SOUND;
    tiltTutCollectableSounds{1} = PLUS_1_SHAPE_COLLECT_2_SOUND;
    tiltTutCollectableSounds{2} = PLUS_1_SHAPE_COLLECT_3_SOUND;

    if (SELF_ID == 0) {
        for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
            new j = random (0, SCREENS_MAX - 1);
            new temp = tiltTutCollectableSounds{j};
            tiltTutCollectableSounds{j} = tiltTutCollectableSounds{soundI};
            tiltTutCollectableSounds{soundI} = temp;
        }
        ++mappingPkt;
    }

    GetMapping();

    SetApplicationState(FSM:firstLaunch);
}

public ON_Quit() {
    if (SELF_ID == 0) {
        SaveData();
    }
}

public ON_Shake(const count) {
    if (count > 0) {
        SetDefaultMascot();
    }
    if ((SELF_ID == 0) && (applicationState == FSM:shakeTutorial)) {
        if (beginShakeTutorial) {
            shakeTutorialStage += count;
            if (shakeTutorialStage >= SENSITIVITY_MENU_CHANGE_SCRIPT) {
                SaveData();
            }
        }
        if (!beginShakeTutorial && (count > 0)) {
            beginShakeTutorial = 1;
        }
    }
}

public ON_Tap(const count, const display, const bool:opposite) {
    if (count > 0) {
        SetDefaultMascot();
    }
    if (SELF_ID == 0) {
        switch(applicationState) {
            case tapTutorial: {
                if (fillTapTutorial) {
                    if(count > 1) {
                        ++tapTutorialStage;
                        if (tapTutorialStage >= 2) {
                            SND_play(TAPS_5_6_SOUND, SOUND_VOLUME);
                        } else if (tapTutorialStage >= 1) {
                            SND_play(TAPS_3_4_SOUND, SOUND_VOLUME);
                        } else if (tapTutorialStage >= 0) {
                            SND_play(TAPS_1_2_SOUND, SOUND_VOLUME);
                        }
                        mascotTapReactAnimFlag = 1;
                    }
                }
                if (!fillTapTutorial && beginTapTutorial && (count == 2)) {
                    fillTapTutorial = 1;
                }
            }
            case tiltTutorial: {
                if (count >= 2) {
                    if (selectorTutorial) {
                        if (tiltTutSelector.pos == 1) {
                            SND_play(EXCELLENT_2_SOUND, SOUND_VOLUME);
                            tiltAnimEffectTime = 0;
                            tiltTutEndFlag = 1;
                        }
                    } else if (finishTiltTutorial) {
                        SND_play(ACTION_SOUND, SOUND_VOLUME);
                        finishTiltTutorial = 0;
                        selectorTutorial = 1;
                    }
                }
            }
            case successScreen: {
                if ((previousAppState == FSM:tapTutorial) && (count >= 2)) {
                    SetApplicationState(FSM:tiltTutorial);
                    SND_play(ACTION_SOUND, SOUND_VOLUME);
                }
            }
        }
    }
}

public ON_Twist(twist[TOPOLOGY_TWIST_INFO]) {
    if (SELF_ID == 0) {
        GetMapping();
        ++mappingPkt;
    }

    SetDefaultMascot();
    
    if (SELF_ID == 0) {
        // Exit from app by 2 consequent rotations per each uart
        if (CheckTwists(twist)) {
            quit();
        }

        if (applicationState == FSM:twistTutorial) {
            ++twistTutorialStage;
            if (twistTutorialStage == 1) {
                SND_play(GOOD_SOUND, SOUND_VOLUME);
            }
            if (twistTutorialStage >= MAX_TWIST_TUTORIAL_STAGES) {
                SetApplicationState(FSM:successScreen);
                SND_play(EXCELLENT_1_SOUND, SOUND_VOLUME);
            }
        } else if ((applicationState == FSM:successScreen) && (previousAppState == FSM:twistTutorial)) {
            SetApplicationState(FSM:tapTutorial);
        } else if (applicationState == FSM:firstLaunch) {
            SetApplicationState(FSM:start);
        }
    }

    if (applicationState == FSM:tiltTutorial) {
        tiltTutBall.module = MODULES_MAX;
        tiltTutSelector.module = MODULES_MAX;
        DotRoad.module = MODULES_MAX;
    }
}

public ON_Load(id, size, const pkt[]) {
    if (size == 0) {
        return;
    }
    alreadyLaunched = pkt[0];
}

public ON_Packet(type, size, const pkt[]) {
    switch (type) {
        case PKT_GENERAL_DATA: {
            alreadyLaunched = pkt[3];
            SetApplicationState(parseByte(pkt, 3));
            new flags = pkt[4];
            beginTapTutorial = flags & 0x1;
            finishTiltTutorial = (flags >> 3) & 0x1;
            selectorTutorial = (flags >> 4) & 0x1;
            beginShakeTutorial = (flags >> 5) & 0x1;
            getstartedGreetingFlag = (flags >> 6) & 0x1;
            fillTapTutorial = (flags >> 7) & 0x1;
            flStartFlag = (flags >> 8) & 0x1;
            tiltTutEndFlag = (flags >> 9) & 0x1;
            if (parseByte(pkt, 4) > tapTutorialStage) {
                mascotTapReactAnimFlag = 1;
            }
            for (new soundI = 0; soundI < SCREENS_MAX; ++soundI) {
                tiltTutCollectableSounds{soundI} = (parseByte(pkt, 7) >> (soundI * 2)) & 0x3;
            }
            shakeTutorialStage = parseByte(pkt, 1);
            tapTutorialStage = parseByte(pkt, 4);
            sideTapIndicatorPos = parseByte(pkt, 5);
            twistTutorialStage = parseByte(pkt, 6);
            tutorialStartTimer = pkt[2];
        }
        case PKT_BALL_TILT_TUT: {
            new packetNumberReceived = pkt[3];
            if ((ballPkt < packetNumberReceived) || ((ballPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                ballPkt = packetNumberReceived;
                tiltTutBall.module = parseByte(pkt, 1);
                tiltTutBall.screen = parseByte(pkt, 2);
                tiltTutBall.angle = pkt[1] & 0xFFFF;
                tiltTutBall.pos = (pkt[1] >> 16) & 0xFFFF;
                tiltTutBall.spd = pkt[4];
                for (new item = 0; item < SCREENS_MAX; ++item) {
                    collectables{item} = (parseByte(pkt, 3) >> item) & 0x1;
                }
                if (tiltTutBall.module == SELF_ID) {
                    ballTransitionAnimFlag = (pkt[2] & 0xFF) ^ 2;
                }
            }
        }
        case PKT_SELECTOR_TILT_TUT: {
            new packetNumberReceived = pkt[2];
            if ((selectorPkt < packetNumberReceived) || ((selectorPkt - packetNumberReceived) > (0xFFFFF >> 1))) {
                selectorPkt = packetNumberReceived;
                tiltTutSelector.module = parseByte(pkt, 1);
                if (tiltTutSelector.module == SELF_ID) {
                    selectorInAnimation = pkt[3];
                }
                tiltTutSelector.screen = parseByte(pkt, 2);
                tiltTutSelector.pos = pkt[1];
            }
        }
        case PKT_MAPPING: {
            new packetNumberReceived = pkt[4];
            if (mappingPkt < packetNumberReceived) {
                mappingPkt = packetNumberReceived;
                new dataI = 0;
                for (new screenI = 3, offset = 0; screenI < MODULES_MAX * SCREENS_MAX; ++screenI, ++offset) {
                    if (offset >= 6) {
                        offset = 0;
                        ++dataI;
                    }
                    getStarted_AllScreensData[screenI].face = (pkt[dataI] >> (5 * offset)) & 0x7;
                    getStarted_AllScreensData[screenI].pos = (pkt[dataI] >> (3 + (5 * offset))) & 0x3;
                }
            }
        }
        case PKT_DOTROAD_TILT_TUT: {
            new packetNumberReceived = pkt[1];
            if (dotRoadPkt < packetNumberReceived) {
                dotRoadPkt = packetNumberReceived;
                DotRoad.module = parseByte(pkt, 0);
                DotRoad.screen = parseByte(pkt, 1);
                DotRoad.number = parseByte(pkt, 2);
                DotRoad.pos = parseByte(pkt, 3);
            }
        }
    }

}