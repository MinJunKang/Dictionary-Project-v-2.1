#ifndef _DIR_MANAGERV2_
#define _DIR_MANAGERV2_

#include "defs.h"
#include "Dir_Type.h"

/*

root directory를 갱신

1. 특정 directory 안의 모든 파일 이름, 확장자를 저장(p tree 구조)
2. Update
3. Fast Finder(특정 파일을 빠르게 찾는 기능)

모든 경로는 상대경로
.add root_directory
.get_pid(wstring * folder_name) // int형 반환 특정 directory의 pid 반환
.update(wstring * root_directory) // filename 하의 모든 폴더를 tree화 한다.

*/

class Dir_Manager {

private:
	/* Stack Variable */
	vector<Dir_Type *> DirTable;
	vector<Thread_info> dir_thread;
	int current_root_pid;
	wstring Upper_bound_path;

	/* Allocation of new data to DirTable */
	unsigned int Update_Data(Dir_Type new_data);
	unsigned int Alloc_Data(Dir_Type * new_data = nullptr);
	void Dealloc_Data(unsigned int id, bool kill_all = false);

	/* Base Function */
	wstring To_wstr(const string & str);
	string To_str(const wstring & wstr);
	unsigned int Distance(unsigned int pid_1, unsigned int pid_2);

	/* Dir or File Manage Function */
	unsigned int isFileorDir(wstring name);
	vector<wstring> Split_path(wstring & path);
	wstring To_stdPath(wstring path);

	/* Convert between path and pid */
	wstring pid2Path(unsigned int dir_pid);
	int Path2pid(wstring path);

	/* Find the directory and files in current directory */
	vector<wstring> Get_Object(wstring & current_dir); // Find all the files and dir under the current dir


	/* Update Function */
	unsigned int Update_dir(wstring path, int parent_id);
	bool Check_Exclusive(wstring & dir_name);


	void Update_freq();
	/*
	Contents:
	1. run Update() for every minute
	2. for thread
	*/

	void Update(bool root2base = false);
	/*
	Contents :
	1. Update all the hierarchy and rearrange it
	2. If the current path exits after update, maintain it
	3. Otherwise, it will be reset to highest path
	*/

	/* Stack Variable for Update */
	bool update_state; // check if update utility is working
	unsigned int update_interval; // set the period of update
	thread m_th; // for threading
	mutex mtx; // for threading

	/* Thread Allocation and Deallocation */
	template <typename T>
	int Alloc_thread(void (Dir_Manager::*func)(int &, vector<T> &), vector<T> & input);
	int Alloc_thread(void (Dir_Manager::*func)(int &));
	bool Dealloc_thread(int id);
	int Get_valid_id();
	bool IsValid_thread(int id);

	mutex _lock;
	const static int time_lock = 10;

	/* Thread function */
	void FILE_Copy_thread(int & control_id, vector<wstring> & inputs);
	void FILE_Move_thread(int & control_id, vector<wstring> & inputs);
	void FILE_Delete_thread(int & control_id, vector<wstring> & inputs);
public:

	bool updating; // check if the directory is now updating
	unsigned int update_expected_time; // expecting time to be updated

	Dir_Manager(wstring Upper_bound_path);
	~Dir_Manager();

	wstring Path2name(wstring path);
	/*
	Contents :
	extract name from path
	*/

	wstring Get_pid_path(int dir_pid, bool with_format = false);
	/*
	Contents :
	Find path from pid
	*/

	void Set_bound_path(wstring Upper_bound_path);
	/*
	Contents :
	Change upper_bound_path
	*/

	/* Get Root */
	int Get_Root_pid();
	/*
	Contents :
	Get current root_dir's pid
	*/

	/* Get Root path */
	wstring Get_Root_Path(bool with_format = false);
	/*
	Contents :
	Get current root_dir's path
	*/

	/* Set pid's path to root */
	int Set_Root_pid(unsigned int dir_pid); //**
	/*
	Contents :
	Set dir_pid's dir to root dir
	*/

	/* Set pid's path to root */
	int Root2bound(); //**
	/*
	Contents :
	Set dir_pid's dir to Upper bound root
	*/

	/* Find path's pid */
	int Find_path(wstring path, bool set_root = 0); // **
	/*
	Contents :
	1. Find path's directory
	2. Return pid if it hits, otherwise -1
	3. set to root or not is determined by 3rd parameter
	*/

	/* Find dir name's pid */
	int Find_dir(wstring dir_name, bool set_root = false, bool exact = true);
	/*
	Contents :
	1. Find dir_name's directory
	2. if it success, return dir_name's pid, otherwise return -1
	3. set to root or not is determined by 3rd parameter
	4. Return closest pid if there is two or more dir
	*/

	/* Show Current dir's under dir */
	vector<wstring> ls();
	/*
	Contents :
	1. Show all the dir name of current root's below
	*/

	int Go_upper(bool set_upper_bound = false);
	/*
	Contents :
	Go to current root dir's upper dir
	1. if it success, return upper dir's pid otherwise return -1
	*/

	int Go_lower(wstring dir_name);
	/*
	Contents :
	Go to current root dir's posth lower dir(by name)
	1. if it success, return lower dir's pid otherwise return -1 (it means there's no matched dir to pid)
	*/

	int Go_lower(int pos);
	/*
	Contents :
	Go to current root dir's posth lower dir(by pos)
	1. if it success, return lower dir's pid otherwise return -1 (it means there's no matched dir to pid)
	*/

	vector<wstring> Get_Root_file();
	/*
	Contents :
	1. Find all files in current root dir and return them as path
	*/

	void Update_begin(unsigned int time_interval = 5);
	/*
	Contents :
	1. Start Update(time interval : seconds)
	*/

	void Update_pause();
	/*
	Contents :
	1. Pause Update
	*/

	bool Create_dir(wstring dir_name);
	/*
	Contents :
	1. Create new directory under the current root path
	2. If creation is successful, return true otherwise false
	*/

	bool Delete_dir(wstring dir_name, bool remove_files = false);
	/*
	Contents :
	1. Delete directory under the current root path
	2. If deletion is successful, return true otherwise false
	3. Deletion is only possible for empty directory
	*/

	/* File Manage Function */
	// 1. FILE Copy
	void FILE_Copy(vector<pair<wstring, wstring>> file_path, bool remove_previous = false); // original file, dst file
	void FILE_Copy(wstring src_file, wstring dst_file, bool remove_previous = false);
	// 2. FILE Merge
	void FILE_Merge(vector<wstring> src_file, wstring dst_dir, bool remove_original = false); // original file, dst file
	// 3. FILE Move
	void FILE_Move(vector<pair<wstring, wstring>> file_path); // original file, dst file
	void FILE_Move(wstring src_file, wstring dst_file);
	// 4. FILE Delete
	void FILE_Delete(vector<wstring> files);
	void FILE_Delete(wstring file);
	// 5. IsFile / IsDir
	bool IsFile(wstring file);
	bool IsDir(wstring dir);
};

template<typename T>
inline int Dir_Manager::Alloc_thread(void(Dir_Manager::* func)(int &, vector<T>&), vector<T>& input)
{
	int new_thread_id;

	new_thread_id = Get_valid_id();

	if (new_thread_id < 0) // create new space to allocate new thread
	{
		Thread_info new_info;
		this->dir_thread.push_back(new_info);
		this->dir_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id), std::ref(input));
		this->dir_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
	else
	{
		this->dir_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id), std::ref(input));
		this->dir_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
}


#endif
