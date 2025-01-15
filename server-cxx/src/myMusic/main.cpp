#include <iostream>
#include <string>

#include "myMusic/test/test.h"

#ifndef MyHttpServer_HOME
#define MyHttpServer_HOME "."
#endif

using std::cin;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char** argv) {
    // 指定配置文件路径
    std::string config_path;
    if (argc >= 2) {
        config_path = argv[1];
    }
    cout << add(7, 77) << endl;
    cout << "build time: " << __DATE__ << " | " << __TIME__ << endl;
    int num;
    cout << "<< input a number to stop" << endl << ">>>";
    cin >> num;

    return 0;
}
