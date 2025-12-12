#include "sounds.h"

#define WAIT_US_AUDIO 120000
//defines the frequency arrays and beat arrays for all sounds
float correct_freq[] = {800, 1000};
float correct_beat[] = {2,4};

float wrong_freq[] = {500, 500};
float wrong_beat[] = {2,4};

float accepted_freq[] = {100, 100};
float accepted_beat[] = {2,4};

float intro_song_freq[] = {466,466,415,415,698,698,622};
float intro_song_beat[] = {1,1,1,1,3,3,6};
//plays the correct sound
void play_correct(){
    for(int i = 0; i < 2; i++){
        buzzer.period(1.0f/(1.5f * correct_freq[i]));
        buzzer = 0.5f;
        wait_us(correct_beat[i] * WAIT_US_AUDIO * 0.30f);
    }
    buzzer = 0.0f;
}
//plays the "wrong" sound effect
void play_wrong(){
    for(int i = 0; i < 2; i++){
        buzzer.period(1.0f/(1.5f * wrong_freq[i]));
        buzzer = 0.5f;
        wait_us(wrong_beat[i] * WAIT_US_AUDIO * 0.30f);
        buzzer = 0.0f;
    }
}
//plays the "input" sound effect
void play_accepted(){
    for(int i = 0; i < 2; i++){
        buzzer.period(1.0f/(1.5f * accepted_freq[i]));
        buzzer = 0.5f;
        wait_us(accepted_beat[i] * WAIT_US_AUDIO * 0.30f); 
        buzzer = 0.0f;
    }
}
//plays the intro music (get rickrolled lol)
void play_rickroll(){
    for(int i = 0; i < 7; i++){
        buzzer.period(1.0f / (1.5f * intro_song_freq[i]));
        buzzer = 0.5f;
        wait_us(intro_song_beat[i] * WAIT_US_AUDIO);
        buzzer = 0.0f;
        wait_us(intro_song_beat[i] * WAIT_US_AUDIO * 0.30f);
    }
}

