#ifndef FASTWY_H
#define FASTWY_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <boost/math/distributions/hypergeometric.hpp>

#include "database.h"

#include "genusdata.h"

using namespace std;
using namespace boost::math;

using uint = unsigned int;
using ullint = unsigned long long int;
using uchar = unsigned char;

class FastWY
{
  private:
	//何番目のアイテム集合の前から何番目かを表す
	struct Position
	{
		uint sequence_index;
		uint event_index;
		uint itemset_index;
		bool operator==(const Position &x)
		{
			return (sequence_index == x.sequence_index && event_index == x.event_index && itemset_index == x.itemset_index);
		}
		bool operator!=(const Position &x)
		{
			return (sequence_index != x.sequence_index || event_index != x.event_index || itemset_index != x.itemset_index);
		}
	};

	using projectDB = vector<Position>;

	struct Node
	{
		list<Node>::iterator parent;
		vector<list<Node>::iterator> child;
		vector<Event> pattern;
		string patternStr;
		uint supportSum;
		// uint support;
		vector<uint> support;
		projectDB pdb;
		vector<uint> x;
		double f_val; //P値の最小値（f値）
		double p;	 //P値
		double adjusted_p; //補正後P値
		bool closed;  //CloSpan用．残すべきならTrue
	};

	//パラメータ関係
	const uint MAXVAL = 0xffffffff;
	uint mItemSize;
	// uint minsup;
	double mMinsup;
	uint mMinpat;
	uint mMaxpat;
	int mInterval;
	uint mSupMode;
	uint mCloSpan;
	uint mWild;
	uint mSide;
	uint mGenusMode;
	uint mZeroMode;
	//for G option
	Genusdata mGenusdata;
	vector<vector<uint>> mIndex; //mTempYのシャッフルに対応したインデックスを保存

	vector<double> mL; //P値の最小値保存用
	uint mR;		   //リサンプリング回数

	vector<vector<double>> mTempY; //P値計算用
	vector<double> mPmin;		   //現在の最小のP値
	uint mPlusN;				   //ラベルが＋のデータ数
	double mAlpha;				   //有意水準

	using Key = pair<uint, uint>; //1番目:n11，2番目:n.1
	map<Key, double> mPvalueMap;

	//ここにファイルから読み込んだデータが入る
	vector<vector<Event>> mTransaction;
	vector<Event> mPattern;
	list<Node> mTree;
	using TreeIter = list<Node>::iterator;

	//CloSpanチェック用
	uint mFlagCScheckEnd = 0;
	unordered_map<uint, list<TreeIter>> mCheckPDB;

	//Itemsetチェック用
	uint mFlagItemsetExist = 0;

	Event mWildEvent;

	/**
	 * @fn
	 * 数字として管理しているパターンを文字列にする
	 * @return あるノードにおけるパターンを文字列で出力
	 *mEventSizeとは１；１；１だったら３ということ
	 */
	string pat2str(void)
	{
		stringstream ss;
		for (uint i = 0; i < mPattern.size(); ++i)
		{
			ss << (i ? " " : "");
			if (mPattern[i].first.size() > 0)
			{
				ss << "(";
				for (uint j = 0; j < mPattern[i].first.size(); ++j)
				{
					ss << mPattern[i].first[j];
					ss << (j + 1 < mPattern[i].first.size() ? "_" : "");
				}
				ss << ")";
				if (mPattern[i].second.size() > 0)
					ss << ":";
			}
			for (uint j = 0; j < mItemSize; j++)
			{
				if (mPattern[i].second[j] == MAXVAL)
					ss << "*";
				else
					ss << mPattern[i].second[j];
				ss << (j + 1 < mItemSize ? ":" : "");
			}
		}
		return ss.str();
	}

	string pat2str(const vector<Event> aPattern)
	{
		stringstream ss;
		for (uint i = 0; i < aPattern.size(); ++i)
		{
			ss << (i ? " " : "");
			if (aPattern[i].first.size() > 0)
			{
				ss << "(";
				for (auto it : aPattern[i].first)
				{
					ss << it;
					if (it != aPattern[i].first.back())
						ss << "_";
				}
				ss << ")";
				if (aPattern[i].second.size() > 0)
					ss << ":";
			}
			for (auto it : aPattern[i].second)
			{
				if (it == MAXVAL)
					ss << "*";
				else
					ss << it;
				if (it != aPattern[i].second.back())
					ss << ":";
			}
		}
		return ss.str();
	}

	bool calculate(Node &aNode);
	bool calculate_new(Node &aNode);
	void project(const TreeIter aNodeIter);
	void project_new(projectDB &aPdb, const TreeIter aParent);
	/**
	 *あるデータの中の任意のパターンのサポートを数える
	 *mode:0,(1,2,3以外の数字が入った時) supportは１レコード1まで
	 *mode:1 単純なパターンの数え上げ，1レコードにいくらでも可能
	 *@return supprt数
	 **/
	uint calcSup(uint aId, vector<Event> aPattern);

	/*
	 *trainTransaction[aId]であるデータ内からパターンがaPatternであるものを数え上げる
	 *@return 数え上げた数
	 */
	uint calcPat(uint aId, vector<Event> aPattern);

	/*
	 * 入力された系列が包含関係かどうか調べる．
	 * 包含関係なら1番目か2番目のどちらが短いか（subsequence）を返す．
	 * @return 1 or 2 or 0(包含関係ではない or 全く同一系列)
	 * @author takuto
	 */
	uint isSubsequence(const vector<Event> aSeq1, const vector<Event> aSeq2);

	/*
	 * Event1がEvent2を包含しているか調べる
	 * @return true (Event1は2を包含している) or false
	 */
	bool isInclude(const Event aEvent1, const Event aEvent2);
	/* aNodeIterがaChildIterの親かどうか辿って調べる
	 *
	 */
	bool isParent(const TreeIter aNodeIter, const TreeIter aChildIter);

	void checkProjectedDB(const TreeIter aCurrentIter);
	/*
	 * CloSpan用
	 * @return PDB内の総アイテム数
	 * @author takuto
	 */
	uint sum_items_pdb(const projectDB aPdb);
	//カイ2乗検定
	double chiSquareTest(const Node aNode);

	//フィッシャーの正確確率検定
	//double exactTest_double(const Node aNode);
	//vector<double> exactTest_double_vector(const Node aNode);

	double exactTest(const Node aNode);
	vector<double> exactTest_vector(const Node aNode);

	//子の持つパターン情報を更新
	void childPatternUpdate(const TreeIter aChildIter);

	//木を辿る
	void search_tree(const TreeIter aNodeIter);

	//補正後の有意水準決定後に木を辿る
	void search_tree_final(const TreeIter aNodeIter);

  public:
	uint mN;
	vector<double> mY;
	double mDelta; //補正後有意水準
	unordered_map<uint, list<TreeIter>> mSignificant;

	//コンストラクタ
	FastWY(double aMinsup, uint aMinpat, uint aMaxpat, uint aR, double aAlpha, uint aCloSpan, uint aSupMode, int aInterval, uint aWild, uint aSide, uint aGenusMode, Genusdata aGenusdata, uint aZeroMode)
	{
		mMinsup = aMinsup;
		mMinpat = aMinpat;
		mMaxpat = aMaxpat;
		mR = aR;
		mAlpha = aAlpha;
		mCloSpan = aCloSpan;
		mSupMode = aSupMode;
		mInterval = aInterval;
		mWild = aWild;
		mSide = aSide;
		mGenusMode = aGenusMode;
		mGenusdata = aGenusdata;
		mZeroMode = aZeroMode;
	};

	//組み合わせ数計算（対数版）
	double log_combination(const uint aS, const uint aT);
	void init(const vector<vector<Event>> aTransaction, const vector<double> aY);
	void main();
	void printTree(string aFilename);
	void printSigPattern(string aFilename);

	//for G option
	bool skipCalcPflag(uint i, const Node aNode, uint supPlus, uint N);
	uint skipSearchTreeFlag(const Node aNode);

	//for Z option
	bool skipCalcPflag_z(const Node aNode, uint supPlus, uint N);

	//for G option (single-category)
	int count_category(const Node aNode);

};

#endif
