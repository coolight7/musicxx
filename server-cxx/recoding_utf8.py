# -- coding: utf-8 --
# 批量修改源码文件的编码方式为utf-8
# 注意，将此 py 放在 sln 同目录
# UTF-8-SIG 对应 vs 设置的 UTF-8（带 BOM）
 
import os
from chardet import detect
 
# 代码格式获取
def predict_encoding(file_path):
    with open(file_path, 'rb') as f:
        d = detect(f.read())
        file_encoding = d['encoding']
    return file_encoding
 
# 获取目录下所有源文件绝对路径
def get_filepaths(dir):
    filepaths = []
    for rootpath, _, files in os.walk(dir):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".cc") or file.endswith(".h") or file.endswith(".hpp"):
                filepaths.append(os.path.join(rootpath, file))
    return filepaths

if __name__ == '__main__':
    filepaths = get_filepaths("./")
    for fn in filepaths:
        with open(fn, 'rb+') as fp:
            content = fp.read()
            if len(content)==0:
                continue
            else:
                codeType = detect(content)['encoding']
                content = content.decode(codeType, "ignore").encode("UTF-8")
                fp.seek(0)
                fp.write(content)
                print(fn, "：已修改为 utf8 不带 BOM 编码")
 