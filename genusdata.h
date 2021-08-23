/* Gオプションで渡す属データを扱う
特にゲッター辺り命名規則雑なのは今後修正
*/

#ifndef GENUSDATA_H
#define GENUSDATA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

using uint = unsigned int;

class Genusdata
{
private:
  uint mSpecies; // 
  vector<string> mPairedList; // paired-categoryの一覧
  vector<string> mGenus;
  vector<uint> mFoodx;
  vector<uint> mNonFoodx;
  vector<vector<uint>> mTempIndex;    //シャッフルしたインデックスの格納
  vector<vector<uint>> mTempFoodx;    //シャッフル後のFoodインデックス集合
  vector<vector<uint>> mTempNonFoodx; //シャッフル後のnonFoodインデックス集合
  uint mKind;                         //NonFoodアレルゲン条件の閾値

public:
  void init(const string aFilename, uint aR, uint aKind, uint aSpecies, vector<double> mY); //mGenus, mFoodx, mNonFoodx, mKindを設定 mTempIndexをaR*mGenus.sizeの二次元で初期化
  void make_foodx(uint aR);                               //mTempIndexの確定後にmTempFoodx, mTempNonFoodxの作成

  void set_tempindex(const vector<vector<uint>> aTempIndex); //mTempIndexに値をセット

  vector<string> get_genus() const;              //genusを取得
  vector<uint> get_foodx() const;                //mFoodxを取得
  vector<uint> get_nonfoodx() const;             //mNonFoodxを取得
  vector<vector<uint>> get_tempindex() const;    //mTempIndexを取得
  vector<vector<uint>> get_tempfoodx() const;    //mTempFoodxを取得
  vector<uint> get_tempfoodx(uint i) const;      //override: mTempFoodx[i]を取得
  vector<vector<uint>> get_tempnonfoodx() const; //mTempNonFoodxを取得
  vector<uint> get_tempnonfoodx(uint i) const;   //override: mTempNonFoodx[i]を取得
  uint get_mkind() const;                        //mKindを取得
};

#endif