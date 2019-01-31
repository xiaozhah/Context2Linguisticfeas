#define MAX_SENT_NUM 15000
#define MAX_QUES_NUM 2000
#define MAX_SubQUES_NUM 100
#define MAX_Lab_NUM 1000
#define MAX_STR_LEN 1024
#define MAX_PATH_LEN 1024
#define MAX_SENT_LEN 8000
#define STATE_NUM 5

struct LAB_INFO
{
    char sz_lab_str[MAX_STR_LEN];
    int State_frame_beg[STATE_NUM];
    int State_frame_end[STATE_NUM];
    int State_frame_len[STATE_NUM];
};

struct QUES_INFO
{
    int OneQues_Num;  //一个问题包含的子问题的个数
    char Ques_Name[256]; //问题的名称
    char Ques_Str[100][256]; //所有示例
};

int LoadQues(char *sz_filename, QUES_INFO *Ques_Infoo);

void AnswerQues(float **Answer_Data, LAB_INFO *Lab_Infoo, QUES_INFO *Ques_Infoo, int Label_Num, int Ques_Num);

void Output_Linguisticfeas(char *sz_OutputTrainFile,float **Answer_Data, int Label_Num, int Ques_Num);

bool RMatch(char *s, char *p, int slen, int minplen, int numstars);
bool DoMatch(char *s, char *p);
int LoadFullLab(char *sz_filename, LAB_INFO *Lab_Infoo);
