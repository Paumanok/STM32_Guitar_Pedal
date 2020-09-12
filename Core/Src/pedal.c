/*
 * pedal.c
 *
 *  Created on: Aug 30, 2020
 *      Author: Matthew Smith
 */

#include "pedal.h"

void init_effects(void){
	init_iir();
	init_delay();
}

pedal_t* init_pedal(void){
	init_iir(); //no effect profile, always on;

	pedal_t* p = (pedal_t*)malloc(sizeof(pedal_t));
	p->n_effects = NUM_EFFECTS;
	p->cur_effect = 0;
	p->effects[0] = init_delay();
	p->effects[1] = init_dummy("dummy1");
	p->effects[2] = init_dummy("dummy2");
	return p;
}



/////////////////////// IIR ///////////////////////////
IIR iir;

float iir_coeffs [5] = {
				0.003916123487156427f,
				0.007832246974312854f,
				0.003916123487156427f,
				1.8153396116625289f,
				-0.8310041056111546
};

void init_iir(void){
	arm_biquad_cascade_df1_init_f32 ( &(iir.iirsettings), 1, &iir_coeffs[0], &(iir.iir_state[0]));
	return;
}

void do_iir(uint16_t* input, uint16_t* output){
	float f_in = (float)*input;
	float f_out;
	arm_biquad_cascade_df1_f32 (&(iir.iirsettings), &f_in, &f_out, BLOCK_SIZE_FLOAT);
	*output = (uint16_t)f_out;
	return;
}


/////////////////////////// DELAY /////////////////////////////

delay_conf delay;
//returns effect_t which describes effect, but also inits delay conf for internal use
effect_t* init_delay(void){
	delay.delay_time = DEFAULT_DELAY_TIME;
	delay.delay_offset = DEFAULT_DELAY_OFFSET;
	delay.dry_wet_ratio = DEFAULT_DRY_WET_RATIO;

	for(int i=0; i < MAX_DELAY_LEN; i++){
		delay.delay_buf[i] = 0;
	}

	effect_t* e = (effect_t*)malloc(sizeof(effect_t));
	strcpy(e->name,"Delay" );
	e->n_settings = 2;
	e->cur_setting = delta_t;
	strcpy(e->setting_name[0], "Delta t");
	strcpy(e->setting_name[1], "Wet/Dry Mix");
	e->locked = TRUE;
	e->enabled = TRUE;
	e->edit_mode = FALSE;
	e->do_effect = do_delay;
	e->update_effect = update_delay;
	e->get_setting_val = get_delay_setting_value;
	e->effect_state = &delay;
	return e;

}

void do_delay(uint16_t* input, uint16_t* output){

	if((delay.delay_offset % delay.dry_wet_ratio) == 0){
		*output = *input;
	}else{
		*output = delay.delay_buf[delay.delay_offset];
	}

	delay.delay_buf[delay.delay_offset] = *input;

	if(delay.delay_offset<(DELAY_INC_LEN*delay.delay_time)){
		delay.delay_offset++;
	}else{
		delay.delay_offset = 0;
	}

}

void update_delay(update_t updt, int setting){
	if(setting == delta_t){

		update_delay_time(updt);

	}else if(setting == wd_mix){

		 update_delay_WD(updt);
	}
}

int get_delay_setting_value(int setting){
	if(setting == delta_t){
		return delay.delay_time;
	}else{
		return delay.dry_wet_ratio;
	}
}

void update_delay_time(update_t updt){
	if(updt == Dec){
		if(delay.delay_time > 0) delay.delay_time--;
	}
	if(updt == Inc){
		if(delay.delay_time < (MAX_DELAY_LEN/DELAY_INC_LEN)) delay.delay_time++;
	}
}

void update_delay_WD(update_t updt){
	if(updt == Dec){
		if(delay.dry_wet_ratio > 1) delay.dry_wet_ratio--;
	}
	if(updt == Inc){
		if(delay.dry_wet_ratio < 10) delay.dry_wet_ratio++;
	}
}


/////////// test functions /////////
effect_t* init_dummy(char* name){
	effect_t* e = (effect_t*)malloc(sizeof(effect_t));
	strcpy(e->name, name);
	e->n_settings = 3;
	strcpy(e->setting_name[0],"setting 1");
	strcpy(e->setting_name[1],"setting 2");
	strcpy(e->setting_name[2],"setting 3");
	e->locked = TRUE;
	e->enabled = TRUE;
	e->edit_mode = FALSE;
	e->do_effect = do_dummy;
	return e;
}

void do_dummy(uint16_t* input, uint16_t* output){
	//do nothing
}
