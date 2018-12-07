
#ifndef CONSOLE_DESIGN
#define CONSOLE_DESIGN

#include "defs.h"

class Design_Tool {
private:

	/* ui's design */
	vector<ui_type *> UI; // button or message

	/* basic text size */
	_2D_pos pixel_size;
	unsigned int text_size;
	double ratio_x, ratio_y;
	_2D_pos cmd_size;
	COORD size;

	/* for comfort */
	int num_button;
	int num_message;

	/* Basic function */
	int Assign_id(ui_type * new_ui);
	short Mouse_to_Console_x(short x_pos);
	short Mouse_to_Console_y(short y_pos);
	_2D_pos GetFontSize(HANDLE windowHandle); // Get basic text size
	bool SetFontSize(HANDLE windowHandle, unsigned int size); // Set basic text size
	void SetColor(Color colFront, Color colBack);
	void gotoxy(short x, short y);
	void Clean_Console();
	short wherex();
	short wherey();
	void enter_gotoxy(short x);
	void space_gotoxy(short y, short space);
	void Print_Message(wstring & str, short start_pos, short end_pos, int type);
	bool valueInRange(int value, int min, int max);
	int getkey();
	int rounder(double num);

	/* Condition Checker */
	bool CMD_Condition(ui_type * console_info);
	bool Button_Condition(ui_type * button_info);
	bool Message_Condition(ui_type * message_info);

	/* Convenient function */
	_2D_pos String_size(const wstring string);
	wstring To_wstr(const string & str);
	string To_str(const wstring & wstr);

	/* Thread operation */
	mutex _lock;
	vector<Thread_info> console_thread;
	const static int time_lock = 10;

	template <typename T>
	int Alloc_thread(void (Design_Tool::*func)(int &, vector<T> &), vector<T> & input);
	int Alloc_thread(void (Design_Tool::*func)(int &));
	bool Dealloc_thread(int id);
	int Get_valid_id();
	bool IsValid(int id);

	/* Thread function */
	void cmd_buffer_get(int& control_id);
	void cmd_buffer_set(int& control_id);
	void Mouse_Check(int & control_id, vector<int *> & pos);
	void Get_Input(int & control_id, vector<wchar_t *> & input);

	std::condition_variable check;

public:
	/* Mouse_state checker */
	_2D_pos Mouse_Checker(short & mouse_state);

	/* Constructor */
	Design_Tool(float buffer_ratio_x, float buffer_ratio_y, unsigned int text_size, ui_type * console_info = nullptr) :num_button(0), num_message(0)
	{
		/* setting for console */
		SetConsoleTitle("Easy_Dic_ver2.1");

		if (buffer_ratio_x > 1 || buffer_ratio_y <= 0) {
			Print_Error(L"Design_Tool", L"DT0000", false); // DT0000 Warning
			buffer_ratio_x = 1;
		}
		if (buffer_ratio_x > 1 || buffer_ratio_y <= 0) {
			Print_Error(L"Design_Tool", L"DT0000", false); // DT0000 Warning
			buffer_ratio_y = 1;
		}
		SetFontSize(GetStdHandle(STD_OUTPUT_HANDLE), text_size);
		this->pixel_size = GetFontSize(GetStdHandle(STD_OUTPUT_HANDLE));
		this->text_size = text_size;
		this->size = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));

		this->ratio_x = buffer_ratio_x;
		this->ratio_y = buffer_ratio_y;

		this->cmd_size.x = (short)(this->size.X * buffer_ratio_x);
		this->cmd_size.y = (short)(this->size.Y * buffer_ratio_y);


#ifdef WINMARK
		/* set cmd size */
		wstring cmd_commend = L"mode con: cols=";
		wstring cmd_commend2 = L" lines=";

		SetWindowPos(FindWindow(NULL, TEXT("Easy_Dic_ver1.3")), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
		_wsystem((cmd_commend + to_wstring(this->cmd_size.x) + cmd_commend2 + to_wstring(this->cmd_size.y)).c_str());

		//Alloc_thread(&Design_Tool::cmd_buffer_set); // deprecated
		//Alloc_thread(&Design_Tool::cmd_buffer_get);
#endif
		if (console_info != nullptr)
		{
			/* Reset the basic info */
			console_info->idx = 0;
			console_info->type = CONSOLE;
			console_info->usage = true;

			if (CMD_Condition(console_info))
				UI.push_back(console_info);
			else
				Print_Error(L"Design_Tool", L"DT0001"); // Error code DT0001
		}
		else
		{
			ui_type * console_info = new ui_type;
			/* Initialize */
			console_info->pos_x = (short)(this->cmd_size.x * 0.2);
			console_info->pos_y = (short)(this->cmd_size.y * 0.1);
			console_info->idx = 0;
			console_info->type = CONSOLE;
			console_info->width = (short)(this->cmd_size.x * 0.6);
			console_info->height = (short)(this->cmd_size.x * 0.6);
			console_info->usage = true;

			UI.push_back(console_info);
		}
	}
	~Design_Tool();
	/* Set the console basic info */
	bool Set_Console(ui_type * console_info);

	/* Change title */
	bool Changetitle(wstring new_title);

	/* Change CMD size */
	bool ChangeCMD_ratio(_2D_pos ratio);

	/* Change text size */
	bool Changetext_size(unsigned int text_size);

	/* Add new button. */
	int Addbutton(const wstring button_name, _2D_pos pos, Color colfront = Color::WHITE, Color colback = Color::BLACK);

	/* Add new message. */
	int AddMessage(const wstring message, _2D_pos pos, Color colfront = Color::WHITE, Color colback = Color::BLACK);

	/* Delete ui */
	bool DeleteUI(int idx);

	/* Print the designed console */
	vector<int> Printdesign(int num_pick = 1, bool if_input = false, wstring * input = nullptr);

	/* Design's visual programmer tool */
	void Virtual_Tool(Color MARK = Color::GRAY);
};






#endif


