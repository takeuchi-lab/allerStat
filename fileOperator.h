/*
入力ファイルの扱い, paired-categoryの管理など
アミノ酸配列データを受け取ってtDatabase, tGenusdataを作成
アミノ酸配列と数字系列を変換するなど
*/


#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


using namespace std;

using uint = unsigned int;

class FileOperator
{
    private:
        string tableFileName; //Tオプションで受け取るファイル名(suptable形式)


    public:
        string tablefile_to_numseqfile(); //元ファイルを数字系列のファイル(デフォルトの形式)に変換して一時ファイルとして保存, ファイル名を返す
        //TODO: ファイルの存在を確認し存在すれば別の名前を使用する, 一時ファイル名を保持
        string tablefile_to_genusfile(); //元ファイルからgenusdataで読むgenusfileの一時ファイルを作成
        //TODO: genusdataにpaired絡みの操作を実装
};

#endif