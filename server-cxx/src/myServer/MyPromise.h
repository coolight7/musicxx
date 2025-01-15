#ifndef MYPROMISE_H
#define MYPROMISE_H

#include <functional>

#include "workflow/WFTaskFactory.h"
#include "workflow/WFMySQLConnection.h"

template<typename _ReType, typename _DoTaskType, typename _ThenReType>
class MyPromise_c {
protected:
	std::function<MyPromise_c<void>* (_ReType*)> _callback = nullptr;
	// 负责帮助加锁保持 [beCount_task] 存活到 [do_task] 
	WFCounterTask* count_task = nullptr;
	// 被锁定的task
	SubTask* beCount_task = nullptr;
	// 等待执行的task
	SubTask* do_task = nullptr;
	// callback 入参
	_ReType* value_ptr = nullptr;
	// 结果指针
	MyPromise_c<_ThenReType>* result_ptr = nullptr;
	// 下一个Promise指针
	MyPromise_c<_ThenReType>* next_ptr = nullptr;
	
	// 由上一个异步操作完成后掉用
	_ThenReType* callback(_ReType* in_value_ptr) {
		value_ptr = in_value_ptr;
		if (_callback) {
			result_ptr = _callback(in_value_ptr);
		}
		if (next_ptr) {
			result_ptr->then(next_ptr->callback);
		}
		return result_ptr;
	}

public:
	MyPromise_c() {}
	MyPromise_c(_DoTaskType* in_do_task, SubTask* in_beCount_task, WFCounterTask* count_task = nullptr) {
		WFCounterTask* counter = WFTaskFactory::create_counter_task(1, nullptr);
		series_of(in_beCount_task)->push_back(counter);
		in_do_task->set_callback(
			[counter, this](SubTask* task) {
				this->callback(nullptr);
				counter->count();
			}
		);
	}

	MyPromise_c<_ThenReType>* then(std::function<MyPromise_c<_ThenReType>* (_ReType*)> callback) {
		_callback = callback;
		next_ptr = new MyPromise_c<_ThenReType>();
		return next_ptr;
	}

};

#endif // !MYPROMISE_H
