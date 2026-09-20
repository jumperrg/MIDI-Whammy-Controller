#include <Arduino.h>
#include <MIDI.h>
#include <Bounce2.h>

#include "config.h"

const uint8_t footswitchPins[NUM_FOOTSWITCHES] = {
    FOOTSWITCH_1_PIN,
    FOOTSWITCH_2_PIN,
    FOOTSWITCH_3_PIN,
    FOOTSWITCH_4_PIN};

Bounce footswitches[NUM_FOOTSWITCHES];

// When each button's next auto-repeat press is due, while held.
unsigned long nextAutoScrollTime[NUM_FOOTSWITCHES] = {0, 0, 0, 0};

// Shared position for each footswitch pair, initialized to the low end of
// its range (1-indexed to match the numbers printed on the pedal).
int8_t groupAProgram = GROUP_A_PROGRAM_MIN;
int8_t groupBProgram = GROUP_B_PROGRAM_MIN;

enum ScrollDirection
{
  DIRECTION_UP,
  DIRECTION_DOWN
};

enum Group
{
  GROUP_NONE,
  GROUP_A,
  GROUP_B
};

// Which group was pressed last, so switching to the other group resends its
// remembered position instead of stepping it.
Group lastActiveGroup = GROUP_NONE;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// MIDI Program Change is sent 0-indexed on the wire, so we subtract 1 from
// the 1-indexed program number when actually sending.
//
// The current position is resent as-is (not stepped) only on the first press
// after switching to this group from the other one, so switching sides never
// skips a preset. Every other press - up or down - steps normally.
void scroll(int8_t *program, int8_t programMin, int8_t programMax,
            ScrollDirection direction, bool groupJustActivated)
{
  if (groupJustActivated)
  {
    MIDI.sendProgramChange(*program - 1, MIDI_CHANNEL);
    return;
  }

  if (direction == DIRECTION_UP)
  {
    (*program)++;
    if (*program > programMax)
    {
      *program = programMin;
    }
  }
  else
  {
    (*program)--;
    if (*program < programMin)
    {
      *program = programMax;
    }
  }
  MIDI.sendProgramChange(*program - 1, MIDI_CHANNEL);
}

void handleFootswitchPress(uint8_t index)
{
  Group group = (index <= 1) ? GROUP_A : GROUP_B;
  bool groupJustActivated = (group != lastActiveGroup);
  lastActiveGroup = group;

  switch (index)
  {
  case 0: // group A up
    scroll(&groupAProgram, GROUP_A_PROGRAM_MIN, GROUP_A_PROGRAM_MAX,
           DIRECTION_UP, groupJustActivated);
    break;
  case 1: // group A down
    scroll(&groupAProgram, GROUP_A_PROGRAM_MIN, GROUP_A_PROGRAM_MAX,
           DIRECTION_DOWN, groupJustActivated);
    break;
  case 2: // group B up
    scroll(&groupBProgram, GROUP_B_PROGRAM_MIN, GROUP_B_PROGRAM_MAX,
           DIRECTION_UP, groupJustActivated);
    break;
  case 3: // group B down
    scroll(&groupBProgram, GROUP_B_PROGRAM_MIN, GROUP_B_PROGRAM_MAX,
           DIRECTION_DOWN, groupJustActivated);
    break;
  }
}

void setup()
{
  for (uint8_t i = 0; i < NUM_FOOTSWITCHES; i++)
  {
    footswitches[i].attach(footswitchPins[i], INPUT_PULLUP);
    footswitches[i].interval(DEBOUNCE_MS);
  }

  Serial1.begin(MIDI_BAUD_RATE, SERIAL_8N1, MIDI_RX_PIN, MIDI_TX_PIN);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
  for (uint8_t i = 0; i < NUM_FOOTSWITCHES; i++)
  {
    footswitches[i].update();

    // Pull-ups: pressed = LOW (FALLING)
    if (footswitches[i].fell())
    {
      handleFootswitchPress(i);
      nextAutoScrollTime[i] = millis() + LONG_PRESS_MS;
    }
    else if (footswitches[i].read() == LOW && millis() >= nextAutoScrollTime[i])
    {
      handleFootswitchPress(i);
      nextAutoScrollTime[i] = millis() + AUTO_SCROLL_INTERVAL_MS;
    }
  }
}
