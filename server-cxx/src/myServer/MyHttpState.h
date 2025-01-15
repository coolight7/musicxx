#ifndef MYHTTPSTATE_H
#define MYHTTPSTATE_H

class MyHttpState_e {
public:

    static constexpr int Continue = 100, // 继续
        OK                        = 200, // 成功
        Created = 201, // 已创建。成功请求并创建了新的资源
        PartialContent      = 206, // 部分内容。请求分块时需要使用
        MovedPermanently    = 301, // 永久移动
        Found               = 302, // 临时移动
        BadRequest          = 400, // 请求格式错误
        Unauthorized        = 401, // 需要用户身份认证
        Forbidden           = 403, // 拒绝请求
        NotFound            = 404, // 未找到
        MethodNotAllowed    = 405, // 方法不允许
        PreconditionFailed  = 412, // 客户端希望的条件不满足
        InternalServerError = 500, // 服务器内部错误
        BadGateway = 502, // 网关或代理服务器未及时从远端服务器获取请求
        ServiceUnavailable = 503; // 服务不可访问
};

namespace mimicry {
    class ResponeState_e {
    public:

        static constexpr int Undefined = -1,
                             Continue  = 1000, // 继续
            ContinueCustom  = 1777, // 继续，并附带自定义提示
            OK              = 2000, // 成功
            OKCustom        = 2777, // 成功，并附带自定义提示
            JumpCustom      = 3777, // 跳转标记，并附带自定义提示
            LoginStateError = 4000, // 用户需要登录
            Permission      = 4001, // 权限错误
            Forbidden       = 4003, // 拒绝请求
            NotFound        = 4004, // 请求资源无法找到
            ValueHiatus     = 4010, // 参数缺失
            ValueError      = 4011, // 参数错误
            DeletedError    = 4012, // 已被删除
            RepearError     = 4013, // 重复动作
            LockedError     = 4014, // 已被封禁
            LimitError      = 4015, // 违反限制
            Toofast         = 4016, // 访问太快
            TooMany         = 4017, // 访问次数太多
            VerifyError     = 4018, // 验证码等验证方式不通过
            InsufficientCredit = 4019, // 信用评估较低，不可使用
            FaildCustom = 4777, // 客户端错误，并附带自定义提示
            FaildTag    = 4900, // 失败
            Exception   = 5000, // 服务器错误
            ExceptionCustom = 5777; // 服务端错误，并附带自定义提示

        static int faildTag(int num) {
            return (FaildTag + num);
        }
    };

}; // namespace mimicry

#endif // !MYHTTPSTATE_H