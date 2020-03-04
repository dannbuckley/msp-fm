/*
 * tea5767.c
 *
 * Based on TEA5767 library for Arduino written by Marcos R. Oliveira.
 *
 *  Created on: Mar 3, 2020
 *      Author: Daniel Buckley
 */

#include <msp430.h>
#include "tea5767.h"
#include "i2c.h"

uint8_t transmission_data[5] = {0, 0, 0, 0, 0};

uint8_t reception_data[5] = {0, 0, 0, 0, 0};

void set_frequency(float freq) {
    frequency = freq;
    unsigned int freqW;

    if (hiInjection) {
        set_high_LO_injection();
        freqW = ((frequency * 1000000) + 225000) / 8192;
    } else {
        set_low_LO_injection();
        freqW = ((frequency * 1000000) - 225000) / 8192;
    }

    transmission_data[FIRST_DATA] = ((transmission_data[FIRST_DATA] & 0xC0) | ((freqW >> 8) & 0x3F));
    transmission_data[SECOND_DATA] = freqW & 0xFF;
}

void transmit_data(void) {
    // transmit 5 bytes to radio with 100 ms delay after all data is transferred
    // I2C_Master_WriteReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
    uint8_t temp_transmit[4] = {transmission_data[1], transmission_data[2], transmission_data[3], transmission_data[4]};
    I2C_Master_WriteReg(TEA5767_I2C_ADDRESS, transmission_data[0], temp_transmit, 4);
    __delay_cycles(1600);
}

void init_transmission_data(void) {
    transmission_data[FIRST_DATA] = 0;
    transmission_data[SECOND_DATA] = 0;
    transmission_data[THIRD_DATA] = 0xB0;
    transmission_data[FOURTH_DATA] = 0x10;
    transmission_data[FIFTH_DATA] = 0;
}

void read_status(void) {
    // read 5 bytes from radio with 100 ms delay after all data is received
    // I2C_Master_ReadReg(uint8_t dev_addr, uint8_t reg_addr, uint8_t count)
    I2C_Master_ReadReg(TEA5767_I2C_ADDRESS, 0, 5);
    CopyArray(ReceiveBuffer, reception_data, 5);
    __delay_cycles(1600);
}

float get_freq_in_mhz(unsigned int freqW) {
    if (hiInjection) {
        return ((freqW * 8192.0) - 225000.0) / 1000000.0;
    } else {
        return ((freqW * 8192.0) + 225000.0) / 1000000.0;
    }
}

void calculate_opt_LO_injection(float freq) {
    unsigned char signalHigh;
    unsigned char signalLow;

    set_high_LO_injection();
    set_frequency((float)(freq + 0.45));
    signalHigh = get_signal_level();

    set_low_LO_injection();
    set_frequency((float)(freq - 0.45));
    signalLow = get_signal_level();

    hiInjection = (signalHigh < signalLow) ? 1 : 0;
}

void set_high_LO_injection(void) {
    transmission_data[THIRD_DATA] |= BIT4;
}

void set_low_LO_injection(void) {
    transmission_data[THIRD_DATA] &= ~(BIT4);
}

void load_freq(void) {
    read_status();

    transmission_data[FIRST_DATA] = (transmission_data[FIRST_DATA] & 0xC0) | (reception_data[FIRST_DATA] & 0x3F);
    transmission_data[SECOND_DATA] = reception_data[SECOND_DATA];
}

unsigned char is_ready(void) {
    read_status();
    return reception_data[FIRST_DATA] >> 7;
}

unsigned char is_band_limit_reached(void) {
    read_status();
    return (reception_data[FIRST_DATA] >> 6) & 1;
}

void init_TEA5767N(void) {
    initClockTo16MHz();
    initGPIO();
    initI2C();
    init_transmission_data();
    radio_settings = 0;
}

void select_freq(float freq, bool mute_during_update) {
    if (mute_during_update) {
        radio_settings &= ~(RADIO_MUTED);
        toggle_mute();
    }

    calculate_opt_LO_injection(freq);
    set_frequency(freq);

    if (mute_during_update) {
        toggle_mute();
    }
}

void select_channel(int channel, bool mute_during_update) {
    if (mute_during_update) {
        radio_settings &= ~(RADIO_MUTED);
        toggle_mute();
    }

    float freq = 87.9 + ((channel - 200) * 0.2);
    select_freq(freq, false);

    if (mute_during_update) {
        toggle_mute();
    }
}

float read_freq_in_mhz(void) {
    load_freq();

    unsigned int freqW = (((reception_data[FIRST_DATA] & 0x3F) * 256) + reception_data[SECOND_DATA]);
    return get_freq_in_mhz(freqW);
}

void set_search_up(void) {
    transmission_data[THIRD_DATA] |= BIT7;
}

void set_search_down(void) {
    transmission_data[THIRD_DATA] &= ~(BIT7);
}

void set_search_stop_level_low(void) {
    transmission_data[THIRD_DATA] &= ~(BIT5 + BIT6);
    transmission_data[THIRD_DATA] |= (LOW_STOP_LEVEL << 5);
}

void set_search_stop_level_mid(void) {
    transmission_data[THIRD_DATA] &= ~(BIT5 + BIT6);
    transmission_data[THIRD_DATA] |= (MID_STOP_LEVEL << 5);
}

void set_search_stop_level_high(void) {
    transmission_data[THIRD_DATA] &= ~(BIT5 + BIT6);
    transmission_data[THIRD_DATA] |= (HIGH_STOP_LEVEL << 5);
}

void toggle_mute(void) {
    radio_settings ^= RADIO_MUTED;

    if (radio_settings & RADIO_MUTED) {
        transmission_data[FIRST_DATA] |= BIT7;
    } else {
        transmission_data[FIRST_DATA] &= ~(BIT7);
    }

    transmit_data();
}

void toggle_mute_left(void) {
    radio_settings ^= RADIO_MUTED_LEFT;

    if (radio_settings & RADIO_MUTED_LEFT) {
        transmission_data[THIRD_DATA] |= BIT1;
    } else {
        transmission_data[THIRD_DATA] &= ~(BIT1);
    }

    transmit_data();
}

void toggle_mute_right(void) {
    radio_settings ^= RADIO_MUTED_RIGHT;

    if (radio_settings & RADIO_MUTED_RIGHT) {
        transmission_data[THIRD_DATA] |= BIT2;
    } else {
        transmission_data[THIRD_DATA] &= ~(BIT2);
    }

    transmit_data();
}

void toggle_soft_mute(void) {
    radio_settings ^= RADIO_SOFT_MUTED;

    if (radio_settings & RADIO_SOFT_MUTED) {
        transmission_data[FOURTH_DATA] |= BIT3;
    } else {
        transmission_data[FOURTH_DATA] &= ~(BIT3);
    }

    transmit_data();
}

void toggle_standby(void) {
    radio_settings ^= RADIO_STANDBY;

    if (radio_settings & RADIO_STANDBY) {
        transmission_data[FOURTH_DATA] |= BIT6;
    } else {
        transmission_data[FOURTH_DATA] &= ~(BIT6);
    }

    transmit_data();
}

void toggle_high_cut_control(void) {
    radio_settings ^= RADIO_HIGH_CUT_CONTROL;

    if (radio_settings & RADIO_HIGH_CUT_CONTROL) {
        transmission_data[FOURTH_DATA] |= BIT2;
    } else {
        transmission_data[FOURTH_DATA] &= ~(BIT2);
    }

    transmit_data();
}

void toggle_stereo_noise_cancelling(void) {
    radio_settings ^= RADIO_STEREO_NOISE_CANCELLING;

    if (radio_settings & RADIO_STEREO_NOISE_CANCELLING) {
        transmission_data[FOURTH_DATA] |= BIT1;
    } else {
        transmission_data[FOURTH_DATA] &= ~(BIT1);
    }

    transmit_data();
}

void toggle_forced_mono(void) {
    radio_settings ^= RADIO_FORCED_MONO;

    if (radio_settings & RADIO_FORCED_MONO) {
        transmission_data[THIRD_DATA] |= BIT3;
    } else {
        transmission_data[THIRD_DATA] &= ~(BIT3);
    }

    transmit_data();
}

unsigned char search_next(bool mute_during_search) {
    unsigned char bandLimitReached;

    if (mute_during_search) {
        radio_settings &= ~(RADIO_MUTED);
        toggle_mute();
    }

    if (is_search_up()) {
        select_freq(read_freq_in_mhz() + 0.1, false);
    } else {
        select_freq(read_freq_in_mhz() - 0.1, false);
    }

    // turn on search mode
    transmission_data[FIRST_DATA] |= BIT6;
    transmit_data();

    while (!is_ready()) {}

    bandLimitReached = is_band_limit_reached();
    load_freq();

    // turn off search mode
    transmission_data[FIRST_DATA] &= ~(BIT6);
    transmit_data();

    if (mute_during_search) {
        toggle_mute();
    }

    return bandLimitReached;
}

unsigned char search_from(float freq, bool mute_during_search) {
    select_freq(freq, false);
    return search_next(mute_during_search);
}

unsigned char search_from_beginning(bool mute_during_search) {
    unsigned char bandLimitReached;
    set_search_up();

    if (mute_during_search) {
        radio_settings &= ~(RADIO_MUTED);
        toggle_mute();
    }

    bandLimitReached = search_from(87.0, false);

    if (mute_during_search) {
        toggle_mute();
    }

    return bandLimitReached;
}

unsigned char search_from_end(bool mute_during_search) {
    unsigned char bandLimitReached;
    set_search_down();

    if (mute_during_search) {
        radio_settings &= ~(RADIO_MUTED);
        toggle_mute();
    }

    bandLimitReached = search_from(108.0, false);

    if (mute_during_search) {
        toggle_mute();
    }

    return bandLimitReached;
}

unsigned char get_signal_level(void) {
    transmit_data();
    read_status();
    return reception_data[FOURTH_DATA] >> 4;
}

unsigned char is_stereo(void) {
    read_status();
    return reception_data[THIRD_DATA] >> 7;
}

unsigned char is_search_up(void) {
    return (transmission_data[THIRD_DATA] & BIT7) != 0;
}

unsigned char is_search_down(void) {
    return (transmission_data[THIRD_DATA] & BIT7) == 0;
}

bool is_muted(void) {
    return radio_settings & RADIO_MUTED;
}

bool is_muted_left(void) {
    return radio_settings & RADIO_MUTED_LEFT;
}

bool is_muted_right(void) {
    return radio_settings & RADIO_MUTED_RIGHT;
}

bool is_soft_muted(void) {
    return radio_settings & RADIO_SOFT_MUTED;
}

bool is_standby(void) {
    read_status();
    return (transmission_data[FOURTH_DATA] & BIT6) != 0;
}
