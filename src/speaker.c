
#include "stm32f0xx.h"
#include <stdint.h>

#include "midi.h"
#include "midiplay.h"

// The number of simultaneous voices to support.
#define VOICES 15

// An array of "voices".  Each voice can be used to play a different note.
// Each voice can be associated with a channel (explained later).
// Each voice has a step size and an offset into the wave table.
struct {
    uint8_t in_use;
    uint8_t note;
    uint8_t chan;
    uint8_t volume;
    int     step;
    int     offset;
} voice[VOICES];

// We'll use the Timer 6 IRQ to recompute samples and feed those
// samples into the DAC.

void makeVoiceZero(){
    for(int i =0; i<VOICES;i++){
        voice[i].in_use =  0;
        voice[i].note =  0;
        voice[i].chan =  0;
        voice[i].volume=  0;
        voice[i].step  =  0;
        voice[i].offset =  0;
    }
}
void TIM15_IRQHandler(void)
{
    // TODO: Remember to acknowledge the interrupt right here.
    TIM15->SR &= ~1; //interrupt acknowledged.


    int sample = 0;
    for(int x=0; x < sizeof voice / sizeof voice[0]; x++) {
        if (voice[x].in_use) {
            voice[x].offset += voice[x].step;
            if (voice[x].offset >= N<<16)
                voice[x].offset -= N<<16;
            sample += (wavetable[voice[x].offset>>16] * voice[x].volume) >> 4;
        }
    }
    sample = (sample >> 10) + 2048;
    if (sample > 4095)
        sample = 4095;
    else if (sample < 0)
        sample = 0;
    DAC->DHR12R1 = sample;
}

// Initialize the DAC so that it can output analog samples
// on PA4.  Configure i==========
void init_dac(void)
{
    // TODO: you fill this in.

    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR |= 3<<3;
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_EN1;
}

// Initialize Timer 6 so that it calls TIM6_DAC_IRQHandler
// at exactly RATE times per second.  You'll need to select
// a PSC value and then do some math on the system clock rate
// to determine the value to set for ARR.  Set it to trigger
// the DAC by enabling the Update Tigger in the CR2 MMS field.
void init_tim15(void)
{
    // TODO: you fill this in.
      RCC->APB2ENR |= (1<<16);//clock
      TIM15->PSC = 0;
      TIM15->ARR = 48000000/RATE -1;
      TIM15->CR2 &= ~(7<<4);
      TIM15->CR2 |= (2<<4);
      TIM15->DIER |= 1;
      TIM15->CR1 |= 1;
      NVIC->ISER[0] = 1<<20;

}

// Find the voice current playing a note, and turn it off.
void note_off(int time, int chan, int key, int velo)
{
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].note == key) {
            voice[n].in_use = 0; // disable it first...
            voice[n].chan = 0;   // ...then clear its values
            voice[n].note = key;
            voice[n].step = step[key];
            return;
        }
    }
}

// Find an unused voice, and use it to play a note.
void note_on(int time, int chan, int key, int velo)
{
    if (velo == 0) {
        note_off(time, chan, key, velo);
        return;
    }
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use == 0) {
            voice[n].note = key;
            voice[n].step = step[key];
            voice[n].offset = 0;
            voice[n].volume = velo;
            voice[n].chan = chan;
            voice[n].in_use = 1;
            return;
        }
    }
}

void set_tempo(int time, int value, const MIDI_Header *hdr)
{
    // This assumes that the TIM2 prescaler divides by 48.
    // It sets the timer to produce an interrupt every N
    // microseconds, where N is the new tempo (value) divided by
    // the number of divisions per beat specified in the MIDI file header.
    TIM3->ARR = value/hdr->divisions - 1;
}

const float pitch_array[] = {
0.943874, 0.945580, 0.947288, 0.948999, 0.950714, 0.952432, 0.954152, 0.955876,
0.957603, 0.959333, 0.961067, 0.962803, 0.964542, 0.966285, 0.968031, 0.969780,
0.971532, 0.973287, 0.975046, 0.976807, 0.978572, 0.980340, 0.982111, 0.983886,
0.985663, 0.987444, 0.989228, 0.991015, 0.992806, 0.994599, 0.996396, 0.998197,
1.000000, 1.001807, 1.003617, 1.005430, 1.007246, 1.009066, 1.010889, 1.012716,
1.014545, 1.016378, 1.018215, 1.020054, 1.021897, 1.023743, 1.025593, 1.027446,
1.029302, 1.031162, 1.033025, 1.034891, 1.036761, 1.038634, 1.040511, 1.042390,
1.044274, 1.046160, 1.048051, 1.049944, 1.051841, 1.053741, 1.055645, 1.057552,
};

void pitch_wheel_change(int time, int chan, int value)
{
    //float multiplier = pow(STEP1, (value - 8192.0) / 8192.0);
    float multiplier = pitch_array[value >> 8];
    for(int n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].chan == chan) {
            voice[n].step = step[voice[n].note] * multiplier;
        }
    }
}



// Configure timer 2 so that it invokes the Update interrupt
// every n microseconds.  To do so, set the prescaler to divide
// by 48.  Then the CNT will count microseconds up to the ARR value.
// Basically ARR = n-1
// Set the ARPE bit in the CR1 so that the timer waits until the next
// update before changing the effective ARR value.
// Call NVIC_SetPriority() to set a low priority for Timer 2 interrupt.
// See the lab 6 text to understand how to do so.
void init_tim3(int n) {
    // TODO: you fill this in.
    RCC->APB1ENR |= (1<<1);//clock
    TIM3->PSC = 48-1;
    TIM3->ARR = n-1;
    TIM3->CR1 |= TIM_CR1_ARPE;

    NVIC_SetPriority(TIM3_IRQn,3);


    TIM3->DIER |= 1;
    TIM3->CR1 |= 1;
    NVIC->ISER[0] = 1<<16;
}


void TIM3_IRQHandler(void)
{
    // TODO: Remember to acknowledge the interrupt right here!
    TIM3->SR &= ~1;

    midi_play();
}

void startMusic(MIDI_Player * mp)
{
    init_wavetable_hybrid2();
    init_dac();
    init_tim15();
    mp = midi_init(midifile);
    // The default rate for a MIDI file is 2 beats per second
    // with 48 ticks per beat.  That's 500000/48 microseconds.
    init_tim3(10417);
//    for(;;) {
//        asm("wfi");
//        // If we hit the end of the MIDI file, start over.
//        if (mp->nexttick == MAXTICKS)
////            mp = midi_init(midifile);
//            break;
//    }
//    return mp;
}

void stopMusic(){
    TIM3->CR1 &= ~TIM_CR1_CEN;
    makeVoiceZero();
    //DAC->CR &= ~DAC_CR_EN1;
}
void startMusicAgain(MIDI_Player * id){
//    free(id);
    id = midi_init(midifile);
    return;
}
int checkMusicStatus(MIDI_Player * id){
    if (id->nexttick == MAXTICKS) return 1;

    return 0;
}
void unmuteMusic(MIDI_Player * id){
    //DAC->CR |= DAC_CR_EN1;

    id = midi_init(midifile);
    TIM3->CR1 |= TIM_CR1_CEN;
}
