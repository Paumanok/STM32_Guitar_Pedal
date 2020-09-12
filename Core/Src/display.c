/*
 * display.c
 *
 *  Created on: Sep 7, 2020
 *      Author: Matt
 */
#include "display.h"

  //ssd1306_Init();
  //HAL_Delay(3000);
  //ssd1306_TestFPS();
  //ssd1306_TestFonts();
  //ssd1306_TestAll();
  //ssd1306_Fill(Black);
  //ssd1306_SetCursor(2,0);
  //ssd1306_WriteString("AAAAA", Font_7x10, White );
  //ssd1306_UpdateScreen();
  //ssd1306_Fill(White);


void init_display_ui(pedal_t* pedal){
	state.cur_screen = Locked;
	state.cur_delay_screen = DelayTime;
	ssd1306_Init();
	HAL_Delay(1000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	draw_lock_screen(pedal);
}

void dumb_update_display(void){
	static int call_count = 0;
	/*switch(state.cur_screen){
		case Locked:
			draw_lock_screen();
			break;
		default:
			draw_lock_screen();
			break;
		call_count++;
	}
*/
	switch(call_count){
		case 0:
			draw_word_screen("0");
			break;
		case 1:
			draw_word_screen("1");
			break;
		case 2:
			draw_word_screen("2");
			break;
		case 3:
			draw_word_screen("3");
			break;
		case 4:
			draw_word_screen("4");
			break;
		default:
			call_count = 0;
	}
	call_count++;
}

void update_display(pedal_t * pedal, press_t* press){
	//while(!isEmpty(queue)){
		bool_t is_locked = pedal->effects[pedal->cur_effect]->locked;
		bool_t editing = pedal->effects[pedal->cur_effect]->edit_mode;
		//press_t press = dequeue(queue);

		switch(*press){
		case A:
			if(is_locked){
				pedal->effects[pedal->cur_effect]->locked = FALSE;
				draw_effect_screen(pedal);
			}else if(!editing){
				pedal->effects[pedal->cur_effect]->edit_mode = TRUE;
				draw_effect_edit_screen(pedal);
			}
			break;

		case B:
			if(!is_locked && editing){
				pedal->effects[pedal->cur_effect]->edit_mode = FALSE;
				draw_effect_screen(pedal);
			}else if(!is_locked && !editing){
				pedal->effects[pedal->cur_effect]->locked = TRUE;
				draw_lock_screen(pedal);

			}
			break;

		case Left:
			if(is_locked){
				if(pedal->cur_effect > 0){
					pedal->cur_effect--;
					draw_lock_screen(pedal);
				}
			}else if(!is_locked && !editing){
				if(pedal->effects[pedal->cur_effect]->cur_setting > 0){
					pedal->effects[pedal->cur_effect]->cur_setting--;
					draw_effect_screen(pedal);
				}
			}else if(!is_locked && editing){
				//update setting
				pedal->effects[pedal->cur_effect]->update_effect(Dec, pedal->effects[pedal->cur_effect]->cur_setting);
				//update display
				draw_effect_edit_screen(pedal);
			}
			break;

		case Right:
			if(is_locked){
				if(pedal->cur_effect < pedal->n_effects){
					pedal->cur_effect++;
					draw_lock_screen(pedal);
				}
			}else if(!is_locked && !editing){
				if(pedal->effects[pedal->cur_effect]->cur_setting < pedal->effects[pedal->cur_effect]->n_settings){
					pedal->effects[pedal->cur_effect]->cur_setting++;
					draw_effect_screen(pedal);
				}
			}else if(!is_locked && editing){
				//update setting
				pedal->effects[pedal->cur_effect]->update_effect(Inc, pedal->effects[pedal->cur_effect]->cur_setting);
				//update display
				draw_effect_edit_screen(pedal);
			}
			break;

		default:
			break;
		}
		*press = None;
	//}
}

void draw_lock_screen(pedal_t* pedal){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5,5);
	ssd1306_WriteString("Locked", Font_11x18, White);
	ssd1306_SetCursor(30, 5+18);
	ssd1306_WriteString(pedal->effects[pedal->cur_effect]->name, Font_11x18, White);
	ssd1306_UpdateScreen();
}

void draw_effect_screen(pedal_t* pedal){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5,5);
	//ssd1306_WriteString("Locked\n", Font_11x18, White);
	ssd1306_WriteString(pedal->effects[pedal->cur_effect]->name, Font_11x18, White);
	ssd1306_SetCursor(30, 5+18);
	int cur_setting =  pedal->effects[pedal->cur_effect]->cur_setting;
	ssd1306_WriteString(pedal->effects[pedal->cur_effect]->setting_name[cur_setting], Font_11x18, White);
	ssd1306_UpdateScreen();
}
//TODO something died in here, not sure what
void draw_effect_edit_screen(pedal_t* pedal){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(5,5);

	int cur_setting =  pedal->effects[pedal->cur_effect]->cur_setting;
	ssd1306_WriteString(pedal->effects[pedal->cur_effect]->setting_name[cur_setting], Font_11x18, White);
	ssd1306_SetCursor(30, 5+18);
	//need to convert int value to something printable
	char val_buf[10];
	itoa(pedal->effects[pedal->cur_effect]->get_setting_val(cur_setting), val_buf,10);
	ssd1306_WriteString( val_buf , Font_11x18, White);
	ssd1306_UpdateScreen();
}

void draw_word_screen(char* string){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(30,30);
	ssd1306_WriteString(string, Font_11x18, White);
	ssd1306_UpdateScreen();
}
