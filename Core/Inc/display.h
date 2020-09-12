/*
 * display.h
 *
 *  Created on: Sep 7, 2020
 *      Author: Matt
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "common.h"

typedef enum{Locked, Delay}screen_t;
typedef enum{DelayTime, DelayWD}delay_screen_t;


typedef struct{
	screen_t cur_screen; 				//where are we overall
	delay_screen_t cur_delay_screen;	//where are we in delay

}display_state;

display_state state;

void init_display_ui(pedal_t* pedal);
void dumb_update_display(void);
void update_display(pedal_t * pedal, press_t* press);
void draw_delay_screen(void);
void draw_lock_screen(pedal_t* pedal);
void draw_effect_screen(pedal_t* pedal);
void draw_effect_edit_screen(pedal_t* pedal);
void draw_word_screen(char*);

//////////Effect extern functions /////////////////
extern void update_delay_time(update_t updt);
extern void update_delay_WD(update_t updt);



#endif /* INC_DISPLAY_H_ */
