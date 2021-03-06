#pragma once

#include <module_base.h>

class InstanceTimer : public TBModule {

public:	
	InstanceTimer() = default;
	 virtual ~InstanceTimer() = default;
	
	const char* Name() const override { return "Timer"; }

	void LoadSettings(CSimpleIni *ini) override;
	void SaveSettings(CSimpleIni *ini) override;
    void DrawSettings() override;

	// Draw user interface. Will be called every frame if the element is visible
	void Draw(IDirect3DDevice9* pDevice) override;

private:
    // those function write to extra_buffer and extra_color.
    // they return true if there is something to draw.
    bool GetUrgozTimer();
    bool GetDeepTimer();
    bool GetDhuumTimer();
    bool GetTrapTimer();

	bool click_to_print_time = false;
    bool show_extra_timers = false;

    char timer_buffer[32] = "";
    char extra_buffer[32] = "";
    ImColor extra_color = 0;
};
