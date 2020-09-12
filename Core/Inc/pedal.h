/*
 * pedal.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Matt
 */
#include "stm32f407xx.h"
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common.h"

#define NUM_EFFECTS 1

#define BLOCK_SIZE_FLOAT 1

#define DELAY_INC_LEN 1000
#define MAX_DELAY_LEN 24000
#define DEFAULT_DELAY_TIME 20 //DELAY_TIME*INC_LEN / MAX_DELAY_LEN
#define DEFAULT_DELAY_OFFSET 0
#define DEFAULT_DRY_WET_RATIO 2

typedef struct{
	int delay_time;
	int delay_offset;
	int dry_wet_ratio;
	uint16_t delay_buf[MAX_DELAY_LEN];
}delay_conf;


typedef struct{
	float buf_in [BLOCK_SIZE_FLOAT];
	float buf_out [BLOCK_SIZE_FLOAT];
	float iir_state [4];
	arm_biquad_casd_df1_inst_f32 iirsettings;
}IIR;


void init_effects(void);
pedal_t* init_pedal(void);

void init_iir(void);
void do_iir(uint16_t* input, uint16_t* output);

typedef enum{delta_t, wd_mix} delay_settings_t;
effect_t* init_delay(void);
void do_delay(uint16_t* input, uint16_t* output);
void update_delay(update_t updt, int setting);
void update_delay_time(update_t updt);
void update_delay_WD(update_t updt);
int get_delay_setting_value(int setting);


effect_t* init_dummy(char*);
void do_dummy(uint16_t*, uint16_t*);
