// -*- mode: c++ -*-
// Snowkeep's Handsdown Gold Implementation
// Based on: Copyright 2016-2022 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

/**
 * These #include directives pull in the Kaleidoscope firmware core,
 * as well as the Kaleidoscope plugins we use in the Model 100's firmware
 */

// The Kaleidoscope core
#include "Kaleidoscope.h"

// Support for storing the keymap in EEPROM
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-FirmwareVersion.h"

#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-LEDEffect-BootGreeting.h"
#include "Kaleidoscope-LEDEffect-SolidColor.h"
#include "Kaleidoscope-LEDEffect-Breathe.h"
#include "Kaleidoscope-LEDEffect-Chase.h"
#include "Kaleidoscope-LEDEffect-DigitalRain.h"
#include "Kaleidoscope-LEDEffect-Rainbow.h"
#include "Kaleidoscope-LED-Stalker.h"
#include "Kaleidoscope-LED-AlphaSquare.h"
#include "Kaleidoscope-LED-Palette-Theme.h"
#include "Kaleidoscope-LED-ActiveModColor.h"
#include "Kaleidoscope-Colormap.h"
#include "Kaleidoscope-IdleLEDs.h"
#include "Kaleidoscope-DefaultLEDModeConfig.h"

// Support for Keyboardio's internal keyboard testing mode
#include "Kaleidoscope-HardwareTestMode.h"
#include "Kaleidoscope-HostPowerManagement.h"

// Support for magic combos (key chords that trigger an action)
#include "Kaleidoscope-MagicCombo.h"
#include "Kaleidoscope-USB-Quirks.h"
#include "Kaleidoscope-LayerNames.h"

#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-Escape-OneShot.h"
#include "Kaleidoscope-Qukeys.h"
#include "Kaleidoscope-CharShift.h"
#include "Kaleidoscope-Chord.h"
#include "Kaleidoscope-DynamicMacros.h"
#include "Kaleidoscope-TapDance.h"


enum {
  MACRO_VERSION_INFO,
  MACRO_BRACES,
  MACRO_UPDIR,
  MACRO_SHBANG,
  MACRO_LBUP,
  MACRO_LBDN,
  MACRO_LSOSM,
  MACRO_RSOSM,
};


enum {
  PRIMARY,
  SYM,
  NUM,
  NAV,
  MIR,
  TIL,
};  // layers

#define LFN  MIR
#define RFN  NAV

// bunch of shift shortcuts
#define K_LCBR   Key_LeftCurlyBracket
#define K_RCBR   Key_RightCurlyBracket
#define K_Colon  LSHIFT(Key_Semicolon)
#define K_Pipe   LSHIFT(Key_Backslash)
#define K_Tild   LSHIFT(Key_Backtick)
#define K_LT     LSHIFT(Key_Comma)
#define K_Plus   LSHIFT(Key_Equals)
#define K_Unds   LSHIFT(Key_Minus)
#define K_GT     LSHIFT(Key_Period)
#define K_DQuot  LSHIFT(Key_Quote)
#define K_Ques   LSHIFT(Key_Slash)
#define K_Exlm   LSHIFT(Key_1)
#define K_At     LSHIFT(Key_2)
#define K_Hash   LSHIFT(Key_3)
#define K_Dolr   LSHIFT(Key_4)
#define K_Perc   LSHIFT(Key_5)
#define K_Circ   LSHIFT(Key_6)
#define K_Ampr   LSHIFT(Key_7)
#define K_Astr   LSHIFT(Key_8)
#define K_LParen LSHIFT(Key_9)
#define K_RParen LSHIFT(Key_0)

#define Key_EXCLM LSHIFT(Key_1)
#define Key_AT LSHIFT(Key_2)
#define Key_HASH LSHIFT(Key_3)
#define Key_DOLLR LSHIFT(Key_4)
#define Key_PRCNT LSHIFT(Key_5)
#define Key_CARET LSHIFT(Key_6)
#define Key_AND LSHIFT(Key_7)
#define Key_STAR LSHIFT(Key_8)
#define Key_LCB LSHIFT(Key_LeftBracket)
#define Key_RCB LSHIFT(Key_RightBracket)
#define Key_Plus LSHIFT(Key_Equals)
#define Key_Underscore LSHIFT(Key_Minus)
#define Key_QMARK LSHIFT(Key_Slash)
#define Key_PIPE LSHIFT(Key_Backslash)
#define Key_COLON LSHIFT(Key_Semicolon)
#define Key_CLT LSHIFT(Key_Comma)
#define Key_PGT LSHIFT(Key_Period)


enum {
    PBS,    // Pipe backslash
    EQP,    // =/+
    LPB,    // (/[
    RPB,    // )/]
    COLON,  // :/;
    MNP,    // Media Next/Play
    VOLD,   // Vol +/-
    AQ,     // '/"
    CLT,    // ,/<
    MUS,    // -/_
    BTT,    // `/~
    PGT,    // ./>
    COPY,   // Ctrl-c,v,x
};


/* This comment temporarily turns off astyle's indent enforcement
 *   so we can make the keymaps actually resemble the physical key layout better
 */
// clang-format off


void tapDanceAction(uint8_t tapDanceIndex, KeyAddr key_addr, uint8_t tapCount, kaleidoscope::plugin::TapDance::ActionType tapDanceAction)
{
    switch (tapDanceIndex)
    {
    case LPB:
        if (tapDanceAction == kaleidoscope::plugin::TapDance::Hold)
        {
            return tapDanceActionKeys(tapCount, tapDanceAction, Key_LCB);
        }
        else
        {
            return tapDanceActionKeys(tapCount, tapDanceAction,
                                      Key_LeftParen,
                                      Key_LeftBracket);
        }
    case RPB:
        if (tapDanceAction == kaleidoscope::plugin::TapDance::Hold)
        {
            return tapDanceActionKeys(tapCount, tapDanceAction, Key_RCB);
        }
        else
        {
            return tapDanceActionKeys(tapCount, tapDanceAction,
                                      Key_RightParen,
                                      Key_RightBracket);
        }
    case COPY:
        if (tapDanceAction == kaleidoscope::plugin::TapDance::Hold)
        {
            return tapDanceActionKeys(tapCount, tapDanceAction, LCTRL(Key_V));
        }
        else
        {
            return tapDanceActionKeys(tapCount, tapDanceAction,
                                      LCTRL(Key_C),
                                      LCTRL(Key_X));
        }
    case COLON:
        if (tapDanceAction == kaleidoscope::plugin::TapDance::Hold)
        {
            return tapDanceActionKeys(tapCount, tapDanceAction, Key_COLON);
        }
        else
        {
            return tapDanceActionKeys(tapCount, tapDanceAction,
                                      Key_Semicolon,
                                      Key_Semicolon);
        }                              
    case CLT:
        if (tapDanceAction == kaleidoscope::plugin::TapDance::Hold)
        {
            return tapDanceActionKeys(tapCount, tapDanceAction, Key_CLT);
        }
        else
        {
            return tapDanceActionKeys(tapCount, tapDanceAction,
                                      Key_Comma,
                                      Key_Comma);
        }                                        
    case PGT:
        if (tapDanceAction == kaleidoscope::plugin::TapDance::Hold)
        {
            return tapDanceActionKeys(tapCount, tapDanceAction, Key_PGT);
        }
        else
        {
            return tapDanceActionKeys(tapCount, tapDanceAction,
                                      Key_Period,
                                      Key_Period);                                                              
        }    
     case MUS:
        if (tapDanceAction == kaleidoscope::plugin::TapDance::Hold)
        {
            return tapDanceActionKeys(tapCount, tapDanceAction, Key_Underscore);
        }
        else
        {
            return tapDanceActionKeys(tapCount, tapDanceAction,
                                      Key_Minus,
                                      Key_Underscore);                                                                
        }         
    }
}


KEYMAPS(

  // Edit this keymap to make a custom layout
  [PRIMARY] = KEYMAP_STACKED
  (Key_Escape,     Key_1, Key_2, Key_3, Key_4, Key_5, Key_F11,
   OSM(LeftShift), Key_Y, Key_C, Key_L, Key_M, Key_K, TD(COPY),
   Key_Enter,      Key_I, Key_S, Key_R, Key_T, Key_G,
   Key_Delete,     Key_Q, Key_V, Key_W, Key_D, Key_J, TD(LPB),
   Key_Backspace,  Key_Spacebar, Key_Tab, OSL(LFN),
   OSL(NUM),

   Key_F12,        Key_6, Key_7, Key_8,   Key_9,   Key_0, Key_CapsLock,
   CS(8),          Key_Z, Key_F, Key_U,   CS(2),   CS(3), OSM(RightShift),
                   Key_P, Key_N, Key_E,   Key_A,   Key_O, CS(0),
   TD(RPB),        Key_B, Key_H, TD(CLT), TD(PGT), Key_X, M(MACRO_BRACES),
   OSM(RightGui),  Key_Enter, Key_Backspace, TD(MUS),
   ShiftToLayer(SYM)),

  [SYM] = KEYMAP_STACKED
  (XXX,            Key_LEDEffectNext, ___,  ___, ___,   ___,            Key_CapsLock,
   M(MACRO_LBUP),  XXX,               XXX,  XXX, XXX,   XXX,            ___,
   M(MACRO_LBDN),  XXX,               XXX,  XXX, XXX,   XXX,
   ___,            XXX,               XXX,  XXX, XXX,   M(MACRO_UPDIR), ___,
   ___, ___, ___, ___,
   ___,

   M(MACRO_VERSION_INFO),  ___,    ___,    ___,   ___,    ___,    ___,
   ___,                    ___, Key_Backtick, CS(4), ___, XXX,    XXX,
                           K_Dolr, K_Exlm, K_At,  K_Hash, K_Perc, XXX,
   ___,                    K_Circ, K_Unds, CS(9), K_Tild, XXX,    XXX,
   ___, ___, ___, ___,
   ___),

  [NUM] =  KEYMAP_STACKED
  (XXX,      XXX,           XXX,          XXX,           XXX,            XXX, Key_CapsLock,
   ___,      XXX,           Key_PageDown, Key_Home,      Key_PageUp,     XXX, XXX,
   ___,      Key_LeftArrow, Key_UpArrow,  Key_DownArrow, Key_RightArrow, XXX, 
   ___,      XXX,           XXX,          Key_End,       XXX,            XXX, XXX, 
   ___, ___, ___, ___,
   ___,

   ___,      XXX,       XXX,    XXX,    XXX,      XXX,        ___,
   ___,      XXX,       Key_4,  Key_5,  Key_6,    Key_Slash,  XXX,
             Key_0,     Key_1,  Key_2,  Key_3,    K_Plus,     Key_Equals,
   ___,      K_Astr,    Key_7,  Key_8,  Key_9,    Key_Period, ___,
   ___, ___, ___, ___,
   ___), 

  [NAV] = KEYMAP_STACKED
  (XXX,     Key_F1, Key_F2,          Key_F3,                   Key_F4,               Key_F5, ___,
   XXX,     XXX,    Consumer_Rewind, Consumer_VolumeIncrement, Consumer_FastForward, XXX,    ___,
   XXX,     XXX,    Consumer_Play,   Consumer_VolumeDecrement, XXX,                  XXX,
   XXX,     XXX,    Consumer_Stop,   Consumer_Mute,            XXX,                  XXX,    ___,
   ___, ___, ___, ___,
   ___,

   ___, Key_F6,               Key_F7,              Key_F8,                      Key_F9,                     Key_F10,              XXX,
   ___, XXX,                  LGUI(Key_LeftArrow), Key_End,                     Key_Home,                   LGUI(Key_RightArrow), Key_PageUp,
        LCTRL(Key_LeftArrow), Key_LeftArrow,       Key_DownArrow,               Key_UpArrow,                Key_RightArrow,       LCTRL(Key_RightArrow),
   ___, XXX,                  LALT(Key_LeftArrow), LALT(LSHIFT(Key_DownArrow)), LALT(LSHIFT(Key_UpArrow)),  LALT(Key_RightArrow), Key_PageDown,
   ___, ___, ___, ___,
   ___),

  [MIR] = KEYMAP_STACKED
  (___,      Key_6, Key_7, Key_8,   Key_9,   Key_0, ___,
   ___,      Key_Z, Key_F, Key_U,   CS(2),   CS(3), OSM(RightShift),
   ___,      Key_P, Key_N, Key_E,   Key_A,   Key_O,
   ___,      Key_B, Key_H, TD(CLT), TD(PGT), Key_X, M(MACRO_BRACES),
   ___, ___, ___, ___,
   ___,

   ___,      ___, ___, ___, ___, ___, ___,
   ___,      ___, ___, ___, ___, ___, ___,
             ___, ___, ___, ___, ___, ___,
   ___,      ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___,
   ___),

  [TIL] = KEYMAP_STACKED
  (___,    ___,                ___,                     ___,                          ___,                       ___,                ___,
   ___,    Key_Y,              LCTRL(LALT(Key_U)),      LCTRL(LALT(Key_Home)),        Key_M,                     LCTRL(LALT(Key_I)), ___,
   ___,    LCTRL(LALT(Key_H)), LCTRL(LALT(Key_Delete)), LCTRL(LALT(LSHIFT(Key_End))), LCTRL(LALT(Key_PageDown)), LCTRL(LALT(Key_Y)), 
   ___,    Key_I,              LCTRL(LALT(Key_J)),      LCTRL(LALT(Key_F14)),         Key_D,                     LCTRL(LALT(Key_K)), ___,
   ___, ___, ___, ___,
   ___,

   ___,      ___, ___, ___, ___, ___, ___,
   ___,      ___, ___, ___, ___, ___, ___,
             ___, ___, ___, ___, ___, ___,
   ___,      ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___,
   ___)
) // KEYMAPS(


/* Re-enable astyle's indent enforcement */
// clang-format on

/** toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event)
{
    switch (event)
    {
    case kaleidoscope::plugin::HostPowerManagement::Suspend:
        LEDControl.disable();
        break;
    case kaleidoscope::plugin::HostPowerManagement::Resume:
        LEDControl.enable();
        break;
    case kaleidoscope::plugin::HostPowerManagement::Sleep:
        break;
    }
}

namespace kaleidoscope {
  namespace plugin {

    class ModBlocker : public Plugin {
      public:
        EventHandlerResult onAddToReport(Key key) {
          if (active_ && key.isKeyboardShift())
            return EventHandlerResult::ABORT;
          return EventHandlerResult::OK;
        }

        void enable() {
          active_ = true;
        }
        void disable() {
          active_ = false;
        }

      private:
        bool active_{false};
    };
  }
}

kaleidoscope::plugin::ModBlocker ModBlocker;


static void versionInfoMacro(uint8_t key_state) {
  if (keyToggledOn(key_state)) {
    Macros.type(PSTR("Keyboardio Model 100 - Firmware version "));
    Macros.type(PSTR(KALEIDOSCOPE_FIRMWARE_VERSION));
  }
}


const macro_t *macroAction(uint8_t macro_id, KeyEvent &event) {
  switch (macro_id) {

  case MACRO_VERSION_INFO:
    versionInfoMacro(event.state);
    break;

  case MACRO_BRACES:
    if (keyToggledOn(event.state)) {
      if (Kaleidoscope.hid().keyboard().wasModifierKeyActive(Key_LeftAlt)) {
        Macros.tap(K_LParen);
        Macros.tap(K_RParen);
      }
      else if (Kaleidoscope.hid().keyboard().wasModifierKeyActive(Key_LeftControl)) {
        Macros.tap(K_LT);
        Macros.tap(K_GT);
      }
      else {
        // this handles shift too
        Macros.tap(Key_LeftBracket);
        Macros.tap(Key_RightBracket);
      }
      ModBlocker.enable();
      Macros.tap(Key_LeftArrow);
      ModBlocker.disable();
      return MACRO_NONE;
    }
    break;
   
 case MACRO_RSOSM:
    if (keyToggledOn(event.state)) {
        event.key = OSM(RightShift);
    }
    break;

  case MACRO_LSOSM:
    if (keyToggledOn(event.state)) {
        event.key = OSM(LeftShift);
    }
    break;

  case MACRO_UPDIR:
    if (keyToggledOn(event.state)) {
      return Macros.type(PSTR("../"));
    }
    break;

  case MACRO_SHBANG:
    if (keyToggledOn(event.state)) {
      return Macros.type(PSTR("#!"));
    }
    break;

  case MACRO_LBDN:
    if (keyToggledOn(event.state)) {
      uint8_t brightness = LEDControl.getBrightness();
      if (brightness > 10)
        brightness -= 10;
      else
        brightness = 0;

      LEDControl.setBrightness(brightness);
    }
    break;

  case MACRO_LBUP:
    if (keyToggledOn(event.state)) {
      uint8_t brightness = LEDControl.getBrightness();
      if (brightness < 245)
        brightness += 10;
      else
        brightness = 255;

      LEDControl.setBrightness(brightness);
    }
    break;


  }
  return MACRO_NONE;
}


// These 'solid' color effect definitions define a rainbow of
// LED color modes calibrated to draw 500mA or less on the
// Keyboardio Model 100.


static kaleidoscope::plugin::LEDSolidColor solidRed(160, 0, 0);
static kaleidoscope::plugin::LEDSolidColor solidOrange(140, 70, 0);
static kaleidoscope::plugin::LEDSolidColor solidYellow(130, 100, 0);
static kaleidoscope::plugin::LEDSolidColor solidGreen(0, 160, 0);
static kaleidoscope::plugin::LEDSolidColor solidBlue(0, 70, 130);
static kaleidoscope::plugin::LEDSolidColor solidIndigo(0, 0, 170);
static kaleidoscope::plugin::LEDSolidColor solidViolet(130, 0, 120);


/** hostPowerManagementEventHandler dispatches power management events (suspend,
 * resume, and sleep) to other functions that perform action based on these
 * events.
 */
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}


/* These are the names of your magic combos. They will be used by the
 * `USE_MAGIC_COMBOS` call below.
 */
enum {
  // Toggle between Boot (6-key rollover; for BIOSes and early boot) and NKRO
  // mode.
  COMBO_TOGGLE_NKRO_MODE,
  // Enter test mode
  COMBO_ENTER_TEST_MODE
};

/** Wrappers, to be used by MagicCombo. **/

/**
 * This simply toggles the keyboard protocol via USBQuirks, and wraps it within
 * a function with an unused argument, to match what MagicCombo expects.
 */
static void toggleKeyboardProtocol(uint8_t combo_index) {
  USBQuirks.toggleKeyboardProtocol();
}

/**
 * Toggles between using the built-in keymap, and the EEPROM-stored one.
 */
static void toggleKeymapSource(uint8_t combo_index) {
  if (Layer.getKey == Layer.getKeyFromPROGMEM) {
    Layer.getKey = EEPROMKeymap.getKey;
  } else {
    Layer.getKey = Layer.getKeyFromPROGMEM;
  }
}

/**
 *  This enters the hardware test mode
 */
static void enterHardwareTestMode(uint8_t combo_index) {
  HardwareTestMode.runTests();
}


/** Magic combo list, a list of key combo and action pairs the firmware should
 * recognise.
 */
USE_MAGIC_COMBOS({.action = toggleKeyboardProtocol,
                  // Left Fn + Esc + Shift
                  .keys = {R3C6, R2C6, R3C7}},
                 {.action = enterHardwareTestMode,
                  // Left Fn + Prog + LED
                  .keys = {R3C6, R0C0, R0C6}},
                 {.action = toggleKeymapSource,
                  // Left Fn + Prog + Shift
                  .keys = {R3C6, R0C0, R3C7}});

// First, tell Kaleidoscope which plugins you want to use.
// The order can be important. For example, LED effects are
// added in the order they're listed here.
KALEIDOSCOPE_INIT_PLUGINS(

  EEPROMSettings,
  EEPROMKeymap,

  Focus,
  FocusSettingsCommand,
  FocusEEPROMCommand,

  BootGreetingEffect,
  HardwareTestMode,
  LEDControl,
  LEDOff,
  LEDRainbowEffect,
  LEDRainbowWaveEffect,
  LEDChaseEffect,
  LEDDigitalRainEffect,
  solidRed,
  solidOrange,
  solidYellow,
  solidGreen,
  solidBlue,
  solidIndigo,
  solidViolet,
  LEDBreatheEffect,
  AlphaSquareEffect,
  StalkerEffect,
  LEDPaletteTheme,
  ColormapEffect,
  Macros,
  HostPowerManagement,
  MagicCombo,
  USBQuirks,
  Qukeys,
  OneShot,
  EscapeOneShot,
  TapDance,
  IdleLEDs,
  PersistentIdleLEDs,
  FirmwareVersion,
  LayerNames,
  DefaultLEDModeConfig,
  CharShift,
  Chord
);


void setup() {

  Kaleidoscope.setup();

  BootGreetingEffect.hue = 85;

  AlphaSquare.color = CRGB(255, 0, 0);

  LEDRainbowEffect.brightness(255);
  LEDRainbowWaveEffect.brightness(255);

  HardwareTestMode.setActionKey(R3C6);

  StalkerEffect.variant = STALKER(BlazingTrail);

  EEPROMKeymap.setup(8);

  ColormapEffect.max_layers(8);

  Layer.move(EEPROMSettings.default_layer());

  LayerNames.reserve_storage(17 * 8);

  DefaultLEDModeConfig.activateLEDModeIfUnconfigured(&LEDOff);

  DynamicMacros.reserve_storage(48);

  TapDance.setTimeout(150);

  Chord. setTimeout(20);

  Qukeys.setHoldTimeout(150);
  Qukeys.setOverlapThreshold(70);
  Qukeys.setMinimumHoldTime(200);
  Qukeys.setMinimumPriorInterval(40);
  Qukeys.setMaxIntervalForTapRepeat(200);

  
    // DESIGN: Key Addresses for Qukeys Plugin
    //
    // r0c0 r0c1 r0c2 r0c3 r0c4 r0c5 r0c6
    // r1c0 r1c1 r1c2 r1c3 r1c4 r1c5 r1c6
    // r2c0 r2c1 r2c2 r2c3 r2c4 r2c5
    // r3c0 r3c1 r3c2 r3c3 r3c4 r3c5 r2c6
    // r0c7 r1c7 r2c7 r3c7
    // r3c6
    //
    // r0c9 r0c10 r0c11 r0c12 r0c13 r0c14 r0c15
    // r1c9 r1c10 r1c11 r1c12 r1c13 r1c14 r1c15
    //      r2c10 r2c11 r2c12 r2c13 r2c14 r2c15
    // r2c9 r3c10 r3c11 r3c12 r3c13 r3c14 r3c15
    // r3c8 r2c8 r1c8 r0c8
    // r3c9
    //

  QUKEYS(
    kaleidoscope::plugin::Qukey(0, KeyAddr(0, 7), Key_Meh),                // R/cmd
    kaleidoscope::plugin::Qukey(0, KeyAddr(1, 7), Key_LeftGui),            // R/cmd
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 7), Key_LeftControl),        // R/cmd
    kaleidoscope::plugin::Qukey(0, KeyAddr(1, 8), ShiftToLayer(3)),
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 0), ShiftToLayer(TIL)),      // R/cmd
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 1), Key_LeftShift),          // R/cmd
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 3), Key_LeftAlt),            // N/ctrl
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 2), ShiftToLayer(1)),        // N/ctrl
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 4), ShiftToLayer(2)),        // N/ctrl
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 11), Key_RightGui),          // A/shift
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 12), Key_RightAlt),          // E/ctrl
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 13), Key_RightControl),      // I/alt
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 14), Key_RightShift),        // H/cmd
    kaleidoscope::plugin::Qukey(0, KeyAddr(3, 6), Key_Hyper),              // LeftPalm/Hyper
    kaleidoscope::plugin::Qukey(0, KeyAddr(3, 7), LGUI(Key_LeftControl)),          
  )       


  CS_KEYS(
    kaleidoscope::plugin::CharShift::KeyPair(Key_Semicolon, K_Colon),  // 0
    kaleidoscope::plugin::CharShift::KeyPair(Key_Period, K_Ampr),      // 1
    kaleidoscope::plugin::CharShift::KeyPair(Key_Slash, K_Astr),       // 2
    kaleidoscope::plugin::CharShift::KeyPair(K_DQuot, K_Ques),         // 3
    kaleidoscope::plugin::CharShift::KeyPair(Key_Quote, K_Exlm),       // 4
    kaleidoscope::plugin::CharShift::KeyPair(Key_Minus, K_Plus),       // 5
    kaleidoscope::plugin::CharShift::KeyPair(Key_Comma, K_Pipe),       // 6
    kaleidoscope::plugin::CharShift::KeyPair(K_LParen, K_LT),          // 7
    kaleidoscope::plugin::CharShift::KeyPair(K_RParen, K_GT),          // 8
    kaleidoscope::plugin::CharShift::KeyPair(Key_Backslash, K_Pipe),   // 9
  );


  CHORDS(
    CHORD(Key_S, Key_R), Key_Escape,
    CHORD(Key_W, Key_D), LGUI(Key_C),
    CHORD(Key_D, Key_J), LGUI(Key_V),
    CHORD(Key_V, Key_W), LGUI(Key_X),
    CHORD(Key_Q, Key_V), LGUI(Key_Z),
    CHORD(Key_S, Key_L), LCTRL(LSHIFT(Key_Tab)),
    CHORD(Key_Q, Key_V, Key_W), LGUI(LSHIFT(Key_Z)),
    CHORD(Key_L, Key_T), LCTRL(Key_Tab),
    CHORD(Key_I, Key_S), LGUI(Key_Tab),
    CHORD(Key_Space, Key_W), LGUI(Key_Comma),
    CHORD(Key_Space, Key_V), LGUI(Key_H),
    CHORD(Key_Space, Key_J), LGUI(Key_O),
    CHORD(Key_Space, Key_D), LGUI(Key_Period),
    CHORD(Key_Space, Key_R), Key_DownArrow,
    CHORD(Key_Space, Key_S), Key_UpArrow,
    CHORD(Key_Space, Key_T), Key_RightArrow,
    CHORD(Key_Space, Key_I), Key_LeftArrow,
    CHORD(Key_Tab, Key_S), LCTRL(Key_UpArrow),
    CHORD(Key_Tab, Key_R), LCTRL(Key_DownArrow),
    CHORD(Key_H, TD(CLT)), TD(MUS),
    CHORD(Key_Space, Key_Backspace), LCTRL(LSHIFT(LGUI(Key_T))),
    CHORD(Key_R, Key_T), M(MACRO_LSOSM),
    CHORD(Key_N, Key_E), M(MACRO_RSOSM),
    CHORD(Key_R, Key_D), HYPER(Key_LeftArrow),
    CHORD(Key_R, Key_V), HYPER(Key_RightArrow)
  )
}

 
void loop() {
  Kaleidoscope.loop();
}
