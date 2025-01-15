#ifndef MYHTTPFILE_H
#define MYHTTPFILE_H

#include <string>

#include "MyHttpTask.h"

// 文件/文件夹
class MyHttpFile_c {
protected:
public:

    // 提供文件下载
    static bool out_file(
        MyHttpTask_c*      in_task,
        const std::string& in_path,
        bool               enable_range = true,
        bool               enable_ETag  = true
    );
    // 提供文件夹内所有文件下载
    static bool out_dir(
        MyHttpTask_c*      in_task,
        const std::string& in_dir_path,
        const std::string& in_defaultFileName = ""
    );
    /*文件上传
     * 支持form-data 和 binary 类型的请求体， 默认两个都允许
     * 仅保存一个文件，即包含fileName和fileType
     * 如果请求体包含了多个文件，则只保存第一个符合的
     *
     * \param in_task
     * \param in_path 文件保存路径，该路径需要包含具体文件名
     * \param max_size 限制文件最大大小，默认负值不限制
     * \param body_type 指定允许的body类型
     */
    static bool upload_file(
        MyHttpTask_c*      in_task,
        const std::string& in_path,
        int                max_size = -1,
        int body_type = mimicry::HttpBodyType_e::Form_data_int | mimicry::HttpBodyType_e::Binary_int
    );
    // 将in_data保存的in_path指定的路径（当路径不存在时将自动创建）
    static bool save_file(const std::string& in_data, const std::string& in_path);
    /*读取文件
     * \param in_path 文件所在路径
     * \param re_data 读取的数据将保存到re_data中
     * \param shift	 偏移shift个字节后读取，默认0不偏移
     * \param read_len 读取最大长度，默认-1读取整个文件
     *
     * \return 返回实际读取到的字节数
     */
    static size_t read_file(
        const std::string& in_path,
        std::string&       re_data,
        size_t             shift    = 0,
        int                read_len = -1
    );
};

// 相关全局函数定义
namespace mimicry {
    struct myStat {
        unsigned long st_size;
        // 时间单位：秒
        time_t mtime;
        time_t atime;
        time_t ctime;
    };

    std::string getFileWETag(const mimicry::myStat& in_fstat);
    bool        getFileStat(const std::string& in_path, mimicry::myStat& in_stat);
    std::string getFileMIME(const std::string& filePath);

} // namespace mimicry

#endif // !MYHTTPFILE_H