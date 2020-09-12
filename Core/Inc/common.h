/*
 * common.h
 *
 *  Created on: Sep 7, 2020
 *      Author: Matt
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include <stdlib.h>
#include <string.h>


typedef enum{Dec, Inc} update_t;
typedef enum{FALSE,TRUE}bool_t;
typedef enum{None,A, B, Left, Right}press_t;

////////////////// QUEUE IMPLEMENTATION /////////////////////////
#define QUEUE_MAX 5 //keep it low to keep away super humans and osu players

typedef struct{
	int front;
	int rear;
	int n_items;
	press_t* items;

}queue_t;

queue_t* init_queue(void);
int size(queue_t*);
bool_t isEmpty(queue_t*);
int peak(queue_t*);
int enqueue(queue_t*, press_t);
press_t dequeue(queue_t*);

////////////////// END QUEUE IMPLEMENTATION ///////////////////////

#define MAX_N_SETTINGS 6
#define MAX_SETTING_NAME_LEN 15
#define MAX_EFFECTS 10

typedef void (*do_effect_fptr)(__uint16_t* input, __uint16_t* output);
typedef void (*update_effect_fptr)(update_t updt, int setting);
typedef int (*get_setting_val_fptr)(int setting);

typedef struct{
	char name[10];
	int n_settings;
	int cur_setting;
	char setting_name[MAX_N_SETTINGS][MAX_SETTING_NAME_LEN];
	bool_t locked;
	bool_t edit_mode;
	bool_t enabled;
	do_effect_fptr do_effect;
	update_effect_fptr update_effect;
	get_setting_val_fptr get_setting_val;
	void* effect_state;

}effect_t;

typedef struct{
	int n_effects;
	int cur_effect;
	effect_t* effects[MAX_EFFECTS];
}pedal_t;

extern void dumb_update_display(void);
extern void update_display(pedal_t* pedal, press_t* press);
extern void update_delay_time(update_t updt);
extern void update_delay_WD(update_t updt);
void update_pedal_state(void);



#endif /* INC_COMMON_H_ */
