#include "Data_Manager.h"

wstring Data_Manager::To_wstr(const string & str)
{
	unsigned int required_len = str.length();

	wchar_t * dest = new wchar_t[required_len + 1];

	mbstowcs(dest, str.c_str(), required_len);

	dest[required_len] = L'\0';

	wstring converted_str(dest);

	delete[] dest;

	return converted_str;
}

string Data_Manager::To_str(const wstring & wstr)
{
	unsigned int required_len = 0;
	wchar_t buf[2];
	char dest_buf[2];
	buf[1] = L'\0';

	unsigned int len = wstr.length();

	for (int i = 0; i < len; i++) {
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

int Data_Manager::Find_idxq(wstring name, bool exact)
{
	if (exact) {
		for (int i = 0; i < this->data.size(); i++) {
			if (this->data[i] != nullptr)
				if (this->data[i]->get_name() == name)
					return i;
		}
	}
	else {
		for (int i = 0; i < this->data.size(); i++) {
			if (this->data[i] != nullptr)
				if (this->data[i]->get_name().find(name) != wstring::npos)
					return i;
		}
	}
	return -1;
}

vector<int> Data_Manager::Find_idx(wstring name, bool exact)
{
	vector<int> idxs;
	if (exact) {
		for (int i = 0; i < this->data.size(); i++) {
			if (this->data[i] != nullptr)
				if (this->data[i]->get_name() == name)
					idxs.push_back(i);
		}
	}
	else {
		for (int i = 0; i < this->data.size(); i++) {
			if (this->data[i] != nullptr)
				if (this->data[i]->get_name().find(name) != wstring::npos)
					idxs.push_back(i);
		}
	}

	return idxs;
}


int Data_Manager::Declare_Type(wstring str) {
	wchar_t c;
	unsigned int type = 0; // wstring(default)
	for (int i = 0; i < str.length(); i++) {
		c = str[i];
		if (isdigit(c) && type == 0)
			type = 9; // Long Long int
		else if (type == 9 && c == '.')
			type = 13; // Long double
		else if(!isdigit(c)){
			type = 0;
			break;
		}
	}
	return type;
}

int Data_Manager::Data_Alloc(Array * new_data)
{
	int new_data_id = Get_valid_id_data();

	if (new_data == nullptr)
		Print_Error(L"Data_Manager", L"DTM0002"); // Error - Data_Alloc : should allocate the memory

	if (new_data_id < 0) // create new space to allocate new thread
	{
		this->data.push_back(new_data);
		return abs(new_data_id) - 1;
	}
	else
	{
		this->data[abs(new_data_id) - 1] = new_data;
		return abs(new_data_id) - 1;
	}
}

bool Data_Manager::Data_Dealloc(int id)
{
	if (this->data.size() <= id || id < 0)
		return false;

	if (this->data[id] != nullptr)
	{
		delete this->data[id];
		this->data[id] = nullptr;
	}
	else
		return false;

	if (id == this->data.size() - 1) {
		this->data.pop_back();
	}

	return true;
}

int Data_Manager::Get_valid_id_data()
{
	if (this->data.size() == 0)
		return -1; // there is no valid id, system creates new space to allocate id
	else
	{
		for (unsigned int i = 0; i < this->data.size(); i++)
		{
			if (this->data[i] == nullptr)
				return i + 1;
		}

		return -(this->data.size() + 1); // there is no valid id, system creates new space to allocate id
	}
}

/* Thread function */
int Data_Manager::Thread_Alloc(void (Data_Manager::*func)(int &))
{
	int new_thread_id;

	new_thread_id = Get_valid_id_thread();

	if (new_thread_id < 0) // create new space to allocate new thread
	{
		Thread_info new_info;
		this->data_thread.push_back(new_info);
		this->data_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id));
		this->data_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
	else
	{
		this->data_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id));
		this->data_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
}

template <typename T>
int Data_Manager::Thread_Alloc(void (Data_Manager::*func)(int &, vector<T> &), vector<T> & input) {
	int new_thread_id;

	new_thread_id = Get_valid_id_thread();

	if (new_thread_id < 0) // create new space to allocate new thread
	{
		Thread_info new_info;
		this->data_thread.push_back(new_info);
		this->data_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id), std::ref(input));
		this->data_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
	else
	{
		this->data_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id), std::ref(input));
		this->data_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
}

bool Data_Manager::Thread_Dealloc(int id)
{
	if (this->data_thread.size() <= id || id < 0)
		return false;

	if (this->data_thread[id].state)
	{
		this->data_thread[id].put_state(false);
		while (1) {
			if (this->data_thread[id].id->joinable() == true) {
				break;
			}
		}
		if (this->data_thread[id].id->joinable()) {
			this->data_thread[id].id->join();
			delete this->data_thread[id].id;
			this->data_thread[id].id = nullptr;
		}
	}
	else
		return false;

	if (id == this->data_thread.size() - 1) {
		this->data_thread.pop_back();
	}

	return true;
}

bool Data_Manager::IsValid(int id) {
	if (this->data_thread.size() <= id)
		return false;
	else
		if (this->data_thread[id].id == nullptr)
			return false;
		else
			return this->data_thread[id].state;
}

int Data_Manager::Get_valid_id_thread()
{
	if (this->data_thread.size() == 0)
		return -1; // there is no valid id, system creates new space to allocate id
	else
	{
		for (unsigned int i = 0; i < this->data_thread.size(); i++)
		{
			if (this->data_thread[i].state.load() == false)
				return i + 1;
		}

		return -(this->data_thread.size() + 1); // there is no valid id, system creates new space to allocate id
	}
}


/* Destructor */
Data_Manager::~Data_Manager()
{
	for (unsigned int id = 0; id < this->data_thread.size(); id++)
	{
		this->data_thread[id].state.store(false);
		if (this->data_thread[id].id != nullptr) {
			if (this->data_thread[id].id->joinable()) {
				this->data_thread[id].id->join();
				delete this->data_thread[id].id;
				this->data_thread[id].id = nullptr;
			}
		}
	}
	this->data_thread.clear();

	for (int i = 0; i < this->data.size(); i++) {
		if (this->data[i] != nullptr)
			delete this->data[i];
	}
	this->data.clear();
}

int Data_Manager::putq(Array data)
{
	if (!data.dim())
		Print_Error(L"Data_Manager", L"DMR0005");

	Array * new_data = new Array(data.get_name());

	*new_data = data;

	return Data_Alloc(new_data);
}

vector<int> Data_Manager::put(vector<Array> new_data)
{
	vector<int> idxs;

	for (int i = 0; i < new_data.size(); i++) {
		idxs.push_back(putq(new_data[i]));
	}

	return idxs;
}

bool Data_Manager::revise(int idx, Array data)
{
	if (!valid(idx)) {
		Print_Error(L"Data_Manager", L"DMR0002", false);
		return false;
	}
	if (!data.dim())
		Print_Error(L"Data_Manager", L"DMR0003");

	*this->data[idx] = data;
	return true;
}

bool Data_Manager::revise(wstring name, Array data, bool exact)
{
	if (!valid(name, exact)) {
		Print_Error(L"Data_Manager", L"DMR0002", false);
		return false;
	}
	if (!data.dim())
		Print_Error(L"Data_Manager", L"DMR0004");

	vector<int> idxs = Find_idx(name, exact);
	for (int i = 0; i < idxs.size(); i++) {
		*this->data[idxs[i]] = data;
	}
	return true;
}

void Data_Manager::clear_all()
{
	for (unsigned int i = 0; i < data.size(); i++) {
		if (data[i] != nullptr)
			delete data[i];
	}
	data.clear();
}

bool Data_Manager::erase(int idx)
{
	if (!valid(idx)) {
		Print_Error(L"Data_Manager", L"DMR0006", false);
		return false;
	}

	return Data_Dealloc(idx);
}

bool Data_Manager::erase(wstring name, bool exact)
{
	if (!valid(name, exact)) {
		Print_Error(L"Data_Manager", L"DMR0006", false);
		return false;
	}

	vector<int> idxs = Find_idx(name, exact);
	for (int i = 0; i < idxs.size(); i++) {
		if (Data_Dealloc(idxs[i]) == false)
			Print_Error(L"Data_Manager", L"DMR0007");
	}

	return true;
}

Array & Data_Manager::getq(int idx)
{
	if (!valid(idx))
		Print_Error(L"Data_Manager", L"DMR0000");

	return *data[idx];
}

vector<Array> Data_Manager::get(vector<int> idx)
{
	vector<Array> result;

	for (int i = 0; i < idx.size(); i++) {
		result.push_back(getq(idx[i]));
	}

	return result;
}

vector<Array> Data_Manager::get(wstring name, bool exact)
{
	if (!valid(name, exact))
		Print_Error(L"Data_Manager", L"DMR0001");
	vector<Array> result;
	vector<int> idxs = Find_idx(name, exact);

	for (int i = 0; i < idxs.size(); i++) {
		result.push_back(*this->data[idxs[i]]);
	}
	return result;
}

bool Data_Manager::valid(int idx)
{
	if (this->data.size() <= idx || idx < 0)
		return false;
	if (this->data[idx] == nullptr)
		return false;
	return true;
}

bool Data_Manager::valid(wstring name, bool exact)
{
	if (Find_idxq(name, exact) == -1)
		return false;
	else
		return true;
}

unsigned int Data_Manager::size()
{
	return this->data.size();
}

vector<int> Data_Manager::Read(const char * filename, wstring name, vector<unsigned int>* decoder, unsigned int start_pt, unsigned int num)
{
	FILE * fp;

	vector<int> result;
	wchar_t c;
	wstring tmp_str;
	unsigned int dim;
	vector<unsigned int> size;
	Array * new_arr = nullptr;
	fp = fopen(filename, "rb");
	unsigned int tmp_size;
	unsigned int count = 0;
	unsigned int count_element = 0;
	bool if_alloc = false;
	bool lock = false;

	if (decoder == nullptr) {
		decoder = new vector<unsigned int>;
		if_alloc = true;
	}
	if (start_pt != 0 || num != 0)
		lock = true;

	if (fp) {

		while (!feof(fp)) {
			c = fgetwc(fp);
			if (lock) {
				if (c == L'\n')
					count++;
				if (count == start_pt + 1) {
					count = 0;
					lock = false;
					goto P1;
				}
			}
			else {
				if (c == L'\n') {
				P1:
					count_element = 0;
					size.clear();
					decoder->clear();
					if (new_arr != nullptr)
						result.push_back(Data_Alloc(new_arr));

					dim = (unsigned int)fgetwc(fp);
					for (int i = 0; i < dim; i++) {
						fgetwc(fp); tmp_size = (unsigned int)fgetwc(fp);
						size.push_back(tmp_size);
					}
					new_arr = new Array(name + to_wstring(count));
					new_arr->init(size);
					count++;
					tmp_size = 1;
					for (int i = 0; i < size.size(); i++) {
						tmp_size *= size[i];
					}
					if (num != 0 && num + 1 == count) {
						lock = true;
						break;
					}
				}
				else if (c == L'\t') {
					if (decoder->size() != tmp_size) {
						decoder->push_back(Declare_Type(tmp_str));
					}
					new_arr->put_value(tmp_str, count_element, decoder->at(count_element));
					tmp_str.clear();
					count_element++;
				}
				else {
					tmp_str.push_back(c);
				}
			}
		}

		if (lock == false) {
			count_element = 0;
			size.clear();
			if (new_arr != nullptr)
				result.push_back(Data_Alloc(new_arr));
		}

		fclose(fp);
	}

	if (if_alloc)
		delete decoder;

	return result;
}

vector<int> Data_Manager::Read(string filename, wstring name, vector<unsigned int>* decoder, unsigned int start_pt, unsigned int num)
{
	FILE * fp;

	vector<int> result;
	wchar_t c;
	wstring tmp_str;
	unsigned int dim;
	vector<unsigned int> size;
	Array * new_arr = nullptr;
	fp = fopen(filename.c_str(), "rb");
	unsigned int tmp_size;
	unsigned int count = 0;
	unsigned int count_element = 0;
	bool if_alloc = false;
	bool lock = false;

	if (decoder == nullptr) {
		decoder = new vector<unsigned int>;
		if_alloc = true;
	}
	if (start_pt != 0 || num != 0)
		lock = true;

	if (fp) {

		while (!feof(fp)) {
			c = fgetwc(fp);
			if (lock) {
				if (c == L'\n')
					count++;
				if (count == start_pt + 1) {
					count = 0;
					lock = false;
					goto P1;
				}
			}
			else {
				if (c == L'\n') {
				P1:
					count_element = 0;
					size.clear();
					decoder->clear();
					if (new_arr != nullptr)
						result.push_back(Data_Alloc(new_arr));

					dim = (unsigned int)fgetwc(fp);
					for (int i = 0; i < dim; i++) {
						fgetwc(fp); tmp_size = (unsigned int)fgetwc(fp);
						size.push_back(tmp_size);
					}
					new_arr = new Array(name + to_wstring(count));
					new_arr->init(size);
					count++;
					tmp_size = 1;
					for (int i = 0; i < size.size(); i++) {
						tmp_size *= size[i];
					}
					if (num != 0 && num + 1 == count) {
						lock = true;
						break;
					}
				}
				else if (c == L'\t') {
					if (decoder->size() != tmp_size) {
						decoder->push_back(Declare_Type(tmp_str));
					}
					new_arr->put_value(tmp_str, count_element, decoder->at(count_element));
					tmp_str.clear();
					count_element++;
				}
				else {
					tmp_str.push_back(c);
				}
			}
		}

		if (lock == false) {
			count_element = 0;
			size.clear();
			if (new_arr != nullptr)
				result.push_back(Data_Alloc(new_arr));
		}

		fclose(fp);
	}

	if (if_alloc)
		delete decoder;

	return result;
}

vector<int> Data_Manager::Read(wstring filename, wstring name, vector<unsigned int>* decoder, unsigned int start_pt, unsigned int num)
{
	FILE * fp;

	vector<int> result;
	wchar_t c;
	wstring tmp_str;
	unsigned int dim;
	vector<unsigned int> size;
	Array * new_arr = nullptr;
	fp = fopen(To_str(filename).c_str(), "rb");
	unsigned int tmp_size;
	unsigned int count = 0;
	unsigned int count_element = 0;
	bool if_alloc = false;
	bool lock = false;

	if (decoder == nullptr) {
		decoder = new vector<unsigned int>;
		if_alloc = true;
	}
	if (start_pt != 0 || num != 0)
		lock = true;

	if (fp) {

		while (!feof(fp)) {
			c = fgetwc(fp);
			if (lock) {
				if (c == L'\n')
					count++;
				if (count == start_pt + 1) {
					count = 0;
					lock = false;
					goto P1;
				}
			}
			else {
				if (c == L'\n') {
				P1:
					count_element = 0;
					size.clear();
					decoder->clear();
					if (new_arr != nullptr)
						result.push_back(Data_Alloc(new_arr));

					dim = (unsigned int)fgetwc(fp);
					for (int i = 0; i < dim; i++) {
						fgetwc(fp); tmp_size = (unsigned int)fgetwc(fp);
						size.push_back(tmp_size);
					}
					new_arr = new Array(name + to_wstring(count));
					new_arr->init(size);
					count++;
					tmp_size = 1;
					for (int i = 0; i < size.size(); i++) {
						tmp_size *= size[i];
					}
					if (num != 0 && num + 1 == count) {
						lock = true;
						break;
					}
				}
				else if (c == L'\t') {
					if (decoder->size() != tmp_size) {
						decoder->push_back(Declare_Type(tmp_str));
					}
					new_arr->put_value(tmp_str, count_element, decoder->at(count_element));
					tmp_str.clear();
					count_element++;
				}
				else {
					tmp_str.push_back(c);
				}
			}
		}

		if (lock == false) {
			count_element = 0;
			size.clear();
			if (new_arr != nullptr)
				result.push_back(Data_Alloc(new_arr));
		}

		fclose(fp);
	}

	if (if_alloc)
		delete decoder;

	return result;
}

void Data_Manager::Write(const char * filename, Array & in, const char * mode)
{
	FILE * fp;
	fp = fopen(filename, mode);
	wstring str;
	if (fp) {
		fputwc(L'\n', fp);
		str = in.Array2str();
		fputwc(in.dim(), fp);
		for (int i = 0; i < in.dim(); i++) {
			fputwc(L' ', fp); fputwc(in.get_size(i), fp);
		}
		fputws(str.c_str(), fp);
		fclose(fp);
	}
}

void Data_Manager::Write(string filename, Array & in, const char * mode)
{
	FILE * fp;
	fp = fopen(filename.c_str(), mode);
	wstring str;
	if (fp) {
		fputwc(L'\n', fp);
		str = in.Array2str();
		fputwc(in.dim(), fp);
		for (int i = 0; i < in.dim(); i++) {
			fputwc(L' ', fp); fputwc(in.get_size(i), fp);
		}
		fputws(str.c_str(), fp);
		fclose(fp);
	}
}

void Data_Manager::Write(wstring filename, Array & in, const char * mode)
{
	FILE * fp;
	fp = fopen(To_str(filename).c_str(), mode);
	wstring str;
	if (fp) {
		fputwc(L'\n', fp);
		str = in.Array2str();
		fputwc(in.dim(), fp);
		for (int i = 0; i < in.dim(); i++) {
			fputwc(L' ', fp); fputwc(in.get_size(i), fp);
		}
		fputws(str.c_str(), fp);
		fclose(fp);
	}
}

void Data_Manager::Write(const char * filename, vector<Array> in, const char * mode)
{
	FILE * fp;
	fp = fopen(filename, mode);
	wstring str;
	unsigned int size = in.size();
	if (fp) {
		for (int i = 0; i < size; i++) {
			fputwc(L'\n', fp);
			str = in[i].Array2str();
			fputwc(in[i].dim(), fp);
			for (int j = 0; j < in[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(string filename, vector<Array> in, const char * mode)
{
	FILE * fp;
	fp = fopen(filename.c_str(), mode);
	wstring str;
	unsigned int size = in.size();
	if (fp) {
		for (int i = 0; i < size; i++) {
			fputwc(L'\n', fp);
			str = in[i].Array2str();
			fputwc(in[i].dim(), fp);
			for (int j = 0; j < in[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(wstring filename, vector<Array> in, const char * mode)
{
	FILE * fp;
	fp = fopen(To_str(filename).c_str(), mode);
	wstring str;
	unsigned int size = in.size();
	if (fp) {
		for (int i = 0; i < size; i++) {
			fputwc(L'\n', fp);
			str = in[i].Array2str();
			fputwc(in[i].dim(), fp);
			for (int j = 0; j < in[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(const char * filename, const char * mode)
{
	vector<Array> in_data;
	unsigned int data_size_1 = this->data.size();
	for (int i = 0; i < data_size_1; i++) {
		if (this->data[i] != nullptr)
			in_data.push_back(*this->data[i]);
	}

	FILE * fp;
	fp = fopen(filename, mode);
	wstring str;
	unsigned int data_size_2 = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size_2; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(string filename, const char * mode)
{
	vector<Array> in_data;
	unsigned int data_size_1 = this->data.size();
	for (int i = 0; i < data_size_1; i++) {
		if (this->data[i] != nullptr)
			in_data.push_back(*this->data[i]);
	}

	FILE * fp;
	fp = fopen(filename.c_str(), mode);
	wstring str;
	unsigned int data_size_2 = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size_2; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(wstring filename, const char * mode)
{
	vector<Array> in_data;
	unsigned int data_size_1 = this->data.size();
	for (int i = 0; i < data_size_1; i++) {
		if (this->data[i] != nullptr)
			in_data.push_back(*this->data[i]);
	}

	FILE * fp;
	fp = fopen(To_str(filename).c_str(), mode);
	wstring str;
	unsigned int data_size_2 = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size_2; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(const char * filename, vector<int> idxs, const char * mode)
{
	vector<Array> in_data = get(idxs);

	FILE * fp;
	fp = fopen(filename, mode);
	wstring str;
	unsigned int data_size = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(string filename, vector<int> idxs, const char * mode)
{
	vector<Array> in_data = get(idxs);

	FILE * fp;
	fp = fopen(filename.c_str(), mode);
	wstring str;
	unsigned int data_size = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(wstring filename, vector<int> idxs, const char * mode)
{
	vector<Array> in_data = get(idxs);

	FILE * fp;
	fp = fopen(To_str(filename).c_str(), mode);
	wstring str;
	unsigned int data_size = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(const char * filename, wstring name, bool exact, const char * mode)
{
	vector<Array> in_data = get(name, exact);

	FILE * fp;
	fp = fopen(filename, mode);
	wstring str;
	unsigned int data_size = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(string filename, wstring name, bool exact, const char * mode)
{
	vector<Array> in_data = get(name, exact);

	FILE * fp;
	fp = fopen(filename.c_str(), mode);
	wstring str;
	unsigned int data_size = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

void Data_Manager::Write(wstring filename, wstring name, bool exact, const char * mode)
{
	vector<Array> in_data = get(name, exact);

	FILE * fp;
	fp = fopen(To_str(filename).c_str(), mode);
	wstring str;
	unsigned int data_size = in_data.size();
	if (fp) {
		for (int i = 0; i < data_size; i++) {
			fputwc(L'\n', fp);
			str = in_data[i].Array2str();
			fputwc(in_data[i].dim(), fp);
			for (int j = 0; j < in_data[i].dim(); j++) {
				fputwc(L' ', fp); fputwc(in_data[i].get_size(j), fp);
			}
			fputws(str.c_str(), fp);
		}
		fclose(fp);
	}
}

