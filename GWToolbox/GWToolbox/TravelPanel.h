#pragma once

#include <string>
#include <Windows.h>

#include <GWCA\GwConstants.h>
#include <OSHGui\OSHGui.hpp>

#include "ToolboxPanel.h"
#include "logger.h"

class TravelPanel : public ToolboxPanel {
public:
	TravelPanel();

	void BuildUI() override;
	void UpdateUI() override {};
	void MainRoutine() override {};

	void TravelFavorite(int fav_idx);

private:
	static const int BUTTON_HEIGHT = 25;
	const int n_outposts = 185;

	class TravelCombo : public OSHGui::ComboBox {
	public:
		TravelCombo();
	};

	GwConstants::District district_;
	int district_number_;

	void AddTravelButton(std::wstring text, int grid_x, int grid_y, 
		GwConstants::MapID map_id);
	void UpdateDistrict(int gui_index);
	std::wstring IndexToOutpostName(int index);
	GwConstants::MapID IndexToOutpostID(int index);

	OSHGui::ComboBox* combo_boxes_[3];
};