#include "Dic_Ver2.1.h"

/* Variable for Setting */
Array setting(L"Setting");

ui_type * Create_UI_Info(wstring name, unsigned int pos_x, unsigned int pos_y, unsigned int width, unsigned int height)
{
	ui_type * new_UI = new ui_type;

	new_UI->name = name;
	new_UI->pos_x = pos_x;
	new_UI->pos_y = pos_y;
	new_UI->width = width;
	new_UI->height = height;

	return new_UI;
}

void Phase_1(Data_Manager * data, Dir_Manager * dir_master)
{
	Design_Tool main_console(0.7,0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"   작업 선택", 20, 2, 40, 18));
	_2D_pos pos;
	pos.x = 25; pos.y = 7;
	main_console.Addbutton(L" 1. 단어 추가하기", pos);
	pos.x = 25; pos.y = 12;
	main_console.Addbutton(L" 2. 단어 검색하기", pos);
	pos.x = 47; pos.y = 13;
	main_console.Addbutton(L" 뒤로가기", pos);

	auto choice = main_console.Printdesign();
	wstring group_name;

	switch (choice[0]) {
	case 1:
		
		if (Group(dir_master).size() == 0) {
			Inform_Message(L" 등록된 그룹이 없습니다.\n새 그룹을 등록합니다.");
			group_name = Get_Input(L"  새 그룹 만들기", L" 새로운 그룹 이름을 입력하세요");
			Create_Group(data, dir_master, group_name);
			setting[0] = group_name;
		}
		if (dir_master->Find_dir(L"Voca", true) != -1) {
			if (dir_master->IsFile(dir_master->Get_Root_Path(true) + setting[0].get_raw() + L".txt") == true)
				setting[0] >> group_name;
			else {
				group_name = Group(dir_master)[0];
				setting[0] = group_name;
			}
			dir_master->Root2bound();
		}

		Add_Word(data, dir_master, group_name);

		break;
	case 2:
		if (Group(dir_master).size() != 0)
			Search_Word(data, dir_master);
		else
			Inform_Message(L"등록된 단어가 없습니다.");
		break;
	default:
		break;
	}
}

void Phase_2(Data_Manager * data, Dir_Manager * dir_master)
{
	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  단어 시험", 30, 2, 60, 28));
	_2D_pos pos;

	Inform_Message(L" 단어 시험장에 온 것을 환영합니다.\n 먼저, 단어 시험 범위를 지정합니다.");

	vector<wstring> groups = Group_Select(data, dir_master);

	if (groups.size() == 0) {
		Inform_Message(L"선택한 그룹이 없습니다. \n그룹과 단어를 등록하세요");
		return;
	}

	vector<int> choice;
	Array Criterion(L"Criterion");
	Criterion.randn(1, 4);
	Criterion[0] = 0.0; // minimum error rate
	Criterion[1] = 1.0; // maximum error rate
	Criterion[2] = 0.2; // number of test
	Criterion[3] = 0.5; // The minor error rate

	pos.x = 35; pos.y = 7;
	main_console.AddMessage(L"단어 난이도를 선택하세요",pos);
	/* 시험 단어의 난이도를 계산하여 구분 */
	/* 
	상 : 에러율 70% 이상 or 시험 횟수가 많음에도 에러율이 50% 이상인 단어로 선정
	중 : 에러율 70% 이하 40% 이상 or 시험 횟수가 많음에도 에러율이 30% 이상인 단어로 선정
	하 : 에러율 30% 이하 0% 이상
	>> a % 이상인 단어만 뽑아 그 중에 시험횟수가 상위 20 %의 시험 횟수에 있는 단어 선정 또는 기준치에 맞으면 그냥 선정
	*/
	pos.x = 35; pos.y = 10;
	unsigned int idx_1 = main_console.Addbutton(L"1. 상", pos); 
	pos.x = 35; pos.y = 14;
	unsigned int idx_2 = main_console.Addbutton(L"2. 중", pos); 
	pos.x = 35; pos.y = 18;
	unsigned int idx_3 = main_console.Addbutton(L"3. 하", pos); 
	pos.x = 53; pos.y = 23;
	unsigned int idx_exit = main_console.Addbutton(L" 단어 시험 종료", pos);

	choice = main_console.Printdesign();
		
	if (choice[0] == idx_exit)
		return;
	else {
		Set_cirterion(Criterion, choice[0] - idx_1);
	}

	Test(data, dir_master, classify_word(data, dir_master, groups, Criterion));
	return;
}

void Phase_3(Data_Manager * data, Dir_Manager * dir_master)
{
	if (Locker() == false)
		return;

	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  설  정", 30, 2, 60, 28));
	_2D_pos pos;
	vector<int> choice;

	pos.x = 35; pos.y = 7;
	main_console.Addbutton(L" 1. 단어 시험 타이머 설정", pos);
	pos.y += 3;
	main_console.Addbutton(L" 2. 그룹 설정", pos);
	pos.y += 3;
	main_console.Addbutton(L" 3. 단어장 시험 정보 초기화", pos);
	pos.y += 3;
	main_console.Addbutton(L" 4. 설정 자동 저장 관리", pos);
	pos.y += 3;
	main_console.Addbutton(L" 5. 단어장 정보 보기", pos);
	pos.y += 3;
	main_console.Addbutton(L" 6. 비밀번호 설정", pos);
	pos.x = 70;
	main_console.Addbutton(L" \n  종  료 \n ", pos, Color::BROWN);

	while (1) {
		choice = main_console.Printdesign();
		switch (choice[0]) {
		case 1:
			Set_Timer();
			break;
		case 2:
			Group_Setting(data, dir_master);
			break;
		case 3:
			Reset_Test_Info(data, dir_master);
			break;
		case 4:
			Set_Setting();
			break;
		case 5:
			View_Info(data, dir_master);
			break;
		case 6:
			Set_Password();
			break;
		case 7:
			return;
		}
	}
}

void Inform_Message(wstring Message)
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  안내 사항", 20, 2, 40, 18));
	_2D_pos pos;
	pos.x = 25; pos.y = 7;
	main_console.AddMessage(Message, pos);
	pos.x = 35; pos.y = 13;
	main_console.Addbutton(L" 확 인", pos);

	main_console.Printdesign();
}

bool Inform_Message_v2(wstring Message)
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  안내 사항", 20, 2, 40, 18));
	_2D_pos pos;
	vector<int> choice;
	int idx_1, idx_2;
	pos.x = 25; pos.y = 7;
	main_console.AddMessage(Message, pos);
	pos.x = 25; pos.y = 13;
	idx_1 = main_console.Addbutton(L" 예", pos);
	pos.x = 40; pos.y = 13;
	idx_2 = main_console.Addbutton(L" 아니오", pos);

	choice = main_console.Printdesign();
	if (choice[0] == idx_1)
		return true;
	else if(choice[0] == idx_2)
		return false;
}

vector<wstring> Group_Select(Data_Manager * data, Dir_Manager * dir_master, unsigned int max_choose)
{
	vector<wstring> groups = Group(dir_master);
	vector<wstring> selected_groups;

	unsigned int group_size = groups.size();

	if (max_choose == 0 || max_choose > group_size)
		max_choose = group_size;
	
	if (group_size != 0) {
		bool * if_select = new bool[group_size];
		for (unsigned int i = 0; i < group_size; i++) {
			if_select[i] = false;
		}

		Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  그룹 선택 (최대 : " + to_wstring(max_choose) + L")", 30, 2, 60, 28));
		_2D_pos pos;
		vector<int> choice;

		int page_pos = 0;
		unsigned int max_word = 5;
		int page_max = (int)((group_size - 1) / max_word);
		unsigned int idxs[5];
		unsigned int idx_prev, idx_next, idx_conf;
		unsigned int idx_page;
		unsigned int count_picked = 0;

		pos.y = 24; pos.x = 70;
		idx_prev = main_console.Addbutton(L" ◀", pos);
		pos.x = 84;
		idx_next = main_console.Addbutton(L" ▶", pos);
		pos.y = 24; pos.x = 54;
		idx_conf = main_console.Addbutton(L"선택 완료", pos);

		while (1) {
			pos.y = 25; pos.x = 76;
			idx_page = main_console.AddMessage(L"Page " + to_wstring(page_pos), pos, Color::GREEN);
			unsigned int size = ((group_size - page_pos * max_word) > max_word) ? max_word : (group_size - page_pos * max_word);
			for (unsigned int i = 0; i < max_word; i++) {
				idxs[i] = 0;
			}
			pos.x = 33; pos.y = 6;
			/* Make UI */
			for (unsigned i = page_pos * max_word; i < page_pos * max_word + size; i++) {
				if (if_select[i] == false)
					idxs[i - page_pos * max_word] = main_console.Addbutton(L"그룹 " + to_wstring(i - page_pos * max_word + 1) + L" : " + groups[i], pos);
				else
					idxs[i - page_pos * max_word] = main_console.AddMessage(L"그룹 " + to_wstring(i - page_pos * max_word + 1) + L" : " + groups[i], pos);
				pos.y += 3;
			}
			choice = main_console.Printdesign();
			if (choice[0] == idx_prev) {
				if (page_pos != 0)
					page_pos--;
			}
			else if (choice[0] == idx_next) {
				if (page_pos <= page_max - 1)
					page_pos++;
			}
			else if (choice[0] == idx_conf) {
				if (count_picked != 0)
					break;
			}
			else {
				for (unsigned int i = 0; i < max_word; i++) {
					if (idxs[i] == choice[0]) {
						if_select[i + page_pos * max_word] = true;
						count_picked++;
						break;
					}
				}
				if (count_picked == max_choose)
					break;
			}
			for (unsigned int i = 0; i < max_word; i++) {
				if (idxs[i] != 0) {
					main_console.DeleteUI(idxs[i]);
					idxs[i] = 0;
				}
			}
			main_console.DeleteUI(idx_page);
		}

		for (unsigned int i = 0; i < group_size; i++) {
			if (if_select[i])
				selected_groups.push_back(groups[i]);
		}

		delete if_select;
	}
	else
		Inform_Message(L"표시할 그룹이 없습니다.\n 설정에서 새로 그룹을 추가하세요.");

	return selected_groups;
}

void Add_Word(Data_Manager * data, Dir_Manager * dir_master, wstring group_name)
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  단어 입력", 20, 2, 40, 18));
	_2D_pos pos;
	pos.x = 25; pos.y = 7;
	int idx_message;
	wstring word;
	wstring tmp;
	vector<wstring> meaning;
	vector<Array> new_words;
	while (1) {
		main_console.Changetitle(L"  단어 입력");
		idx_message = main_console.AddMessage(L" 새로운 단어를 입력하세요", pos);
		main_console.Printdesign(0, true, &word);
		main_console.DeleteUI(idx_message);
		meaning.clear();

		while (1) {
			main_console.Changetitle(L"  단어 뜻 입력");
			idx_message = main_console.AddMessage(word + L"\n의" + to_wstring(meaning.size() + 1) + L" 번째 의미를 입력하세요.", pos);
			main_console.Printdesign(0, true, &tmp);
			main_console.DeleteUI(idx_message);
			meaning.push_back(tmp);
			if (DisplayWord(word, meaning, group_name) == false)
				break;
		}
		new_words.push_back(Data2Array(word, meaning, 0.0, group_name,0));
		if (Inform_Message_v2(L"단어를 더 추가하시겠습니까?") == false)
			break;
	}
	Revise_Data(data, dir_master, new_words, group_name);
	new_words.clear();
}

void Search_Word(Data_Manager * data, Dir_Manager * dir_master)
{

	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  단어 검색", 20, 2, 40, 18));

	vector<wstring> groups = Group_Select(data, dir_master);
	wstring word;
	_2D_pos pos;
	pos.x = 25; pos.y = 7;
	main_console.AddMessage(L" 검색할 단어를 입력하세요", pos);
	while (1) {
		main_console.Printdesign(0, true, &word);
		ChooseWord(data,dir_master,Search(data, dir_master, word, groups));
		if (Inform_Message_v2(L"검색을 계속할까요?") == false)
			break;
	}
}

bool DisplayWord(wstring word, vector<wstring> meaning, wstring group_name)
{
	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  단어 정보", 30, 2, 60, 28));
	_2D_pos pos;
	vector<int> choice;
	int idx_1, idx_2;
	/* Button */
	pos.x = 50; pos.y = 23;
	idx_1 = main_console.Addbutton(L"  끝내기 ", pos, Color::BROWN);
	pos.x = 35; pos.y = 23;
	idx_2 = main_console.Addbutton(L"  계속 입력 ", pos, Color::BROWN);

	unsigned int meaning_size = meaning.size();
	unsigned int idx_prev, idx_next;
	int idx_page;
	int page_pos = 0;
	unsigned int word_per_page = 8;
	int idxs[8];
	int page_num = (int)((meaning_size - 1) / word_per_page);

	pos.y = 23; pos.x = 70;
	idx_prev = main_console.Addbutton(L" ◀", pos);
	pos.x = 84;
	idx_next = main_console.Addbutton(L" ▶", pos);
	pos.x = 33; pos.y = 6;
	main_console.AddMessage(L"단어 : " + word, pos);
	pos.x = 33; pos.y = 7;
	main_console.AddMessage(L"소속 그룹 : " + group_name, pos);

	while (1) {
		unsigned int size = ((meaning_size - page_pos * word_per_page) >= word_per_page)? word_per_page : (meaning_size - page_pos * word_per_page);
		pos.y = 24; pos.x = 76;
		idx_page = main_console.AddMessage(L"Page " + to_wstring(page_pos + 1), pos, Color::GREEN);
		pos.x = 33; pos.y = 8;
		for (unsigned int i = page_pos * word_per_page; i < (page_pos + 1) * word_per_page; i++) {
			if (i < page_pos * word_per_page + size) {
				idxs[i - page_pos * word_per_page] = main_console.AddMessage(L"뜻 " + to_wstring(i + 1) + L" : " + meaning[i], pos);
				pos.y += 1;
			}
			else {
				idxs[i - page_pos * word_per_page] = 0;
			}
		}

		choice = main_console.Printdesign();

		if (choice[0] == idx_1 || choice[0] == idx_2)
			break;
		else if (choice[0] == idx_prev && page_pos != 0)
			page_pos--;
		else if (choice[0] == idx_next && page_pos <= page_num - 1)
			page_pos++;

		for (unsigned int i = 0; i < word_per_page; i++) {
			if (idxs[i] != 0) {
				main_console.DeleteUI(idxs[i]);
				idxs[i] = 0;
			}
		}
		main_console.DeleteUI(idx_page);
	}
	if (choice[0] == idx_1)
		return false;
	else
		return true;
}

void DisplayWord_v2(Data_Manager * data, Dir_Manager * dir_master, Array word)
{
	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  단어 정보 : " + word[0].get_raw(), 30, 2, 60, 28));
	_2D_pos pos;

	vector<Array> words;
	vector<wstring> meaning;
	vector<int> choice;
	wstring tmp_meaning;

	unsigned int word_size = word.get_size();

	/* Get meaning part */
	for (unsigned int i = 1; i < word_size; i++) {
		if (word[i].type() != (unsigned int)Type::DEFAULT_TYPE)
			break;
		meaning.push_back(word[i].get_raw());
	}

	unsigned int meaning_size = meaning.size();
	unsigned int idx_prev, idx_next; 
	int idx_page;
	int page_pos = 0;
	unsigned int word_per_page = 3;
	int idxs[3];
	int page_num = (int)((meaning_size - 1) / word_per_page);
	bool if_changed = false;

	pos.x = 33; pos.y = 6;
	unsigned int idx_1 = main_console.Addbutton(L"오답률 : " + to_wstring((int)(stod(word[meaning_size + 1].get_raw()) * 100)) + L"%", pos);
	pos.x = 33; pos.y = 9;
	unsigned int idx_2 = main_console.Addbutton(L"소속 그룹 : " + word[meaning_size + 2].get_raw(), pos);
	pos.x = 33; pos.y = 12;
	unsigned int idx_3 = main_console.Addbutton(L"시험 횟수 : " + word[meaning_size + 3].get_raw() + L" 회", pos);

	pos.y = 24; pos.x = 70;
	idx_prev = main_console.Addbutton(L" ◀", pos);
	pos.y = 24; pos.x = 50;
	unsigned int exit = main_console.Addbutton(L" 확 인", pos);
	pos.x = 84;
	idx_next = main_console.Addbutton(L" ▶", pos);

	while (1) {
		unsigned int size = ((meaning_size - page_pos * word_per_page) >= word_per_page) ? word_per_page : (meaning_size - page_pos * word_per_page);
		pos.y = 25; pos.x = 76;
		idx_page = main_console.AddMessage(L"Page " + to_wstring(page_pos + 1), pos, Color::GREEN);
		pos.x = 33; pos.y = 15;
		for (unsigned int i = page_pos * word_per_page; i < (page_pos + 1) * word_per_page; i++) {
			if (i < page_pos * word_per_page + size) {
				idxs[i - page_pos * word_per_page] = main_console.Addbutton(L"뜻 " + to_wstring(i + 1) + L" : " + meaning[i], pos);
				pos.y += 3;
			}
			else {
				idxs[i - page_pos * word_per_page] = 0;
			}
		}

		choice = main_console.Printdesign();

		if (choice[0] == exit)
			break;
		else if (choice[0] == idx_prev && page_pos != 0)
			page_pos--;
		else if (choice[0] == idx_next && page_pos <= page_num - 1)
			page_pos++;
		else if (choice[0] == idx_1) {
			if (Locker()) {
				if (Inform_Message_v2(L"오답률을 초기화하시겠습니까?")) {
					word[meaning_size + 1] = 0.0;
					main_console.DeleteUI(idx_1);
					pos.x = 33; pos.y = 6;
					idx_1 = main_console.Addbutton(L"오답률 : " + word[meaning_size + 1].get_raw(), pos);
					if_changed = true;
				}
			}
		}
		else if (choice[0] == idx_2) {
			if (Inform_Message_v2(L"다른 그룹으로 바꾸시겠습니까?")) {
				word[meaning_size + 2] = Group_Select(data, dir_master, 1)[0];
				main_console.DeleteUI(idx_2);
				pos.x = 33; pos.y = 9;
				idx_2 = main_console.Addbutton(L"소속 그룹 : " + word[meaning_size + 2].get_raw(), pos);
				if_changed = true;
			}
		}
		else if(choice[0] == idx_3){
			if (Locker()) {
				if (Inform_Message_v2(L"시험 횟수를 초기화하시겠습니까?")) {
					word[meaning_size + 3] = 0;
					main_console.DeleteUI(idx_3);
					pos.x = 33; pos.y = 12;
					idx_3 = main_console.Addbutton(L"시험 횟수 : " + word[meaning_size + 3].get_raw(), pos);
					if_changed = true;
				}
			}
		}
		else {
			for (unsigned int i = 0; i < word_per_page; i++) {
				if (choice[0] == idxs[i]) {
					if (Inform_Message_v2(L"뜻 " + meaning[i + page_pos * word_per_page] + L" 를 바꾸시겠습니까?")) {
						tmp_meaning = Get_Input(L"새로운 뜻 입력", L"Enter를 입력하면 뜻이 삭제됩니다.");
						meaning = change_meaning(meaning, tmp_meaning, i + page_pos * word_per_page);
						wstring tmp_name = word[0].get_raw();
						double error = stod(word[meaning_size + 1].get_raw());
						wstring group_name = word[meaning_size + 2].get_raw();
						int test_num = stoi(word[meaning_size + 3].get_raw());
						meaning_size = meaning.size();
						word.randn(1, 4 + meaning_size);
						
						word[0] = tmp_name;
						for (unsigned int i = 0; i < meaning_size; i++) {
							word[i + 1] = meaning[i];
						}
						word[meaning_size + 1] = error;
						word[meaning_size + 2] = group_name;
						word[meaning_size + 3] = test_num;
						
						page_num = (int)(meaning_size / word_per_page);
					}
					if_changed = true;
					break;
				}
			}
		}

		for (unsigned int i = 0; i < word_per_page; i++) {
			main_console.DeleteUI(idxs[i]);
		}
		main_console.DeleteUI(idx_page);
	}
	if (if_changed) {
		words.push_back(word);
		Revise_Data_v2(data, dir_master, words, word[meaning_size + 2].get_raw());
	}
}

void DisplayWord_v3(Array word, wstring message, unsigned int idx)
{
	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(to_wstring(idx) + L" 번째 답안 ", 30, 2, 60, 28));
	_2D_pos pos;
	vector<int> choice;

	/* Calculate word, meaning, groupname */
	unsigned int word_size = word.get_size();
	vector<wstring> meaning;
	/* Get the meaning */
	for (unsigned int i = 1; i < word_size - 3; i++) {
		meaning.push_back(word[i].get_raw());
	}
	/* Get the group name */
	wstring group_name = word[word_size - 2].get_raw();

	/* Button */
	pos.x = 55; pos.y = 23;
	int idx_1 = main_console.Addbutton(L"  확 인 ", pos, Color::BROWN);

	unsigned int meaning_size = meaning.size();
	unsigned int idx_prev, idx_next;
	int idx_page;
	int page_pos = 0;
	unsigned int word_per_page = 5;
	int idxs[5];
	int page_num = (int)((meaning_size - 1) / word_per_page);

	pos.y = 23; pos.x = 70;
	idx_prev = main_console.Addbutton(L" ◀", pos);
	pos.x = 84;
	idx_next = main_console.Addbutton(L" ▶", pos);
	pos.x = 33; pos.y = 6;
	main_console.AddMessage(message, pos);
	pos.x = 33; pos.y = 10;
	main_console.AddMessage(L" 단어 : " + word[0].get_raw(), pos);
	pos.x = 33; pos.y = 11;
	main_console.AddMessage(L" 소속 그룹 : " + group_name, pos);

	while (1) {
		unsigned int size = ((meaning_size - page_pos * word_per_page) >= word_per_page) ? word_per_page : (meaning_size - page_pos * word_per_page);
		pos.y = 24; pos.x = 76;
		idx_page = main_console.AddMessage(L"Page " + to_wstring(page_pos + 1), pos, Color::GREEN);
		pos.x = 33; pos.y = 12;
		for (unsigned int i = page_pos * word_per_page; i < (page_pos + 1) * word_per_page; i++) {
			if (i < page_pos * word_per_page + size) {
				idxs[i - page_pos * word_per_page] = main_console.AddMessage(L" 뜻 " + to_wstring(i + 1) + L" : " + meaning[i], pos);
				pos.y += 1;
			}
			else {
				idxs[i - page_pos * word_per_page] = 0;
			}
		}

		choice = main_console.Printdesign();

		if (choice[0] == idx_1)
			break;
		else if (choice[0] == idx_prev && page_pos != 0)
			page_pos--;
		else if (choice[0] == idx_next && page_pos <= page_num - 1)
			page_pos++;

		for (unsigned int i = 0; i < word_per_page; i++) {
			if (idxs[i] != 0) {
				main_console.DeleteUI(idxs[i]);
				idxs[i] = 0;
			}
		}
		main_console.DeleteUI(idx_page);
	}
}

void ChooseWord(Data_Manager * data, Dir_Manager * dir_master, vector<Array> words)
{
	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"  단어 검색 목록", 30, 2, 60, 28));
	_2D_pos pos;
	vector<int> choice;

	unsigned int idx[6];
	pos.x = 33; pos.y = 7;
	unsigned int word_size = words.size();
	for (unsigned int i = 0; i < word_size; i++) {
		idx[i] = main_console.Addbutton(words[i][0].get_raw(),pos);
		pos.y += 3;
	}
	pos.x = 70; pos.y = 24;
	unsigned int exit = main_console.Addbutton(L" 없습니다", pos);
	pos.x = 33; pos.y = 6;
	main_console.AddMessage(L"찾을 단어를 선택하세요.", pos);
	choice = main_console.Printdesign();
	if (choice[0] != exit) {
		DisplayWord_v2(data, dir_master, words[choice[0] - 1]);
	}
}

wstring Get_Input(wstring title, wstring Message)
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(title, 20, 2, 40, 18));
	_2D_pos pos;
	pos.x = 25; pos.y = 7;
	main_console.AddMessage(Message, pos);

	wstring input;
	main_console.Printdesign(0, true, &input);

	return input;
}

void Set_cirterion(Array & criterion,unsigned int level)
{
	switch (level) {
	case 0:
		criterion[0] = 0.7;
		criterion[1] = 1.0;
		criterion[2] = 0.2;
		criterion[3] = 0.5;
		break;
	case 1:
		criterion[0] = 0.4;
		criterion[1] = 0.7;
		criterion[2] = 0.2;
		criterion[3] = 0.3;
		break;
	case 2:
		criterion[0] = 0.0;
		criterion[1] = 0.3;
		criterion[2] = 0.0;
		criterion[3] = 0.0;
		break;
	}
}

bool Locker()
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L" 비밀번호 인증", 20, 2, 40, 18));
	_2D_pos pos;
	wstring input;

	pos.x = 24; pos.y = 9;
	main_console.AddMessage(L"인증이 필요합니다.",pos);
	pos.x = 24; pos.y = 10;
	main_console.AddMessage(L"비밀번호를 입력해주세요.", pos);
	pos.x = 24; pos.y = 12;
	main_console.AddMessage(L"[뒤로 가려면 Enter를 누르세요]", pos);

	while (1) {
		main_console.Printdesign(0, true, &input);

		if (input.length() == 0)
			return false;
		if (Decoder(input, setting[3].get_raw()))
			break;
		else
			Inform_Message(L" 틀렸습니다. \n 다시 입력해주세요.");
	}

	return true;
}

void Test(Data_Manager * data, Dir_Manager * dir_master, vector<Array> words)
{
	if (words.size() == 0) {
		Inform_Message(L"해당되는 단어가 없습니다.");
		return;
	}

	/* Test begin */
	// 1st step : set the data sheet
	// 2nd step : prepare data to out
	// 3rd step : show test info(test limit time) before begin
	// 4th step : start the test program and timer
	// 5th step : save the test result for data
	// 6th step : update the state of words

	/* Calculate number of words */
	unsigned int word_num = words.size();
	/* For the timer */
	PROCESS_INFORMATION tmp; DWORD ExitCode;

	/* 1st step */
	if (dir_master->Find_dir(L"Train_Data", true) != -1) {
		if (dir_master->IsFile(dir_master->Get_Root_Path(true) + L"Train_Data.txt") == false)
			data->Write(dir_master->Get_Root_Path(true) + L"Train_Data.txt");
		dir_master->Root2bound();
	}

	/* 2nd step */
	vector<Array> Test_data;
	for (unsigned int i = 0; i < word_num; i++) {
		Array data(L"data" + to_wstring(i));
		unsigned int word_size = words[i].get_size();
		data.randn(1, 6);
		data[0] = words[i][0].get_raw();
		data[1] = words[i][word_size - 2].get_raw();
		data[2] = stod(words[i][word_size - 3].get_raw());
		data[3] = stol(words[i][word_size - 1].get_raw());
		Test_data.push_back(data);
	}

	/* 3rd step */
	if(stoi(setting[2].get_raw()) == true)
		Inform_Message(L" 단어 시험이 곧 시작됩니다. \n 시간제한은 " + setting[1].get_raw() + L" 분 입니다.");
	else
		Inform_Message(L" 단어 시험이 곧 시작됩니다. \n 시간제한은 없습니다.");

	/* 4th step */
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"단어 시험지", 20, 2, 40, 18));
	_2D_pos pos;
	vector<int> choice; wstring answer;
	unsigned int question_way;
	unsigned int idx_msg, idx_msg_2;
	bool if_timer = stoi(setting[2].get_raw());

	// Set timer's time
	Write_Time(stoi(setting[1].get_raw()));

	/* Test begin */
	for (unsigned int i = 0; i < word_num; i++) {

		/* Pick the question way */
		unsigned int word_size = words[i].get_size();
		if (rand() % 2 == 1)
			question_way = rand() % (word_size - 4) + 1;
		else
			question_way = 0;

		/* Create Question */
		pos.x = 24; pos.y = 9;
		if (question_way == 0) {
			idx_msg = main_console.AddMessage(L"단어 : " + words[i][0].get_raw(), pos);
			pos.y += 2;
			idx_msg_2 = main_console.AddMessage(L"뜻을 입력하세요.",pos);
		}
		else {
			idx_msg = main_console.AddMessage(L"뜻 : " + words[i][question_way].get_raw(), pos);
			pos.y += 2;
			idx_msg_2 = main_console.AddMessage(L"단어를 입력하세요.",pos);
		}

		if (if_timer) {
			/* Create Timer */
			tmp = Create_Timer();

			/* Create Question */
			choice = main_console.Printdesign(0, true, &answer);

			/* Stop the timer */
			TerminateProcess(tmp.hProcess, 1);

			/* Get exit code */
			GetExitCodeProcess(tmp.hProcess, &ExitCode);

			/* Check the answer */
			Check_Test(words, i, answer, (bool)ExitCode, Test_data, question_way);
		}
		else {
			/* Check the answer */
			Check_Test(words, i, answer, false, Test_data, question_way);
		}

		/* Clear all the message */
		main_console.DeleteUI(idx_msg);
		main_console.DeleteUI(idx_msg_2);
	}

	/* 5th step */
	if (dir_master->Find_dir(L"Train_Data", true) != -1) {
		if (dir_master->IsFile(dir_master->Get_Root_Path(true) + L"Train_Data.txt")) {
			data->put(Test_data);
			data->Write(dir_master->Get_Root_Path(true) + L"Train_Data.txt","ab");
			data->clear_all();
		}
		dir_master->Root2bound();
	}

	/* 6th step */
	for (unsigned int i = 0; i < word_num; i++) {
		unsigned int result = stoi(Test_data[i][4].get_raw());
		unsigned int word_size = words[i].get_size();
		unsigned int test_num = stoi(words[i][word_size - 1].get_raw());
		double error = stod(words[i][word_size - 3].get_raw());
		if (result != 2) {
			words[i][word_size - 3] = (double)((error * test_num + !result) / (test_num + 1));
			words[i][word_size - 1] = test_num + 1;
		}
		Revise_Data_v3(data, dir_master, words[i]);
	}
}

void Set_Timer()
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"시험용 타이머 설정", 20, 2, 40, 18));
	_2D_pos pos;
	wstring input;
	unsigned int new_time;

	pos.x = 24; pos.y = 8;
	main_console.AddMessage(L" 현재 설정된 시간은 " + setting[1].get_raw() + L"초 입니다.", pos);
	pos.y += 2;
	main_console.AddMessage(L" 시간 설정은 60초 이내로 가능합니다.", pos);
	pos.y += 2;
	main_console.AddMessage(L" 뒤로 가려면 \n Enter를 누르세요", pos);

	while (1) {
		main_console.Printdesign(0, true, &input);

		if (input.length() != 0) {
			new_time = stoi(input);
			if (new_time >= 60 || new_time == 0) {
				Inform_Message(L" 60초 이내로 설정해주세요");
				continue;
			}
			else {
				setting[1] = new_time;
				Inform_Message(L" 변경된 시간은 " + setting[1].get_raw() + L"초 입니다.");
				break;
			}
		}
		else {
			Inform_Message(L" 현재 설정된 시간은 " + setting[1].get_raw() + L"초 입니다.");
			break;
		}
	}
}

void Group_Setting(Data_Manager * data, Dir_Manager * dir_master)
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"그룹 설정", 20, 2, 40, 18));
	_2D_pos pos;
	vector<int> choice;

	pos.x = 24; pos.y = 7;
	main_console.Addbutton(L" 1. 단어 추가할 그룹 설정", pos);
	pos.y += 3;
	main_console.Addbutton(L" 2. 그룹 추가/삭제", pos);
	pos.x = 40; pos.y = 13;
	main_console.Addbutton(L" 뒤로 가기", pos);

	while (1) {
		choice = main_console.Printdesign();
		switch (choice[0]) {
		case 1:
			Select_Add_Group(data, dir_master);
			break;
		case 2:
			Group_Add_Delete(data, dir_master);
			break;
		case 3:
			return;
		}
	}
}

void Reset_Test_Info(Data_Manager * data, Dir_Manager * dir_master)
{
	Inform_Message(L" 시험 정보를 삭제할 \n 그룹을 선택하세요.");

	vector<wstring> groups;

	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L" 단어장 정보 ", 30, 2, 60, 28));
	_2D_pos pos;
	vector<int> choice;
	vector<Array> words;

	int page_pos = 0;
	unsigned int max_word = 4;
	int page_max, group_size;
	unsigned int idxs[4];
	unsigned int idx_prev, idx_next, idx_conf, idx_go, idx_msg;
	unsigned int idx_page;

	pos.y = 24; pos.x = 70;
	idx_prev = main_console.Addbutton(L" ◀", pos);
	pos.x = 84;
	idx_next = main_console.Addbutton(L" ▶", pos);
	pos.y = 24; pos.x = 54;
	idx_conf = main_console.Addbutton(L" 뒤로 가기", pos);
	pos.y = 24; pos.x = 33;
	idx_go = main_console.Addbutton(L" 계속 진행", pos);

selection:
	groups = Group_Select(data, dir_master);
	group_size = groups.size();

	page_max = (int)((group_size - 1) / max_word);
	
	pos.x = 33; pos.y = 7;
	idx_msg = main_console.AddMessage(L" 선택된 그룹 수 : " + to_wstring(group_size) + L"개", pos);

	if (group_size != 0) {
		while (1) {
			pos.y = 25; pos.x = 76;
			idx_page = main_console.AddMessage(L"Page " + to_wstring(page_pos), pos, Color::GREEN);
			unsigned int size = ((group_size - page_pos * max_word) > max_word) ? max_word : (group_size - page_pos * max_word);
			for (unsigned int i = 0; i < max_word; i++) {
				idxs[i] = 0;
			}
			pos.x = 33; pos.y = 9;
			/* Make UI */
			for (unsigned i = page_pos * max_word; i < page_pos * max_word + size; i++) {
				idxs[i - page_pos * max_word] = main_console.AddMessage(L" 그룹 " + to_wstring(i - page_pos * max_word + 1) + L" : " + groups[i], pos);
				pos.y += 2;
			}
			choice = main_console.Printdesign();
			if (choice[0] == idx_prev) {
				if (page_pos != 0)
					page_pos--;
			}
			else if (choice[0] == idx_next) {
				if (page_pos <= page_max - 1)
					page_pos++;
			}
			else if (choice[0] == idx_conf) {
				for (unsigned int i = 0; i < max_word; i++) {
					if (idxs[i] != 0) {
						main_console.DeleteUI(idxs[i]);
						idxs[i] = 0;
					}
				}
				main_console.DeleteUI(idx_page);
				main_console.DeleteUI(idx_msg);
				goto selection;
			}
			else if (choice[0] == idx_go) {
				break;
			}

			for (unsigned int i = 0; i < max_word; i++) {
				if (idxs[i] != 0) {
					main_console.DeleteUI(idxs[i]);
					idxs[i] = 0;
				}
			}
			main_console.DeleteUI(idx_page);
		}
	}

	/* Clear all the Groups test info */
	if (group_size != 0) {
		if (Inform_Message_v2(L"모든 시험 정보를 삭제할까요? \n에러율, 시험횟수 자료들이 \n모두 초기화됩니다.")) {
			if (dir_master->Find_dir(L"Voca", true) != -1) {

				for (unsigned int i = 0; i < group_size; i++) {
					words = Get_Group_Data(data, dir_master, groups[i]);
					unsigned int word_num = words.size();
					for (unsigned int j = 0; j < word_num; j++) {
						unsigned int word_size = words[j].get_size();
						words[j][word_size - 3] = 0.0;
						words[j][word_size - 1] = 0;
					}
					data->Write(dir_master->Get_Root_Path(true) + groups[i] + L".txt", words);
				}

				dir_master->Root2bound();
			}
		}
	}

}

void Set_Setting()
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"설정 자동저장 여부 변경", 20, 2, 40, 18));
	_2D_pos pos;
	vector<int> choice;

	pos.x = 22; pos.y = 10;
	if(stoi(setting[4].get_raw()) == false)
		main_console.AddMessage(L" 현재 자동저장 설정은 off입니다.", pos);
	else
		main_console.AddMessage(L" 현재 자동저장 설정은 on입니다.", pos);
	pos.y += 3;
	unsigned int idx_1 = main_console.Addbutton(L" 자동 저장 켜기", pos);
	pos.x = 40;
	unsigned int idx_2 = main_console.Addbutton(L" 자동 저장 끄기", pos);

	choice = main_console.Printdesign(1);

	if (choice[0] == idx_1) {
		Inform_Message(L"자동 저장 기능을 켭니다.");
		setting[4] = true;
	}
	else {
		Inform_Message(L"저동 저장 기능을 끕니다.");
		setting[4] = false;
	}
}

void View_Info(Data_Manager * data, Dir_Manager * dir_master)
{
	vector<wstring> groups = Group(dir_master);
	unsigned int group_size = groups.size();

	wstring version_info = L"Easy Dictionary ver 2.1";

	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L" 단어장 정보 ", 30, 2, 60, 28));
	_2D_pos pos;
	vector<int> choice;

	if (group_size != 0) {

		int page_pos = 0;
		unsigned int max_word = 4;
		int page_max = (int)((group_size - 1) / max_word);
		unsigned int idxs[4];
		unsigned int idx_prev, idx_next, idx_conf;
		unsigned int idx_page;

		pos.y = 24; pos.x = 70;
		idx_prev = main_console.Addbutton(L" ◀", pos);
		pos.x = 84;
		idx_next = main_console.Addbutton(L" ▶", pos);
		pos.y = 24; pos.x = 54;
		idx_conf = main_console.Addbutton(L" 뒤로 가기", pos);
		pos.x = 33; pos.y = 7;
		main_console.AddMessage(L" 버전 정보 : " + version_info, pos);
		pos.y += 2;
		main_console.AddMessage(L" 총 그룹 수 : " + to_wstring(group_size) + L"개", pos);

		while (1) {
			pos.y = 25; pos.x = 76;
			idx_page = main_console.AddMessage(L"Page " + to_wstring(page_pos), pos, Color::GREEN);
			unsigned int size = ((group_size - page_pos * max_word) > max_word) ? max_word : (group_size - page_pos * max_word);
			for (unsigned int i = 0; i < max_word; i++) {
				idxs[i] = 0;
			}
			pos.x = 33; pos.y = 11;
			/* Make UI */
			for (unsigned i = page_pos * max_word; i < page_pos * max_word + size; i++) {
				idxs[i - page_pos * max_word] = main_console.Addbutton(L"그룹 " + to_wstring(i - page_pos * max_word + 1) + L" : " + groups[i], pos);
				pos.y += 3;
			}
			choice = main_console.Printdesign();
			if (choice[0] == idx_prev) {
				if (page_pos != 0)
					page_pos--;
			}
			else if (choice[0] == idx_next) {
				if (page_pos <= page_max - 1)
					page_pos++;
			}
			else if (choice[0] == idx_conf) {
				break;
			}
			else {
				for (unsigned int i = 0; i < max_word; i++) {
					if (idxs[i] == choice[0]) {
						Show_Group_Info(data, dir_master, groups[i + page_pos * max_word]);
						break;
					}
				}
			}
			for (unsigned int i = 0; i < max_word; i++) {
				if (idxs[i] != 0) {
					main_console.DeleteUI(idxs[i]);
					idxs[i] = 0;
				}
			}
			main_console.DeleteUI(idx_page);
		}
	}
	else {
		pos.y = 24; pos.x = 54;
		main_console.Addbutton(L" 뒤로 가기", pos);
		pos.x = 33; pos.y = 7;
		main_console.AddMessage(L" 버전 정보 : " + version_info, pos);
		pos.y += 2;
		main_console.AddMessage(L" 총 그룹 수 : " + to_wstring(group_size) + L"개", pos);

		main_console.Printdesign();
	}
}

void Set_Password()
{
	if (Locker() == false)
		return;

	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"비밀번호 변경 (최대 15자리)", 20, 2, 40, 18));
	_2D_pos pos;
	wstring input;

	pos.x = 24; pos.y = 10;
	main_console.AddMessage(L" 변경할 새로운 비밀번호 입력하세요.", pos);
	pos.y += 2;
	main_console.AddMessage(L" 뒤로 가려면 \n Enter를 누르세요", pos);

	while (1) {
		main_console.Printdesign(0, true, &input);

		if (input.length() != 0) {
			if (Inform_Message_v2(L"변경된 비밀번호가 \n" + input + L"\n이 맞나요?")) {
				setting[3] = Encoder(input);
				Inform_Message(L"설정되었습니다.");
				break;
			}
			else
				continue;
		}
		else {
			break;
		}
	}
}

void Show_Group_Info(Data_Manager * data, Dir_Manager * dir_master, wstring group_name)
{
	Design_Tool main_console(0.8, 0.8, DEFAULT_TEXT_SIZE, Create_UI_Info(L"그룹 정보", 20, 2, 50, 20));
	_2D_pos pos;

	unsigned int test_num_sum = 0;
	double error_average = 0;
	unsigned int word_num = 0;
	vector<Array> words;

	/* Get the words of group */
	words = Get_Group_Data(data, dir_master, group_name);

	/* Calculate the numbers */
	word_num = words.size();
	for (unsigned int i = 0; i < word_num; i++) {
		unsigned int word_size = words[i].get_size();
		unsigned int test_num = stoi(words[i][word_size - 1].get_raw());
		error_average += stod(words[i][word_size - 3].get_raw()) * test_num;
		test_num_sum += test_num;
	}
	if (test_num_sum == 0)
		error_average = 0;
	else
		error_average = (double)error_average / test_num_sum;

	/* Show the information */
	pos.x = 24; pos.y = 8;
	main_console.AddMessage(L" 그룹 이름 : " + group_name, pos);
	pos.y += 2;
	main_console.AddMessage(L" 총 단어 수 : " + to_wstring(word_num) + L" 개", pos);
	pos.y += 2;
	main_console.AddMessage(L" 총 시험 친 횟수 : " + to_wstring(test_num_sum) + L" 회", pos);
	pos.y += 2;
	main_console.AddMessage(L" 평균 시험 오답률 : " + to_wstring((int)(error_average * 100)) + L"%", pos);
	pos.x = 41; pos.y = 16;
	main_console.Addbutton(L" 확  인 ", pos);

	main_console.Printdesign();
}

void Select_Add_Group(Data_Manager * data, Dir_Manager * dir_master)
{
	Design_Tool main_console(0.7, 0.7, DEFAULT_TEXT_SIZE, Create_UI_Info(L"단어 추가용 그룹 설정", 20, 2, 40, 18));
	_2D_pos pos;
	vector<int> choice;
	wstring group;

	pos.x = 24; pos.y = 10;
	unsigned int idx_1 = main_console.Addbutton(L" 1. 기존 그룹에서 선택", pos);
	pos.y += 3;
	unsigned int idx_2 = main_console.Addbutton(L" 2. 새로운 그룹 만들기", pos);
	pos.x = 50; pos.y = 12;
	main_console.Addbutton(L"\n  종 료\n ", pos, Color::BROWN);
	pos.x = 24; pos.y = 7;
	main_console.AddMessage(L"현재 단어를 추가하는 그룹은 \n " + setting[0].get_raw() + L" 입니다.", pos);

	choice = main_console.Printdesign();

	if (choice[0] == idx_1) {
		if (Group(dir_master).size() != 0) {
			group = Group_Select(data, dir_master, 1)[0];
			setting[0] = group;
			Inform_Message(L"설정되었습니다.");
		}
		else
			Inform_Message(L" 등록된 그룹이 없습니다.");
	}
	else if(choice[0] == idx_2){
		group = Get_Input(L"  새 그룹 만들기", L" 새로운 그룹 이름을 입력하세요");
		Create_Group(data, dir_master, group);
		Inform_Message(L"설정되었습니다.");
		setting[0] = group;
	}
}

void Group_Add_Delete(Data_Manager * data, Dir_Manager * dir_master)
{
	vector<wstring> groups;

	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L" 그룹 추가/삭제 ", 30, 2, 60, 28));
	_2D_pos pos;
	vector<int> choice;
	vector<Array> words;
	wstring new_group;

	int page_pos = 0;
	unsigned int max_word = 4;
	int page_max, group_size;
	unsigned int idxs[4];
	unsigned int idx_prev, idx_next, idx_conf, idx_go, idx_msg;
	unsigned int idx_page;

	pos.y = 24; pos.x = 70;
	idx_prev = main_console.Addbutton(L" ◀", pos);
	pos.x = 84;
	idx_next = main_console.Addbutton(L" ▶", pos);
	pos.y = 24; pos.x = 54;
	idx_conf = main_console.Addbutton(L" 그룹 추가", pos);
	pos.y = 24; pos.x = 33;
	idx_go = main_console.Addbutton(L" 뒤로 가기", pos);

selection:
	groups = Group(dir_master);
	group_size = groups.size();

	page_max = (int)((group_size - 1) / max_word);

	pos.x = 33; pos.y = 7;
	idx_msg = main_console.AddMessage(L" 그룹 수 : " + to_wstring(group_size) + L"개", pos);

	if (group_size != 0) {
		while (1) {
			pos.y = 25; pos.x = 76;
			idx_page = main_console.AddMessage(L"Page " + to_wstring(page_pos), pos, Color::GREEN);
			unsigned int size = ((group_size - page_pos * max_word) > max_word) ? max_word : (group_size - page_pos * max_word);
			for (unsigned int i = 0; i < max_word; i++) {
				idxs[i] = 0;
			}
			pos.x = 33; pos.y = 9;
			/* Make UI */
			for (unsigned i = page_pos * max_word; i < page_pos * max_word + size; i++) {
				idxs[i - page_pos * max_word] = main_console.Addbutton(L" 그룹 " + to_wstring(i - page_pos * max_word + 1) + L" : " + groups[i], pos);
				pos.y += 3;
			}
			choice = main_console.Printdesign();
			if (choice[0] == idx_prev) {
				if (page_pos != 0)
					page_pos--;
			}
			else if (choice[0] == idx_next) {
				if (page_pos <= page_max - 1)
					page_pos++;
			}
			else if (choice[0] == idx_conf) {
				new_group = Get_Input(L"  새 그룹 만들기", L" 새로운 그룹 이름을 입력하세요");
				Create_Group(data, dir_master, new_group);
				
				for (unsigned int i = 0; i < max_word; i++) {
					if (idxs[i] != 0) {
						main_console.DeleteUI(idxs[i]);
						idxs[i] = 0;
					}
				}
				main_console.DeleteUI(idx_page);
				main_console.DeleteUI(idx_msg);

				Inform_Message(L" 설정되었습니다.");
				goto selection;
			}
			else if (choice[0] == idx_go) {
				break;
			}
			else {
				for (unsigned int i = 0; i < max_word; i++) {
					if (choice[0] == idxs[i]) {
						if (Inform_Message_v2(groups[i + page_pos * max_word] + L"\n을 삭제하시겠습니까?"))
							Delete_Group(data, dir_master, groups[i + page_pos * max_word]);
						break;
					}
				}
				for (unsigned int i = 0; i < max_word; i++) {
					if (idxs[i] != 0) {
						main_console.DeleteUI(idxs[i]);
						idxs[i] = 0;
					}
				}
				main_console.DeleteUI(idx_page);
				main_console.DeleteUI(idx_msg);
				goto selection;
			}

			for (unsigned int i = 0; i < max_word; i++) {
				if (idxs[i] != 0) {
					main_console.DeleteUI(idxs[i]);
					idxs[i] = 0;
				}
			}
			main_console.DeleteUI(idx_page);
		}
	}
	else {
		choice = main_console.Printdesign();
		if (choice[0] == idx_prev) {
			if (page_pos != 0)
				page_pos--;
		}
		else if (choice[0] == idx_next) {
			if (page_pos <= page_max - 1)
				page_pos++;
		}
		else if (choice[0] == idx_conf) {
			new_group = Get_Input(L"  새 그룹 만들기", L" 새로운 그룹 이름을 입력하세요");
			Create_Group(data, dir_master, new_group);
			main_console.DeleteUI(idx_msg);

			Inform_Message(L" 설정되었습니다.");
			goto selection;
		}
	}
}

vector<Array> Search(Data_Manager * data, Dir_Manager * dir_master, wstring word, vector<wstring> group_name)
{
	vector<Array> tmp;
	vector<Array> result;
	vector<pair<double, int>> similarity;

	unsigned int group_name_size = group_name.size();
	for (unsigned int i = 0; i < group_name_size; i++) {
		tmp = Get_Group_Data(data, dir_master, group_name[i]);
		unsigned int tmp_size = tmp.size();
		
		for (unsigned int j = 0; j < tmp_size; j++) {
			similarity.push_back(make_pair(Return_similarity(word, tmp[j][0].get_raw()),result.size()));
			result.push_back(tmp[j]);
		}
		tmp.clear();
	}
	
	sort(similarity.begin(), similarity.end(), greater<std::pair<double, int>>());

	unsigned int size = (similarity.size() >= 6) ? 6 : similarity.size();

	tmp.clear();

	for (unsigned int i = 0; i < size; i++) {
		tmp.push_back(result[similarity[i].second]);
	}

	similarity.clear();
	result.clear();
	return tmp;
}

vector<Array> classify_word(Data_Manager * data, Dir_Manager * dir_master, vector<wstring> group_name, Array & criterion)
{
	vector<Array> words;
	vector<Array> test_words;
	vector<pair<long, unsigned int>> word_testnum;

	double error_min = stod(criterion[0].get_raw());
	double error_max = stod(criterion[1].get_raw());
	double test_percent = stod(criterion[2].get_raw());
	double error_min_2 = stod(criterion[3].get_raw());

	unsigned int num_group = group_name.size();
	/* Get all the words in groups */
	for (unsigned int i = 0; i < num_group; i++) {
		auto tmp = Get_Group_Data(data, dir_master, group_name[i]);
		unsigned int size_tmp = tmp.size();
		for (unsigned int j = 0; j < size_tmp; j++) {
			words.push_back(tmp[j]);
		}
	}

	unsigned int num_word = words.size();
	/* Make vector to sort */
	for (unsigned int i = 0; i < num_word; i++) {
		unsigned int size = words[i].get_size();
		double error = stod(words[i][size - 3].get_raw());
		long test_num = stol(words[i][size - 1].get_raw());
		if (error >= error_min && error <= error_max)
			test_words.push_back(words[i]);
		else
			word_testnum.push_back(make_pair(test_num, i));
	}

	/* Sort the words with testnum */
	sort(word_testnum.begin(), word_testnum.end(), greater<std::pair<long, unsigned int>>());

	unsigned int idx_end = (unsigned int)(test_percent * num_word);
	if (word_testnum.size() == 1 && num_word == 1)
		idx_end = 1;

	for (unsigned int i = 0; i < idx_end; i++) {
		unsigned int idx = word_testnum[i].second;
		unsigned int size = words[idx].get_size();
		double error = stod(words[idx][size - 3].get_raw());
		if (error >= error_min_2)
			test_words.push_back(words[idx]);
	}

	return test_words;
}

vector<wstring> change_meaning(vector<wstring> prev_value, wstring input, unsigned int pos)
{
	vector<wstring> result;
	unsigned int prev_size = prev_value.size();
	
	if (input == L"") {
		if (prev_size != 1) {
			for (unsigned int i = 0; i < prev_size; i++) {
				if (i == pos)
					continue;
				result.push_back(prev_value[i]);
			}
		}
	}
	else {
		result = prev_value;
		result[pos] = input;
	}
	return result;
}

PROCESS_INFORMATION & Create_Timer()
{
	STARTUPINFO StartupInfo = { 0 };
	StartupInfo.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION ProcessInfo;
	StartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	int res = CreateProcess(nullptr, const_cast<LPSTR>("Timer.exe"), nullptr, nullptr, false, 0, nullptr, nullptr, &StartupInfo, &ProcessInfo);

	if (!res) {
		// process creation failed! 
		Print_Error(L"Dic_process", L"There is no timer.exe");
	}
	else
		return ProcessInfo;
}

Array Data2Array(wstring word, vector<wstring> meaning, double error_rate, wstring group_name, int test_num)
{
	Array new_array(word);

	unsigned int meaning_size = meaning.size();

	new_array.randn(1, 4 + meaning_size);

	new_array[0] = word;
	for (unsigned int i = 0; i < meaning_size; i++) {
		new_array[i + 1] = meaning[i];
	}
	new_array[meaning_size + 1] = error_rate;
	new_array[meaning_size + 2] = group_name;
	new_array[meaning_size + 3] = test_num;

	return new_array;
}



void Create_Group(Data_Manager * data, Dir_Manager * dir_master,wstring group_name)
{
	dir_master->Update_pause();
	if (dir_master->Find_dir(L"Voca", true) != -1) {
		data->Write(dir_master->Get_Root_Path(true) + group_name + L".txt");
		dir_master->Root2bound();
	}
	dir_master->Update_begin();
}

void Delete_Group(Data_Manager * data, Dir_Manager * dir_master, wstring group_name)
{
	int result = -1;
	dir_master->Update_pause();
	if (dir_master->Find_dir(L"Voca", true) != -1) {
		wstring file_path = dir_master->Get_Root_Path(true) + group_name + L".txt";
		result = remove(To_str(file_path).c_str());
		dir_master->Root2bound();
	}
	dir_master->Update_begin();
	if (result == -1)
		Inform_Message(group_name + L" 삭제 실패!!");
	else
		Inform_Message(group_name + L" 삭제 되었습니다.");
}

vector<wstring> Group(Dir_Manager * dir_master)
{
	vector<wstring> file_loc;

	dir_master->Root2bound();

	if (dir_master->Find_dir(L"Voca", true) != -1) {
		file_loc = dir_master->Get_Root_file();
		unsigned int file_size = file_loc.size();
		for (unsigned int i = 0; i < file_size; i++) {
			file_loc[i] = dir_master->Path2name(file_loc[i].substr(0, file_loc[i].find_last_of(L'.')));
		}
		dir_master->Root2bound();
	}

	return file_loc;
}

vector<Array> Get_Group_Data(Data_Manager * data, Dir_Manager * dir_master, wstring group_name)
{
	vector<int> idxs;
	vector<Array> result;

	dir_master->Root2bound();

	if (dir_master->Find_dir(L"Voca", true) != -1) {
		idxs = data->Read(dir_master->Get_Root_Path(true) + group_name + L".txt", group_name);
		result = data->get(idxs);
		data->clear_all();

		dir_master->Root2bound();
	}

	return result;
}

void Revise_Data(Data_Manager * data, Dir_Manager * dir_master, vector<Array> input, wstring group_name)
{
	vector<Array> result;
	vector<int> idxs;
	vector<bool> if_revise;
	
	unsigned int input_size = input.size();
	for (unsigned int i = 0; i < input_size; i++) {
		if_revise.push_back(false);
	}

	if (dir_master->Find_dir(L"Voca", true) != -1) {
		idxs = data->Read(dir_master->Get_Root_Path(true) + group_name + L".txt", group_name);
		result = data->get(idxs);
		unsigned int result_size = result.size();
		for (unsigned int i = 0; i < result_size; i++) {
			for (unsigned int j = 0; j < input_size; j++) {
				if (result[i][0] == input[j][0]) {
					if_revise[j] = data->revise(idxs[i], combine(result[i], input[j]));
					
				}
			}
		}

		for (unsigned int i = 0; i < input_size; i++) {
			if (if_revise[i] == false)
				data->putq(input[i]);
		}
		data->Write(dir_master->Get_Root_Path(true) + group_name + L".txt");
		data->clear_all();
		result.clear();
		input.clear();
	}
}

void Revise_Data_v2(Data_Manager * data, Dir_Manager * dir_master, vector<Array> input, wstring group_name)
{
	vector<Array> result;
	vector<int> idxs;
	vector<bool> if_revise;

	unsigned int input_size = input.size();
	for (unsigned int i = 0; i < input_size; i++) {
		if_revise.push_back(false);
	}

	if (dir_master->Find_dir(L"Voca", true) != -1) {
		idxs = data->Read(dir_master->Get_Root_Path(true) + group_name + L".txt", group_name);
		result = data->get(idxs);
		unsigned int result_size = result.size();
		for (unsigned int i = 0; i < result_size; i++) {
			for (unsigned int j = 0; j < input_size; j++) {
				if (result[i][0] == input[j][0]) {
					if_revise[j] = data->revise(idxs[i], input[j]);
				}
			}
		}

		data->Write(dir_master->Get_Root_Path(true) + group_name + L".txt");
		data->clear_all();
		result.clear();
		input.clear();
		dir_master->Root2bound();
	}
}

void Revise_Data_v3(Data_Manager * data, Dir_Manager * dir_master, Array input)
{
	vector<Array> result;
	vector<int> idxs;

	unsigned int input_size = input.get_size();
	wstring group_name = input[input_size - 2].get_raw();

	if (dir_master->Find_dir(L"Voca", true) != -1) {
		idxs = data->Read(dir_master->Get_Root_Path(true) + group_name + L".txt", group_name);
		result = data->get(idxs);
		unsigned int result_size = result.size();
		for (unsigned int i = 0; i < result_size; i++) {
			if (result[i][0] == input[0]) {
				data->revise(idxs[i], input);
			}
		}

		data->Write(dir_master->Get_Root_Path(true) + group_name + L".txt");
		data->clear_all();
		result.clear();
		dir_master->Root2bound();
	}
}

Array combine(Array old_data, Array new_data)
{
	Array result(new_data.get_name());

	vector<unsigned int> idx_new;
	unsigned int new_data_size = new_data.get_size();
	unsigned int old_data_size = old_data.get_size();

	for (unsigned int i = 1; i < new_data_size; i++) {
		if (new_data[i].type() != (unsigned int)Type::DEFAULT_TYPE)
			break;
		for (unsigned int j = 1; j < old_data_size; j++) {
			if (old_data[j].type() != (unsigned int)Type::DEFAULT_TYPE)
				break;
			if (new_data[i] == old_data[j]) {
				idx_new.push_back(i);
				break;
			}
		}
	}
	unsigned int idx_new_size = idx_new.size();
	unsigned int size = new_data_size + old_data_size - idx_new_size - 4;

	result.randn(1, size);
	result[0] = new_data[0];
	unsigned int count = 0;
	unsigned int count2 = 0;
	unsigned int pos_error = 0;

	for (unsigned int i = 1; i < new_data_size; i++) {
		if (new_data[i].type() != (unsigned int)Type::DEFAULT_TYPE)
			break;
		if (idx_new_size != 0) {
			if (idx_new[count] != i) {
				result[count2 + 1] = new_data[i];
				count2++;
			}
			else if (idx_new.size() - 1 != count)
				count++;
		}
		else {
			result[count2 + 1] = new_data[i];
			count2++;
		}
	}

	for (unsigned int i = 1; i < old_data_size; i++) {
		if (old_data[i].type() != (unsigned int)Type::DEFAULT_TYPE) {
			pos_error = i;
			break;
		}
		result[count2 + 1] = old_data[i];
		count2++;
	}
	result[count2 + 1] = old_data[pos_error];
	result[count2 + 2] = old_data[pos_error + 1];
	result[count2 + 3] = old_data[pos_error + 2];

	return result;
}

vector<int> Compare_Data(Data_Manager * data, Dir_Manager * dir_master, vector<Array> inputs, wstring group_name)
{
	vector<Array> result;
	vector<int> idxs;

	unsigned int input_size = inputs.size();
	if (dir_master->Find_dir(L"Voca", true) != -1) {
		idxs = data->Read(dir_master->Get_Root_Path(true) + group_name + L".txt", group_name);
		result = data->get(idxs);
		unsigned int result_size = result.size();
		data->clear_all();
		idxs.clear();
		for (unsigned int i = 0; i < input_size; i++) {
			for (unsigned int j = 0; j < result_size; j++) {
				if (inputs[i] == result[j])
					idxs.push_back(i);
			}
		}
		dir_master->Root2bound();
	}
	result.clear();
	return idxs;
}

void Setting_Initialize(Data_Manager * data, Dir_Manager * dir_master)
{
	vector<wstring> group;
	vector<int> ids;
	wstring group_name;

	dir_master->Root2bound();

	if (dir_master->Find_dir(L"Setting", true) != -1) {
		ids = data->Read(dir_master->Get_Root_Path(true) + L"Setting_Info.txt", L"Setting");

		dir_master->Root2bound();

		if (ids.size() != 0) {
			setting = data->getq(ids[0]);
			data->clear_all();
		}
		else {
			/* Set to default setting */
			setting.randn(1, 5);
			group = Group(dir_master);
			if (group.size() != 0) {
				setting[0] = group[0]; // words add group
			}
			setting[1] = 15; // timer time
			setting[2] = true; // if we use timer or not
			setting[3] = Encoder(L"000000");
			setting[4] = true; // setting is saved
		}
		
	}
}

void Setting_Save(Data_Manager * data, Dir_Manager * dir_master)
{
	dir_master->Root2bound();

	if (dir_master->Find_dir(L"Setting", true) != -1) {

		if (stoi(setting[4].get_raw()) == true) {
			data->putq(setting);
			data->Write(dir_master->Get_Root_Path(true) + L"Setting_Info.txt");
			data->clear_all();
		}

		dir_master->Root2bound();
	}
}

/* Minimum edit distance */
double Minimum_edit_distance(wstring s1, wstring s2) // Minimum edit distance
{
	double result = 0;
	int len_1 = s1.length();
	int len_2 = s2.length();

	int max_length = (len_1 >= len_2) ? len_1 : len_2;

	/* using minimum edit distance algorithm */

	/* Memory allocation */
	int ** matrix = new int*[len_1 + 1];
	for (int i = 0; i < len_1 + 1; i++)
		matrix[i] = new int[len_2 + 1];

	/* Initialization */
	for (int i = 0; i <= len_2; i++)
		matrix[0][i] = i;

	for (int i = 0; i <= len_1; i++)
		matrix[i][0] = i;


	for (int i = 1; i < len_1 + 1; i++)
	{
		for (int j = 1; j < len_2 + 1; j++)
		{
			if (s1[i - 1] == s2[j - 1])
				matrix[i][j] = matrix[i - 1][j - 1];
			else
			{
				matrix[i][j] = Minimum_value(matrix[i - 1][j], matrix[i - 1][j - 1], matrix[i][j - 1]) + 1;
			}
		}
	}

	result = 1 - (double)(matrix[len_1][len_2]) / (max_length + 0.00000000001);

	/* Memory deallocation */
	for (int i = 0; i < len_1 + 1; i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;

	return result;
}

int Minimum_value(int val_1, int val_2, int val_3) // minimum edit distance를 위한 함수
{

	if (val_1 <= val_2 && val_1 <= val_3)
		return val_1;
	else if (val_2 <= val_1 && val_2 <= val_3)
		return val_2;
	else
		return val_3;

}

double Return_similarity(wstring s1, wstring s2)
{
	vector<wstring> word_1;
	vector<wstring> word_2;

	int pos_1 = 0;
	int pos_2 = 0;

	double sum = 0;
	double result = 0;

	unsigned int s1_length = s1.length();
	unsigned int s2_length = s2.length();

	/* Make shringle */
	for (int i = 0; i < s1_length; i++)
	{
		if (s1[i] == L' ')
		{
			word_1.push_back(s1.substr(pos_1, i - pos_1));
			pos_1 = i + 1;
		}
		else if (i == s1_length - 1)
		{
			word_1.push_back(s1.substr(pos_1, i - pos_1 + 1));
			pos_1 = i;
		}
	}

	/* Make shringle 2 */
	for (int i = 0; i < s2_length; i++)
	{
		if (s2[i] == L' ')
		{
			word_2.push_back(s2.substr(pos_2, i - pos_2));
			pos_2 = i + 1;
		}
		else if (i == s2_length - 1)
		{
			word_2.push_back(s2.substr(pos_2, i - pos_2 + 1));
			pos_2 = i;
		}
	}

	unsigned int word_1_size = word_1.size();
	unsigned int word_2_size = word_2.size();

	for (int i = 0; i < word_1_size; i++)
	{
		for (int j = 0; j < word_2_size; j++)
		{
			sum += Minimum_edit_distance(word_1.at(i), word_2.at(j));
		}
	}

	result = (double)sum / (word_1_size * word_2_size);

	word_1.clear();
	word_2.clear();

	return result;
}

void Write_Time(unsigned int num) {
	FILE *fp;
	fp = fopen("time_limit.txt", "wt");

	if (fp) {
		fprintf(fp, "%d", num);
		fclose(fp);
	}
}

bool Check_Test(vector<Array> words, unsigned int idx, wstring answer, bool timer, vector<Array> & data_info, unsigned int question_type)
{
	unsigned int word_num = words.size();
	
	vector<unsigned int> word_idx;
	wstring message;
	bool correctness = false;
	unsigned int idx_answer = idx;

	if (question_type == 0) {
		word_idx.push_back(idx);
		unsigned int word_size = words[idx].get_size();
		for (unsigned int i = 1; i < word_size - 3; i++) {
			if (words[idx][i].get_raw() == answer) {
				correctness = true;
				message = L" 답안 " + answer + L"는 정답입니다.";
				break;
			}
		}
		if (correctness == false)
			message = L" 답안 " + answer + L"는 오답입니다.";
	}
	else {
		wstring real_answer = words[idx][question_type].get_raw();
		for (unsigned int i = 0; i < word_num; i++) {
			unsigned int word_size = words[i].get_size();
			for (unsigned int j = 1; j < word_size - 3; j++) {
				if (words[i][j].get_raw() == real_answer) {
					if (words[i][0].get_raw() == answer) {
						idx_answer = i;
						correctness = true;
					}
					word_idx.push_back(i);
					break;
				}
			}
		}
		if (correctness)
			message = L" 답안 " + answer + L"는 정답입니다.";
		else
			message = L" 답안 " + answer + L"는 오답입니다.";
	}
	if (timer == false) {
		correctness = false;
		message += L"\n 시간 제한안에 하지 못했습니다.";
	}

	unsigned int idx_num = word_idx.size();

	if (idx_num >= 2) {
		if (Inform_Message_v2(L"답안이 두개 이상입니다. \n 보시겠습니까?")) {
			for (unsigned int i = 0; i < idx_num; i++) {
				DisplayWord_v3(words[word_idx[i]], message, i + 1);
			}
		}
		else
			Inform_Message(message);
	}
	else {
		DisplayWord_v3(words[idx], message, 1);
	}

	/* Change the data info log */
	if (idx_answer == idx) {
		data_info[idx_answer][4] = correctness;
		data_info[idx_answer][5] = answer;
	}
	else {
		data_info[idx][4] = 2;
		data_info[idx_answer][5] = answer;
		data_info[idx_answer][4] = correctness;
		data_info[idx_answer][5] = answer;
	}

	return correctness;
}

/* Creating the password */
wstring Encoder(wstring password, int first_hash)
{
	unsigned int len = password.length();
	wchar_t * hash_codes = new wchar_t[2 * (len + 1)];
	wstring hash_result;

	/* Hash Table Generate */
	bool * hash_table = new bool[256];
	memset(hash_table, false, sizeof(bool) * 256);
	int hash_num; wchar_t code;

	if (first_hash == -1) {
		srand(time(nullptr) + clock());
		hash_num = (rand() + clock()) % 13 + 1;
	}
	else
		hash_num = first_hash;

	hash_codes[0] = (wchar_t)hash_num;

	for (unsigned int i = 0; i < len; i++) {
		hash_codes[2 * i + 1] = Hash_Generator(hash_table, hash_num, code, password[i], i);
		hash_codes[2 * i + 2] = code;
		hash_num = (int)hash_codes[2 * i + 1];
	}

	hash_codes[2 * len + 1] = L'\0';
	hash_result = hash_codes;

	delete[] hash_table;
	delete[] hash_codes;

	return hash_result;
}

bool Decoder(wstring raw_code,wstring hash_codes)
{
	if (Encoder(raw_code,(int)hash_codes[0]) == hash_codes)
		return true;
	else
		return false;
}

int Hash_Generator(bool * hash_table, int prev_hash, wchar_t & code, wchar_t msg,bool first)
{
	int msg_2 = msg % (prev_hash + 1) + 1;

	/* Activation function(sigmoid) to locate to hash table */
	if (first == false)
		srand(msg_2);

	int func_out = (int)fabs(512 * gaussianRandom()) + (int)fabs(256 * gaussianRandom()) + msg;
	func_out = func_out % 256;

	/* Hash update */
	hash_table[func_out] = true;


	/* Get the code */
	int code_num = 1;
	for (unsigned int i = 1; i < 128; i++) {
		code_num += hash_table[i] * (int)pow(2, i % 31);
	}
	code = (wchar_t)code_num;

	code_num = 1;
	for (unsigned int i = 128; i < 256; i++) {
		code_num += hash_table[i] * (int)pow(2, i % 31);
	}

	return code_num % 19;
}

double gaussianRandom(void) {
	double v1, v2, s;

	do {
		v1 = 2 * ((double)rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		v2 = 2 * ((double)rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		s = v1 * v1 + v2 * v2;
	} while (s >= 1 || s == 0);

	s = sqrt((-2 * log(s)) / s);

	return v1 * s;
}
