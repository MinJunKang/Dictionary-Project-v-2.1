#ifndef _DIC_VER_2_1_
#define _DIC_VER_2_1_

#include "Console_Design.h"
#include "Data_Manager.h"
#include "Multi_Array.h"
#include "Dir_Manager.h"

/* Create UI information */
ui_type * Create_UI_Info(wstring name, unsigned int pos_x, unsigned int pos_y, unsigned int width, unsigned int height);

/*  Main UIs  */
void Phase_1(Data_Manager * data, Dir_Manager * dir_master);
void Phase_2(Data_Manager * data, Dir_Manager * dir_master);
void Phase_3(Data_Manager * data, Dir_Manager * dir_master);

/* Sub UIs */
void Inform_Message(wstring Message);
bool Inform_Message_v2(wstring Message);
vector<wstring> Group_Select(Data_Manager * data, Dir_Manager * dir_master, unsigned int max_choose = 0);
void Add_Word(Data_Manager * data, Dir_Manager * dir_master, wstring group_name);
void Search_Word(Data_Manager * data, Dir_Manager * dir_master);
bool DisplayWord(wstring word, vector<wstring> meaning, wstring group_name);
void DisplayWord_v2(Data_Manager * data, Dir_Manager * dir_master, Array word);
void DisplayWord_v3(Array word, wstring message, unsigned int idx);
void ChooseWord(Data_Manager * data, Dir_Manager * dir_master, vector<Array> words);
wstring Get_Input(wstring title, wstring Message);
void Set_cirterion(Array & criterion, unsigned int level);
/* Locker */
bool Locker();

/* UI for Phase 2 */
void Test(Data_Manager * data, Dir_Manager * dir_master, vector<Array> words);

/* UI for Phase 3 */
void Set_Timer();
void Group_Setting(Data_Manager * data, Dir_Manager * dir_master);
void Reset_Test_Info(Data_Manager * data, Dir_Manager * dir_master);
void Set_Setting();
void View_Info(Data_Manager * data, Dir_Manager * dir_master);
void Set_Password();
void Show_Group_Info(Data_Manager * data, Dir_Manager * dir_master, wstring group_name);
void Select_Add_Group(Data_Manager * data, Dir_Manager * dir_master);
void Group_Add_Delete(Data_Manager * data, Dir_Manager * dir_master);


/* Make Array for word */
Array Data2Array(wstring word, vector<wstring> meaning, double error_rate, wstring group_name, int test_num);

/* Create new group */
void Create_Group(Data_Manager * data, Dir_Manager * dir_master, wstring group_name);
/* Delete the group */
void Delete_Group(Data_Manager * data, Dir_Manager * dir_master, wstring group_name);

/* Get the list of groups */
vector<wstring> Group(Dir_Manager * dir_master);

/* Get the data of group */
vector<Array> Get_Group_Data(Data_Manager * data, Dir_Manager * dir_master, wstring group_name);

/* Compare the data with the data of group */
void Revise_Data(Data_Manager * data, Dir_Manager * dir_master, vector<Array> input, wstring group_name); // combine the input
void Revise_Data_v2(Data_Manager * data, Dir_Manager * dir_master, vector<Array> input, wstring group_name); // just copy the input
void Revise_Data_v3(Data_Manager * data, Dir_Manager * dir_master, Array input);

/* Combine the meaning with new data */
Array combine(Array old_data, Array new_data);

/* Setting Initialize */
void Setting_Initialize(Data_Manager * data, Dir_Manager * dir_master);
/* Auto setting save */
void Setting_Save(Data_Manager * data, Dir_Manager * dir_master);

/* Search Algorithm */
int Minimum_value(int val_1, int val_2, int val_3);
double Minimum_edit_distance(wstring s1, wstring s2);
double Return_similarity(wstring s1, wstring s2);

vector<Array> Search(Data_Manager * data, Dir_Manager * dir_master, wstring word, vector<wstring> group_name);

/* Word classification */
vector<Array> classify_word(Data_Manager * data, Dir_Manager * dir_master, vector<wstring> group_name, Array & criterion);

/* Change the meaning vector */
vector<wstring> change_meaning(vector<wstring> prev_value, wstring input, unsigned int pos);


/* Create Process */
PROCESS_INFORMATION & Create_Timer();
void Write_Time(unsigned int num);

/* Confirm the test result */
bool Check_Test(vector<Array> words, unsigned int idx, wstring answer, bool timer, vector<Array> & data_info, unsigned int question_type);

/* Password */
wstring Encoder(wstring password, int first_hash = -1); // secret hash code generator
bool Decoder(wstring raw_code, wstring hash_codes); // raw code를 encoding 한 후 결과가 hash gode와 맞으면 open
int Hash_Generator(bool * hash_table, int prev_hash, wchar_t & code, wchar_t msg, bool first);
double gaussianRandom(void);



#endif
