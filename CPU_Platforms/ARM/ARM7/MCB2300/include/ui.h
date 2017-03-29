


#ifndef ui_ao_h
#define ui_ao_h

#include "baseao.h"


#include <stdlib.h>   

typedef struct ADEventTag {
	BaseEvt super;
	int adValue;
} ADEvent;


typedef struct UI_AOTag {
    QActive super;
	int ad_value;
	int wait_time;
} UI_AO;
    

enum UI_AO_Signals {
	BASE_TIME_SIG = Q_USER_SIG,
	BUTTON_DOWN,
	BUTTON_UP,
	AD_READY_SIG,
	AD_CHANGED
};

extern UI_AO  l_UI_AO;

void UI_AO_ctor(UI_AO *me);
															   
#endif
