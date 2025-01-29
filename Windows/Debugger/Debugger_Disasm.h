#pragma once

#include "Windows/W32Util/DialogManager.h"
#include "Windows/W32Util/TabControl.h"
#include "Windows/Debugger/Debugger_Lists.h"
#include "Core/Core.h"
#include "Core/MIPS/MIPSDebugInterface.h"
#include "Core/Debugger/Breakpoints.h"
#include <vector>

#include "Common/CommonWindows.h"

class CtrlDisAsmView;
class BreakpointManager;

class CDisasm : public Dialog {
private:
	int minWidth;
	int minHeight;
	MIPSDebugInterface *cpu;
	u64 lastTicks_;

	HWND statusBarWnd;
	CtrlBreakpointList* breakpointList;
	CtrlThreadList* threadList;
	CtrlStackTraceView* stackTraceView;
	CtrlModuleList* moduleList;
	CtrlWatchList *watchList_;
	TabControl* leftTabs;
	TabControl* bottomTabs;
	BreakpointManager *breakpoints_;
	std::vector<BreakPoint> displayedBreakPoints_;
	std::vector<MemCheck> displayedMemChecks_;
	bool keepStatusBarText = false;
	bool hideBottomTabs = false;
	bool deferredSymbolFill_ = false;

	BOOL DlgProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void UpdateSize(WORD width, WORD height);
	void SavePosition();
	void updateThreadLabel(bool clear);
	void step(CPUStepType stepType);
	void runToLine();

public:
	int index;

	CDisasm(HINSTANCE _hInstance, HWND _hParent, MIPSDebugInterface *cpu);
	~CDisasm();

	void Show(bool bShow, bool includeToTop = true) override;

	void Update() override {
		UpdateDialog();
		SetDebugMode(Core_IsStepping(), false);
		breakpointList->reloadBreakpoints();
	};
	void UpdateDialog();
	void SetDebugMode(bool _bDebug, bool switchPC);

	void Goto(u32 addr);
	void NotifyMapLoaded();

private:
	CtrlDisAsmView *DisAsmView();
	void ProcessUpdateDialog();

	bool updateDialogScheduled_ = false;
};
