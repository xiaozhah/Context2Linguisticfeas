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
    
    char sz_ListFile_All[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/BC18/filelist.lst";
    char sz_QuesFile[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/BC18/questions.hed";
    char sz_InputLabDir[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/BC18/fulllab";
    char sz_OutputAnsResDir[MAX_PATH_LEN] = "/Users/xzhou/programming/TTS/context/BC18/fulllab_answers_C++";
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
