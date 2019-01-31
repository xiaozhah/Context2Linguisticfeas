#include <stdio.h>
#include <string.h>
#include "iostream"
#include <stdlib.h>
#include <assert.h>
#include "function.hpp"
#include <algorithm>
using namespace std;


int LoadQues(char *sz_filename, QUES_INFO *Ques_Infoo)
{
    int nRett, Ques_Idx, OneQues_num, len;
    char sz_line[MAX_STR_LEN], tmp[100];
    
    char seps[] = " ,}\n";
    char *tokenn, *pdestt;
    
    FILE *fp_ques = fopen(sz_filename, "r");
    
    Ques_Idx = 0;
    while (fgets(sz_line, MAX_STR_LEN, fp_ques))
    {
        if (strstr(sz_line, "QS"))
        {
            OneQues_num = 0;
            
            nRett = sscanf(sz_line, "QS %s", tmp);//中文
            //nRett = sscanf(sz_line, "QS \"%[^\"]s\"", tmp);英文
            len = (int)strlen(tmp);
            memcpy(Ques_Infoo[Ques_Idx].Ques_Name, tmp, sizeof(char)*len);
            
            pdestt = strstr(sz_line, "{");
            tokenn = strtok(pdestt + 1, seps);
            
            while (tokenn != NULL)
            {
                //printf("%s\n", tokenn);
                strcpy(Ques_Infoo[Ques_Idx].Ques_Str[OneQues_num], tokenn);
                OneQues_num++;
                
                tokenn = strtok(NULL, seps);
            }
            
            Ques_Infoo[Ques_Idx].OneQues_Num = OneQues_num;
            
            Ques_Idx++;
        }
    }
    
    fclose(fp_ques);
    
    return Ques_Idx;
}

int LoadFullLab(char *sz_filename, LAB_INFO *Lab_Infoo)
{
    int Lab_Num;
    char sz_line[MAX_STR_LEN];
    FILE *fp_lab;
    
    Lab_Num = 0;
    
    fp_lab = fopen(sz_filename, "r");
    
    while (fgets(sz_line, MAX_STR_LEN, fp_lab))
    {
        sz_line[strcspn(sz_line,"\r\n")] = 0;
        strcpy(Lab_Infoo[Lab_Num].sz_lab_str, sz_line);
        Lab_Num++;
    }
    
    fclose(fp_lab);
    
    return Lab_Num;
}

void AnswerQues(float **Answer_Data, LAB_INFO *Lab_Infoo, QUES_INFO *Ques_Infoo, int Label_Num, int Ques_Num)
{
    int ii, jj, kk;
    for (ii = 0; ii < Label_Num; ii++)  // each lab
        for (jj = 0; jj < Ques_Num; jj++) // each ques
            for (kk = 0; kk < Ques_Infoo[jj].OneQues_Num; kk++)  // each one little ques
                if (DoMatch(Lab_Infoo[ii].sz_lab_str, Ques_Infoo[jj].Ques_Str[kk]))
                    Answer_Data[ii][jj] = 1.0f;
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

bool DoMatch(char *s, char *p)
{
    int slen, minplen, numstars;
    char *q, c;
    
    slen = (int)strlen(s);
    minplen = 0; numstars = 0; q = p;
    while ((c = *q++))
        if (c == '*') ++numstars; else ++minplen;
    return RMatch(s, p, slen, minplen, numstars);
}

bool RMatch(char *s, char *p, int slen, int minplen, int numstars)
{
    if (slen == 0 && minplen == 0)
        return true;
    if (numstars == 0 && minplen != slen)
        return false;
    if (minplen > slen)
        return false;
    if (*p == '*')
        return RMatch(s + 1, p + 1, slen - 1, minplen, numstars - 1) ||
        RMatch(s, p + 1, slen, minplen, numstars - 1) ||
        RMatch(s + 1, p, slen - 1, minplen, numstars);
    if (*p == *s || *p == '?')
        return RMatch(s + 1, p + 1, slen - 1, minplen - 1, numstars);
    else
        return false;
}
