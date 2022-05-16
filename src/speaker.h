#ifndef __SPEAKER_H
#define __SPEAKER_H
#include "stdlib.h"
#include "stm32f0xx.h"
#include "midi.h"
void startMusic(MIDI_Player * mp);
int checkMusicStatus(MIDI_Player * id);
void stopMusic(void);
void unmuteMusic(MIDI_Player * id);
void startMusicAgain(MIDI_Player * id);
void makeVoiceZero();
#endif
