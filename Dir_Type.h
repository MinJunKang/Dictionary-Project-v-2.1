#ifndef _DIR_TYPE_
#define _DIR_TYPE_

#include "defs.h"

/* For Dir_Manager class data type */
class Dir_Type {
	friend class Dir_Manager;
protected:
	int upper_pid;
	wstring Directory;
	vector<unsigned int> child_pid;
	vector<wstring> file;
	bool update_state;
public:
	bool operator==(Dir_Type new_data) {
		if (new_data.Directory != this->Directory)
			return false;
		else if (new_data.upper_pid != this->upper_pid)
			return false;
		else if (new_data.file != this->file)
			return false;
		return true;
	}
	void operator=(Dir_Type new_data) {
		this->upper_pid = new_data.upper_pid;
		this->Directory = new_data.Directory;
		this->child_pid = new_data.child_pid;
		this->file = new_data.file;
	}
	void clear() {
		this->Directory.clear();
		this->child_pid.clear();
		this->file.clear();
		this->update_state = false;
	}
	bool useable(bool wipe_out = true) {
		if (update_state)
			return false;
		else {
			if (wipe_out)
				clear();
			return true;
		}
	}
	Dir_Type() :update_state(false), upper_pid(-1) {}
	Dir_Type(Dir_Type & data) :update_state(true) {
		this->Directory = data.Directory;
		this->child_pid.clear();
		this->upper_pid = data.upper_pid;
		this->file = data.file;
	}
	virtual ~Dir_Type() { clear(); }
};













#endif


