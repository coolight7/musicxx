## future
    *   ipv6
## bug
    *   给WFMySQLConnection::create_query_task传递lamdba时需要使用std::bind代替捕获，否则会捕获失败而报错
    *   短时间重复同一个WFMySQLConnection创建的task执行相同sql时会出错
    *   win虚拟机和本机的mysql连接有问题

## log 
    *   2023/6/5
        *   新增负载检查
        *   新增定时器/循环定时器
        *   优化访问限制控制
    *   2023/3/28
        *   修复部分可能json抛出异常导致崩溃的问题
        *   调整配置信息位置
        *   新增一些Resp错误码
        *   sqlstring增加转义 `
    *   2023/3/19
        *   优化默认主页对应的路由
        *   优化out_dir()
    *   2023/3/14
        *   新增Ip访问限制
        *   支持IP访问限速
    *   2023/3/13
        *   实现请求头限制拦截
        *   移除MyLog_c中的部分静态日志函数，替换为静态单例使用
    *   2023/3/6
        *   增加配置文件json解析错误的try catch
        *   增加配置文件配置项
        *   优化部分空字符串判断
        *   修复路由位置不存在时，返回的错误却是405的问题
    *   2023/2/19
        *   增加自定义提示的响应状态码
    *   2022/11/10
        *   新增Http代理MyProxy
        *   新增解析url获取arg、uri、host等函数
        *   修复添加路由时指定多个类型枚举时出错的问题
    *   2022/10/27
        *   MyHttpFile_c:out_file()：
            *   调整趋向标准规定
        *   增加部分注释
    *   2022/10/25
        *   调整兼容linux编译
        *   增加SHA512的生成和格式判断
    *   2022/10/22
        *   依赖workflow改用源码编译(workflow-10.0.3)
        *   MyHttpFile_c:out_file()支持ETag缓存
        *   调整CMakeLists.txt
    *   2022/10/11
        *   调整MySqlTask_c::countHttpTask()，当callback的参数MySqlTask_c* 为nullptr时直接return
        *   优化MySql_c::dosql_log()和dosql_errlog()
            *   当参数MyHttpTask_c*不为nullptr时默认调用MySqlTask_c::countHttpTask()延续生命周期
            *   增加部分参数的非空判断
    *   2022/10/9
        *   新增MySqlTask_c::to_type()，将结果数组赋值给指定目标
        *   MyRouter：增加不使用缓存的get_nocache()
    *   2022/10/5
        *   mimicry:
            *   优化read_form_data()，大幅提升速度和解析准确性
            *   HttpBodyType_e增加int枚举
            *   调整Http_form_data_s定义
        *   MyHttpTask：
            *   新增find_header()查找报头中的字段
            *   优化get_header()的缓存操作
            *   get_path()增加缓存
        *   MyHttpFile：
            *   新增文件上传，文件保存
            *   优化文件下载
    *   2022/10/4
        *   MyHttpTask：
            *   重构
            *   使用MyHttpServerTask继承WFServer
            *   分裂MyHttpRequest和MyHttpRespone
            *   Header的获取增加缓存
            *   get_path()默认支持解析 ../ ./ 去除重复 / 并执行urldecode
        *   MyHttpServer：
            *   同步MyHttpTask的调整
            *   增加重写函数new_session
        *   MyRouter：
            *   新增常用路由地址哈希缓存
            *   支持移除指定路由位置
    *   2022/10/3
        *   MyHttpTask:
            *   arg参数增加从body获取
            *   新增获取body
            *   新增body类型枚举 mimicry::HttpBodyType_e
        *   Cookie:
            *   由于设计构思问题，整个重构
        *   新增解析form-data类型的body的函数
    *   2022/9/26
        *   新增创建/判断是否存在文件和文件夹函数
        *   优化参数和cookie获取和设置，增加url编解码和去无用的头尾空格
        *   调整mimicry::myStat结构体内的变量名定义，避免linux下和宏名冲突
        *   调整适配linux
    *   2022/9/22
        *   新增 MyBaseEntity_c 简化api回复的json构造
        *   MyHttpTask_c:
            *   新增 resp_append_entity()，resp_append_success()，resp_append_err()，简化api的格式化回复
        *   MySqlTask_c:
            *   新增 MyCellType_e mysql结果类型枚举
            *   新增 get_firstSql_result()，简化获取mysql的第一个sql的返回结果
            *   新增 is_type()，简化判断mysql返回结果的类型判断
            *   新增 countHttpTask()，简化count HttpTask的操作
        *   使用类的静态变量替换枚举定义
        *   使用 std::move 右值引用 等进行优化
        *   初始化使用{}代替()的写法，避免声明并初始化变量的语句被识别为函数
        *   部分函数增加const限制
    *   2022/9/15
        *   GlobalUtil:
            *   新增getSHA1()获取字符串的SHA1值
        *   MyHttpServer_c:
            *   新增401 403 405的默认错误html
            *   修复403的判定
        *   MyHttpTask_c:
            *   新增getMyTask()函数，包装WFHttpTask*，隐藏new操作
            *   新增MyHttpProcess_t的空指针变量Nullptr_MyHttpProcess，用于const MyHttpProcess_t&的函数返回值
            *   修复get_request_argStr()，支持获取放在请求头内data的请求参数
        *   MySql_c:
            *   新增dosql()的日志记录函数
        *   MySqlTask_c:
            *   增加SqlString_c的支持
        *   优化多数std::function的传参为const &，注意部分场景需要std::bind代替lamdba捕获，否则会报错
        *   将SqlString_c和Cookie_c移入mimicry命名空间
    *   2022/9/08
        *   新增时间戳转Http时间函数
        *   新增字符串转义函数
        *   新增连接mysql的功能支持
        *   Http文件服务支持分块
        *   修复Http文件服务中文件读取长度错误的问题
    *   2022/9/02
        *   支持文件IO
        *   支持路由通配符 ‘*’
        *   支持路由自动去除uri中连续的‘/’
    *	2022/8/24
        *   支持对请求和响应的cookie的get和set
        *   支持路由
    *	2022/8/15
	    *	新建文件夹