
#include "Dic_Ver2.1.h"

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#include <crtdbg.h>


int main(int argc, char * argv[]) {

	/* Memory Check code */
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(4613);
#endif

	Setting();

	Data_Manager * voca_data = new Data_Manager();
	Dir_Manager * dir_master = new Dir_Manager(To_wstr(argv[0]));
	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"   Dic_ver2.1 Console", 27, 2, 68, 28));

	//dir_master->Update_begin();

	/* Mkdir */
	if(dir_master->Find_dir(L"Voca") == -1)
		dir_master->Create_dir(L"Voca");
	if (dir_master->Find_dir(L"Setting") == -1)
		dir_master->Create_dir(L"Setting");
	// for the training data (word, group_name, prev_error, prev_test_num, correctness(0 or 1 if not refered 2), Answer if right otherwise anycode
	if (dir_master->Find_dir(L"Train_Data") == -1) 
		dir_master->Create_dir(L"Train_Data");
	if (dir_master->Find_dir(L"QUICKCARD") == -1)
		dir_master->Create_dir(L"QUICKCARD");
	else {
		dir_master->Find_dir(L"QUICKCARD", true);
		if (dir_master->IsFile(L"DATA.txt") == false) {
			voca_data->Write(L"DATA.txt");
		}
		else {
			dir_master->Root2bound();
		}
	}

	/* Initialize Setting */
	Setting_Initialize(voca_data, dir_master);
	
	/* Main Console */
	vector<int> choice;
	_2D_pos pos;
	
	pos.x = 79; pos.y = 23;
	unsigned int idx_1 = main_console.Addbutton(L"\n    종  료  \n ", pos, Color::BROWN);
	pos.x = 30; pos.y = 9;
	unsigned int idx_2 = main_console.Addbutton(L" 1. 단어 추가 / 검색", pos);
	pos.x = 30; pos.y = 14;
	unsigned int idx_3 = main_console.Addbutton(L" 2. 단어 시험 시작  ", pos);
	pos.x = 30; pos.y = 19;
	unsigned int idx_4 = main_console.Addbutton(L" 3.     설   정     ", pos);

	/* Make UI for QUICK MEMORY CARD */
	vector<Array> words;
	vector<unsigned int> word_idx;
	vector<bool> word_check;
	vector<wstring> views;
	unsigned int view_num = 0;

	Array card_setting(L"card setting"); 
	// (forward, backward, random), (include record or not), (k - e or e - k or random), group names ...
	/* Set the basic setting */
	card_setting.randn(1, 4);
	card_setting[0] = 2; // random
	card_setting[1] = 0; // not include
	card_setting[2] = 1; // e - k way
	card_setting[3] = basic_group();

	pos.x = 86; pos.y = 6;
	unsigned int idx_5 = main_console.Addbutton(L"※설정", pos);
	pos.x = 57; pos.y = 19;
	unsigned int idx_6 = main_console.Addbutton(L" ◀", pos);
	pos.x = 65; pos.y = 19;
	unsigned int idx_7 = main_console.Addbutton(L" ▶", pos);

	/* button for case by case */
	pos.x = 85; pos.y = 19;
	unsigned int idx_8 = main_console.Addbutton(L"   ", pos);
	pos.x = 72; pos.y = 15;
	unsigned int idx_9 = main_console.Addbutton(L"  + ", pos);

	pos.x = 55; pos.y = 6;
	main_console.AddMessage(L"┌-----------------------------", pos);
	pos.x = 55; pos.y = 22;
	main_console.AddMessage(L"└-------------------------------------*", pos);
	pos.x = 57; pos.y = 9;
	main_console.AddMessage(L"-------------------------------┘", pos);
	pos.x = 57; pos.y = 18;
	main_console.AddMessage(L"------------------------------------", pos);
	pos.x = 75; pos.y = 20;
	main_console.AddMessage(L" 외운 단어", pos);

	for (short i = 7; i < 22; i++) {
		pos.x = 55; pos.y = i;
		main_console.AddMessage(L"│", pos);
	}
	for (short i = 9; i < 22; i++) {
		pos.x = 93; pos.y = i;
		main_console.AddMessage(L"│", pos);
	}

	bool check = false;
	bool hidden = true;
	int present_word_idx = 0; // default to 0 if choose_word called!

	// forward
	words = Forward_word(voca_data, dir_master, card_setting, word_idx, word_check);
	select_next_idx(word_idx, word_check, present_word_idx, card_setting);
	select_prev_idx(word_idx, word_check, present_word_idx, card_setting);
	
	if (words.size() != 0 && present_word_idx != -1) {
		check = word_check[word_idx[present_word_idx]];
	}
	//arrange_idx(word_idx, present_word_idx, card_setting);

	vector<unsigned int> idx_msg_view;
	
	while(1){
		views.clear();

		/* view the word */
		if (words.size() != 0 && present_word_idx != -1) {
			views = view_word(word_idx, words, present_word_idx, card_setting);
			view_num = views.size();

			/* view first thing */
			pos.x = 57; pos.y = 7;
			idx_msg_view.push_back(main_console.AddMessage(adapt_message(views[0], 29), pos));

			/* and some seconds... */
			if (hidden == false) {
				if (view_num <= 3) {
					pos.x = 57; pos.y = 10;
					for (unsigned int i = 1; i < view_num; i++) {
						idx_msg_view.push_back(main_console.AddMessage(adapt_message(views[i], 29), pos));
						pos.y += 2;
					}
				}
				else {
					pos.x = 57; pos.y = 10;
					idx_msg_view.push_back(main_console.AddMessage(adapt_message(views[1], 29), pos));
					pos.x = 57; pos.y = 12;
					idx_msg_view.push_back(main_console.AddMessage(adapt_message(views[2], 29), pos));
					pos.x = 77; pos.y = 13;
					idx_msg_view.push_back(main_console.AddMessage(L"...", pos));
				}
			}
		}
		else {
			pos.x = 57; pos.y = 7;
			idx_msg_view.push_back(main_console.AddMessage(L"해당 단어가 없습니다.", pos));
		}

		choice = main_console.Printdesign();

		if (choice[0] == idx_1) {
			if (words.size() != 0 && present_word_idx != -1) {
				Backward_word(voca_data, dir_master, words[word_idx[present_word_idx]], check);
			}
			break;
		}
		else if (choice[0] == idx_2) {
			Phase_1(voca_data, dir_master); // choose_word called
			card_setting[3] = basic_group();
			if (words.size() != 0 && present_word_idx != -1) {
				Backward_word(voca_data, dir_master, words[word_idx[present_word_idx]], check);
			}
			present_word_idx = 0;
			words = Forward_word(voca_data, dir_master, card_setting, word_idx, word_check);
			select_next_idx(word_idx, word_check, present_word_idx, card_setting);
			select_prev_idx(word_idx, word_check, present_word_idx, card_setting);
			if (words.size() != 0 && present_word_idx != -1) {
				check = word_check[word_idx[present_word_idx]];
			}
			else
				check = false;
			arrange_idx(word_idx, present_word_idx, card_setting);
		}
		else if(choice[0] == idx_3)
			Phase_2(voca_data, dir_master);
		else if (choice[0] == idx_4) {
			Phase_3(voca_data, dir_master); // choose_word called
			card_setting[3] = basic_group();
			if (words.size() != 0 && present_word_idx != -1) {
				Backward_word(voca_data, dir_master, words[word_idx[present_word_idx]], check);
			}
			present_word_idx = 0;
			words = Forward_word(voca_data, dir_master, card_setting, word_idx, word_check);
			select_next_idx(word_idx, word_check, present_word_idx, card_setting);
			select_prev_idx(word_idx, word_check, present_word_idx, card_setting);
			if (words.size() != 0 && present_word_idx != -1) {
				check = word_check[word_idx[present_word_idx]];
			}
			else
				check = false;
			arrange_idx(word_idx, present_word_idx, card_setting);
		}
		else if (choice[0] == idx_5) {
			if (card_setting_UI(voca_data, dir_master, card_setting)) {
				if (words.size() != 0 && present_word_idx != -1) {
					Backward_word(voca_data, dir_master, words[word_idx[present_word_idx]], check);
				}
				present_word_idx = 0;
				words = Forward_word(voca_data, dir_master, card_setting, word_idx, word_check);
				select_next_idx(word_idx, word_check, present_word_idx, card_setting);
				select_prev_idx(word_idx, word_check, present_word_idx, card_setting);
				
				if (words.size() != 0 && present_word_idx != -1) {
					check = word_check[word_idx[present_word_idx]];
				}
				else
					check = false;
			}
			arrange_idx(word_idx, present_word_idx, card_setting);
		}
		else if (choice[0] == idx_6) {
			hidden = true;

			// goto before word
			if (words.size() != 0 && present_word_idx != -1) {
				word_check[word_idx[present_word_idx]] = check;
				Backward_word(voca_data, dir_master, words[word_idx[present_word_idx]], check);
				select_prev_idx(word_idx, word_check, present_word_idx, card_setting);
				if (words.size() != 0 && present_word_idx != -1) {
					check = word_check[word_idx[present_word_idx]];
				}
				else
					check = false;
			}

		}
		else if (choice[0] == idx_7) {
			if (hidden) {
				hidden = false;
			}
			else {

				if (words.size() != 0 && present_word_idx != -1) {
					word_check[word_idx[present_word_idx]] = check;
					Backward_word(voca_data, dir_master, words[word_idx[present_word_idx]], check);
					select_next_idx(word_idx, word_check, present_word_idx, card_setting);
					if (words.size() != 0 && present_word_idx != -1) {
						check = word_check[word_idx[present_word_idx]];
					}
					else
						check = false;
				}

				hidden = true;
			}
		}
		else if (choice[0] == idx_8) { //
			pos.x = 85; pos.y = 19;
			main_console.DeleteUI(idx_8);
			if (check == false) {
				idx_8 = main_console.Addbutton(L" √", pos);
				check = true;
			}
			else {
				main_console.Addbutton(L"   ", pos);
				check = false;
			}
		}
		else if (choice[0] == idx_9) { //
			if (views.size() != 0 && hidden == false && present_word_idx != -1) {
				Display_Word(views, words[word_idx[present_word_idx]]);
			}
		}

		for (unsigned int i = 0; i < idx_msg_view.size(); i++) {
			main_console.DeleteUI(idx_msg_view[i]);
		}
		idx_msg_view.clear();

		pos.x = 85; pos.y = 19;
		main_console.DeleteUI(idx_8);
		if (check) {
			idx_8 = main_console.Addbutton(L" √", pos);
		}
		else {
			main_console.Addbutton(L"   ", pos);
		}
	}

	//dir_master->Update_pause();
	Setting_Save(voca_data, dir_master);

	delete voca_data;
	delete dir_master;

	Ending();
	return 0;
}