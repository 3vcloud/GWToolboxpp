#include "HealthWindow.h"

#include <string>

#include "GWCA\APIMain.h"

#include "GWToolbox.h"
#include "Config.h"
#include "GuiUtils.h"

HealthWindow::HealthWindow() {

	Config& config = GWToolbox::instance().config();
	int x = config.IniReadLong(HealthWindow::IniSection(), HealthWindow::IniKeyX(), 400);
	int y = config.IniReadLong(HealthWindow::IniSection(), HealthWindow::IniKeyY(), 100);

	SetLocation(x, y);
	SetSize(Drawing::SizeI(WIDTH, HEIGHT));

	Drawing::Theme::ControlTheme theme = Application::InstancePtr()
		->GetTheme().GetControlColorTheme(HealthWindow::ThemeKey());
	SetBackColor(theme.BackColor);

	int offsetX = 2;
	int offsetY = 2;

	DragButton* label_shadow = new DragButton();
	label_shadow->SetText(L"Health");
	label_shadow->SetSize(WIDTH, LABEL_HEIGHT);
	label_shadow->SetLocation(1, 1);
	label_shadow->SetFont(GuiUtils::getTBFont(12.0f, true));
	label_shadow->SetBackColor(Drawing::Color::Empty());
	label_shadow->SetForeColor(Drawing::Color::Black());
	label_shadow->SetEnabled(false);
	AddControl(label_shadow);

	DragButton* label = new DragButton();
	label->SetText(L"Health");
	label->SetSize(WIDTH, LABEL_HEIGHT);
	label->SetLocation(0, 0);
	label->SetFont(GuiUtils::getTBFont(12.0f, true));
	label->SetBackColor(Drawing::Color::Empty());
	label->SetForeColor(theme.ForeColor);
	label->GetMouseUpEvent() += MouseUpEventHandler([this](Control*, MouseEventArgs) {
		SaveLocation();
	});
	AddControl(label);

	percent_shadow = new DragButton();
	percent_shadow->SetText(L"");
	percent_shadow->SetSize(WIDTH, PERCENT_HEIGHT);
	percent_shadow->SetLocation(offsetX, LABEL_HEIGHT + offsetY);
	percent_shadow->SetFont(GuiUtils::getTBFont(26.0f, true));
	percent_shadow->SetBackColor(Drawing::Color::Empty());
	percent_shadow->SetForeColor(Drawing::Color::Black());
	percent_shadow->SetEnabled(false);
	AddControl(percent_shadow);

	percent = new DragButton();
	percent->SetText(L"");
	percent->SetSize(WIDTH, PERCENT_HEIGHT);
	percent->SetLocation(0, LABEL_HEIGHT);
	percent->SetFont(GuiUtils::getTBFont(26.0f, true));
	percent->SetBackColor(Drawing::Color::Empty());
	percent->SetForeColor(theme.ForeColor);
	percent->GetMouseUpEvent() += MouseUpEventHandler([this](Control*, MouseEventArgs) {
		SaveLocation();
	});
	AddControl(percent);

	absolute_shadow = new DragButton();
	absolute_shadow->SetText(L"");
	absolute_shadow->SetSize(WIDTH, ABSOLUTE_HEIGHT);
	absolute_shadow->SetLocation(offsetX, LABEL_HEIGHT + PERCENT_HEIGHT + offsetY);
	absolute_shadow->SetFont(GuiUtils::getTBFont(16.0f, true));
	absolute_shadow->SetBackColor(Drawing::Color::Empty());
	absolute_shadow->SetForeColor(Drawing::Color::Black());
	absolute_shadow->SetEnabled(false);
	AddControl(absolute_shadow);

	absolute = new DragButton();
	absolute->SetText(L"");
	absolute->SetSize(WIDTH, ABSOLUTE_HEIGHT);
	absolute->SetLocation(0, LABEL_HEIGHT + PERCENT_HEIGHT);
	absolute->SetFont(GuiUtils::getTBFont(16.0f, true));
	absolute->SetBackColor(Drawing::Color::Empty());
	absolute->SetForeColor(theme.ForeColor);
	absolute->GetMouseUpEvent() += MouseUpEventHandler([this](Control*, MouseEventArgs) {
		SaveLocation();
	});
	AddControl(absolute);

	bool show = config.IniReadBool(HealthWindow::IniSection(), HealthWindow::IniKeyShow(), false);
	ShowWindow(show);

	SetFreeze(config.IniReadBool(MainWindow::IniSection(),
		MainWindow::IniKeyFreeze(), false));

	SetHideTarget(config.IniReadBool(MainWindow::IniSection(),
		MainWindow::IniKeyHideTarget(), false));

	std::shared_ptr<HealthWindow> self = std::shared_ptr<HealthWindow>(this);
	Form::Show(self);
}

void HealthWindow::SaveLocation() {
	CalculateAbsoluteLocation();
	int x = absoluteLocation_.X;
	int y = absoluteLocation_.Y;
	Config& config = GWToolbox::instance().config();
	config.IniWriteLong(HealthWindow::IniSection(), HealthWindow::IniKeyX(), x);
	config.IniWriteLong(HealthWindow::IniSection(), HealthWindow::IniKeyY(), y);
}

void HealthWindow::UpdateUI() {
	using namespace GWAPI::GW;
	using namespace std;

	if (!enabled) return;

	GWAPI::GWCA api;

	wstring s1;
	wstring s2;

	Agent* target = api().Agents().GetTarget();
	if (target && target->Type == 0xDB) {
		s1 = to_wstring(lroundf(target->HP * 100));
		s1 += L" %";
			
		if (target->MaxHP > 0) {
			s2 = to_wstring(lroundf(target->HP * target->MaxHP));
			s2 = s2 + L" / " + to_wstring(target->MaxHP);
		} else {
			s2 = L"- / -";
		}
		if (!isVisible_) ToolboxWindow::ShowWindow(true);
	} else {
		s1 = L"-";
		s2 = L"- / -";
		if (hide_target && isVisible_) ToolboxWindow::ShowWindow(false);
	}
	percent->SetText(s1);
	percent_shadow->SetText(s1);
	absolute->SetText(s2);
	absolute_shadow->SetText(s2);
}

void HealthWindow::ShowWindow(bool show) {
	ToolboxWindow::ShowWindow(show);
	enabled = show;
}
