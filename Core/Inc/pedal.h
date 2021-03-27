/*
 * pedal.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Matthew Smith
 */
#include "stm32f407xx.h"
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common.h"
#include "math.h"

#define NUM_EFFECTS 3

#define BLOCK_SIZE_FLOAT 1


/////// GENERAL
#define BACK_BUF_MAX_LEN 48000 //88KB ram
void init_effects(void);
pedal_t* init_pedal(void);
uint16_t back_buf[BACK_BUF_MAX_LEN];

///// DELAY
#define DELAY_INC_LEN 1000
#define MAX_DELAY_LEN BACK_BUF_MAX_LEN //88kB ram
#define DEFAULT_DELAY_TIME 20 //DELAY_TIME*INC_LEN / MAX_DELAY_LEN
#define DEFAULT_DELAY_OFFSET 0
#define DEFAULT_DRY_WET_RATIO 1
#define DEFAULT_NUM_LAYERS 3

typedef struct{
	int delay_time;
	int delay_offset;
	int dry_wet_ratio;
	int n_layers;
	uint16_t* delay_buf;
}delay_conf;

typedef enum{delta_t, wd_mix, n_layers} delay_settings_t;
effect_t* init_delay(void);
void do_delay(uint16_t* input, uint16_t* output);
void do_space_delay(uint16_t* input, uint16_t* output);
void update_delay(update_t updt, int setting);
//void update_delay_time(update_t updt);
//void update_delay_WD(update_t updt);
int get_delay_setting_value(int setting);


////// DIST
#define DEFAULT_THETA 5
#define MIN_THETA 1
#define MAX_THETA 10

typedef struct{
	int theta;
}at_dist_conf;

typedef enum{theta} at_dist_settings_t;
effect_t* init_at_distortion(void);
void do_at_dist(uint16_t* input, uint16_t* output);
void update_at_dist(update_t updt, int setting);
int get_at_dist_setting_value(int setting);


///////// IIR
typedef struct{
	float buf_in [BLOCK_SIZE_FLOAT];
	float buf_out [BLOCK_SIZE_FLOAT];
	float iir_state [4];
	arm_biquad_casd_df1_inst_f32 iirsettings;
}IIR;

void init_iir(void);
void do_iir(uint16_t* input, uint16_t* output);


////////// DUMMY
effect_t* init_dummy(char*);
void do_dummy(uint16_t*, uint16_t*);
