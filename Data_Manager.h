#ifndef __DATA_MANAGE__
#define __DATA_MANAGE__

#include "defs.h"
#include "Multi_Array.h"

class Data_Manager {
private:
	vector<Array *> data;

	/* Basic Function */
	wstring To_wstr(const string & str);
	string To_str(const wstring & wstr);

	/* Finding idx of array by name */
	int Find_idxq(wstring name, bool exact = true); // only for the error check
	vector<int> Find_idx(wstring name, bool exact = true);

	/* Declare the type of data */
	int Declare_Type(wstring str);

	/* Data Register and Manage */
	int Data_Alloc(Array * new_data = nullptr);
	// 1. Data Alloc
	bool Data_Dealloc(int id);
	// 2. Data DeAlloc
	int Get_valid_id_data();

	/* Thread Manage */
	vector<Thread_info> data_thread;

	template <typename T>
	int Thread_Alloc(void (Data_Manager::*func)(int &, vector<T> &), vector<T> & input);
	int Thread_Alloc(void (Data_Manager::*func)(int &));
	bool Thread_Dealloc(int id);
	int Get_valid_id_thread();
	bool IsValid(int id);

	mutex _lock;
public:
	/* Constructor and Destructor */
	~Data_Manager();

	/* Data allocation */
	/* Find with idx or unique name(or category) */
	int putq(Array new_data);
	vector<int> put(vector<Array> new_data);

	/* Data revise */
	/* Find with idx or unique name(or category) */
	bool revise(int idx, Array data);
	bool revise(wstring name, Array data, bool exact = true);
	void clear_all();

	/* Data deallocation */
	/* Find with idx or unique name(or category) */
	bool erase(int idx);
	bool erase(wstring name, bool exact = true);

	/* Get data */
	Array & getq(int idx);
	vector<Array> get(vector<int> idx);
	vector<Array> get(wstring name, bool exact = true);

	/* Check if the data exists */
	bool valid(int idx);
	bool valid(wstring name, bool exact = true);

	/* Get the size of data */
	unsigned int size(); // It doesn't care about the actual existence of data

	/* I/O Function */
	vector<int> Read(const char * filename, wstring name, vector<unsigned int> * decoder = nullptr, unsigned int start_pt = 0, unsigned int num = 0);
	vector<int> Read(string filename, wstring name, vector<unsigned int> * decoder = nullptr, unsigned int start_pt = 0, unsigned int num = 0);
	vector<int> Read(wstring filename, wstring name, vector<unsigned int> * decoder = nullptr, unsigned int start_pt = 0, unsigned int num = 0);
	// 1. Read Function : output will be N dim vector, can set begin and end file pointer to read

	void Write(const char * filename, Array & in, const char * mode = "wb");
	void Write(string filename, Array & in, const char * mode = "wb");
	void Write(wstring filename, Array & in, const char * mode = "wb");
	void Write(const char * filename, vector<Array> in, const char * mode = "wb");
	void Write(string filename, vector<Array> in, const char * mode = "wb");
	void Write(wstring filename, vector<Array> in, const char * mode = "wb");
	void Write(const char * filename, const char * mode = "wb");
	void Write(string filename, const char * mode = "wb");
	void Write(wstring filename, const char * mode = "wb");
	void Write(const char * filename, vector<int> idxs, const char * mode = "wb");
	void Write(string filename, vector<int> idxs, const char * mode = "wb");
	void Write(wstring filename, vector<int> idxs, const char * mode = "wb");
	void Write(const char * filename, wstring name, bool exact = true, const char * mode = "wb");
	void Write(string filename, wstring name, bool exact = true, const char * mode = "wb");
	void Write(wstring filename, wstring name, bool exact = true, const char * mode = "wb");
	// 2. Write Function : input is N dim vector and it will be saved as pickle format

};

#endif

