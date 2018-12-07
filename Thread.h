#ifndef _THREAD_H_
#define _THREAD_H_

#include "defs.h"

/* Thread class */
class Thread_info {
public:
	std::thread * id;
	std::atomic_bool state;

	void put_id(std::thread * new_id) {
		id = std::move(new_id);
	}
	void put_state(bool new_state) {
		state.store(new_state);
	}

	Thread_info() {};

	explicit Thread_info(const Thread_info & object) {
		id = std::move(object.id);
		state.store(object.state);
	};

	explicit Thread_info(std::thread * new_id, bool new_state) {
		id = std::move(new_id);
		state.store(new_state);
	};

};


#endif


