#include "MyLog.h"

MyLog_c* MyLog_c::_instance = new MyLog_c("Global ", "GlobalLog", "./global.log");

bool MyLog_c::doWhenLevel(MyLogLevel_e in_level, MyLogLevel_e in_needLevel) {
	return (in_level <= in_needLevel);
}

void MyLog_c::cline(size_t num) {
	spdlog::info(prefix + std::string(num, '-'));
}
void MyLog_c::cinfo(const std::string& in_msg)
{
	if (MyLog_c::doWhenLevel(
		MyLogLevel_e::Info,
#ifdef NDEBUG
		this->releaseViewlevel
#else
		this->debugViewlevel
#endif // NDEBUG
	)) {
		spdlog::info(prefix + in_msg);
	}
}
void MyLog_c::cwarn(const std::string& in_msg)
{
	if (MyLog_c::doWhenLevel(
		MyLogLevel_e::Warn,
#ifdef NDEBUG
		this->releaseViewlevel
#else
		this->debugViewlevel
#endif // NDEBUG
	)) {
		spdlog::warn(prefix + in_msg);
	}
}
void MyLog_c::cerror(const std::string& in_msg)
{
	if (MyLog_c::doWhenLevel(
		MyLogLevel_e::Error,
#ifdef NDEBUG
		this->releaseViewlevel
#else
		this->debugViewlevel
#endif // NDEBUG
	)) {
		spdlog::error(prefix + in_msg);
	}
}
void MyLog_c::cdebug(const std::string& in_msg)
{
	spdlog::debug(prefix + in_msg);
}

void MyLog_c::cfinfo(const std::string& in_msg)
{
	if (MyLog_c::doWhenLevel(
		MyLogLevel_e::Info,
#ifdef NDEBUG
		this->releaseViewlevel
#else
		this->debugOutFilelevel
#endif // NDEBUG
	)) {
		this->flog->info(in_msg);
		if (
#ifdef NDEBUG
			this->releaseIsViewWhenOutFile
#else
			this->debugIsViewWhenOutFile
#endif // NDEBUG
		) {
			this->cinfo(in_msg);
		}
	}
}
void MyLog_c::cfwarn(const std::string& in_msg)
{
	if (MyLog_c::doWhenLevel(
		MyLogLevel_e::Warn,
#ifdef NDEBUG
		this->releaseViewlevel
#else
		this->debugOutFilelevel
#endif // NDEBUG
	)) {
		this->flog->warn(in_msg);
		if (
#ifdef NDEBUG
			this->releaseIsViewWhenOutFile
#else
			this->debugIsViewWhenOutFile
#endif // NDEBUG
			) {
			this->cwarn(in_msg);
		}
	}
}
void MyLog_c::cferror(const std::string& in_msg)
{
	if (MyLog_c::doWhenLevel(
		MyLogLevel_e::Error,
#ifdef NDEBUG
		this->releaseViewlevel
#else
		this->debugOutFilelevel
#endif // NDEBUG
	)) {
		this->flog->error(in_msg);
		if (
#ifdef NDEBUG
			this->releaseIsViewWhenOutFile
#else
			this->debugIsViewWhenOutFile
#endif // NDEBUG
			) {
			this->cerror(in_msg);
		}
	}
}
void MyLog_c::cfdebug(const std::string& in_msg)
{
	this->flog->debug(in_msg);
}

void MyLog_c::cfflush()
{
	if(flog)
		this->flog->flush();
}
