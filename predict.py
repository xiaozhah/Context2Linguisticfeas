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

if __name__ == '__main__':
    labdir   = "./fulllab";
    que_file = "./questions.hed";
    outdir   = "./fulllab_answers_python";

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