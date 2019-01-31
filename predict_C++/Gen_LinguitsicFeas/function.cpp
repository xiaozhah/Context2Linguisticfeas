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
                Ques_Infoo[Ques_Idx].Ques_Str[OneQues_num] = regex(q);
                
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
        sz_line[strcspn(sz_line, "\r\n")]=0;
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
                status = regex_match(sz_FullLab_Str[i],Ques_Infoo[j].Ques_Str[k]);
                if(status==1)
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
