#include "mbed.h"

#define WAIT_US 120000   // Adjust tempo: smaller = faster, 120k ≈ normal
//Website : https://onlinetonegenerator.com/multiple-tone-generator.html

using namespace mbed;

PwmOut buzzer(p27);


float song_freq[] = {466,466,415,415,698,698,622};

float song_beat[] = {1,1,1,1,3,3,6};

float correct_freq[] = {800, 1000};
float correct_beat[] = {2,4};

float wrong_freq[] = {500, 500};
float wrong_beat[] = {2,4};

float accepted_freq[] = {100, 100};
float accepted_beat[] = {2,4};
 
int main() {
    while (1) {
        //RickRoll - Start
        for(int i = 0; i < 7; i++){
          buzzer.period(1.0f / (1.5f * song_freq[i]));
          buzzer = 0.5f;
          wait_us(song_beat[i] * WAIT_US);
          buzzer = 0.0f;
          wait_us(song_beat[i] * WAIT_US * 0.30f);
        }
        wait_us(3000000);

        //Correct
        for(int i = 0; i < 2; i++){
          buzzer.period(1.0f/(1.5f * correct_freq[i]));
          buzzer = 0.5f;
          wait_us(correct_beat[i] * WAIT_US * 0.30f);
        }

        buzzer = 0.0f;

        wait_us(3000000);

        //Wrong
        for(int i = 0; i < 2; i++){
          buzzer.period(1.0f/(1.5f * wrong_freq[i]));
          buzzer = 0.5f;
          wait_us(wrong_beat[i] * WAIT_US * 0.30f);
          buzzer = 0.0f;
          wait_us(50000);
        }

        wait_us(3000000);

        //Accepted_Input
        for(int i = 0; i < 2; i++){
          buzzer.period(1.0f/(1.5f * accepted_freq[i]));
          buzzer = 0.5f;
          wait_us(accepted_beat[i] * WAIT_US * 0.30f);
          buzzer = 0.0f;
          wait_us(50000);
        }
        wait_us(3000000);
    }
}
