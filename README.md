# Context2Linguistic

<!-- toc -->

# 生成上下文信息 linguistic features
根据fulllab文件夹和questions属性问题集回答得到linguistic，每个音句一个文件存储在fulllab_answers中

显示文件夹差异
`diff -r fulllab_answers_perl fulllab_answers_python`

## 修改进度
1. 都使用编译正则，代码运行更加快速，Python最快！
2. 在Yanping13K（新旧前端,以及更新的913维前端）,BC18三个音库上Python与Perl与C++l答案一样
3. 原来perl代码判断空行是`if ( $line_question eq "\n" )`,但是在Mac上执行的话，即使是空行这个判断也为假，猜测是Mac的换行是"\r\n"，因此改为了`if ( $line_question =~ /^\s*$/ )`,即判断空白字符串的正则。C++也考虑到了`\r\n`。
4. 重新生成和修改了C++代码，使用了编译的正则表达式，但是速度依然不及Python
5. 准备将C++工程拷贝到VS2017，这样可以查看哪里画的时间较多针对性优化

<!-- more -->

## Python代码
```python
import os,re,glob
import numpy as np
from tqdm import tqdm

def Get_Ques2Regular(ques_file):
    lines = filter(lambda i:i.strip()!='', open(ques_file,'rt').readlines())
    ques_lists = []
    for line in lines:
        ques_list = []
        sub_ques = re.findall(r'[{](.*)[}]', line)[0].split(',')
        for q in sub_ques:
            q = q.replace('*',r'.*').replace('?',r'.').replace('$',r'\$')\
                 .replace('+',r'\+').replace('|',r'\|').replace('^',r'\^')
            q = re.sub(r'^([a-z])',r'^\1', q)
            q = re.sub(r'([^*])$',r'\1$', q)
            # Compile pattern is Very important. Make 10X faster than originals!
            # Original(1) ques_list.append(q)
            ques_list.append(re.compile(q))
        ques_lists.append(ques_list)
    return ques_lists

def fulllab2ling(lab):
    lab = lab.rstrip()
    linguistic_vec = np.zeros(len(ques_lists), dtype=np.float32)
    for i, sub_ques in enumerate(ques_lists):
        for sub_que in sub_ques:
            # Original(2) re.match(sub_que, q)
            if(sub_que.match(lab)):
                linguistic_vec[i] = 1
                break
    return linguistic_vec

def SaveMkdir(dir):
    try:
        if not os.path.exists(dir):
            os.mkdir(dir)
    except:
        os.makedirs(dir)

if __name__ == '__main__':
    labdir   = "./fulllab";
    que_file = "./questions.hed";
    outdir   = "./fulllab_answers_python";

    SaveMkdir(outdir)
    ques_lists = Get_Ques2Regular(que_file)
    ref_files = glob.glob(os.path.join(labdir,'*.lab'))
    for name in tqdm(sorted(ref_files)):
        basename = os.path.basename(name)[:-4]
        tqdm.write('process %s' % basename)
        lab_file = os.path.join(labdir, basename+'.lab')
        linguistic_file = os.path.join(outdir, basename+'.dat')

        labs = open(lab_file,'rt').readlines()
        linguistic_Mat = np.r_[map(fulllab2ling,labs)]
        linguistic_Mat.tofile(linguistic_file)
```

## Perl代码

```perl
$| = 1;

$labdir = "./fulllab";
$que    = "./questions.hed";
$outdir = "./fulllab_answers_perl";
mkdir $outdir, 0755;
opendir DIR, $labdir;
$i = 1;
Get_Ques2Regular($que);
foreach $_ ( sort {$a <=> $b} readdir DIR ) 
{
  if (/(.*)\.lab/) 
  {
    $filename = $1;
    print $filename, "\t", "$i\n";
    $i++;
    $lab = sprintf("$labdir/$filename.lab");
    $out = sprintf("$outdir/$filename.dat");
    feature( $lab, $que, $out );
  }
}
closedir DIR;

sub Get_Ques2Regular
{
  open( question, "<@_[0]" ) or die "questions.hed can't be open";
  %Ques2Regular;
  while ( $line_question = <question> )
  {
    if ( $line_question =~ /^\s*$/ )
    {
        next;
    }
    else 
    {
     if ( $line_question =~ /\{(.*)\}/ ) 
      {
        @phoneall = split( /,/, $1 );
      }
      foreach $phoneone (@phoneall) 
      {
        $original_phoneone = $phoneone;
        $phoneone =~ s/\*/\.\*/g;    #(*->.*)
        $phoneone =~ s/\?/\./g;      #(?->. )
        $phoneone =~ s/\$/\\\$/g;    #($->\$)
        $phoneone =~ s/\+/\\\+/g;    #(+->\+)
        $phoneone =~ s/\|/\\\|/g;    #(|->\|)
        $phoneone =~ s/\^/\\\^/g;    #(^->\^)
        $phoneone =~ s/^([a-z])/\^$1/; # such as m-* -> ^m-*
        $phoneone =~ s/([^*])$/$1\$/; # such as m-* -> ^m-*
        $phoneone = qr/$phoneone/i; # Compile regex pattern
        $Ques2Regular{$original_phoneone} = $phoneone unless exists $Ques2Regular{$original_phoneone};
      }
    }
  }
  close(question);
}

sub feature 
{
  open( label,    "<@_[0]" ) or die "00000001.lab can't be open";
  open( question, "<@_[1]" ) or die "questions.hed can't be open";
  open( output,   ">@_[2]" ) or die "output.txt is null";
  binmode output;

  while ( $line_label = <label> ) 
  {
    seek question, 0, 0;
    $line_label =~ s/\s+$//; # similar to line_label.rstrip() in Python
    while ( $line_question = <question> ) 
    {
      if ( $line_question =~ /^\s*$/ )
      {
          next;
      }
      else {
          if ( $line_question =~ /\{(.*)\}/ ) 
          {
              @phoneall = split( /,/, $1 );
          }
          $temp = 0;
          foreach $phoneone (@phoneall) 
          {
              $phoneone = $Ques2Regular{$phoneone};
              if ( $line_label =~ /$phoneone/ ) 
              {
                  $temp = 1;
                  print output pack( "f", 1 );
                  last;
              }
          }
          if ( $temp == 0 ) 
          {
              print output pack( "f", 0 );
          }
        }
    }
  }
  close(label);
  close(question);
  close(output);
}
```
## C++代码(Mac上编译成功)
需要准备fullab文件夹，question.hed，filelist.lst
```C++
#include <stdio.h>
#include <string.h>
#include "iostream"
#include <stdlib.h>
#include <assert.h>
#include "function.hpp"
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>

int main()
{
  int Ques_Num, Label_Num, Sent_Num;
  char sz_InputLabFile[MAX_PATH_LEN], sz_OutputAnsResFile[MAX_PATH_LEN];
  char sz_listline[MAX_STR_LEN];

  clock_t  start, end;
  start = clock();

  char sz_ListFile_All[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/yp13K_IFLY/filelist.lst";
  char sz_QuesFile[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/yp13K_IFLY/questions.hed";
  char sz_InputLabDir[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/yp13K_IFLY/fulllab";
  char sz_OutputAnsResDir[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/yp13K_IFLY/fulllab_answers_C++";
    //In Windows it should be _mkdir(sz_OutputAnsResDir);
    //And it also needs #include <direct.h>
    mkdir(sz_OutputAnsResDir, 0777);
    
  QUES_INFO* Ques_Infoo = new QUES_INFO[MAX_QUES_NUM];
    for(int i=0;i<MAX_QUES_NUM;i++)
        memset(Ques_Infoo[i].Ques_Name, 0, sizeof(char)*MAX_STR_LEN);
    
    char** fullab_str = new char* [MAX_Lab_NUM];
    for(int i=0;i<MAX_Lab_NUM;i++)
        fullab_str[i] = new char[MAX_STR_LEN];

  float** Answer_Data = new float* [MAX_Lab_NUM];
  for (int i = 0; i < MAX_Lab_NUM; i++)
    Answer_Data[i] = new float[MAX_QUES_NUM];
    
  Ques_Num = LoadQues(sz_QuesFile, Ques_Infoo);
  assert(Ques_Num <= MAX_QUES_NUM);
    
    FILE *fp_list;
  fp_list = fopen(sz_ListFile_All, "r");
    int* SentenceList = new int[MAX_SENT_NUM];
  memset(SentenceList, 0, sizeof(int)*MAX_Lab_NUM);
    Sent_Num = 0;
    while (fgets(sz_listline, MAX_STR_LEN, fp_list))
  {
    sscanf(sz_listline, "%d", &SentenceList[Sent_Num]);
    Sent_Num++;
  }
  fclose(fp_list);

  for (int i = 0; i < Sent_Num; i++)
  {
        printf("Now Processing %08d\n", SentenceList[i]);

    for (int j = 0; j < MAX_Lab_NUM; j++)
      memset(Answer_Data[j], 0, sizeof(float)*MAX_QUES_NUM);

    sprintf(sz_InputLabFile, "%s/%08d.lab", sz_InputLabDir, SentenceList[i]);

    Label_Num = LoadFullLab(sz_InputLabFile, fullab_str);
    AnswerQues(Answer_Data, fullab_str, Ques_Infoo, Label_Num, Ques_Num);
    sprintf(sz_OutputAnsResFile, "%s/%08d.dat", sz_OutputAnsResDir, SentenceList[i]);
        
    Output_Linguisticfeas(sz_OutputAnsResFile, Answer_Data, Label_Num, Ques_Num);
  }

    for(int i=0;i<Ques_Num;i++)
        for(int j=0;j<Ques_Infoo[i].OneQues_Num;j++)
        regfree(&Ques_Infoo[i].Ques_Str[j]);
    
    delete[]Ques_Infoo;Ques_Infoo=NULL;
    delete[] SentenceList;SentenceList=NULL;
  for (int i = 0; i < MAX_Lab_NUM; i++)
    {
    delete[] Answer_Data[i];Answer_Data[i] = NULL;
        delete[] fullab_str[i];fullab_str[i]=NULL;
    }
  delete[] Answer_Data;Answer_Data=NULL;

  end = clock();
    printf("\nThe elapsed time is: %0.2fs\n", (double)(end - start) / CLOCKS_PER_SEC);
  return 1;
}
```
### function.cpp
```C++
#include <stdio.h>
#include <string.h>
#include "iostream"
#include <stdlib.h>
#include <assert.h>
#include "function.hpp"
#include <algorithm>
#include <regex>
using namespace std;

int LoadQues(char *sz_filename, QUES_INFO *Ques_Infoo)
{
  int nRett, Ques_Idx, OneQues_num, len;
  char sz_line[MAX_STR_LEN], tmp[100];

  char seps[] = " ,}\r\n";
  char *tokenn, *pdestt;
    string q;

    regex pattern1("\\*");
    regex pattern2("\\?");
    regex pattern3("\\$");
    regex pattern4("\\+");
    regex pattern5("\\|");
    regex pattern6("\\^");
    regex pattern7("^([a-z])");
    regex pattern8("([^*])$");
    
  FILE *fp_ques = fopen(sz_filename, "r");
  Ques_Idx = 0;
  while (fgets(sz_line, MAX_STR_LEN, fp_ques))
  {
    if (strstr(sz_line, "QS"))
    {
      OneQues_num = 0;

      nRett = sscanf(sz_line, "QS %s", tmp);//Chinese
      //nRett = sscanf(sz_line, "QS \"%[^\"]s\"", tmp);//English
      len = (int)strlen(tmp);
      memcpy(Ques_Infoo[Ques_Idx].Ques_Name, tmp, sizeof(char)*len);

      pdestt = strstr(sz_line, "{");
      tokenn = strtok(pdestt + 1, seps);

      while (tokenn != NULL)
      {
                q = string(tokenn);
                q = regex_replace(q, pattern1, ".*");
                q = regex_replace(q, pattern2, ".");
                q = regex_replace(q, pattern3, "\\$");
                q = regex_replace(q, pattern4, "\\+");
                q = regex_replace(q, pattern5, "\\|");
                q = regex_replace(q, pattern6, "\\^");
                q = regex_replace(q, pattern7, "^$1");
                q = regex_replace(q, pattern8, "$1$");
                
                assert(regcomp(&Ques_Infoo[Ques_Idx].Ques_Str[OneQues_num], q.c_str(), REG_EXTENDED|REG_NOSUB) == 0);
                
        OneQues_num++;

        tokenn = strtok(NULL, seps);
      }
            
            assert(OneQues_num < MAX_SubQUES_NUM);
      Ques_Infoo[Ques_Idx].OneQues_Num = OneQues_num;
            
      Ques_Idx++;
    }
  }

  fclose(fp_ques);
  return Ques_Idx;
}

int LoadFullLab(char *sz_filename, char **sz_FullLab_Str)
{
  char sz_line[MAX_STR_LEN];

  for (int i = 0; i < MAX_Lab_NUM; i++)
    memset(sz_FullLab_Str[i], 0, sizeof(char)*MAX_STR_LEN);

  int Lab_Num = 0;
  FILE *fp_lab;
  fp_lab = fopen(sz_filename, "r");
  while (fgets(sz_line, MAX_STR_LEN, fp_lab))
  {
    sz_line[strcspn(sz_line,"\r\n")] = 0;
    strcpy(sz_FullLab_Str[Lab_Num], sz_line);
    Lab_Num++;
  }
  fclose(fp_lab);
  return Lab_Num;
}

void AnswerQues(float **Answer_Data, char **sz_FullLab_Str, QUES_INFO *Ques_Infoo, int Label_Num, int Ques_Num)
{
    int i,j,k,status;
  for (i = 0; i < Label_Num; i++)  // each lab
    for (j = 0; j < Ques_Num; j++) // each ques
      for (k = 0; k < Ques_Infoo[j].OneQues_Num; k++)  // each one little ques
            {
                status = regexec(&Ques_Infoo[j].Ques_Str[k], sz_FullLab_Str[i], (size_t) 0, NULL, 0);
                if(status==0)
                {
                    Answer_Data[i][j] = 1;
                    break;
                }
            }
}

void Output_Linguisticfeas(char *sz_OutputTrainFile,
  float **Answer_Data, int Label_Num, int Ques_Num)
{
  FILE *fp_out;
  fp_out = fopen(sz_OutputTrainFile, "wb");
  for (int i = 0; i < Label_Num; i++)
        fwrite(Answer_Data[i], sizeof(float), Ques_Num, fp_out);
  fclose(fp_out);
}
```
### function.hpp
```C++
#define MAX_SENT_NUM 15000
#define MAX_QUES_NUM 2000
#define MAX_SubQUES_NUM 100
#define MAX_Lab_NUM 1000
#define MAX_STR_LEN 1024
#define MAX_PATH_LEN 1024
#define MAX_SENT_LEN 8000
#include "regex.h"

struct QUES_INFO
{
  int OneQues_Num;
  char Ques_Name[MAX_STR_LEN];
    regex_t Ques_Str[MAX_SubQUES_NUM];
};

int LoadQues(char *sz_filename, QUES_INFO *Ques_Infoo);
int LoadFullLab(char *sz_filename, char **sz_FullLab_Str);

void AnswerQues(float **Answer_Data, char **, QUES_INFO *Ques_Infoo, int Label_Num, int Ques_Num);

void Output_Linguisticfeas(char *sz_OutputTrainFile,float **Answer_Data, int Label_Num, int Ques_Num);
```

在Windows上编译需要在头文件中引入`#include <direct.h>`并且`mkdir(dir, 0777);`改为`_mkdir(dir);`
另外regex.h是Linux/Unix自带的，windows引用必须添加头文件regex.h、regex.lib、regex2.dll
可以在[GnuWin](https://sourceforge.net/projects/gnuwin32/files/regex/2.7/regex-2.7-bin.zip/download)下载到

## C++属性问题集使用正则
### 旧版
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

### 新版 更快更省内存 使用编译的正则
可在Mac直接编译但是在Windows的VS不行原因是缺少regex.h库
~~据说下载[这里](http://gnuwin32.sourceforge.net/packages/regex.htm)的二进制包就行但是没找到办法~~后来在[GnuWin](https://sourceforge.net/projects/gnuwin32/files/regex/2.7/regex-2.7-bin.zip/download)下载到
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
