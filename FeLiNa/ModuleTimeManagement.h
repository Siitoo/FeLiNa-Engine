#ifndef _MODULE_TIME_MANAGEMENT_
#define _MODULE_TIME_MANAGEMENT_

#ifndef GAME_MODE
#include "Module.h"
#include "Application.h"
#include "Timer.h"

#define SCALAR_TIME 0.0F

class ModuleTimeManagement: public Module
{
public:

	ModuleTimeManagement(Application* app, bool start_enabled = true);
	~ModuleTimeManagement();

	bool Start();
	bool CleanUp();

	float ReadGameClock() const;
	float ReadRealTimeClock() const;

	void StartGameTime();
	
	void PlayGameTime();

	void PauseGameClock();

	void FinishUpdate();
	
	float Real_Time_Delta_time = 0.0f;

	bool tick_selected = false;

	bool tick_done = true;

	float time_multiplier = 1.0f;

private:

	double real_time = 0;
	double game_time = 0;
	bool game_clock_active = false;

	uint Frame_count = 0;
};



#endif




#endif
