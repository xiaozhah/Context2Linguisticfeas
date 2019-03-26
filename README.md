# 生成上下文信息 linguistic features
根据fulllab文件夹和questions属性问题集回答得到linguistic，每个音句一个文件存储在fulllab_answers中

显示文件夹差异
`diff -r fulllab_answers_perl fulllab_answers_python`

## 修改进度
1. 都使用编译正则，代码运行更加快速，Python最快！
2. 在Yanping13K（新旧前端,以及更新的913维前端）,BC18三个音库上Python与Perl与C++l答案一样
3. 原来perl代码判断空行是`if ( $line_question eq "\n" )`,但是在Mac上执行的话，即使是空行这个判断也为假，猜测是Mac的换行是"\r\n"，因此改为了`if ( $line_question =~ /^\s*$/ )`,即判断空白字符串的正则。C++也考虑到了`\r\n`。

4. python readlines()读出来的含有换行符要去掉，python是`line=line.rstrip()`,perl是`$line =~ s/\s+$//`,C++是`line[strcspn(line,"\r\n")] = 0;`
5. 重新生成和修改了C++代码，使用了编译的正则表达式，但是速度依然不及Python
6. 准备将C++工程拷贝到VS2017，这样可以查看哪里画的时间较多针对性优化


这个例子中生成的二进制文件用float32存储，每个*.dat文件存储的是N*1246形状的01矩阵。其中1246是属性问题集合中问题的个数

## [Python代码](https://github.com/xiaozhah/Context2Linguisticfeas/blob/master/predict.py)

## [Perl代码](https://github.com/xiaozhah/Context2Linguisticfeas/blob/master/predict.pl)

## [C++代码(OSX的Xcode版本)](https://github.com/xiaozhah/Context2Linguisticfeas/tree/master/predict_C%2B%2B)
需要准备fullab文件夹，question.hed，filelist.lst

在Windows上编译需要在头文件中引入`#include <direct.h>`并且`mkdir(dir, 0777);`改为`_mkdir(dir);`
另外regex.h是Linux/Unix自带的，windows引用必须添加头文件regex.h、regex.lib、regex2.dll
可以在[GnuWin](https://sourceforge.net/projects/gnuwin32/files/regex/2.7/regex-2.7-bin.zip/download)下载到

## C++属性问题集使用正则
### [旧版 predict_C++_regex_slow](https://github.com/xiaozhah/Context2Linguisticfeas/tree/master/predict_C%2B%2B/predict_C%2B%2B_regex_slow)
```C++
#include <iostream>
#include <regex>
using namespace std;

int main() {
    string input = "*-sil+*";
    
  regex pattern1("\\*");
    regex pattern2("\\?");
    regex pattern3("\\$");
    regex pattern4("\\+");
    regex pattern5("\\|");
    regex pattern6("\\^");
    regex pattern7("^([a-z])");
    
    input = regex_replace(input, pattern1, ".*");
    input = regex_replace(input, pattern2, ".");
    input = regex_replace(input, pattern3, "\\$");
    input = regex_replace(input, pattern4, "\\+");
    input = regex_replace(input, pattern5, "\\|");
    input = regex_replace(input, pattern6, "\\^");
    input = regex_replace(input, pattern7 , "^$1");

    cout << input << endl;
    regex r = regex(input);
    
    cout << regex_match("XX-sil+uo/A", r) << endl;
    
    return 0;
}
```

>.*-sil\+.*
1
Program ended with exit code: 0

### [新版 predict_C++_regexh](https://github.com/xiaozhah/Context2Linguisticfeas/tree/master/predict_C%2B%2B/predict_C%2B%2B_regexh) 更快更省内存 使用编译的正则
可在Mac直接编译但是在Windows的VS不行原因是缺少regex.h库
~~据说下载[这里](http://gnuwin32.sourceforge.net/packages/regex.htm)的二进制包就行但是没找到办法~~

后来在[GnuWin](https://sourceforge.net/projects/gnuwin32/files/regex/2.7/regex-2.7-bin.zip/download)下载，但是只提供了**32位**的库文件，对于现在一般64位的工程来说太不合适了

```C++
#include <iostream>
#include <regex>
#include <regex.h>
using namespace std;

int main() {
    int    status;
    regex_t    re;

    string input = "*-sil+*";
    
    regex pattern1("\\*");
    regex pattern2("\\?");
    regex pattern3("\\$");
    regex pattern4("\\+");
    regex pattern5("\\|");
    regex pattern6("\\^");
    regex pattern7("^([a-z])");
    
    input = regex_replace(input, pattern1, ".*");
    input = regex_replace(input, pattern2, ".");
    input = regex_replace(input, pattern3, "\\$");
    input = regex_replace(input, pattern4, "\\+");
    input = regex_replace(input, pattern5, "\\|");
    input = regex_replace(input, pattern6, "\\^");
    input = regex_replace(input, pattern7 , "^$1");
    
    cout << input << endl;
    assert(regcomp(&re, input.c_str(), REG_EXTENDED|REG_NOSUB) == 0);
    
    status = regexec(&re, "XX-sil+uo/A", (size_t) 0, NULL, 0);
    if(status==0)
        cout<<"match"<<endl;
    else
        cout<<"no match"<<endl;
    regfree(&re);
    
    return(0);
}
```
>.*-sil\+.*
match
Program ended with exit code: 0
