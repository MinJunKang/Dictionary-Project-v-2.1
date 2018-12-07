#include "Dir_Manager.h"

/********** Manage Data *********/

unsigned int Dir_Manager::Update_Data(Dir_Type new_data)
{
	Dir_Type * data = nullptr;

	for (unsigned int i = 0; i < this->DirTable.size(); i++) {
		if (!this->DirTable[i]->update_state) {
			if (*this->DirTable[i] == new_data) {
				this->DirTable[i]->update_state = true;
				return i;
			}
		}
	}

	for (unsigned int i = 0; i < this->DirTable.size(); i++) {
		if (this->DirTable[i]->useable()) {
			*this->DirTable[i] = new_data;
			this->DirTable[i]->update_state = true;
			return i;
		}
	}

	data = new Dir_Type(new_data);

	return Alloc_Data(data);
}

unsigned int Dir_Manager::Alloc_Data(Dir_Type * new_data)
{
	if (new_data == nullptr)
		Print_Error(L"Dir_Manager", L"DM0000"); // Error - pid2Path : Invalid dir pid

	for (unsigned int i = 0; i < this->DirTable.size(); i++) {
		if (this->DirTable[i] == nullptr) {
			this->DirTable[i] = new_data;
			return i;
		}
	}

	this->DirTable.push_back(new_data);

	return this->DirTable.size() - 1;
}

void Dir_Manager::Dealloc_Data(unsigned int id, bool kill_all)
{
	if (id >= this->DirTable.size())
		Print_Error(L"Dir_Manager", L"DM0001"); // Error - pid2Path : Invalid dir pid
	if (this->DirTable[id] != nullptr) {
		if (this->DirTable[id]->useable() || kill_all) {
			delete this->DirTable[id];
			this->DirTable[id] = nullptr;
		}
	}
}

/***********************************/

/********** Base Functions *********/

wstring Dir_Manager::To_wstr(const string & str)
{
	unsigned int required_len = str.length();

	wchar_t * dest = new wchar_t[required_len + 1];

	mbstowcs(dest, str.c_str(), required_len);

	dest[required_len] = L'\0';

	wstring converted_str(dest);

	delete[] dest;

	return converted_str;
}

string Dir_Manager::To_str(const wstring & wstr)
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

unsigned int Dir_Manager::Distance(unsigned int pid_1, unsigned int pid_2)
{
	vector<unsigned int> path_1;
	vector<unsigned int> path_2;

	int d1 = 0;
	int d2 = 0;

	bool case_ = false;

	int up_pid = pid_1;

	/* get pid_1 path */
	while (1) {
		path_1.push_back(up_pid);

		up_pid = DirTable[up_pid]->upper_pid;
		if (up_pid == -1) {
			break;
		}
	}

	up_pid = pid_2;

	/* get pid_2 path */
	while (1) {
		path_2.push_back(up_pid);

		up_pid = DirTable[up_pid]->upper_pid;
		if (up_pid == -1)
			break;
	}

	for (unsigned int i = 0; i < path_1.size(); i++) {
		for (unsigned int j = 0; j < path_2.size(); j++) {
			if (path_1[i] == path_2[j]) {
				d1 = i;
				d2 = j;
			}
			if (path_1[i] == pid_2)
				case_ = true;
			if (path_2[j] == pid_1)
				case_ = true;

		}
	}

	if (case_)
		return abs(d1 - d2);
	else
		return d1 + d2;
}

/***********************************/

/**********  Dir or File Manage Function *********/

unsigned int Dir_Manager::isFileorDir(wstring name)
{
	_wfinddatai64_t c_file;
	intptr_t hFile;
	int result;


	if ((hFile = _wfindfirsti64(name.c_str(), &c_file)) == -1L)
		result = -1; // 파일 또는 디렉토리가 없으면 -1 반환
	else
		if (c_file.attrib & _A_SUBDIR)
			result = 0; // 디렉토리면 0 반환
		else
			result = 1; // 그밖의 경우는 "존재하는 파일"이기에 1 반환


	_findclose(hFile);

	return result;
}

vector<wstring> Dir_Manager::Split_path(wstring & path)
{
	vector<wstring> splitted_result;

	wstring split_char = L"/";

	size_t pos = 0;
	size_t p_pos = 0;

	path = To_stdPath(path);

	while (1) {
		pos = path.find(split_char, pos + 1);
		if (pos == wstring::npos) {
			if (path.length() - p_pos != 0) {
				splitted_result.push_back(path.substr(p_pos, path.length() - p_pos));
			}
			break;
		}
		else {
			splitted_result.push_back(path.substr(p_pos, pos - p_pos));
			p_pos = pos + 1;
		}
	}

	return splitted_result;
}

wstring Dir_Manager::To_stdPath(wstring path)
{
	wstring non_format = L"\\";

	size_t start_pos = 0;

	while ((start_pos = path.find(non_format, start_pos)) != std::wstring::npos) {
		path[start_pos] = L'/';
		start_pos += 1;
	}

	return path;
}

/************************************************/

/* Convert between path and pid */

wstring Dir_Manager::pid2Path(unsigned int dir_pid)
{
	wstring path = L"";

	/* If there is no corresponding pid */
	if (dir_pid >= this->DirTable.size()) {
		while (updating) {}
		if (this->DirTable.size() <= dir_pid)
			Print_Error(L"Dir_Manager", L"DM0002"); // Error - pid2Path : Invalid dir pid
		if (this->DirTable[dir_pid]->useable(false)) {
			Print_Error(L"Dir_Manager", L"DM0003", false); // Error - pid2Path : Invalid dir pid
			return path;
		}
	}

	if (this->DirTable[dir_pid]->useable(false)) {
		while (updating) {}
		if (this->DirTable.size() <= dir_pid)
			Print_Error(L"Dir_Manager", L"DM0004"); // Error - pid2Path : Invalid dir pid
		if (this->DirTable[dir_pid]->useable(false)) {
			Print_Error(L"Dir_Manager", L"DM0005", false); // Error - pid2Path : Invalid dir pid
			return path;
		}
	}

	vector<unsigned int> path_pid;

	int up_pid = dir_pid;

	while (1) {
		path_pid.push_back(up_pid);

		up_pid = DirTable[up_pid]->upper_pid;
		if (up_pid == -1)
			break;
	}

	reverse(path_pid.begin(), path_pid.end());

	for (unsigned int i = 0; i < path_pid.size(); i++) {
		path = path + DirTable[path_pid[i]]->Directory;
	}

	path = Upper_bound_path + path;

	return path;
}

int Dir_Manager::Path2pid(wstring path)
{
	path = To_stdPath(path);

	if (path.find(this->Upper_bound_path) == wstring::npos) {
		Print_Error(L"Dir_Manager", L"DM0006", false); // Warning - Path2pid : path is not valid
		return -1;
	}

	path = path.substr(path.find_first_not_of(this->Upper_bound_path, path.length()));

	vector<wstring> splitted_path = Split_path(path);

	int pid = -1;
	unsigned int count = 0;
	unsigned int len = splitted_path.size();

	for (unsigned int i = 0; i < this->DirTable.size(); i++) {
		if (this->DirTable[i]->update_state) {
			if (this->DirTable[i]->Directory == splitted_path[len - count - 1]) {
				if (count == 0)
					pid = i;
				i = this->DirTable[i]->upper_pid - 1;
				count++;
			}
			else {
				pid = -1;
				count = 0;
			}
		}
	}
	if (pid == -1)
		Print_Error(L"Dir_Manager", L"DM0007", false); // Warning - Path2pid : path is not valid
	return pid;
}

/************************************************/

/* Find the directory and files in current directory */

vector<wstring> Dir_Manager::Get_Object(wstring & current_dir)
{
	vector<wstring> objects;

	_wfinddata_t fd;
	intptr_t handle;

	unsigned long result = -2;
	unsigned int count = 0;
	wstring path_back = L"./**";
	wstring path_split = L"/";

	handle = _wfindfirst((current_dir + path_back).c_str(), &fd);  //현재 폴더 내 모든 파일을 찾는다.

	if (handle == NONE) // 폴더 안에 파일이 없는 경우
		return objects;

	while (result != NONE)
	{
		if (count != 0 && count != 1)
		{
			objects.push_back(fd.name);
		}
		result = _wfindnext(handle, &fd);
		count++;
	}
	_findclose(handle);
	return objects;
}

/************************************************/

/****************** Update Directory info *********************/

unsigned int Dir_Manager::Update_dir(wstring path, int parent_id)
{
	unsigned int type;
	vector<wstring> objects = Get_Object(path);
	wstring dir_split = L"/";
	vector<wstring> child_dir;

	Dir_Type new_data;

	/* Update upper pid */
	new_data.upper_pid = parent_id;

	/* Update current dir */
	if (path == Upper_bound_path) {
		new_data.Directory = dir_split;
	}
	else {
		new_data.Directory = Path2name(path) + dir_split;
	}

	/* Objects save */
	for (unsigned int i = 0; i < objects.size(); i++) {
		type = isFileorDir(path + L"/" + objects[i]);
		if (type == _FILE)
			new_data.file.push_back(objects[i]);
		else if (type == DIR)
			child_dir.push_back(objects[i]);
	}

	unsigned int current_pid = Update_Data(new_data);

	/* Update childs */
	for (unsigned int i = 0; i < child_dir.size(); i++) {
		if (Check_Exclusive(child_dir[i]))
			this->DirTable[current_pid]->child_pid.push_back(Update_dir(path + dir_split + child_dir[i], current_pid));
	}

	return current_pid;
}

bool Dir_Manager::Check_Exclusive(wstring & dir_name)
{
	for (unsigned int i = 0; i < sizeof(Dir_Exclusive) / sizeof(wchar_t*); i++) {
		if (dir_name == Dir_Exclusive[i])
			return false;
	}

	return true;
}

void Dir_Manager::Update_freq()
{
	unsigned int startTime, elapseTime;

	startTime = clock();
	mtx.lock();
	while (update_state) {

		elapseTime = clock() - startTime;

		if (elapseTime / CLOCKS_PER_SEC == this->update_interval) {

			Update();

			startTime = clock();

		}
	}
	mtx.unlock();
}

void Dir_Manager::Update(bool root2base)
{

	wstring current_path;
	unsigned int tmp = clock();
	this->updating = true;

	if (this->DirTable.size() != 0)
		current_path = this->DirTable[this->current_root_pid]->Directory;

	/* Initialize */
	for (unsigned int i = 0; i < this->DirTable.size(); i++) {
		this->DirTable[i]->update_state = false;
	}

	if (IsDir(Upper_bound_path) == false)
		Print_Error(L"Dir_Manager", L"DM0008"); // Error - Cannot find upper bound path

	/* Update */
	Update_dir(Upper_bound_path, -1);

	/* Garbage Collection */
	for (int i = 0; i < this->DirTable.size(); i++) {
		Dealloc_Data(i);
	}

	for (unsigned int i = DirTable.size() - 1; i >= 0; i--) {
		if (DirTable[i] != nullptr)
			break;
		else
			DirTable.pop_back();
	}

	/* Update current root pid */
	if (root2base) {
		for (unsigned int i = 0; i < this->DirTable.size(); i++) {
			if (this->DirTable[i]->update_state) {
				if (this->DirTable[i]->upper_pid == -1) {
					this->current_root_pid = i;
					break;
				}
			}
		}
	}
	else {
		if (this->DirTable[this->current_root_pid]->Directory != current_path) {
			if (Find_dir(current_path, true, true) == -1) {
				for (unsigned int i = 0; i < this->DirTable.size(); i++) {
					if (this->DirTable[i]->update_state) {
						if (this->DirTable[i]->upper_pid == -1) {
							this->current_root_pid = i;
							break;
						}
					}
				}
			}
		}
	}
	this->updating = false;
	update_expected_time = (clock() - tmp) / CLOCKS_PER_SEC;
}



/**************************************************************/

/****************** Thread Manage function *********************/

int Dir_Manager::Alloc_thread(void(Dir_Manager::* func)(int &))
{
	int new_thread_id;

	new_thread_id = Get_valid_id();

	if (new_thread_id < 0) // create new space to allocate new thread
	{
		Thread_info new_info;
		this->dir_thread.push_back(new_info);
		this->dir_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id));
		this->dir_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
	else
	{
		this->dir_thread[abs(new_thread_id) - 1].put_state(true);
		thread * new_id = new thread(func, this, std::ref(new_thread_id));
		this->dir_thread[abs(new_thread_id) - 1].put_id(new_id);

		return abs(new_thread_id) - 1;
	}
}

bool Dir_Manager::Dealloc_thread(int id)
{
	if (this->dir_thread.size() <= id || id < 0)
		return false;

	if (this->dir_thread[id].state)
	{
		this->dir_thread[id].put_state(false);
		while (1) {
			if (this->dir_thread[id].id->joinable() == true) {
				break;
			}
		}
		if (this->dir_thread[id].id->joinable()) {
			this->dir_thread[id].id->join();
			delete this->dir_thread[id].id;
			this->dir_thread[id].id = nullptr;
		}
	}
	else
		return false;

	if (id == this->dir_thread.size() - 1) {
		this->dir_thread.pop_back();
	}

	return true;
}

int Dir_Manager::Get_valid_id()
{
	if (this->dir_thread.size() == 0)
		return -1; // there is no valid id, system creates new space to allocate id
	else
	{
		for (unsigned int i = 0; i < this->dir_thread.size(); i++)
		{
			if (this->dir_thread[i].state.load() == false)
				return i + 1;
		}

		return -(this->dir_thread.size() + 1); // there is no valid id, system creates new space to allocate id
	}
}

bool Dir_Manager::IsValid_thread(int id)
{
	if (this->dir_thread.size() <= id)
		return false;
	else
		if (this->dir_thread[id].id == nullptr)
			return false;
		else
			return this->dir_thread[id].state;
}

/***************************************************************/

/****************** FILE I/O Thread function *********************/

void Dir_Manager::FILE_Copy_thread(int & control_id, vector<wstring>& inputs)
{
	int id = abs(control_id) - 1;
	wstring src_file = inputs[0];
	wstring dst_file = inputs[1];
	bool if_remove = (bool)stoi(inputs[2]);

	src_file = To_stdPath(src_file);
	dst_file = To_stdPath(dst_file);

	if (!IsFile(src_file))
		Print_Error(L"Dir_Manager", L"DM0009:" + src_file); // Error - FILE_Copy_thread : invalid path

	char buf[BUFSIZ];
	size_t size;

	if (if_remove)
		FILE_Delete(dst_file);

	if (IsFile(dst_file) == false) {
		int source = open(To_str(src_file).c_str(), O_RDONLY, 0);
		int dest = open(To_str(dst_file).c_str(), O_WRONLY | O_CREAT /*| O_TRUNC/**/, 0644);

		while ((size = read(source, buf, BUFSIZ))) {
			if (size <= 0)
				break;
			write(dest, buf, size);
		}

		close(source);
		close(dest);
	}
}

void Dir_Manager::FILE_Move_thread(int & control_id, vector<wstring>& inputs)
{
	int id = abs(control_id) - 1;
	wstring src_file = inputs[0];
	wstring dst_file = inputs[1];

	src_file = To_stdPath(src_file);
	dst_file = To_stdPath(dst_file);

	if (!IsFile(src_file))
		Print_Error(L"Dir_Manager", L"DM0010:" + src_file); // Error - FILE_Move_thread : invalid path

	rename(To_str(src_file).c_str(), To_str(dst_file).c_str());
}

void Dir_Manager::FILE_Delete_thread(int & control_id, vector<wstring>& inputs)
{
	int id = abs(control_id) - 1;
	wstring file = inputs[0];

	if (IsFile(file))
		remove(To_str(file).c_str());
	else
		Print_Error(L"Dir_Manager", L"DM0011:" + file); // Error - FILE_Delete : invalid path (undefined)
}

/*****************************************************************/

/****************** Constructor *********************/

Dir_Manager::Dir_Manager(wstring Upper_bound_path) :update_state(false), update_interval(5), updating(false)
{

	/* Set bound path, root pid and Update */
	Set_bound_path(Upper_bound_path);

	/* Make the dir/file list and measure time consumed*/
	int start = clock();

	this->update_interval = (clock() - start) / CLOCKS_PER_SEC * 2;
	if (this->update_interval == 0)
		this->update_interval = 5;
}

/****************************************************/

/****************** Destructor *********************/

Dir_Manager::~Dir_Manager()
{
	if (update_state)
		Update_pause();

	for (int i = 0; i < DirTable.size(); i++) {
		if (DirTable[i] != nullptr)
			delete DirTable[i];
	}
	DirTable.clear();

	for (unsigned int id = 0; id < this->dir_thread.size(); id++)
	{
		this->dir_thread[id].state.store(false);
		if (this->dir_thread[id].id != nullptr) {
			if (this->dir_thread[id].id->joinable()) {
				this->dir_thread[id].id->join();
				delete this->dir_thread[id].id;
				this->dir_thread[id].id = nullptr;
			}
		}
	}
	this->dir_thread.clear();
}

/***************************************************/

/***********************************   Public Functions    ***********************************/

wstring Dir_Manager::Path2name(wstring path)
{
	vector<wstring> splitted_path = Split_path(path);

	int num = splitted_path.size();

	if (num >= 1)
		return splitted_path.at(num - 1);
	else {
		Print_Error(L"Dir_Manager", L"DM0012"); // Error - Path2name : invalid path
		return L"";
	}
}

wstring Dir_Manager::Get_pid_path(int dir_pid, bool with_format)
{
	wstring path_format = L"/";
	wstring path = pid2Path(dir_pid);

	if (with_format)
		return path;
	else
		return path.substr(0, path.find_last_of(path_format));
}

void Dir_Manager::Set_bound_path(wstring Upper_bound_path)
{
	Upper_bound_path = To_stdPath(Upper_bound_path);


	bool state = false;

	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

#ifdef WINMARK
	wstring path_exe = L".exe";
#else
	wstring path_exe = L".sh";
#endif
	wstring path_format = L"/";

	if (Upper_bound_path.find(path_exe, 0) != wstring::npos)
		Upper_bound_path = Upper_bound_path.substr(0, Upper_bound_path.find_last_of(path_format));

	if (isFileorDir(Upper_bound_path) == DIR)
		this->Upper_bound_path = Upper_bound_path;
	else
		Print_Error(L"Dir_Manager", L"DM0013"); // Error - Constructor : Upper_bound_path is not valid

	/* Make the dir/file list */
	Update(true);

	/* Resume update */
	if (state)
		Update_begin();
}

int Dir_Manager::Get_Root_pid()
{
	return this->current_root_pid;
}

wstring Dir_Manager::Get_Root_Path(bool with_format)
{
	wstring path_format = L"/";
	wstring path;
	while (path.length() == 0) {
		path = pid2Path(this->current_root_pid);
	}

	if (with_format)
		return path;
	else
		return path.substr(0, path.find_last_of(path_format));
}

int Dir_Manager::Set_Root_pid(unsigned int dir_pid)
{
	/* If there is no corresponding pid */
	if (dir_pid >= this->DirTable.size()) {
		while (updating) {}
		if (this->DirTable.size() <= dir_pid)
			Print_Error(L"Dir_Manager", L"DM0014"); // Error - Set_Root_pid : Invalid dir pid
		if (this->DirTable[dir_pid]->useable(false)) {
			Print_Error(L"Dir_Manager", L"DM0015", false); // Warning - Set_Root_pid : Invalid dir pid
			return -1;
		}
	}

	if (this->DirTable[dir_pid]->useable(false)) {
		while (updating) {}
		if (this->DirTable.size() <= dir_pid)
			Print_Error(L"Dir_Manager", L"DM0016"); // Error - Set_Root_pid : Invalid dir pid
		if (this->DirTable[dir_pid]->useable(false)) {
			Print_Error(L"Dir_Manager", L"DM0017", false); // Warning - Set_Root_pid : Invalid dir pid
			return -1;
		}
	}

	this->current_root_pid = dir_pid;

	return this->current_root_pid;
}

int Dir_Manager::Root2bound()
{
	for (unsigned int i = 0; i < this->DirTable.size(); i++) {
		if (this->DirTable[i]->upper_pid == -1) {
			if (this->DirTable[i]->useable(false)) {
				while (updating) {}
				if (this->DirTable[i]->useable(false))
					Print_Error(L"Dir_Manager", L"DM0029"); // Error - Root2bound : Invalid dir pid
				else {
					this->current_root_pid = i;
					return i;
				}
			}
			else {
				this->current_root_pid = i;
				return i;
			}
		}
	}
}

int Dir_Manager::Find_path(wstring path, bool set_root)
{
	int pid = this->Path2pid(path);

	if (set_root)
		Set_Root_pid(pid);

	return pid;
}

int Dir_Manager::Find_dir(wstring dir_name, bool set_root, bool exact)
{
	int pid = -1;
	int distance = DirTable.size();

	if (exact) {
		for (unsigned int i = 0; i < DirTable.size(); i++)
		{
			if (DirTable[i]->update_state == false)
				continue;
			if (Path2name(DirTable[i]->Directory) == Path2name(dir_name))
			{
				if (distance >= Distance(this->current_root_pid, i)) {
					pid = i;
					distance = Distance(this->current_root_pid, i);
				}
			}
		}
	}
	else {
		for (unsigned int i = 0; i < DirTable.size(); i++)
		{
			if (DirTable[i]->update_state == false)
				continue;
			if (Path2name(DirTable[i]->Directory).find(Path2name(dir_name)) != wstring::npos)
			{
				if (distance >= Distance(this->current_root_pid, i)) {
					pid = i;
					distance = Distance(this->current_root_pid, i);
				}
			}
		}
	}

	if (set_root && pid != -1)
		Set_Root_pid(pid);

	return pid;
}

vector<wstring> Dir_Manager::ls()
{
	vector<wstring> under_dir;

	for (unsigned int i = 0; i < DirTable[current_root_pid]->child_pid.size(); i++) {
		under_dir.push_back(DirTable[DirTable[current_root_pid]->child_pid[i]]->Directory);
	}

	return under_dir;
}

int Dir_Manager::Go_upper(bool set_upper_bound)
{
	wstring path_format = L"/";
	wstring path = this->Upper_bound_path;
	path = path.substr(0, path.find_last_of(path_format));

	if (set_upper_bound) {
		if (this->DirTable[this->current_root_pid]->upper_pid == -1)
			Set_bound_path(path);
		else
			Set_Root_pid(this->DirTable[this->current_root_pid]->upper_pid);
	}

	return this->current_root_pid;
}

int Dir_Manager::Go_lower(wstring dir_name)
{
	for (unsigned int i = 0; i < DirTable[current_root_pid]->child_pid.size(); i++)
	{
		if (DirTable[DirTable[current_root_pid]->child_pid[i]]->Directory == dir_name)
		{
			Set_Root_pid(DirTable[current_root_pid]->child_pid[i]);
			break;
		}
	}

	return this->current_root_pid;
}

int Dir_Manager::Go_lower(int pos)
{
	if (pos < DirTable[current_root_pid]->child_pid.size() && pos >= 0)
	{
		Set_Root_pid(DirTable[current_root_pid]->child_pid[pos]);
	}

	return this->current_root_pid;
}

vector<wstring> Dir_Manager::Get_Root_file()
{

	vector<wstring> files = this->DirTable[this->current_root_pid]->file;

	wstring root_path = Get_Root_Path(true);

	for (unsigned int i = 0; i < files.size(); i++) {
		files[i] = root_path + files[i];
	}

	return files;
}

void Dir_Manager::Update_begin(unsigned int time_interval)
{
	this->update_interval = time_interval;
	this->update_state = true;

	Update();

	m_th = thread(&Dir_Manager::Update_freq, this);
}

void Dir_Manager::Update_pause()
{
	this->update_state = false;
	while (!m_th.joinable()) {} // wait until thread is joinable
	if (m_th.joinable()) {
		m_th.join();
	}
}

bool Dir_Manager::Create_dir(wstring dir_name)
{
	bool state = false;

	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	int nResult = mkdir(To_str(Get_Root_Path(true) + dir_name).c_str());


	if (nResult == 0)
	{
		/* Make the dir/file list */
		Update();

		/* Resume update */
		if (state)
			Update_begin();

		return true;
	}
	else if (nResult == -1)
	{
		// 13 : not valid, 17 : already exists
		if (errno == 17)
			Print_Error(L"Dir_Manager", L"DM0018:" + dir_name, FALSE); // Warning - Create_dir : Dir already exists
		else if (errno == 13)
			Print_Error(L"Dir_Manager", L"DM0019:" + dir_name); // Error - Create_dir : path is not valid
		else
			Print_Error(L"Dir_Manager", L"DM0020:" + dir_name + to_wstring(errno)); // Error - Create_dir : Unknown Error code

																					/* Resume update */
		if (state)
			Update_begin();

		return false;
	}
}

bool Dir_Manager::Delete_dir(wstring dir_name, bool remove_files)
{
	bool state = false;

	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	int result;
	result = Find_dir(dir_name);
	if (result == -1)
		Print_Error(L"Dir_Manager", L"DM0021:" + dir_name); // Error - Delete_dir : Dir not exists
	wstring dir_path = Get_pid_path(result);

	if (remove_files) {
		vector<wstring> files = this->DirTable[result]->file;

		for (unsigned int i = 0; i < files.size(); i++) {
			files[i] = dir_path + files[i];
		}
		FILE_Delete(files);
	}

	int nResult = rmdir(To_str(dir_path).c_str());

	if (nResult == 0)
	{
		/* Make the dir/file list */
		Update();

		/* Resume update */
		if (state)
			Update_begin();

		return true;
	}
	else if (nResult == -1)
	{
		// 13 : not valid, 17 : already exists
		if (errno == 2)
			Print_Error(L"Dir_Manager", L"DM0022:" + dir_name); // Error - Delete_dir : Dir not exists
		else if (errno == 41)
			Print_Error(L"Dir_Manager", L"DM0023:" + dir_name); // Error - Delete_dir : Not empty dir
		else
			Print_Error(L"Dir_Manager", L"DM0024:" + dir_name + to_wstring(errno)); // Error - Delete_dir : Unknown Error code

																					/* Resume update */
		if (state)
			Update_begin();

		return false;
	}
}

bool Dir_Manager::IsFile(wstring file)
{
	if (isFileorDir(file) == _FILE)
		return true;
	else
		return false;
}

bool Dir_Manager::IsDir(wstring dir)
{
	if (isFileorDir(dir) == DIR)
		return true;
	else
		return false;
}

/*********************************************************************************************/

/********************************* File Manage Functions *************************************/

/*********************************************************************************************/

void Dir_Manager::FILE_Copy(vector<pair<wstring, wstring>> file_path, bool remove_previous)
{
	bool state = false;
	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	vector<int> ids;
	vector<vector<wstring>> files;
	vector<wstring> file;
	for (int i = 0; i < file_path.size(); i++) {
		file.push_back(file_path[i].first);
		file.push_back(file_path[i].second);
		file.push_back(to_wstring(remove_previous));
		files.push_back(file);
		file.clear();
	}
	for (int i = 0; i < files.size(); i++) {
		ids.push_back(Alloc_thread<wstring>(&Dir_Manager::FILE_Copy_thread, files[i]));
	}
	for (int i = 0; i < ids.size(); i++) {
		Dealloc_thread(ids[i]);
	}
	if (state)
		Update_begin();
}

void Dir_Manager::FILE_Copy(wstring src_file, wstring dst_file, bool remove_previous)
{
	bool state = false;
	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	src_file = To_stdPath(src_file);
	dst_file = To_stdPath(dst_file);

	if (!IsFile(src_file))
		Print_Error(L"Dir_Manager", L"DM0025:" + src_file); // Error - FILE_Copy : invalid path

	if (IsFile(dst_file)) {
		if (remove_previous)
			FILE_Delete(dst_file);
	}
	if (!IsFile(dst_file)) {
		char buf[BUFSIZ];
		size_t size;

		int source = open(To_str(src_file).c_str(), O_RDONLY, 0);
		int dest = open(To_str(dst_file).c_str(), O_WRONLY | O_CREAT /*| O_TRUNC*/, 0644);

		while ((size = read(source, buf, BUFSIZ)) > 0) {
			write(dest, buf, size);
		}

		close(source);
		close(dest);
	}

	if (state)
		Update_begin();
}

void Dir_Manager::FILE_Merge(vector<wstring> src_file, wstring dst_dir, bool remove_original)
{
	bool state = false;
	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	wstring path_format = L"/";

	dst_dir = To_stdPath(dst_dir);

	int format_pos = dst_dir.find_last_of(path_format);
	if (format_pos == dst_dir.length() - 1) { // format is not good
		dst_dir = dst_dir.substr(0, format_pos);
	}
	if (IsDir(dst_dir) == false)
		Print_Error(L"Dir_Manager", L"DM0026:" + dst_dir); // Error - FILE_Merge : invalid dir

	vector<pair<wstring, wstring>> input;
	for (int i = 0; i < src_file.size(); i++) {
		input.push_back(make_pair(src_file[i], dst_dir + path_format + Path2name(src_file[i])));
	}
	FILE_Copy(input, remove_original);

	if (state)
		Update_begin();
}

void Dir_Manager::FILE_Move(vector<pair<wstring, wstring>> file_path)
{
	bool state = false;
	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	vector<int> ids;
	vector<vector<wstring>> files;
	for (int i = 0; i < file_path.size(); i++) {
		vector<wstring> file;
		file.push_back(file_path[i].first);
		file.push_back(file_path[i].second);
		files.push_back(file);
	}
	for (int i = 0; i < files.size(); i++) {
		ids.push_back(Alloc_thread<wstring>(&Dir_Manager::FILE_Move_thread, files[i]));
	}
	for (int i = 0; i < ids.size(); i++) {
		Dealloc_thread(ids[i]);
	}

	if (state)
		Update_begin();
}

void Dir_Manager::FILE_Move(wstring src_file, wstring dst_file)
{
	bool state = false;
	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	src_file = To_stdPath(src_file);
	dst_file = To_stdPath(dst_file);

	if (!IsFile(src_file))
		Print_Error(L"Dir_Manager", L"DM0027:" + src_file); // Error - FILE_Move : invalid path

	rename(To_str(src_file).c_str(), To_str(dst_file).c_str());

	if (state)
		Update_begin();
}

void Dir_Manager::FILE_Delete(vector<wstring> files)
{
	bool state = false;
	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	vector<int> ids;
	vector<vector<wstring>> input;
	for (int i = 0; i < files.size(); i++) {
		vector<wstring> file;
		file.push_back(files[i]);
		input.push_back(file);
	}
	for (int i = 0; i < files.size(); i++) {
		ids.push_back(Alloc_thread<wstring>(&Dir_Manager::FILE_Delete_thread, input[i]));
	}
	for (int i = 0; i < ids.size(); i++) {
		Dealloc_thread(ids[i]);
	}

	if (state)
		Update_begin();
}

void Dir_Manager::FILE_Delete(wstring file)
{
	bool state = false;
	/* If it is updating stop it */
	if (update_state) {
		state = true;
		Update_pause();
	}

	if (IsFile(file))
		remove(To_str(file).c_str());
	else
		Print_Error(L"Dir_Manager", L"DM0028:" + file); // Error - FILE_Delete : invalid path (undefined)

	if (state)
		Update_begin();
}
