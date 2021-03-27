/*
 * pedal.c
 *
 *  Created on: Aug 30, 2020
 *      Author: Matthew Smith
 */

#include "pedal.h"

extern inline int circ_index(int last_index, int index, int max);

void init_effects(void){
	init_iir();
	init_delay();
}

pedal_t* init_pedal(void){
	//zero out common back buffer before anything.
	for(int i=0; i < BACK_BUF_MAX_LEN; i++){
			back_buf[i] = 0;
		}

	init_iir(); //no effect profile, always on;

	pedal_t* p = (pedal_t*)malloc(sizeof(pedal_t));
	p->n_effects = NUM_EFFECTS;
	p->cur_effect = 1;
	p->effects[0] = init_at_distortion();
	p->effects[1] = init_delay();
	p->effects[2] = init_dummy("dummy1");
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
	delay.n_layers = DEFAULT_NUM_LAYERS;
	delay.delay_buf = back_buf; //just link the ptr to the start of back_buf so code makes sense


	effect_t* e = (effect_t*)malloc(sizeof(effect_t));
	strcpy(e->name,"Delay" );
	e->n_settings = 3;
	e->cur_setting = delta_t;
	strcpy(e->setting_name[0], "Delta t");
	strcpy(e->setting_name[1], "Wet/Dry Mix");
	strcpy(e->setting_name[2], "Layers");
	e->locked = TRUE;
	e->enabled = TRUE;
	e->edit_mode = FALSE;
	e->do_effect = do_space_delay;
	e->update_effect = update_delay;
	e->get_setting_val = get_delay_setting_value;
	e->effect_state = &delay;
	return e;

}

void do_delay(uint16_t* input, uint16_t* output)
{
	//*output = (*input + (delay.delay_buf[delay.delay_offset]/delay.dry_wet_ratio))/2; //proper signal combo, probably need float
	uint32_t inter = *input;
	uint16_t curr_delay = 0;
	int layer_offset, adj_offset;
	for(int layer = 1; layer <= delay.n_layers; layer++)
	{
		layer_offset = -(layer * (delay.delay_time * DELAY_INC_LEN)); //0 at first delay
		adj_offset = circ_index(delay.delay_offset, layer_offset, MAX_DELAY_LEN);
		curr_delay = delay.delay_buf[adj_offset]/delay.dry_wet_ratio;
		inter += curr_delay;

	}
	*output = (uint16_t)(inter/delay.n_layers);

	delay.delay_buf[delay.delay_offset] = *input;

	if(delay.delay_offset< MAX_DELAY_LEN) //using the whole buffer now
	{
		delay.delay_offset++;
	}else{
		delay.delay_offset = 0;
	}
}

void do_space_delay(uint16_t* input, uint16_t* output){

	uint32_t inter = *input;
	uint16_t curr_delay = 0;
	int layer_offset, adj_offset;
	for(int layer = 0; layer < delay.n_layers; layer++)
	{
		layer_offset = delay.delay_offset + (layer * ((DELAY_INC_LEN*delay.delay_time) / delay.n_layers)); //get offset for current layer
		//layer_offset = (layer * (delay.delay_offset));
		adj_offset = circ_index(delay.delay_offset, layer_offset,(DELAY_INC_LEN*delay.delay_time)); //adjust for circular buffer
		curr_delay = delay.delay_buf[adj_offset]/delay.dry_wet_ratio; //fetch delay value at adjusted index
		inter = (inter + curr_delay); //add to current signal and adjust
	}

	*output = (uint16_t)(inter/(delay.n_layers));
//#define NO_LAYER
#ifdef NO_LAYER
	*output = (*input + (delay.delay_buf[delay.delay_offset]/delay.dry_wet_ratio))/2; //proper signal combo, probably need float
#endif

	delay.delay_buf[delay.delay_offset] = *input;

	if(delay.delay_offset<(DELAY_INC_LEN*delay.delay_time))
	{
		delay.delay_offset++;
	}else{
		delay.delay_offset = 0;
	}

}

void update_delay(update_t updt, int setting){
	if(setting == delta_t){
		if(updt == Dec){
			if(delay.delay_time > 0) delay.delay_time--;
		}
		if(updt == Inc){
			if(delay.delay_time < (MAX_DELAY_LEN/DELAY_INC_LEN)) delay.delay_time++;
		}
		//update_delay_time(updt);
	}else if(setting == wd_mix){
		if(updt == Dec){
			if(delay.dry_wet_ratio > 1) delay.dry_wet_ratio--;
		}
		if(updt == Inc){
			if(delay.dry_wet_ratio < 10) delay.dry_wet_ratio++;
		}
		 //update_delay_WD(updt);
	}else if(setting == n_layers){
		if(updt == Dec){
			if(delay.n_layers > 1) delay.n_layers--;
		}
		if(updt == Inc){
			if(delay.n_layers < 3) delay.n_layers++;
		}
		 //update_delay_WD(updt);
	}
}

int get_delay_setting_value(int setting){
	if(setting == delta_t){
		return delay.delay_time;
	}else if(setting == wd_mix){
		return delay.dry_wet_ratio;
	}else if(setting == n_layers){
		return delay.n_layers;
	}else{
		return 0;
	}
}
/*
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
*/
/////////////////// ARCTAN DISTORTION ////////////////////
at_dist_conf at_dist;

effect_t* init_at_distortion(void){
	at_dist.theta = DEFAULT_THETA;
	effect_t* e = (effect_t*)malloc(sizeof(effect_t));
	strcpy(e->name, "AT_dist");
	e->n_settings = 1;
	e->cur_setting = theta;
	strcpy(e->setting_name[0], "gain"); //not really gain, but theta
	e->locked = TRUE;
	e->enabled = FALSE;
	e->edit_mode = FALSE;
	e->do_effect = do_at_dist;
	e->update_effect = update_at_dist;
	e->get_setting_val = get_at_dist_setting_value;
	e->effect_state = &at_dist;
	return e;
}

void do_at_dist(uint16_t* input, uint16_t* output){
	float inner_val = (float)(at_dist.theta * *input);
	float result = ((2.0/M_PI) * atanf(inner_val));
	*output = (uint16_t)((float)*input * result);
}

void update_at_dist(update_t updt, int setting){
	if(setting == theta){
		if(updt == Dec && at_dist.theta > MIN_THETA){
			at_dist.theta--;
		}
		else if(updt == Inc && at_dist.theta < MAX_THETA){
			at_dist.theta++;
		}
	}
}

int get_at_dist_setting_value(int setting){
	if(setting == theta)
		return at_dist.theta;
	return 0;
}

/////////////////// END ARCTAN DISTORTION ////////////////

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
