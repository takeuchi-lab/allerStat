from sklearn import svm
from sklearn.metrics import confusion_matrix, f1_score, roc_curve, roc_auc_score
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split, GridSearchCV, StratifiedKFold
import pickle
import os
import re  # 正規表現
import seaborn as sns
import matplotlib.pyplot as plt
from tqdm import tqdm_notebook as tqdm
from IPython.display import display
import numpy as np
import pandas as pd
import protein_utility
# import string_alignment
from inspect import currentframe
import sys


sns.set()

SEED = 1

pd.set_option("display.max_columns", 100)

aa_list = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "K", "L",
           "M", "N", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"]
# 20種+4


def chkprint(*args):
    names = {id(v): k for k, v in currentframe().f_back.f_locals.items()}
    print(', '.join(names.get(id(arg), '???')+' = '+repr(arg) for arg in args))

# input: dataframe(amino acid sequence), amino acid list  output: dataframe(Number sequence)(label, sequence)


def aa_to_numseq(df_aa, aa_list, pbar=0, output_columns=["label", "sequence"]):
    df_aa = df_aa.loc[:, output_columns]
    df_numseq = pd.DataFrame(columns=output_columns)
    # メモ:df_aa["label]を代入する時にindexも付いていってしまうからdf_aaのインデックスに被りがあるとsequenceが上書きされていってしまいバグに.
    df_numseq["label"] = df_aa["label"]
    for index_aa, row_aa in df_aa.iterrows():  # df各行の
        numseq_row = ""
        for aa in row_aa["sequence"]:  # sequence列の各文字に対して
            if aa != " ":  # はじまりの空白無視
                if aa not in aa_list:  # はじまりの空白無視
                    print(aa)
                    print(row_aa)
                    sys.stderr.write("Not included item : {0}\n".format(aa))
                    sys.exit()
                numseq_row += str(aa_list.index(aa)) + " "
        df_numseq.at[index_aa, "sequence"] = numseq_row[0:len(
            numseq_row)-1-1]  # 最後の空白削除
    return df_numseq

# input : aa_list(list of usable alphabet)


def show_relation(aa_list):
    for i in range(len(aa_list)):
        print("{0:<2} ".format(aa_list[i]), end="")
    print("")
    for i in range(len(aa_list)):
        print("{0:<2} ".format(i), end="")
    print("")

# input: df(number sequence) output: df(aaseq)(label, pattern)


def numseq_to_aa(df_numseq, aa_list, output_columns=["label", "pattern"]):
    df_numseq = df_numseq.loc[:, output_columns]
    df_aa = pd.DataFrame(columns=output_columns)
    df_aa["label"] = df_numseq["label"]
    for index_numseq, row_numseq in df_numseq.iterrows():
        aaseq_row = ""
        pattern = str(row_numseq["pattern"])
        for num in pattern.split(" "):
            if num != " " and num != "":
                if int(num) >= len(aa_list):
                    print(num)
                    print(row_numseq)
                    sys.stderr.write("Not usable number : {0}\n".format(num))
                    sys.exit()
                aaseq_row += aa_list[int(num)]
        df_aa.at[index_numseq, "pattern"] = aaseq_row
    return df_aa

# 短い方が長い方に含まれていたらTrueを返す(score計算した時の)


def is_match_condition_True(epitope, pattern):
    if len(epitope) <= len(pattern):
        return epitope in pattern
    else:
        return pattern in epitope

# input: pattern, sequence / output: similarity


def calculate_similarity(pattern, sequence):
    if(len(pattern) > len(sequence)):  # パターンの系列長の方が長い場合は0
        return 0
    count_max = 0
    for window_position in range(len(sequence)-len(pattern)):
        count = 0
        for window_i in range(len(pattern)):
            if(pattern[window_i] == sequence[window_position+window_i]):
                count += 1
        count_max = max(count, count_max)
    return count_max

# input: pattern, sequence / output: match or not (1 or 0)


def is_match(pattern, sequence):
    # in 演算子はlen(pattern) > len(sequence)の場合に確定False
    if(pattern in sequence):
        return 1
    else:
        return 0

# input: df_X(train or test), df_output, output_filename  save: df_X_values((train or test)×pattern)  option:similarity(default 0)


def make_csv_for_heatmap(df_X, df_output, output_filename, similarity=0):
    df_X_values = pd.DataFrame(columns=range(0, len(df_output)-1))

    with tqdm(range(len(df_X)*len(df_output))) as pbar:
        for id_X, row_X in df_X.iterrows():
            sequence = row_X["sequence"]
            for id_output, row_output in df_output.iterrows():
                pattern = row_output["pattern"]
                # df_values_map.at[id_train, id_output] = calculate_similarity(sequence, pattern) #一致文字数
                if similarity == 0:
                    df_X_values.at[id_X, id_output] = is_match(
                        pattern, sequence)  # 一致 or 不一致
                elif similarity == 1:
                    df_X_values.at[id_X, id_output] = calculate_similarity(
                        pattern, sequence)  # 類似度([0, 1])
                pbar.update(1)
    print(df_X_values.shape)
    display(df_X_values)
    df_X_values.to_csv(output_filename, header=False)


# input: series(sequence, no header), aa_list output: counted aa list
def count_aa(series, aa_list, show=True):
    count_list = list()
    for aa_index, aa_counted in enumerate(aa_list):
        count = 0
        for row in series:
            count += row.count(aa_counted)
        count_list.append(count)
        if show == True:
            print(aa_counted, ": {0:>6}".format(count))
    print("")
    # return count_list

# input:file(file path), before(word), after(word). Replace string in file.


def replaceForFile(file, before, after):
    if not os.path.isfile(file):
        print("Error on replace_word, not a regular file : " + file)
        sys.exit(1)
    fRead = open(file, mode='r', encoding='utf-8').read()
    fWrite = open(file, mode='w', encoding='utf-8')
    fWrite.write(fRead.replace(before, after))


# 並列処理を考慮して1行分の\bmx_iをリストで作成しreturnする
# freq=1で頻度, 0で01特徴
# similarity=1で類似度による[0, 1]特徴
def make_Xrow_for_fastwy(i_row, df_sequence, sr_output_pattern, aa_list, freq=False, similarity=False):
    X_row = [0]*len(sr_output_pattern)

    row = df_sequence.loc[i_row]
    seq = row["sequence"]

    for id_output, row_output in sr_output_pattern.iteritems():
        pattern = row_output
        if similarity == False and (pattern in seq):
            if freq == False:
                X_row[id_output] = 1
            else:
                X_row[id_output] += 1
        else:
            X_row[id_output] = string_alignment.lgp_blosum62().search(pattern, seq)

    # 10ごとに進捗としてindex表示, 200ごとに改行
    if(i_row % 100 == 0):
        print("{:>5}".format(i_row), end=" ", flush=True)
        if(i_row % 1000 == 0 and i_row != 0):
            print("")
    return [i_row, X_row, df_sequence.at[i_row, "label"]]

# 並列化パーツ


def wrapper(args):
    return make_Xrow_for_fastwy(*args)
