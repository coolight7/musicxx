#ifndef MYMUSIC_SRCINFODBBASE_H
#define MYMUSIC_SRCINFODBBASE_H
#include <string>
#include <vector>
#include <functional>

#include "myServer/MySqlBase.h"
#include "myServer/MySqlTask.h"

class MyMServiceWorkDB_c {
protected:
	MySqlBase_c* sql = nullptr;

	MyMServiceWorkDB_c(const std::string& in_url)
		:sql(new MySqlBase_c{ in_url }) {}


public:
	MyMServiceWorkDB_c(const MyMServiceWorkDB_c&) = delete;


	virtual ~MyMServiceWorkDB_c() {
		delete this->sql;
		this->sql = nullptr;
	}
};


#endif // !MYMUSIC_SRCINFODBBASE_H
