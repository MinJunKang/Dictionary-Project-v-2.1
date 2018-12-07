
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
	Design_Tool main_console(DEFAULT_CMD_X, DEFAULT_CMD_Y, DEFAULT_TEXT_SIZE, Create_UI_Info(L"   Dic_ver2.1 Console", 30, 2, 60, 28));

	dir_master->Update_begin();

	/* Mkdir */
	if(dir_master->Find_dir(L"Voca") == -1)
		dir_master->Create_dir(L"Voca");
	if (dir_master->Find_dir(L"Setting") == -1)
		dir_master->Create_dir(L"Setting");
	// for the training data (word, group_name, prev_error, prev_test_num, correctness(0 or 1 if not refered 2), Answer if right otherwise anycode
	if (dir_master->Find_dir(L"Train_Data") == -1) 
		dir_master->Create_dir(L"Train_Data");

	/* Initialize Setting */
	Setting_Initialize(voca_data, dir_master);
	
	/* Main Console */
	vector<int> choice;
	_2D_pos pos;
	
	pos.x = 77; pos.y = 23;
	main_console.Addbutton(L"\n  종  료 \n ", pos, Color::BROWN);
	pos.x = 37; pos.y = 9;
	main_console.Addbutton(L" 1. 단어 추가 / 검색", pos);
	pos.x = 37; pos.y = 14;
	main_console.Addbutton(L" 2. 단어 시험 시작  ", pos);
	pos.x = 37; pos.y = 19;
	main_console.Addbutton(L" 3.     설   정     ", pos);
	do{
		choice = main_console.Printdesign();
		switch (choice[0]) {
		case 2:
			Phase_1(voca_data, dir_master);
			break;
		case 3:
			Phase_2(voca_data, dir_master);
			break;
		case 4:
			Phase_3(voca_data, dir_master);
			break;
		default:
			break;
		}
	} while (choice[0] != 1);

	dir_master->Update_pause();
	Setting_Save(voca_data, dir_master);

	delete voca_data;
	delete dir_master;

	Ending();
	return 0;
}