#ifndef MIMICRY_LOG_H
#define MIMICRY_LOG_H
#include <string>
#include <mutex>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

enum MyLogLevel_e {
	AllNot,
	Error,
	Warn,
	Info,
	All,
};

class MyLog_c {
protected:
	static MyLog_c* _instance;

	MyLogLevel_e debugViewlevel = MyLogLevel_e::All;
	MyLogLevel_e debugOutFilelevel = MyLogLevel_e::All;
	// 当debug模式时，如果有输出到文件的日志时，是否同步在控制台显示
	bool debugIsViewWhenOutFile = true;
	MyLogLevel_e releaseViewlevel = MyLogLevel_e::All;
	MyLogLevel_e releaseOutFilelevel = MyLogLevel_e::All;
	// 当release模式时，如果有输出到文件的日志时，是否同步在控制台显示
	bool releaseIsViewWhenOutFile = false;
	std::string prefix = "";
	std::shared_ptr<spdlog::logger> flog = nullptr;

public:
	static MyLog_c* getInstance() {
		return _instance;
	}
	// 当 [in_level] >= [in_needLevel] 时，返回true
	static bool doWhenLevel(MyLogLevel_e in_level, MyLogLevel_e in_needLevel);

	MyLog_c(const std::string& in_prefix) noexcept : prefix(in_prefix) {}
	MyLog_c(const std::string& in_prefix, const std::string& loggerName, const std::string& fileName) noexcept
		: prefix(in_prefix){
		try {
			flog = spdlog::basic_logger_mt(loggerName, fileName);
		}
		catch (...) {
			MyLog_c::getInstance()->cinfo("create log(" + loggerName + "): " + fileName);
		}
	}
	MyLog_c() {}

	void set_prefix(const std::string& in_prefix) {
		prefix = in_prefix;
	}

	void cline(size_t num = 10);
	void cinfo(const std::string& in_msg);
	void cwarn(const std::string& in_msg);
	void cerror(const std::string& in_msg);
	void cdebug(const std::string& in_msg);

	void cfinfo(const std::string& in_msg);
	void cfwarn(const std::string& in_msg);
	void cferror(const std::string& in_msg);
	void cfdebug(const std::string& in_msg);
	void cfflush();

	~MyLog_c() {
		this->cfflush();
	}
};

//快捷创建日志
#ifndef _makeMyLogClass_d
#define _makeMyLogClass_d(className, prefix, loggerName, defaultPath)	\
class className : public MyLog_c {							\
protected:													\
	inline static className* instanceObj = nullptr;			\
	inline static std::mutex some_mutex;					\
public:														\
	inline static std::string path_log = defaultPath;		\
	className(const std::string& in_file_path): MyLog_c(prefix, loggerName, in_file_path) {} \
	static className* getInstance() {						\
		std::lock_guard<std::mutex> guard{ className::some_mutex };	\
		if (className::instanceObj == nullptr) {			\
			className::instanceObj = new className(className::path_log);	\
		}													\
		return className::instanceObj;						\
	}														\
	static void deleteInstance() {							\
		std::lock_guard<std::mutex> guard{ className::some_mutex };	\
		if (className::instanceObj != nullptr) {			\
			delete className::instanceObj;					\
			className::instanceObj = nullptr;				\
		}													\
	}	\
};

#endif // !_makeMyLogClass_d


#endif // !MYLOG_H