## 项目文件编码
*	UTF-8 不带BOM
*	可使用根目录下的 [recoding_utf8.py] 将非UTF-8的代码文件进行一键转换
*	由于部分注释等为中文，如果使用其他编码方式打开，可能会乱码甚至编译不通过

## 编译运行
*	vcpkg安装使用：
	*	安装vcpkg：
		*	git clone https://github.com/microsoft/vcpkg
		*	win端执行 .bat 文件，linux端执行 .sh 文件
		*	./bootstrap-vcpkg.[sh | bat]
	*	并利用vcpkg安装本项目依赖库：
		*	vcpkg安装依赖库命令：./vcpkg install [包名]
		*	openssl
		*	fmt
		*	spdlog
		*	nlohmann-json
		*	jwt-cpp
*	拉取workflow：
	*	在github上下载或 git clone [workflow路径] 源码到 [本项目根目录]/mylib/[对应系统的workflow文件夹]
	*	win端：
		*	文件夹名称：workflow_win
		*	下载链接：https://github.com/sogou/workflow/blob/windows
	*	linux端：
		*	文件夹名称：workflow_linux
		*	下载链接：https://github.com/sogou/workflow/blob/master
*	编译workflow
	*	由于linux端的workflow目录内存在一个叫 BUILD 的文件，因此如果使用平常的名称build则会报错，
	这里我们另取名称为 mybuild。
	*	注意：一般您只需要编译当前系统的workflow即可。例如您正在win上使用该项目，也只需要下载workflow_win部分
	并编译即可开始使用。即使有跨平台的打算，也可以把项目复制到linux上，然后下载workflow_linux部分并编译
	即可。
		*	在win端直接进入workflow_linux编译一般是会报错的。在linux上进入workflow_win编译也一样。
		*	对于类似在win上编译用于linux的部分的操作，如果您不清楚是否需要这么做，那就是不需要。
	*	win端 / linux端：
		*	cd { workflow_win 或 workflow_linux }
		*	win端找到workflow_win目录下CMakeList.txt 中 
			*	option(WORKFLOW_BUILD_STATIC_RUNTIME "Use static runtime" ON)
			*	这一句中，把最后的ON改为OFF
		*	cmake -B mybuild -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg根目录]/scripts/buildsystems/vcpkg.cmake
		*	cmake --build mybuild --config Release
		*	cmake --build mybuild --config Debug
*	运行
	*	编译完成后，可执行文件在 [项目根路径]/build/bin/ 内
	*	内容分布和src代码文件夹相同
	*	由于项目的main.cpp可能存在多个，因此可执行文件可能存在多个，比如[myMusic]的可执行文件
	就放在 [项目根路径]/build/bin/myMusic/myMusic[.exe]

## 问题
*	workflow编译失败
	*	检查vcpkg 和它的库是否安装到位
	*	一般不要在win端去编译workflow_linux 或 linux端去编译workflow_win
	cmake构建时的提示中应当包含：
		*	-- Found OpenSSL: optimized;D:/0Acoolight/App/vcpkg/installed/x64-windows/lib/libcrypto.lib;debug;D:/0Acoolight/App/vcpkg/installed/x64-windows/debug/lib/libcrypto.lib (found version "3.0.5")
			*	这一句提示找到了openssl依赖
		*	......
		*	-- Building static libraries
			*	这一句提示编译时会生成静态库
		*	......
		*	-- Building: NetSSL_OpenSSL 
			*	这一句提示构建生成中包含NetSSl模块
		*	......
*	win端项目编译失败
	*	检测到“RuntimeLibrary”的不匹配项: 值“MTd_StaticDebug”不匹配值“MDd_DynamicDebug”
		*	win端workflow_win目录下CMakeList.txt 中 
			*	option(WORKFLOW_BUILD_STATIC_RUNTIME "Use static runtime" ON)
			*	这一句中，把最后的ON改为OFF
*	mysql连接失败 
	*	task error: [https://blog.csdn.net/s634772208/article/details/81155068]
		*	可能是mysql账号验证问题，修改账号验证方式为mysql_native_password
		*	ALTER USER [root@localhost] IDENTIFIED WITH mysql_native_password BY '[密码]';
		*	FLUSH PRIVILEGES;
	*	在win端，如果wsl中运行本项目，然后mysql放在win里，本项目通过localhost:3389并不能
	访问到win端的mysql
