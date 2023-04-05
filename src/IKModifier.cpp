/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Ha Thach (thach@tinyusb.org) for Adafruit Industries
 */

#include "IKModifier.h"
#include "Adafruit_IntelliKeys.h"

void IKModifier::SetState(int state) {
  IKSettings *pSettings = IKSettings::GetSettings();
  if (pSettings == NULL)
    return;

  int theCode = m_universalCode;
  if (m_universalCodeOverride != 0)
    theCode = m_universalCodeOverride;

  switch (pSettings->m_iShiftKeyAction) {
  case kSettingsShiftLatching:
    if (state == kModifierStateOff) {
      if (m_state == kModifierStateLatched)
        m_device->PostKey(theCode, IK_UP);
    } else if (state == kModifierStateLatched) {
      if (m_state == kModifierStateOff)
        m_device->PostKey(theCode, IK_DOWN);
    }
    break;

  case kSettingsShiftLocking:
    if (state == kModifierStateOff) {
      if (m_state != kModifierStateOff)
        m_device->PostKey(theCode, IK_UP);
    } else if (state == kModifierStateLatched) {
      if (m_state == kModifierStateOff)
        m_device->PostKey(theCode, IK_DOWN);
    } else if (state == kModifierStateLocked) {
      if (m_state == kModifierStateOff)
        m_device->PostKey(theCode, IK_DOWN);
    }
    break;

  case kSettingsShiftNoLatch:
    if (state == kModifierStateOff) {
      if (m_state != kModifierStateOff)
        m_device->PostKey(theCode, IK_UP);
    }
    break;

  default:
    break;
  }

  m_state = state;
}

void IKModifier::Execute(int code) {
  int theCode = m_universalCode;
  m_universalCodeOverride = 0;
  if (code != 0) {
    theCode = code;
    m_universalCodeOverride = theCode;
  }

  IKSettings *pSettings = IKSettings::GetSettings();
  if (pSettings == NULL)
    return;

  switch (pSettings->m_iShiftKeyAction) {
  case kSettingsShiftLatching:
    if (m_state == kModifierStateOff) {
      //  was off, turn on and put down
      m_device->PostKey(theCode, IK_DOWN);
      m_state = kModifierStateLatched;
    } else {
      //  was on, put up and turn off
      m_device->PostKey(theCode, IK_UP);
      m_state = kModifierStateOff;
    }
    break;

  case kSettingsShiftLocking:
    if (m_state == kModifierStateOff) {
      //  put key down, state = latched
      m_device->PostKey(theCode, IK_DOWN);
      m_state = kModifierStateLatched;
    } else if (m_state == kModifierStateLatched) {
      //  leave key alone, state = locked
      m_state = kModifierStateLocked;
    } else if (m_state == kModifierStateLocked) {
      //  put key up, state = off
      m_device->PostKey(theCode, IK_UP);
      m_state = kModifierStateOff;
    }
    break;

  case kSettingsShiftNoLatch:
    //  just put the key down/up, no change in state
    {
      int keyDownDelay = 5;
      m_device->PostKey(theCode, IK_DOWN, keyDownDelay);
      // m_device->PostKey(theCode,IK_UP,keyDownDelay);
      m_state = kModifierStateLatched;
    }
    break;

  default:
    break;
  }
}
