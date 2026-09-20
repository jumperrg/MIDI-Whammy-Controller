#pragma once

// ---------------------------------------------------------------------------
// Pin configuration
// ---------------------------------------------------------------------------

// UART pin for MIDI TX (MIDI RX is unused/unconnected here)
#define MIDI_TX_PIN 9
#define MIDI_RX_PIN -1

// Footswitch pins
#define FOOTSWITCH_1_PIN 6
#define FOOTSWITCH_2_PIN 7
#define FOOTSWITCH_3_PIN 5
#define FOOTSWITCH_4_PIN 8

#define NUM_FOOTSWITCHES 4

// ---------------------------------------------------------------------------
// MIDI configuration
// ---------------------------------------------------------------------------

#define MIDI_CHANNEL 1
#define MIDI_BAUD_RATE 31250

// Footswitches 1/2 share one position and scroll it up/down through
// Program Change 12-21, wrapping around.
#define GROUP_A_PROGRAM_MIN 12
#define GROUP_A_PROGRAM_MAX 21

// Footswitches 3/4 share one position and scroll it up/down through
// Program Change 1-11, wrapping around.
#define GROUP_B_PROGRAM_MIN 1
#define GROUP_B_PROGRAM_MAX 11

// ---------------------------------------------------------------------------
// Timing configuration
// ---------------------------------------------------------------------------

#define DEBOUNCE_MS 20

// Long-press auto-scroll: after holding a button this long, it starts
// repeating the press automatically, then keeps repeating at this interval.
#define LONG_PRESS_MS 600
#define AUTO_SCROLL_INTERVAL_MS 100
