#define MAX_SENT_NUM 15000
#define MAX_QUES_NUM 2000
#define MAX_SubQUES_NUM 100
#define MAX_Lab_NUM 1000
#define MAX_STR_LEN 1024
#define MAX_PATH_LEN 1024
#define MAX_SENT_LEN 8000
#include <regex>

struct QUES_INFO
{
	int OneQues_Num;
	char Ques_Name[MAX_STR_LEN];
    std::regex Ques_Str[MAX_SubQUES_NUM];
};

int LoadQues(char *sz_filename, QUES_INFO *Ques_Infoo);
int LoadFullLab(char *sz_filename, char **sz_FullLab_Str);

void AnswerQues(float **Answer_Data, char **, QUES_INFO *Ques_Infoo, int Label_Num, int Ques_Num);

void Output_Linguisticfeas(char *sz_OutputTrainFile,float **Answer_Data, int Label_Num, int Ques_Num);
