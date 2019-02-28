#include "Console_Design.h"

/* Thread function */
int Design_Tool::Alloc_thread(void (Design_Tool::*func)(int &))
{
	int new_thread_id;

	new_thread_id = Get_valid_id();

	if (new_thread_id < 0) // create new space to allocate new thread
	{
		Thread_info new_info;
		this->console_thread.push_back(new_info);
		this->console_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id));
		this->console_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
	else
	{
		this->console_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id));
		this->console_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
}

template <typename T>
int Design_Tool::Alloc_thread(void (Design_Tool::*func)(int &, vector<T> &), vector<T> & input) {
	int new_thread_id;

	new_thread_id = Get_valid_id();

	if (new_thread_id < 0) // create new space to allocate new thread
	{
		Thread_info new_info;
		this->console_thread.push_back(new_info);
		this->console_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id), std::ref(input));
		this->console_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
	else
	{
		this->console_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id), std::ref(input));
		this->console_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
}

bool Design_Tool::Dealloc_thread(int id)
{
	if (this->console_thread.size() <= id || id < 0)
		return false;
	if (this->console_thread[id].state)
	{
		this->console_thread[id].state.store(false);
		while (1) {
			if (this->console_thread[id].id->joinable() == true) {
				break;
			}
		}
		this->console_thread[id].id->join();
		delete this->console_thread[id].id;
		this->console_thread[id].id = nullptr;
	}
	else
		return false;

	if (id == this->console_thread.size() - 1) {
		this->console_thread.pop_back();
	}

	return true;
}

bool Design_Tool::IsValid(int id) {
	if (this->console_thread.size() <= id)
		return false;
	else
		if (this->console_thread[id].id == nullptr)
			return false;
		else
			return this->console_thread[id].state;
}

int Design_Tool::Get_valid_id()
{
	if (this->console_thread.size() == 0)
		return -1; // there is no valid id, system creates new space to allocate id
	else
	{
		for (unsigned int i = 0; i < this->console_thread.size(); i++)
		{
			if (this->console_thread[i].state.load() == false)
				return i + 1;
		}

		return -(int)(this->console_thread.size() + 1); // there is no valid id, system creates new space to allocate id
	}
}

/* basic function */

short Design_Tool::Mouse_to_Console_x(short x_pos)
{
	return (short)(x_pos + 7) / this->pixel_size.x + 1;
}

short Design_Tool::Mouse_to_Console_y(short y_pos)
{
	return (short)(y_pos + 30) / (this->text_size) + 1;
}

int Design_Tool::rounder(double num)
{
	if (num - (int)num > 0.6)
		return (int)num + 1;
	else
		return (int)num;
}

_2D_pos Design_Tool::GetFontSize(HANDLE windowHandle)
{
	_2D_pos text_size;

#ifdef WINMARK
	COORD tmp;

	CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };

	if (!GetCurrentConsoleFontEx(windowHandle, 0, &font))
	{
		return text_size;
	}

	tmp = font.dwFontSize;

	text_size.x = tmp.X;   text_size.y = rounder(1.1507*tmp.Y + 0.1545);
#endif
	return text_size;
}

bool Design_Tool::SetFontSize(HANDLE windowHandle, unsigned int size)
{
#ifdef WINMARK
	COORD text_info;

	text_info.X = 100; text_info.Y = size;

	CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };

	if (!GetCurrentConsoleFontEx(windowHandle, 0, &font))
	{
		return false;
	}

	font.dwFontSize = text_info;

	if (!SetCurrentConsoleFontEx(windowHandle, 0, &font))
	{
		return false;
	}

	return true;
#elif
	return false;
#endif
}

void Design_Tool::SetColor(Color colFront, Color colBack)
{
#ifdef WINMARK
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (((unsigned int)colBack << 4) | (unsigned int)colFront));
#endif
}

void Design_Tool::gotoxy(short x, short y)
{
#ifdef WINMARK
	COORD Pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
#endif
}

void Design_Tool::Clean_Console()
{
#ifdef WINMARK
	system("cls");
#endif
}

short Design_Tool::wherex()
{
#ifdef WINMARK
	CONSOLE_SCREEN_BUFFER_INFO BufInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo);
	return BufInfo.dwCursorPosition.X;
#elif
	return false;
#endif
}

short Design_Tool::wherey()
{
#ifdef WINMARK
	CONSOLE_SCREEN_BUFFER_INFO BufInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo);
	return BufInfo.dwCursorPosition.Y;
#elif
	return false;
#endif
}

void Design_Tool::enter_gotoxy(short x)
{
	short y;
	y = wherey();
	gotoxy(x, y + 2);
}

void Design_Tool::space_gotoxy(short y, short space)
{
	short x;
	x = wherex();
	gotoxy(x + space, y);
}

_2D_pos Design_Tool::Mouse_Checker(short & mouse_state)
{
	POINT a;
	HWND hWnd;
	RECT window_size;

	lock_guard<std::mutex> lock(_lock);

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		mouse_state = CLICKED;
	else if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		mouse_state = CLICKED;
	else
		mouse_state = NOCHANGE;

	_2D_pos pos;

	GetCursorPos(&a);
	hWnd = WindowFromPoint(a);
	ScreenToClient(hWnd, &a);
	ScreenToClient(hWnd, &a);
	GetWindowRect(hWnd, &window_size);
	pos.x = a.x + window_size.left;
	pos.y = a.y + window_size.top;

	return pos;
}

int Design_Tool::Assign_id(ui_type * new_ui)
{
	/* Erase the unnecessary space */
	for (unsigned int i = 1; i < UI.size(); i++) {
		if (UI.back()->usage == true)
			break;
		else {
			delete UI[UI.size() - 1];
			UI.pop_back();
		}
	}

	/* Assign the new ui */
	for (unsigned int i = 1; i < UI.size(); i++) {
		if (UI[i]->usage == false) {
			UI[i] = new_ui;
			if (new_ui->type == BUTTON) {
				UI[i]->idx = i;
				return i;
			}
			else if (new_ui->type == MESSAGE) {
				UI[i]->idx = -i;
				return -i;
			}
		}
	}

	/* Create new space */
	if (new_ui->type == BUTTON)
		new_ui->idx = UI.size();
	else if (new_ui->type == MESSAGE)
		new_ui->idx = -UI.size();
	UI.push_back(new_ui);
	return new_ui->idx;
}

wstring Design_Tool::To_wstr(const string & str)
{
	unsigned int required_len = str.length();

	wchar_t * dest = new wchar_t[required_len + 1];

	mbstowcs(dest, str.c_str(), required_len);

	dest[required_len] = L'\0';

	wstring converted_str(dest);

	delete[] dest;

	return converted_str;
}

string Design_Tool::To_str(const wstring & wstr)
{
	unsigned int required_len = 0;
	wchar_t buf[2];
	char dest_buf[2];
	buf[1] = L'\0';

	for (int i = 0; i < wstr.length(); i++) {
		buf[0] = wstr[i];
		wcstombs(dest_buf, buf, 2);
		if (dest_buf[0] & 0x80)
			required_len += 2;
		else
			required_len += 1;
	}

	char * dest = new char[required_len + 1];

	wcstombs(dest, wstr.c_str(), required_len);

	dest[required_len] = '\0';

	string converted_str(dest);

	delete[] dest;

	return converted_str;
}

_2D_pos Design_Tool::String_size(const wstring str)
{
	wstring enter = L"\n";

	vector<string> splitted_str;

	size_t pos = 0;
	size_t p_pos = 0;
	_2D_pos size;
	int max_xsize = 0;

	/* split the string */
	while (1) {
		pos = str.find(enter, pos + 1);
		if (pos == string::npos) {
			if (str.length() - p_pos != 0) {
				splitted_str.push_back(To_str(str.substr(p_pos, str.length() - p_pos)));
			}
			break;
		}
		else {
			splitted_str.push_back(To_str(str.substr(p_pos, pos - p_pos)));
			p_pos = pos + 1;
		}
	}
	/* Find maximum x size of string */
	for (unsigned int i = 0; i < splitted_str.size(); i++) {
		max_xsize = (max_xsize >= splitted_str[i].length()) ? max_xsize : splitted_str[i].length();
	}

	size.x = max_xsize;
	size.y = splitted_str.size();

	splitted_str.clear();
	return size;
}

void Design_Tool::Print_Message(wstring & str, short start_pos, short end_pos, int type)
{
	wstring enter = L"\n";

	vector<wstring> splitted_str;

	size_t pos = -1;
	size_t p_pos = 0;

	/* split the string */
	while (1) {
		pos = str.find(enter, pos + 1);
		if (pos == string::npos) {
			if (str.length() - p_pos != 0) {
				splitted_str.push_back(str.substr(p_pos, str.length() - p_pos));
			}
			break;
		}
		else {
			splitted_str.push_back(str.substr(p_pos, pos - p_pos));
			p_pos = pos + 1;
		}
	}
	if (type == BUTTON) {
		cout << "┌";
		for (unsigned int i = 0; i < end_pos - start_pos - 2; i++) // 위쪽 사각형 라인
			cout << "─";
		cout << "┐";
		enter_gotoxy(start_pos);
		for (unsigned int i = 0; i < splitted_str.size(); i++) {
			wcout << L"│" << splitted_str[i];
			gotoxy(end_pos - 1, wherey() + 1);
			wcout << L"│";
			enter_gotoxy(start_pos);
		}
		cout << "└";
		for (unsigned int i = 0; i < end_pos - start_pos - 2; i++) // 아래쪽 사각형 라인
			cout << "─";
		cout << "┘";
	}
	else if (type == MESSAGE) {
		for (unsigned int i = 0; i < splitted_str.size(); i++) {
			wcout << splitted_str[i];
			enter_gotoxy(start_pos);
		}
	}
	splitted_str.clear();
}

bool Design_Tool::valueInRange(int value, int min, int max)
{
	return (value >= min) && (value <= max);
}

int Design_Tool::getkey()
{
	int ch = _getch();
	if (ch == 0xE0 || ch == 0)
	{
		ch = _getch();
	}
	return ch;
}

bool Design_Tool::CMD_Condition(ui_type * console_info)
{
	if (console_info->name.size() == 0)
		return false;
	if (console_info->pos_x <= 0 || console_info->pos_y <= 0)
		return false;
	if ((unsigned int)console_info->col_back > 15)
		return false;
	if ((unsigned int)console_info->col_front > 15)
		return false;
	if (console_info->height <= 1 || console_info->width <= 1)
		return false;
	if (console_info->width + console_info->pos_x > this->cmd_size.x)
		return false;
	if (console_info->height + console_info->pos_y > this->cmd_size.y) {
		return false;
	}

	return true;
}

bool Design_Tool::Button_Condition(ui_type * button_info)
{
	if (button_info->name.size() == 0)
		return false;
	if (button_info->type != BUTTON)
		return false;
	if (button_info->pos_x <= this->UI[0]->pos_x || button_info->pos_x + button_info->width > UI[0]->pos_x + UI[0]->width)
		return false;
	if (button_info->pos_y <= this->UI[0]->pos_y + 3 || button_info->pos_y + button_info->height + 2 >= UI[0]->pos_y + UI[0]->height)
		return false;
	/* check if overlapped */
	for (unsigned int i = 1; i < this->UI.size(); i++) {
		if (UI[i]->usage == true) {
			if ((valueInRange(button_info->pos_x, UI[i]->pos_x, UI[i]->pos_x + UI[i]->width - 1) || valueInRange(UI[i]->pos_x, button_info->pos_x, button_info->pos_x + button_info->width - 1)) &&
				(valueInRange(button_info->pos_y, UI[i]->pos_y, UI[i]->pos_y + UI[i]->height - 1) || valueInRange(UI[i]->pos_y, button_info->pos_y, button_info->pos_y + button_info->height - 1))) {
				return false;
			}
		}
	}

	return true;
}

bool Design_Tool::Message_Condition(ui_type * message_info)
{
	if (message_info->name.size() == 0)
		return false;
	if (message_info->type != MESSAGE)
		return false;
	if (message_info->pos_x <= this->UI[0]->pos_x || message_info->pos_x + message_info->width > UI[0]->pos_x + UI[0]->width)
		return false;
	if (message_info->pos_y <= this->UI[0]->pos_y + 3 || message_info->pos_y + message_info->height + 2 >= UI[0]->pos_y + UI[0]->height)
		return false;
	/* check if overlapped */
	for (unsigned int i = 1; i < this->UI.size(); i++) {
		if (UI[i]->usage == true) {
			if ((valueInRange(message_info->pos_x, UI[i]->pos_x, UI[i]->pos_x + UI[i]->width - 1) || valueInRange(UI[i]->pos_x, message_info->pos_x, message_info->pos_x + message_info->width - 1)) &&
				(valueInRange(message_info->pos_y, UI[i]->pos_y, UI[i]->pos_y + UI[i]->height - 1) || valueInRange(UI[i]->pos_y, message_info->pos_y, message_info->pos_y + message_info->height - 1))) {
				return false;
			}
		}
	}
	return true;
}

/* Public function */

bool Design_Tool::Set_Console(ui_type * console_info)
{
	/* Rearrange */
	console_info->idx = 0;
	console_info->type = CONSOLE;
	console_info->usage = true;

	if (CMD_Condition(console_info)) {
		this->UI[0] = console_info;
		return true;
	}
	else
	{
		Print_Error(L"Design_Tool", L"DT0002", false); // Warning code DT0002
		return false;
	}
}

bool Design_Tool::Changetitle(wstring new_title)
{
	if (new_title.size() != 0)
		this->UI[0]->name = new_title;
	else
		return false;

	return true;
}

bool Design_Tool::ChangeCMD_ratio(_2D_pos ratio)
{
	if (this->UI.size() < 1) {
		Print_Error(L"Design_Tool", L"DT0005"); // DT0005 Error
		return false;
	}

	if (ratio.x > 1 || ratio.x <= 0) {
		Print_Error(L"Design_Tool", L"DT0000", false); // DT0000 Warning
		ratio.x = 0.8;
	}
	if (ratio.y > 1 || ratio.y <= 0) {
		Print_Error(L"Design_Tool", L"DT0000", false); // DT0000 Warning
		ratio.y = 0.8;
	}
	int w, h;
	ui_type * tmp;
	tmp = this->UI[0];
	w = tmp->width; h = tmp->height;
	tmp->width = this->cmd_size.x * ratio.x;
	tmp->height = this->cmd_size.y * ratio.y;

	if (CMD_Condition(tmp) == false) {
		tmp->width = w; tmp->height = h;
		Print_Error(L"Design_Tool", L"DT0003", false); // Warning code DT0003
		return false;
	}

	return true;
}

bool Design_Tool::Changetext_size(unsigned int text_size)
{
	if (this->UI.size() < 1) {
		Print_Error(L"Design_Tool", L"DT0005"); // DT0005 Error
		return false;
	}

	this->text_size = text_size;

	SetFontSize(GetStdHandle(STD_OUTPUT_HANDLE), text_size);
	this->pixel_size = GetFontSize(GetStdHandle(STD_OUTPUT_HANDLE));
	this->size = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));

	short ratio_1 = this->size.X / this->cmd_size.x*ratio_x;
	short ratio_2 = this->size.Y / this->cmd_size.y*ratio_y;

	this->cmd_size.x = this->cmd_size.x * ratio_1;
	this->cmd_size.y = this->cmd_size.y * ratio_2;

	int w, h;
	ui_type *tmp;
	tmp = this->UI[0];
	w = tmp->width; h = tmp->height;
	tmp->width *= ratio_1;
	tmp->height *= ratio_2;

	if (CMD_Condition(tmp) == false) {
		tmp->width = w; tmp->height = h;
		Print_Error(L"Design_Tool", L"DT0003", false); // Warning code DT0003
		return false;
	}

	return true;
}

int Design_Tool::Addbutton(const wstring button_name, _2D_pos pos, Color colfront, Color colback)
{
	ui_type * new_button = new ui_type;
	_2D_pos size;

	new_button->name = button_name;
	new_button->pos_x = pos.x;
	new_button->pos_y = pos.y;
	new_button->type = BUTTON;
	new_button->usage = true;

	size = String_size(button_name);

	new_button->width = size.x + 2;
	new_button->height = size.y + 2;
	new_button->col_back = colback;
	new_button->col_front = colfront;

	if (Button_Condition(new_button)) {
		num_button++;
		return Assign_id(new_button);
	}
	else {
		Print_Error(L"Design_Tool", L"DT0004"); // Error code DT0004
		return -1;
	}
}

int Design_Tool::AddMessage(const wstring message, _2D_pos pos, Color colfront, Color colback)
{
	ui_type * new_message = new ui_type;
	_2D_pos size;

	new_message->name = message;
	new_message->pos_x = pos.x;
	new_message->pos_y = pos.y;
	new_message->type = MESSAGE;
	new_message->usage = true;

	size = String_size(message);

	new_message->width = size.x;
	new_message->height = size.y;
	new_message->col_back = colback;
	new_message->col_front = colfront;

	if (Message_Condition(new_message)) {
		num_message++;
		return Assign_id(new_message);
	}
	else {
		Print_Error(L"Design_Tool", L"DT0005"); // Error code DT0005
		return 1;
	}
}

bool Design_Tool::DeleteUI(int idx)
{
	for (unsigned int i = 0; i < this->UI.size(); i++) {
		if (UI[i]->idx == idx) {
			UI[i]->usage = false;
			if (UI[i]->type == BUTTON)
				num_button--;
			else if (UI[i]->type == MESSAGE)
				num_message--;
			return true;
		}
	}

	return false;
}

vector<int> Design_Tool::Printdesign(int num_pick, bool if_input, wstring * input)
{
	if (num_pick < 0 || num_pick > this->num_button)
		Print_Error(L"Design_Tool", L"DT0006"); // Error code DT0006

	vector<int> choice;
	wstring cmd_commend = L"mode con: cols=";
	wstring cmd_commend2 = L" lines=";

	Clean_Console();

	SetWindowPos(FindWindow(NULL, TEXT("Easy_Dic_ver2.1")), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
	_wsystem((cmd_commend + to_wstring(this->cmd_size.x) + cmd_commend2 + to_wstring(this->cmd_size.y)).c_str());

	/* Set Color */
	SetColor(UI[0]->col_front, UI[0]->col_back);

	/* Draw Corner */
	gotoxy(UI[0]->pos_x, UI[0]->pos_y);
	cout << "┌";
	gotoxy(UI[0]->pos_x + UI[0]->width, UI[0]->pos_y);
	cout << "┐";
	gotoxy(UI[0]->pos_x, UI[0]->pos_y + UI[0]->height - 1);
	cout << "└";
	gotoxy(UI[0]->pos_x + UI[0]->width, UI[0]->pos_y + UI[0]->height - 1);
	cout << "┘";

	/* Draw the rectangular */
	gotoxy(UI[0]->pos_x + 2, UI[0]->pos_y);
	for (unsigned int i = 0; i < UI[0]->width - 3; i++) // 위쪽 사각형 라인
		cout << "─";

	gotoxy(UI[0]->pos_x + 2, UI[0]->pos_y + UI[0]->height - 1);
	for (unsigned int i = 0; i < UI[0]->width - 3; i++) // 아래쪽 사각형 라인
		cout << "─";

	for (unsigned int i = 1; i < UI[0]->height - 1; i++) // 왼쪽, 오른쪽 사각형 라인
	{
		gotoxy(UI[0]->pos_x, UI[0]->pos_y + i);
		wcout << L"│";
		gotoxy(UI[0]->pos_x + UI[0]->width, UI[0]->pos_y + i);
		wcout << L"│";
	}
	/* Finished */

	//타이틀 출력
	gotoxy(UI[0]->pos_x + (UI[0]->width - UI[0]->name.length()) / 2 - 2, UI[0]->pos_y + 1);
	wcout << UI[0]->name;

	gotoxy(UI[0]->pos_x + 4, UI[0]->pos_y + 3);

	for (unsigned int i = 0; i < UI[0]->width - 7; i++)
		cout << "─";

	/* ui drawer */
	for (unsigned int i = 1; i < UI.size(); i++)
	{
		if (UI[i]->usage == true) {
			if (UI[i]->type == MESSAGE) {
				gotoxy(UI[i]->pos_x, UI[i]->pos_y);
				SetColor(UI[i]->col_front, UI[i]->col_back);
				Print_Message(UI[i]->name, UI[i]->pos_x, UI[i]->pos_x + UI[i]->width, MESSAGE);
				SetColor(UI[0]->col_front, UI[0]->col_back);
			}
			else if (UI[i]->type == BUTTON) {
				gotoxy(UI[i]->pos_x, UI[i]->pos_y);
				SetColor(UI[i]->col_front, UI[i]->col_back);
				Print_Message(UI[i]->name, UI[i]->pos_x, UI[i]->pos_x + UI[i]->width, BUTTON);
				SetColor(UI[0]->col_front, UI[0]->col_back);
			}
		}
	}
	int pos_x, pos_y;
	vector<int *> input_;
	vector<wchar_t *> input_2;
	input_.push_back(&pos_x);
	input_.push_back(&pos_y);

	int id = -1;
	int id_2 = -1;

	/* Check the mouse click and Check the input if necessary */
	if (num_pick > 0)
		id = Alloc_thread<int*>(&Design_Tool::Mouse_Check, input_);
	if (if_input) {
		wchar_t * result;
		input_2.push_back(result);
		id_2 = Alloc_thread<wchar_t *>(&Design_Tool::Get_Input, input_2);
	}
	while (1) {
		if (IsValid(id)) {
			for (unsigned int i = 1; i < UI.size(); i++) {
				if (UI[i]->type == BUTTON && UI[i]->usage == true) {
					if (pos_x + 1 >= UI[i]->pos_x && pos_x + 1 < UI[i]->pos_x + UI[i]->width) {
						if (pos_y >= UI[i]->pos_y && pos_y < UI[i]->pos_y + UI[i]->height) {
							if (choice.size() != num_pick)
								choice.push_back(UI[i]->idx);
							else {
								pos_x = -1; pos_y = -1;
								Dealloc_thread(id);
							}
							break;
						}
					}
				}
			}
		}
		else {
			if (!IsValid(id_2)) {
				if (if_input)
					this->console_thread[id_2].state.store(true);
				break;
			}
		}
	}

	if (if_input) {
		Dealloc_thread(id_2);
		*input = input_2[0];
		if (input_2[0] != nullptr)
			delete input_2[0];
	}

	/* Return to the base */
	Clean_Console();
	gotoxy(1, 1);
	return choice;
}

void Design_Tool::Virtual_Tool(Color MARK)
{
	vector<_2D_pos> Available_Space;
	vector<int> block_place;
	_2D_pos pos;
	int start, end;
	for (unsigned int i = UI[0]->pos_y + 4; i < UI[0]->pos_y + UI[0]->height - 3; i++) {
		start = UI[0]->pos_x + 1;
		pos.y = i;
		for (unsigned int j = 1; j < UI.size(); j++) {
			if (UI[j]->usage == true) {
				if (UI[j]->pos_y <= i && UI[j]->pos_y + UI[j]->height > i) {
					block_place.push_back(UI[j]->pos_x);
					if (UI[j]->type == BUTTON)
						block_place.push_back(UI[j]->pos_x + UI[j]->width);
					else
						block_place.push_back(UI[j]->pos_x + UI[j]->width - 1);
				}
			}
		}
		sort(block_place.begin(), block_place.end());
		for (unsigned int j = 0; j < block_place.size() / 2; j++) {
			end = block_place[2 * j];
			for (int k = start; k < end; k++) {
				pos.x = k;
				Available_Space.push_back(pos);
			}
			start = block_place[2 * j + 1] + 1;
		}
		end = UI[0]->pos_x + UI[0]->width;
		for (int k = start; k < end; k++) {
			pos.x = k;
			Available_Space.push_back(pos);
		}
		block_place.clear();
	}

	/*  Print Console   */
	Clean_Console();

	wstring cmd_commend = L"mode con: cols=";
	wstring cmd_commend2 = L" lines=";
	SetWindowPos(FindWindow(NULL, TEXT("Easy_Dic_ver2.1")), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
	_wsystem((cmd_commend + to_wstring(this->cmd_size.x) + cmd_commend2 + to_wstring(this->cmd_size.y)).c_str());

	/* Set Color */
	SetColor(UI[0]->col_front, UI[0]->col_back);

	/* Draw Corner */
	gotoxy(UI[0]->pos_x, UI[0]->pos_y);
	cout << "┌";
	gotoxy(UI[0]->pos_x + UI[0]->width, UI[0]->pos_y);
	cout << "┐";
	gotoxy(UI[0]->pos_x, UI[0]->pos_y + UI[0]->height - 1);
	cout << "└";
	gotoxy(UI[0]->pos_x + UI[0]->width, UI[0]->pos_y + UI[0]->height - 1);
	cout << "┘";

	/* Draw the rectangular */
	gotoxy(UI[0]->pos_x + 2, UI[0]->pos_y);
	for (unsigned int i = 0; i < UI[0]->width - 3; i++) // 위쪽 사각형 라인
		cout << "─";

	gotoxy(UI[0]->pos_x + 2, UI[0]->pos_y + UI[0]->height - 1);
	for (unsigned int i = 0; i < UI[0]->width - 3; i++) // 아래쪽 사각형 라인
		cout << "─";

	for (unsigned int i = 1; i < UI[0]->height - 1; i++) // 왼쪽, 오른쪽 사각형 라인
	{
		gotoxy(UI[0]->pos_x, UI[0]->pos_y + i);
		wcout << L"│";
		gotoxy(UI[0]->pos_x + UI[0]->width, UI[0]->pos_y + i);
		wcout << L"│";
	}
	/* Finished */

	wstring tmp_name = L"Visual Tool (Press ESC to exit)";
	//타이틀 출력
	gotoxy(UI[0]->pos_x + (UI[0]->width - tmp_name.length()) / 2 - 2, UI[0]->pos_y + 1);
	wcout << tmp_name;

	gotoxy(UI[0]->pos_x + 4, UI[0]->pos_y + 3);

	for (unsigned int i = 0; i < UI[0]->width - 7; i++)
		cout << "─";

	/* ui drawer */
	for (unsigned int i = 1; i < UI.size(); i++)
	{
		if (UI[i]->usage == true) {
			if (UI[i]->type == MESSAGE) {
				gotoxy(UI[i]->pos_x, UI[i]->pos_y);
				SetColor(UI[i]->col_front, UI[i]->col_back);
				Print_Message(UI[i]->name, UI[i]->pos_x, UI[i]->pos_x + UI[i]->width, MESSAGE);
				SetColor(UI[0]->col_front, UI[0]->col_back);
			}
			else if (UI[i]->type == BUTTON) {
				gotoxy(UI[i]->pos_x, UI[i]->pos_y);
				SetColor(UI[i]->col_front, UI[i]->col_back);
				Print_Message(UI[i]->name, UI[i]->pos_x, UI[i]->pos_x + UI[i]->width, BUTTON);
				SetColor(UI[0]->col_front, UI[0]->col_back);
			}
		}
	}

	SetColor(Color::BLACK, MARK);
	for (unsigned int i = 0; i < Available_Space.size(); i++) {
		gotoxy(Available_Space[i].x, Available_Space[i].y);
		wcout << L" ";
	}

	/* return to original */
	SetColor(UI[0]->col_front, UI[0]->col_back);

	short mouse_event = NOCHANGE;

	int x, y;
	int check_value = 0;
	while (1) {
		gotoxy(this->UI[0]->pos_x + 2, this->UI[0]->pos_y + this->UI[0]->height - 2);
		cout << "X : ";
		gotoxy(this->UI[0]->pos_x + 12, this->UI[0]->pos_y + this->UI[0]->height - 2);
		cout << "Y : ";
		gotoxy(this->UI[0]->pos_x + 22, this->UI[0]->pos_y + this->UI[0]->height - 2);
		cout << "Check : ";
		pos = Mouse_Checker(mouse_event);
		if (mouse_event == CLICKED) {
			check_value = 0;
			x = Mouse_to_Console_x(pos.x) + 2;
			y = Mouse_to_Console_y(pos.y);
			gotoxy(this->UI[0]->pos_x + 6, this->UI[0]->pos_y + this->UI[0]->height - 2);
			cout << "      ";
			gotoxy(this->UI[0]->pos_x + 16, this->UI[0]->pos_y + this->UI[0]->height - 2);
			cout << "      ";
			gotoxy(this->UI[0]->pos_x + 6, this->UI[0]->pos_y + this->UI[0]->height - 2);
			cout << x;
			gotoxy(this->UI[0]->pos_x + 16, this->UI[0]->pos_y + this->UI[0]->height - 2);
			cout << y;
			for (unsigned int i = 0; i < Available_Space.size(); i++) {
				if (x == Available_Space[i].x && y == Available_Space[i].y) {
					check_value = 1;
					break;
				}
			}
			gotoxy(this->UI[0]->pos_x + 30, this->UI[0]->pos_y + this->UI[0]->height - 2);
			if (check_value)
				cout << "T";
			else
				cout << "F";

		}
		if (_kbhit()) {
			if (_getch() == 27) // ESC get
				break;
		}
	}

	/* return to base */
	Clean_Console();
	gotoxy(1, 1);
}














/*     */

/* Thread function */

void Design_Tool::Mouse_Check(int & control_id, vector<int*> & pos_)
{
	int id = abs(control_id) - 1;
	//int start = clock();
	short mouse_event = NOCHANGE;
	_2D_pos pos;

	while (this->console_thread[id].state) {

		//if (clock() - start == time_lock) {
		pos = Mouse_Checker(mouse_event);
		if (mouse_event == CLICKED) {
			*pos_[0] = Mouse_to_Console_x(pos.x);
			*pos_[1] = Mouse_to_Console_y(pos.y);
		}
			//start = clock();
		//}
	}
	this->console_thread[id].state.store(true);
}

void Design_Tool::Get_Input(int & control_id, vector<wchar_t *> & input)
{
	int id = abs(control_id) - 1;

	wchar_t * input_tmp = new wchar_t[1024];
	gotoxy(this->UI[0]->pos_x + 2, this->UI[0]->pos_y + this->UI[0]->height - 3);
	for (int i = 0; i < this->UI[0]->width - 3; i++) {
		cout << "-";
	}
	gotoxy(this->UI[0]->pos_x + 2, this->UI[0]->pos_y + this->UI[0]->height - 2);
	cout << "Insert >> ";
	wcin.clear();
	wcin.getline(input_tmp, 1024); // get input

	input[0] = input_tmp;

	this->console_thread[id].state.store(false);
}

void Design_Tool::cmd_buffer_get(int& control_id)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	int id = abs(control_id) - 1;
	int start = clock();

	while (console_thread[id].state) {

		std::lock_guard<std::mutex> lock(_lock);

		if (clock() - start == time_lock) {

			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

			cmd_size.x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
			cmd_size.y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
			start = clock();
		}
	}
}
void Design_Tool::cmd_buffer_set(int& control_id)
{
	wstring cmd_commend = L"mode con: cols=";
	wstring cmd_commend2 = L" lines=";

	int id = abs(control_id) - 1;
	int start = clock();

	while (this->console_thread[id].state.load()) {

		std::lock_guard<std::mutex> lock(_lock);
		if (clock() - start == time_lock) {
			_wsystem((cmd_commend + to_wstring(this->cmd_size.x) + cmd_commend2 + to_wstring(this->cmd_size.y)).c_str());
			start = clock();
		}

	}
}

/* Destructor */
Design_Tool::~Design_Tool()
{
	for (unsigned int id = 0; id < this->console_thread.size(); id++)
	{
		this->console_thread[id].state.store(false);
		if (this->console_thread[id].id != nullptr) {
			if (this->console_thread[id].id->joinable()) {
				this->console_thread[id].id->join();
				delete this->console_thread[id].id;
				this->console_thread[id].id = nullptr;
			}
		}
	}
	this->console_thread.clear();

	for (int i = 0; i < UI.size(); i++) {
		delete UI[i];
	}
	this->UI.clear();
}