/*
 * tea5767.h
 *
 * Based on TEA5767 library for Arduino written by Marcos R. Oliveira.
 *
 *  Created on: Mar 3, 2020
 *      Author: Daniel Buckley
 */

#ifndef TEA5767_H_
#define TEA5767_H_

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define FIRST_DATA 0
#define SECOND_DATA 1
#define THIRD_DATA 2
#define FOURTH_DATA 3
#define FIFTH_DATA 4

#define LOW_STOP_LEVEL 1
#define MID_STOP_LEVEL 2
#define HIGH_STOP_LEVEL 3

#define HIGH_SIDE_INJECTION 1
#define LOW_SIDE_INJECTION 0

#define STEREO_ON 0
#define STEREO_OFF 1

#define MUTE_RIGHT_ON 1
#define MUTE_RIGHT_OFF 0
#define MUTE_LEFT_ON 1
#define MUTE_LEFT_OFF 0

#define SWP1_HIGH 1
#define SWP1_LOW 0
#define SWP2_HIGH 1
#define SWP2_LOW 0

#define STBY_ON 1
#define STBY_OFF 0

#define JAPANESE_FM_BAND 1
#define US_EUROPE_FM_BAND 0

#define SOFT_MUTE_ON 1
#define SOFT_MUTE_OFF 0

#define HIGH_CUT_CONTROL_ON 1
#define HIGH_CUT_CONTROL_OFF 0

#define STEREO_NOISE_CANCELLING_ON 1
#define STEREO_NOISE_CANCELLING_OFF 0

#define SEARCH_INDICATOR_ON 1
#define SEARCH_INDICATOR_OFF 0

float frequency;
unsigned char hiInjection;
unsigned char frequencyH;
unsigned char frequencyL;

extern uint8_t transmission_data[5];

extern uint8_t reception_data[5];

#define RADIO_MUTED                   BIT0 // bool muted;
#define RADIO_MUTED_LEFT              BIT1 // bool mutedLeft;
#define RADIO_MUTED_RIGHT             BIT2 // bool mutedRight;
#define RADIO_SOFT_MUTED              BIT3 // bool softMuted;
#define RADIO_HIGH_CUT_CONTROL        BIT4 // bool highCutControl;
#define RADIO_STEREO_NOISE_CANCELLING BIT5 // bool stereoNoiseCancelling;
#define RADIO_STANDBY                 BIT6 // bool standby;
#define RADIO_FORCED_MONO             BIT7 // bool forcedMono;
unsigned char radio_settings;

void set_frequency(float);
void transmit_data(void);
void init_transmission_data(void);
void read_status(void);
float get_freq_in_mhz(unsigned int);
void calculate_opt_LO_injection(float);
void set_high_LO_injection(void);
void set_low_LO_injection(void);
void load_freq(void);
unsigned char is_ready(void);
unsigned char is_band_limit_reached(void);

void init_TEA5767N(void);
void select_freq(float freq, bool mute_during_update);
void select_channel(int channel, bool mute_during_update);
float read_freq_in_mhz(void);

void set_search_up(void);
void set_search_down(void);
void set_search_stop_level_low(void);
void set_search_stop_level_mid(void);
void set_search_stop_level_high(void);

void toggle_mute(void);
void toggle_mute_left(void);
void toggle_mute_right(void);
void toggle_soft_mute(void);

void toggle_standby(void);
void toggle_high_cut_control(void);
void toggle_stereo_noise_cancelling(void);
void toggle_forced_mono(void);

unsigned char search_next(bool mute_during_search);
unsigned char search_from(float freq, bool mute_during_search);
unsigned char search_from_beginning(bool mute_during_search);
unsigned char search_from_end(bool mute_during_search);

unsigned char get_signal_level(void);

unsigned char is_stereo(void);
unsigned char is_search_up(void);
unsigned char is_search_down(void);

bool is_muted(void);
bool is_muted_left(void);
bool is_muted_right(void);
bool is_soft_muted(void);
bool is_standby(void);

#endif /* TEA5767_H_ */
