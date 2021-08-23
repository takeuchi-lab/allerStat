/*
 * fastWY.cc
 * 「生命情報処理における機械学習～多重検定と推定量設計」という本を参考に実装している
 *
 */
#include "fastWY.h"
//木丸ごと書き込む時はこっち
void FastWY::printTree(string aFilename)
{
	ofstream tFile(aFilename);
	tFile << "pattern,length,p,adjusted-p,f(d = " << mDelta << "),supportSum,sup+,sup-,label" << '\n';
	for (auto it = ++mTree.begin(); it != mTree.end(); ++it)
	{
		tFile << it->patternStr << "," << it->pattern.size() << "," << it->p << "," << it->adjusted_p << "," << it->f_val << "," << it->supportSum << ",";
		uint sup_p = 0;
		uint sup_m = 0;
		for (uint j = 0; j < it->support.size(); j++)
		{
			if (mY[it->x[j]] > 0)
				sup_p += it->support[j];
			else
				sup_m += it->support[j];
		}
		tFile << sup_p << "," << sup_m << ",";
		if (sup_p > it->supportSum * mPlusN / mN)
			tFile << "1";
		else if (sup_p < it->supportSum * mPlusN / mN)
			tFile << "-1";
		else
			tFile << "N";
		tFile << '\n';
	}
}

void FastWY::printSigPattern(string aFilename)
{
	ofstream tFile(aFilename);
	tFile << "pattern,length,p,adjusted_p,f(d = " << mDelta << "),supportSum,sup+,sup-,label"
		  << "\n";
	for (auto it : mSignificant)
	{
		for (auto itr : it.second)
		{
			tFile << itr->patternStr << "," << itr->pattern.size() << "," << itr->p << "," << itr->adjusted_p << "," << itr->f_val << "," << itr->supportSum << ",";
			uint sup_p = 0;
			uint sup_m = 0;
			for (uint j = 0; j < itr->support.size(); j++)
			{
				if (mY[itr->x[j]] > 0)
					sup_p += itr->support[j];
				else
					sup_m += itr->support[j];
			}
			tFile << sup_p << "," << sup_m << ",";
			if (sup_p > itr->supportSum * mPlusN / mN)
				tFile << "1";
			else if (sup_p < itr->supportSum * mPlusN / mN)
				tFile << "-1";
			else
				tFile << "N";

			tFile << "\n";
		}
	}
}

/* 組み合わせ数計算（対数版）
 * sCtを対数を使って計算する
 */
double FastWY::log_combination(const uint aS, const uint aT)
{
	double tLog_s = 0;
	double tLog_t = 0;

	for (uint i = aS; i > aS - aT; --i)
	{
		tLog_s += log(i);
	}

	for (uint i = aT; i > 0; --i)
	{
		tLog_t += log(i);
	}

	return tLog_s - tLog_t;
}

/* フィッシャーの正確確率検定（両側検定版）
 * 超幾何分布を使って検定する
 * boostのhypergeometricライブラリ必須
 * ※高速化：分割表が同一となる数字，つまり，countとaNode.supportSumが同じならP値も同じ値になるので，使いまわし可能．
 * ※オリジナルのYを使っている点に注意
 */
//double FastWY::exactTest_double(const Node aNode){
//
//	vector<uint> tX = aNode.x;
//
//	//n_11（ラベル1，パターン含む）をカウント
//	uint count = 0;
//	for(auto it : tX){
//		if(mY[it] == 1) count++;
//	}
//
//	Key tKey = pair<uint, uint>(count, aNode.supportSum);
//	if(mPvalueMap.count(tKey) == 0){
//		uint max_for_k = min(mPlusN, aNode.supportSum);
//		uint min_for_k = (uint) max(0, int(mPlusN + aNode.supportSum - mN));
//		hypergeometric_distribution<> hgd(mPlusN, aNode.supportSum, mN);
//		double cutoff = pdf(hgd, count);
//		double tmp_p = 0.0;
//
//		//今の確率（cutoff）以下のものを全て足し上げる
//		for(int k = min_for_k; k < max_for_k + 1; k++){
//			double p = pdf(hgd, k);
//			if(p <= cutoff){
//				tmp_p += p;
//			}
//		}
//		mPvalueMap[tKey] = tmp_p;
//
//	}
//	return mPvalueMap[tKey];
//}
/* フィッシャーの正確確率検定（両側検定版）
 * 超幾何分布を使って検定する
 * boostのhypergeometricライブラリ必須
 * ※高速化：分割表が同一となる数字，つまり，countとaNode.supportSumが同じならP値も同じ値になるので，使いまわし可能．
 */
//vector<double> FastWY::exactTest_double_vector(const Node aNode){
//	vector<double> tP;
//	vector<uint> tX = aNode.x;
//	for(uint i = 0; i < mR; ++i){
//
//		//n_11（ラベル1，パターン含む）をカウント
//		uint count = 0;
//		for(auto it : tX){
//			if(mTempY[i][it] == 1) count++;
//		}
//
//		Key tKey = pair<uint, uint>(count, aNode.supportSum);
//		if(mPvalueMap.count(tKey) == 0){
//			uint max_for_k = min(mPlusN, aNode.supportSum);
//			uint min_for_k = (uint) max(0, int(mPlusN + aNode.supportSum - mN));
//			hypergeometric_distribution<> hgd(mPlusN, aNode.supportSum, mN);
//			double cutoff = pdf(hgd, count);
//			double tmp_p = 0.0;
//
//			//今の確率（cutoff）以下のものを全て足し上げる
//			for(uint k = min_for_k; k < max_for_k + 1; k++){
//				double p = pdf(hgd, k);
//				if(p <= cutoff){
//					tmp_p += p;
//				}
//			}
//			mPvalueMap[tKey] = tmp_p;
//
//		}
//		tP.push_back(mPvalueMap[tKey]);
//	}
//
//	return tP;
//}
/* フィッシャーの正確確率検定
 * オプションによって両側か上側か下側かを切り替える
 * 超幾何分布を使って検定する
 * boostのhypergeometricライブラリ必須
 * ※高速化：分割表が同一となる数字，つまり，countとaNode.supportSumとmPlusNが同じならP値も同じ値になるので，使いまわし可能．
 * ※オリジナルのYを使っている点に注意
 */
double FastWY::exactTest(const Node aNode)
{

	vector<uint> tX = aNode.x;

	//n_11（ラベル1，パターン含む）をカウント
	uint count = 0;
	for (auto it : tX)
	{
		if (mY[it] == 1)
			count++;
	}

	Key tKey = pair<uint, uint>(count, aNode.supportSum);
	if (mPvalueMap.count(tKey) == 0)
	{
		uint max_for_k = (mSide != 2) ? min(mPlusN, aNode.supportSum) : count;
		uint min_for_k = (mSide != 1) ? (uint)max(0, int(mPlusN + aNode.supportSum - mN)) : count;
		hypergeometric_distribution<> hgd(mPlusN, aNode.supportSum, mN);
		double cutoff = pdf(hgd, count);
		double tmp_p = 0.0;

		//今の確率（cutoff）以下のものを全て足し上げる
		for (uint k = min_for_k; k <= max_for_k; k++)
		{
			double p = pdf(hgd, k);
			if (mSide == 0)
			{
				if (p <= cutoff)
				{
					tmp_p += p;
				}
			}
			else
			{
				tmp_p += p;
			}
		}
		mPvalueMap[tKey] = tmp_p;
	}
	return mPvalueMap[tKey];
}

/* フィッシャーの正確確率検定
 * 超幾何分布を使って検定する
 * boostのhypergeometricライブラリ必須
 * ※高速化：分割表が同一となる数字，つまり，countとaNode.supportSumが同じならP値も同じ値になるので，使いまわし可能．
 */
vector<double> FastWY::exactTest_vector(const Node aNode)
{
	vector<double> tP;
	vector<uint> tX = aNode.x;
	for (uint i = 0; i < mR; ++i)
	{
		//n_11（ラベル1，パターン含む）をカウント
		uint count = 0;
		for (auto it : tX)
		{
			if (mTempY[i][it] == 1)
				count++;
		}
		Key tKey = pair<uint, uint>(count, aNode.supportSum);

		//for G option
		//属を考慮した一定条件を満たすパターンはP値の計算をスキップ(P=1)
		if (mGenusMode == 1 && false /* skipCalcPflag(i, aNode, count, mTempY[0].size())*/) // 0107 デバッグのために_del_skipを作成(delta計算の際には目考慮しないバージョン)
		{
			tP.push_back(0.99);
		}
		//for Z option
		//アレルゲンのsup-が非ゼロであったらP値の計算をスキップ(P=1)
		else if (mZeroMode == 1 && skipCalcPflag_z(aNode, count, mTempY[0].size()))
		{
			tP.push_back(1);
		}
		else
		{
			//初めて計算するP値
			if (mPvalueMap.count(tKey) == 0)
			{
				uint max_for_k = (mSide != 2) ? min(mPlusN, aNode.supportSum) : count;
				uint min_for_k = (mSide != 1) ? (uint)max(0, int(mPlusN + aNode.supportSum - mN)) : count;
				hypergeometric_distribution<> hgd(mPlusN, aNode.supportSum, mN);
				double cutoff = pdf(hgd, count);
				double tmp_p = 0.0;

				//今の確率（cutoff）以下のものを全て足し上げる
				for (uint k = min_for_k; k <= max_for_k; k++)
				{
					double p = pdf(hgd, k);
					if (mSide == 0)
					{
						if (p <= cutoff)
						{
							tmp_p += p;
						}
					}
					else
					{
						tmp_p += p;
					}
				}
				mPvalueMap[tKey] = tmp_p;
			}
			tP.push_back(mPvalueMap[tKey]);
		}
	}
	return tP;
}

void FastWY::init(const vector<vector<Event>> aTransaction, const vector<double> aY)
{
	mTransaction = aTransaction;
	mY = aY;
	mItemSize = mTransaction[0][0].second.size();
	mN = mTransaction.size();
	if (mTransaction[0][0].first.size() > 0)
		mFlagItemsetExist = 1;

	Itemset tItemset;
	vector<uint> tWild(mItemSize, MAXVAL);
	Event tEvent(tItemset, tWild);
	mWildEvent = tEvent;

	//nはデータ数
	if (mTransaction.empty() || mY.empty())
	{
		cout << "error:Data or label is empty." << '\n';
		exit(1);
	}
	mPattern.clear();
	mTree.clear();

	mPlusN = 0;
	for (auto it : mY)
	{
		if (it == 1)
			mPlusN++;
	}

	//木の根をここで作成している
	map<Event, projectDB> counter;
	map<Event, uint> dupCheck;
	//イベント間の間隔に制限を設けない時は初期ノードの重複を許さない
	if (mInterval < 0)
	{
		//ルートにはロードしたデータのイベントが全て入っている
		for (uint i = 0; i < mN; ++i)
		{
			//イベントごとにマッピングされる
			for (uint j = 0, size = mTransaction[i].size(); j < size; ++j)
			{
				Event tEvent;
				vector<uint> tItemset;
				tEvent.second = mTransaction[i][j].second;
				uint k = 0;
				if (mTransaction[i][j].first.size() > 0)
				{
					for (; k < mTransaction[i][j].first.size(); ++k)
					{
						tItemset.push_back(mTransaction[i][j].first[k]);
						tEvent.first = tItemset;
						if (dupCheck.find(tEvent) == dupCheck.end())
						{
							dupCheck[tEvent] = 0;
							//i jはi番目の系列のj番目のイベント，アイテムセットがあるならk番目のアイテム
							Position tPosition;
							tPosition.sequence_index = i;
							tPosition.event_index = j;
							tPosition.itemset_index = k;
							counter[tEvent].push_back(tPosition);
						}
						tItemset.clear();
					}
				}
				else
				{
					tEvent.first = tItemset;
					if (dupCheck.find(tEvent) == dupCheck.end())
					{
						dupCheck[tEvent] = 0;
						Position tPosition;
						tPosition.sequence_index = i;
						tPosition.event_index = j;
						tPosition.itemset_index = k;
						counter[tEvent].push_back(tPosition);
					}
				}
			}
			dupCheck.clear();
		}
	}
	else
	{
		for (uint i = 0; i < mN; ++i)
		{
			for (uint j = 0, size = mTransaction[i].size(); j < size; ++j)
			{
				Event tEvent;
				vector<uint> tItemset;
				tEvent.second = mTransaction[i][j].second;
				uint k = 0;
				if (mTransaction[i][j].first.size() > 0)
				{
					for (; k < mTransaction[i][j].first.size(); ++k)
					{
						tItemset.push_back(mTransaction[i][j].first[k]);
						tEvent.first = tItemset;
						//i jはi番目の系列のj番目のイベント，アイテムセットがあるならk番目のアイテム
						Position tPosition;
						tPosition.sequence_index = i;
						tPosition.event_index = j;
						tPosition.itemset_index = k;
						counter[tEvent].push_back(tPosition);
						tItemset.clear();
					}
				}
				else
				{
					tEvent.first = tItemset;
					Position tPosition;
					tPosition.sequence_index = i;
					tPosition.event_index = j;
					tPosition.itemset_index = k;
					counter[tEvent].push_back(tPosition);
				}
			}
		}
	}

	mPattern.clear();

	//root用の空ノード
	Node tRoot;
	mTree.push_back(tRoot);

	//counter.begin()でイテレータの最初を取り出して，end()で終わりを取り出せる
	for (auto it = counter.begin(), end = counter.end(); it != end; ++it)
	{
		//キーつまりイベントが格納される
		mPattern.push_back(it->first);
		Node tNode;
		tNode.parent = mTree.begin();
		tNode.pattern = mPattern;
		tNode.patternStr = pat2str();
		tNode.supportSum = 0;
		tNode.pdb = it->second;
		tNode.f_val = -1;
		tNode.p = 1;
		tNode.adjusted_p = 1; // 2021.6.25 by Hanada
		tNode.closed = true;

		uint oid = MAXVAL;
		//projectDBのサイズ回ループ
		for (uint i = 0, size = tNode.pdb.size(); i < size; ++i)
		{
			//idはレコード番号
			uint id = tNode.pdb[i].sequence_index;
			if (id != oid)
			{
				// tNode.supportSum++;
				double tSup = calcSup(id, mPattern);
				tNode.supportSum += tSup;
				tNode.support.push_back(tSup);
				tNode.x.push_back(id);
			}
			oid = id;
		}

		bool tAddFlag = true;
		if (mMinsup >= 1 && mMinsup > tNode.supportSum)
			tAddFlag = false;
		else if (mMinsup < 1 && mMinsup > (double)tNode.supportSum / mN)
			tAddFlag = false;

		if (tAddFlag)
		{
			mTree.push_back(tNode);
			mTree.begin()->child.push_back(prev(mTree.end()));
		}
		mPattern.pop_back();
	}
}

uint FastWY::isSubsequence(const vector<Event> aSeq1, const vector<Event> aSeq2)
{
	vector<Event> tShort_seq;
	vector<Event> tLong_seq;
	uint tSub_seq;

	if (aSeq1.size() < aSeq2.size())
	{
		tShort_seq = aSeq1;
		tLong_seq = aSeq2;
		tSub_seq = 1;
	}
	else if (aSeq1.size() > aSeq2.size())
	{
		tShort_seq = aSeq2;
		tLong_seq = aSeq1;
		tSub_seq = 2;
	}
	else if (mFlagItemsetExist)
	{
		uint tSum1 = 0;
		uint tSum2 = 0;
		for (auto it : aSeq1)
		{
			tSum1 += it.first.size();
		}
		for (auto it : aSeq2)
		{
			tSum2 += it.first.size();
		}

		if (tSum1 > tSum2)
		{
			tShort_seq = aSeq2;
			tLong_seq = aSeq1;
			tSub_seq = 2;
		}
		else if (tSum1 < tSum2)
		{
			tShort_seq = aSeq1;
			tLong_seq = aSeq2;
			tSub_seq = 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	uint tCount = 0;
	uint diff_size = tLong_seq.size() - tShort_seq.size();

	for (uint i = 0; i <= diff_size; ++i)
	{
		for (uint it = 0; it < tShort_seq.size(); ++it)
		{
			if (isInclude(tLong_seq[it + i], tShort_seq[it]) || tShort_seq[it] == mWildEvent || tLong_seq[it + i] == mWildEvent)
			{
				tCount++;
			}
			else
			{
				tCount = 0;
				break;
			}

			if (tCount == tShort_seq.size())
			{
				return tSub_seq;
			}
		}
	}

	return 0;
}
bool FastWY::isInclude(const Event aEvent1, const Event aEvent2)
{
	if (aEvent1.first.size() < aEvent2.first.size())
	{
		return false;
	}
	else if (aEvent1.second != aEvent2.second)
	{
		return false;
	}
	else
	{
		uint i = 0;
		bool tExistFlag = false;
		for (auto itr : aEvent2.first)
		{
			tExistFlag = false;
			if (i == aEvent1.first.size())
			{
				return false;
			}
			while (itr >= aEvent1.first[i])
			{
				if (itr == aEvent1.first[i])
				{
					tExistFlag = true;
					i++;
					break;
				}
				else
				{
					i++;
				}

				if (i == aEvent1.first.size())
				{
					return false;
				}
			}

			if (!tExistFlag)
			{
				return false;
			}
		}
		return true;
	}
}

/* aNodeIterがaChildIterの親かどうか辿って調べる
 *
 */
bool FastWY::isParent(const TreeIter aNodeIter, const TreeIter aChildIter)
{
	auto current = aChildIter->parent;
	while (current->pattern.size() >= aNodeIter->pattern.size())
	{
		if (current == aNodeIter)
			return true;

		current = current->parent;
	}
	return false;
}

uint FastWY::sum_items_pdb(const projectDB aPdb)
{
	uint tSum = 0;
	for (auto itr : aPdb)
	{
		uint id = itr.sequence_index;
		uint j = itr.event_index;
		if (mTransaction[id][j].first.size() > 0)
		{
			tSum += mTransaction[id][j].first.size() - 1 - itr.itemset_index;
		}
		else
		{
			tSum++;
		}
		for (++j; j < mTransaction[id].size(); ++j)
		{
			if (mTransaction[id][j].first.size() > 0)
			{
				tSum += mTransaction[id][j].first.size();
			}
			else
			{
				tSum++;
			}
		}
	}
	return tSum;
}

uint FastWY::calcSup(uint aId, vector<Event> aPattern)
{
	switch (mSupMode)
	{
	case 1:
		return calcPat(aId, aPattern);
		break;
	default:
		return 1;
		break;
	}
}

uint FastWY::calcPat(uint aId, vector<Event> aPattern)
{
	uint patSize = aPattern.size();
	uint k;
	uint num = 0;
	uint interval;

	//想定しているパターンよりデータが短いなら0を返す
	if (mTransaction[aId].size() < patSize)
		return 0;

	for (uint i = 0; i <= mTransaction[aId].size() - patSize; i++)
	{
		interval = 0;
		k = 0;
		for (uint j = i; j < mTransaction[aId].size(); j++)
		{
			if (mInterval >= 0 && interval > mInterval)
			{
				break;
			}
			if (mTransaction[aId][j] == aPattern[k] || aPattern[k] == mWildEvent)
			{
				k++;
				interval = 0;
				if (k == patSize)
				{
					k = 0;
					num++;
					i = j;
					break;
				}
			}
			else
			{
				interval++;
			}
		}
	}
	return num;
}
/*
 * 両側検定を考慮する
 * x = Iを含むデータ数（サポート），n = ラベル1（例：疾患あり群）のデータ数とすると，
 * フィッシャーの正確確率検定におけるP値の下限は，f(x) = min(L_U(x), L_L(x))
 * x > nの場合，下限値関数にはnを入れる．なぜなら，xを入れると下限値を大きく計算してしまうから．
 * 下限値とはあくまで小さいP値をとる可能性の話なので，「サポートが大きい→小さいサポートの時により小さいP値をとる可能性がある」ということ
 * x = nの時に下限値関数としては最も小さい値になるから，x > nではこの可能性を考慮してnを代入して計算すること
 * なお，下側の下限値計算時はi=0が絶対．上記と同様にi=0の時を可能性として内包しているので．
 *
 * if(x <= n)
 * L_U(x) = log_combination(n, x) - log_combination(N, x)
 * else
 * L_U(x) = 0 - log_combination(N, n)
 *
 * if(x <= N - n)
 * L_L(x) = log_combination(N - n, x) - log_combination(N, x)
 * else
 * L_L(x) = 0 - log_combination(N, N - n)
 *
 * ※combinationの計算でlogを取っているので注意
 * tSortL[round(mR * mAlpha + 1)] < f(x(I))なら，falseを返す
 */
bool FastWY::calculate(Node &aNode)
{
	if (aNode.supportSum < mMinsup)
		return true;

	//node.val(f値)は一度計算したら使いまわせるので初期値以外だったら計算は省く
	if (aNode.f_val == -1)
	{
		double tLow_L;
		double tLow_U;

		if (mSide != 2)
		{
			if (aNode.supportSum <= mPlusN)
			{
				tLow_U = log_combination(mPlusN, aNode.supportSum) - log_combination(mN, aNode.supportSum);
			}
			else
			{
				tLow_U = 0 - log_combination(mN, mPlusN);
			}
		}

		if (mSide != 1)
		{
			if (aNode.supportSum <= (mN - mPlusN))
			{
				tLow_L = log_combination(mN - mPlusN, aNode.supportSum) - log_combination(mN, aNode.supportSum);
			}
			else
			{
				tLow_L = 0 - log_combination(mN, mN - mPlusN);
			}
		}

		switch (mSide)
		{
		case 0:
			aNode.f_val = exp(min(tLow_U, tLow_L));
			break;
		case 1:
			aNode.f_val = exp(tLow_U);
			break;
		case 2:
			aNode.f_val = exp(tLow_L);
			break;
		default:
			aNode.f_val = exp(min(tLow_U, tLow_L));
			break;
		}
	}

	vector<double> tSortL = mL;
	sort(tSortL.begin(), tSortL.end()); // 昇順ソート
	if (tSortL[round(mR * mAlpha + 1)] <= aNode.f_val)
		return false;
	else
		return true;
}

/**
 * @fn
 * project_new内でのみ使用
 * サポートを数えている
 * @return f値がmPminより小さければtrue
 * f値の計算はcalculateのコメント参照
 */
bool FastWY::calculate_new(Node &aNode)
{
	uint oid = MAXVAL;

	//見ているノードの持っているデータベースの長さ回まわす
	//pdb.sizeは同じデータ内であっても複数存在する可能性あり
	//init()でやっていることを行っているので新しいノードを展開した時に多分使用する
	for (uint i = 0, size = aNode.pdb.size(); i < size; ++i)
	{
		uint id = aNode.pdb[i].sequence_index;
		if (oid != id)
		{
			// aNode.supportSum++;
			uint tSup = calcSup(id, aNode.pattern);
			aNode.supportSum += tSup;
			aNode.support.push_back(tSup);
			aNode.x.push_back(id);
		}
		oid = id;
	}

	double tLow_L;
	double tLow_U;

	if (mSide != 2)
	{
		if (aNode.supportSum <= mPlusN)
		{
			tLow_U = log_combination(mPlusN, aNode.supportSum) - log_combination(mN, aNode.supportSum);
		}
		else
		{
			tLow_U = 0 - log_combination(mN, mPlusN);
		}
	}

	if (mSide != 1)
	{
		if (aNode.supportSum <= (mN - mPlusN))
		{
			tLow_L = log_combination(mN - mPlusN, aNode.supportSum) - log_combination(mN, aNode.supportSum);
		}
		else
		{
			tLow_L = 0 - log_combination(mN, mN - mPlusN);
		}
	}

	switch (mSide)
	{
	case 0:
		aNode.f_val = exp(min(tLow_U, tLow_L));
		break;
	case 1:
		aNode.f_val = exp(tLow_U);
		break;
	case 2:
		aNode.f_val = exp(tLow_L);
		break;
	default:
		aNode.f_val = exp(min(tLow_U, tLow_L));
		break;
	}

	if (mMinsup >= 1 && mMinsup > aNode.supportSum)
		return false;
	else if (mMinsup < 1 && mMinsup > (double)aNode.supportSum / mN)
		return false;

	vector<double> tSortL = mL;
	sort(tSortL.begin(), tSortL.end()); // 昇順ソート
	if (tSortL[round(mR * mAlpha + 1)] <= aNode.f_val)
		return false;

	return true;
}

/**
 * @fn
 * aNodeが条件を満たしているか調べ，満たしていたら子を生やす
 * 今見ているpdbの要素全ての一つ後ろの要素をcountermapに突っ込んで
 * それら全てをノードとして生やすためにproject_newする
 *
 *
 */
void FastWY::project(const TreeIter aNodeIter)
{
	//Dec2020デバッグ
	cout << aNodeIter->patternStr << endl;

	projectDB tPdb = aNodeIter->pdb;

	// scan projected database

	//ここからI-Extension
	//大前提：Itemsetは重複するアイテムは存在しない．探索は飛びを幾つでも許すもののみ
	if (mFlagItemsetExist)
	{
		map<Event, projectDB> tCounter;
		for (uint i = 0, size = tPdb.size(); i < size; ++i)
		{
			uint id = tPdb[i].sequence_index;
			uint j = tPdb[i].event_index;
			if (mTransaction[id][j].first.size() - 1 > tPdb[i].itemset_index)
			{
				uint k = tPdb[i].itemset_index + 1;
				for (; k < mTransaction[id][j].first.size(); ++k)
				{
					Event tEvent;
					vector<uint> tItemset = mPattern.back().first;
					tItemset.push_back(mTransaction[id][j].first[k]);
					tEvent.first = tItemset;
					tEvent.second = mTransaction[id][j].second;
					Position tPos;
					tPos.sequence_index = id;
					tPos.event_index = j;
					tPos.itemset_index = k;
					tCounter[tEvent].push_back(tPos);
				}
			}
		}

		// project: next event
		Event tSaveEvent = mPattern.back();
		for (auto it = tCounter.begin(), end = tCounter.end(); it != end; ++it)
		{
			mPattern.pop_back();
			mPattern.push_back(it->first);
			project_new(it->second, aNodeIter);
		}
		mPattern.pop_back();
		mPattern.push_back(tSaveEvent);
	}

	if (mPattern.size() < mMaxpat)
	{
		//ここからS-Extension
		map<Event, projectDB> tCounter;
		if (mWild == 1)
		{
			projectDB tPdbWild;
			//WildCardの処理
			//大前提としてWildCardを考慮するなら飛び無し限定
			//PDBのsecondだけインクリメントして新しいノードに渡す
			for (uint i = 0, size = tPdb.size(); i < size; ++i)
			{
				Position tPosition;
				tPosition.sequence_index = tPdb[i].sequence_index;
				tPosition.event_index = tPdb[i].event_index + 1;
				tPosition.itemset_index = 0;
				if (tPosition.event_index < mTransaction[tPosition.sequence_index].size())
				{
					tPdbWild.push_back(tPosition);
				}
			}

			mPattern.push_back(mWildEvent);
			project_new(tPdbWild, aNodeIter);
			mPattern.pop_back();
		}

		if (mInterval < 0)
		{
			map<Event, uint> dupCheck;
			for (uint i = 0, size = tPdb.size(); i < size; ++i)
			{
				uint id = tPdb[i].sequence_index;
				uint trsize = mTransaction[id].size();
				//シーケンスの一つ隣のindexを取得
				uint j = tPdb[i].event_index + 1;
				//最初に出てきたものだけを採用
				for (; j < trsize; j++)
				{
					Event tEvent;
					vector<uint> tItemset;
					tEvent.second = mTransaction[id][j].second;
					uint k = 0;

					Position tPosition;
					tPosition.sequence_index = id;
					tPosition.event_index = j;

					if (mTransaction[id][j].first.size() > 0)
					{
						for (; k < mTransaction[id][j].first.size(); ++k)
						{
							tItemset.push_back(mTransaction[id][j].first[k]);
							tEvent.first = tItemset;
							if (dupCheck.find(tEvent) == dupCheck.end())
							{
								dupCheck[tEvent] = 0;
								tPosition.itemset_index = k;
								tCounter[tEvent].push_back(tPosition);
							}
							tItemset.clear();
						}
					}
					else
					{
						tEvent.first = tItemset;
						if (dupCheck.find(tEvent) == dupCheck.end())
						{
							dupCheck[tEvent] = 0;
							tPosition.itemset_index = k;
							tCounter[tEvent].push_back(tPosition);
						}
					}
				}
				dupCheck.clear();
			}
		}
		else
		{
			vector<Position> dupCheck;
			for (uint i = 0, size = tPdb.size(); i < size; ++i)
			{
				uint id = tPdb[i].sequence_index;
				uint trsize = mTransaction[id].size();
				//シーケンスの一つ隣のindexを取得
				uint j = tPdb[i].event_index + 1;
				uint j_tmp = j;

				for (; j < trsize; j++)
				{
					//j-kは最大インターバルの計算
					if ((int)j - j_tmp > mInterval)
					{
						break;
					}

					Event tEvent;
					vector<uint> tItemset;
					tEvent.second = mTransaction[id][j].second;
					uint k = 0;

					//pdbに全く同じものが入らないようにする
					Position tPos;
					tPos.sequence_index = id;
					tPos.event_index = j;

					if (mTransaction[id][j].first.size() > 0)
					{
						for (; k < mTransaction[id][j].first.size(); ++k)
						{
							tItemset.push_back(mTransaction[id][j].first[k]);
							tEvent.first = tItemset;
							tPos.itemset_index = k;
							if (find(dupCheck.begin(), dupCheck.end(), tPos) == dupCheck.end())
							{
								dupCheck.push_back(tPos);
								tCounter[tEvent].push_back(tPos);
							}
							tItemset.clear();
						}
					}
					else
					{
						tPos.itemset_index = k;
						tEvent.first = tItemset;
						if (find(dupCheck.begin(), dupCheck.end(), tPos) == dupCheck.end())
						{
							dupCheck.push_back(tPos);
							tCounter[tEvent].push_back(tPos);
						}
					}
				}
			}
		}

		// project: next event
		for (auto it = tCounter.begin(), end = tCounter.end(); it != end; ++it)
		{
			mPattern.push_back(it->first);
			project_new(it->second, aNodeIter);
			mPattern.pop_back();
		}
	}
}

/**
 * @fn
 * 新しいノードを作成する
 * P値の計算にフィッシャーの正確確率検定を使用
 */
void FastWY::project_new(projectDB &aPdb, const TreeIter aParent)
{
	Node tNode;
	tNode.parent = aParent;
	tNode.pattern = mPattern;
	tNode.patternStr = pat2str();
	tNode.pdb = aPdb;
	tNode.supportSum = 0;
	tNode.f_val = -1;
	tNode.p = 1;
	tNode.closed = true;

	//リストの最後にNodeを挿入
	TreeIter current = mTree.insert(mTree.end(), tNode);

	//新しく作ったnodeが条件を満たすかどうかと，サポートをしらべる
	bool flag = calculate_new(*current);
	if (flag)
	{

		//ここでCloSpanチェック
		//tree全てのnodeと新しいnodeを比べて，包含関係にあってPDBが同じものがないかチェックする
		//詳細はCloSpanの論文参照
		if (mCloSpan == 1)
		{
			//親と同じサポートなら親はclosedではない
			if (current->parent->supportSum == current->supportSum)
			{
				current->parent->closed = false;
			}
			mFlagCScheckEnd = 0;

			checkProjectedDB(current);
			if (mFlagCScheckEnd == 2)
			{
				mTree.pop_back();
				return;
			}
		}
		if (current->pattern.size() >= mMinpat)
		{
			//P値の計算
			vector<double> tP = exactTest_vector(*current);

			for (uint i = 0; i < mR; ++i)
			{
				//現在の最小値より小さいなら最小値更新
				if (tP[i] < mL[i])
					mL[i] = tP[i];
			}
		}

		//親に子供のイテレータを追加
		aParent->child.push_back(current);

		if (mFlagCScheckEnd == 0)
		{
			project(current);
		}
	}
}

void FastWY::checkProjectedDB(const TreeIter aCurrentIter)
{
	uint tSumSequence = 0;
	for (auto id : aCurrentIter->x)
	{
		for (uint j = 0; j < mTransaction[id].size(); ++j)
		{
			if (mTransaction[id][j].first.size() > 0)
			{
				tSumSequence += mTransaction[id][j].first.size();
			}
			else
			{
				tSumSequence += 1;
			}
		}
	}
	//ここのキーの計算方法はCloSpanの論文とは異なるので注意（拓人オリジナル）
	uint tKey = sum_items_pdb(aCurrentIter->pdb) + tSumSequence;

	if (mCheckPDB.find(tKey) != mCheckPDB.end())
	{
		for (auto itr = mCheckPDB[tKey].begin(); itr != mCheckPDB[tKey].end();)
		{
			if (aCurrentIter->supportSum == (*itr)->supportSum)
			{
				uint tWhichSub = isSubsequence(aCurrentIter->pattern, (*itr)->pattern);
				if (tWhichSub == 1)
				{
					mFlagCScheckEnd = 2;
					return;
				}
				else if (tWhichSub == 2)
				{
					//子ノードのつけかえ
					aCurrentIter->child = (*itr)->child;
					//不飽和ノードの親が持つ子ノードリストから，この不飽和ノードを削除
					(*itr)->parent->child.erase(find((*itr)->parent->child.begin(), (*itr)->parent->child.end(), *itr));
					//木から不飽和ノードを削除
					mTree.erase(*itr);
					//
					itr = mCheckPDB[tKey].erase(itr);

					//子ノードの持つ親ノードつけかえ
					for (auto it : aCurrentIter->child)
					{
						it->parent = aCurrentIter;
					}

					//子ノード以下のパターンを全て更新
					for (auto it : aCurrentIter->child)
					{
						childPatternUpdate(it);
					}
					mFlagCScheckEnd = 1;
					return;
				}
			}

			itr++;
		}
	}

	mCheckPDB[tKey].push_back(aCurrentIter);
	return;
}
/* 子のパターンを更新
 *
 */
void FastWY::childPatternUpdate(const TreeIter aChildIter)
{
	Event tChildLast = aChildIter->pattern.back();

	Event tParentLast = mPattern.back();
	//I-Extensionかどうかのチェック
	bool tIE = false;
	if (tChildLast.first.size() > tParentLast.first.size())
	{
		mPattern.pop_back();
		tIE = true;
	}
	mPattern.push_back(tChildLast);
	aChildIter->pattern = mPattern;
	aChildIter->patternStr = pat2str();
	for (auto it : aChildIter->child)
	{
		childPatternUpdate(it);
	}
	mPattern.pop_back();
	if (tIE)
		mPattern.push_back(tParentLast);
}

//木を辿る
void FastWY::search_tree(const TreeIter aNodeIter)
{

	//Pmin < f(x(I))なら，IおよびIを根とする枝の探索を中止
	bool flag = calculate(*aNodeIter);
	if (flag)
	{

		if (aNodeIter->pattern.size() >= mMinpat)
		{
			//P値の計算
			vector<double> tP = exactTest_vector(*aNodeIter);

			for (uint i = 0; i < mR; ++i)
			{
				//現在の最小値より小さいなら最小値更新
				if (tP[i] < mL[i])
					mL[i] = tP[i];
			}
		}

		//子の探索
		project(aNodeIter);
	}
}

//補正後の有意水準決定後に木を辿る
void FastWY::search_tree_final(const TreeIter aNodeIter)
{
	//for G option
	uint GopFlag = 0;
	if (mGenusMode == 1)
	{
		GopFlag = skipSearchTreeFlag(*aNodeIter);
	}

	//for Z option
	uint ZopFlag = 0;
	if (mZeroMode == 1)
	{
		uint supPlus, sup, NPlus, N;
		sup = aNodeIter->supportSum;
		NPlus = mPlusN;
		N = mY.size();
		supPlus = 0;
		for (auto it : aNodeIter->x)
		{
			if (mY[it] == 1)
				supPlus++;
		}
		if (sup - supPlus > 0 && (double(supPlus) / double(sup) > double(NPlus) / double(N)))
			ZopFlag = 1;
	}

	if (aNodeIter->f_val <= mDelta)
	{
		//for G option: GopFlagが0の時はOK (1はskip, 2はpruning)
		//for Z option: ZopFlagが0の時はOK (1はskip, pruningはなし)
		if (GopFlag == 0 && ZopFlag == 0 && aNodeIter->closed && aNodeIter->pattern.size() >= mMinpat)
		{
			//P値の計算
			aNodeIter->p = exactTest(*aNodeIter);

			//		cout << "pattern:" << aNodeIter->patternStr << ", p:" << aNodeIter->p << endl;
			//補正後有意水準以下なら棄却
			if (aNodeIter->p <= mDelta)
			{
				// (goto)補正後P値を計算してNodeに追加
				int count = 0;
				for (auto it : mL)
				{
					if (it <= aNodeIter->p)
					{
						count++;
					}
					else
					{
						break;
					}
				}
				double tAdjustedP = double(count) / double(mR);
				aNodeIter->adjusted_p = tAdjustedP;
				//[メモ]CloSpanライクの実装を削除
				mSignificant[aNodeIter->supportSum].push_back(aNodeIter);
			}
		}
		else
		{
			// 1012 resultAllでもskipを確認できるように?(未検証)
			aNodeIter->p = 0.98;
		}
		//子の探索
		//for G option: GopFlagが1以下(2以外)→skipかnormal
		if (aNodeIter->child.size() != 0 && GopFlag <= 1)
		{
			for (auto it : aNodeIter->child)
			{
				search_tree_final(it);
			}
		}
	}
}

//for Z option
//skipCalcPflagのZoption版
//true: skip
bool FastWY::skipCalcPflag_z(const Node aNode, uint supPlus, uint N)
{
	uint sup, NPlus;
	sup = aNode.supportSum;
	NPlus = mPlusN;

	if ((sup - supPlus) > 0 && (double(supPlus) / double(sup) > double(NPlus) / double(N)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//for G option
//P値の計算をスキップする(P=1とする)為の条件判定 → A and (notB OR (B and (notC1 and notC2)))
//false: 普通に計算, true: スキップ
//A. supPlus / sup > NPlus / N (アレルゲンパターン)
//B. supMinus == 0
//C1. Food-Allergen-sup > 0
//D2. k ≧ mKind (含まれている系列の属を nonF-A1, nonF-A2, ...nonF-Akとした時のk)
bool FastWY::skipCalcPflag(uint i, const Node aNode, uint supPlus, uint N)
{
	uint sup, NPlus; //supPlusとNは二度手間なので引数
	sup = aNode.supportSum;
	NPlus = mPlusN;

	vector<uint> list_foodSup, list_nonFoodSup; //food, nonfoodのうちパターンを含む系列のidを格納
	vector<string> list_GenusOnce;

	//条件A
	if (double(supPlus) / double(sup) > double(NPlus) / double(N))
	{
		//条件B
		if (sup - supPlus > 0) //notB
		{
			return true;
		}
		else if (sup - supPlus == 0)
		{
			//条件 notC1 and notC2)
			//C1チェック
			set_intersection(aNode.x.begin(), aNode.x.end(), mGenusdata.get_tempfoodx(i).begin(), mGenusdata.get_tempfoodx(i).end(), back_inserter(list_foodSup));
			if (list_foodSup.size() == 0) //sizeが0→xのidとfoodのidで一致なし→notC1
			{
				//C2チェック
				set_intersection(aNode.x.begin(), aNode.x.end(), mGenusdata.get_tempnonfoodx(i).begin(), mGenusdata.get_tempnonfoodx(i).end(), back_inserter(list_nonFoodSup));
				vector<string> tGenus = mGenusdata.get_genus();
				for (auto it : list_nonFoodSup)
				{
					string genus = tGenus[it];
					auto finder = find(list_GenusOnce.begin(), list_GenusOnce.end(), genus);
					if (finder == list_GenusOnce.end())
					{
						list_GenusOnce.push_back(genus);
					}
					if (list_GenusOnce.size() >= mGenusdata.get_mkind())
						return false;
				}
				//k < mKind
				return true;
			}
			return false;
		}
		else
			cout << "Error: supMinus < 0" << endl; //あとからちゃんとエラー化
	}
	return false;
}

uint FastWY::skipSearchTreeFlag(const Node aNode)
{
	uint sup, supPlus, N, NPlus;
	sup = aNode.supportSum;
	supPlus = 0;
	for (auto it : aNode.x)
	{
		if (mY[it] == 1)
			supPlus++;
	}
	N = mGenusdata.get_genus().size();
	NPlus = mPlusN;

	vector<uint> list_foodSup, list_nonFoodSup; //food, nonfoodのうちパターンを含む系列のidを格納
	vector<string> list_GenusOnce;

	//条件A
	if (double(supPlus) / double(sup) > double(NPlus) / double(N))
	{
		//条件B
		if (sup - supPlus > 0) //notB
		{
			return 1; //skip
		}
		else if (sup - supPlus == 0)
		{
			//条件 notC1 and notC2)
			//C1チェック
			set_intersection(aNode.x.begin(), aNode.x.end(), mGenusdata.get_foodx().begin(), mGenusdata.get_foodx().end(), back_inserter(list_foodSup));
			if (list_foodSup.size() == 0) //sizeが0 → xのidとfoodのidで一致なし→notC1
			{
				//C2チェック
				set_intersection(aNode.x.begin(), aNode.x.end(), mGenusdata.get_nonfoodx().begin(), mGenusdata.get_nonfoodx().end(), back_inserter(list_nonFoodSup));
				vector<string> tGenus = mGenusdata.get_genus();
				for (auto it : list_nonFoodSup)
				{
					string genus = tGenus[it];
					auto finder = find(list_GenusOnce.begin(), list_GenusOnce.end(), genus);
					if (finder == list_GenusOnce.end())
					{
						list_GenusOnce.push_back(genus);
					}
					if (list_GenusOnce.size() >= mGenusdata.get_mkind())
					{
						return 0;
					}
				}
				//k < mKind
				return 2; //Pruning
			}
			return 0;
		}
		else
			cout << "Error: supMinus < 0" << endl;
	}
	// return 0; //非アレルゲンは問答無用でOKの場合
	// for G option (single-category) (of minus)
	// 非アレルゲンはfoodが含まれてればok, 含まれてなければcategory>=2でok　category<2でpruning
	set_intersection(aNode.x.begin(), aNode.x.end(), mGenusdata.get_foodx().begin(), mGenusdata.get_foodx().end(), back_inserter(list_foodSup));
	if (list_foodSup.size() == 0) //sizeが0→xのidとfoodのidで一致なし→notC1
	{
		// 0730 test
		// cout << "pat, count_category: " << aNode.patternStr << ", " << count_category(aNode) << endl;
		if (count_category(aNode) >= 2) // TODO: 2はマジックナンバー 本来はパラメータ
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	return 0; //foodを含んでいる→ok
}

int FastWY::count_category(const Node aNode)
{
	std::list<string> list_category;
	int count = 0;
	for (int i = 0; i < aNode.x.size(); i++)
	{
		if (aNode.x[i] == -1)
		{
			string category = mGenusdata.get_genus()[i];
			auto finder = find(list_category.begin(), list_category.end(), category);
			if (finder == list_category.end())
			{
				// なかったらcategoryを
				list_category.push_back(category);
			}
			count++;
			if (count >= 2) // 2はパラメータ これより大きければ即座にreturnしてしまっても無問題なはず
			{
				return count;
			}
		}
	}
	return count;
}

void FastWY::main()
{
	uint N, NPlus;
	N = mGenusdata.get_genus().size();
	NPlus = mPlusN;
	cout << "N:" << N << endl;
	cout << "NPlus" << NPlus << endl;

	TreeIter root = mTree.begin();
	vector<vector<uint>> tTempIndex;

	//for G option
	if (mGenusMode == 1)
	{
		for (uint i = 0; i < mR; i++)
		{
			vector<uint> tTempTempIndex;
			for (uint j = 0; j < mN; j++)
			{
				tTempTempIndex.push_back(j);
			}
			tTempIndex.push_back(tTempTempIndex);
		}
	}
	cout << "memo1" << endl;
	for (uint rep = 0; rep < mR; ++rep)
	{
		//yをランダムに並べ替えたデータを作る
		mTempY.push_back(mY);
		for (uint j = 0; j < mN; ++j)
		{
			uint k = j + (rand() % (mN - j));
			swap(mTempY[rep][j], mTempY[rep][k]);
			//for G option: Gオプション時はmTempIndexもyに合わせて並び替え
			if (mGenusMode == 1)
			{
				swap(tTempIndex[rep][j], tTempIndex[rep][k]);
			}
		}
		//P値の最小値を初期化
		mL.push_back(mAlpha);
	}
	cout << "memo2" << endl;

	//for G option: mTempIndexにシャッフル後インデックスをセット, mTempFoodx, mTempNonFoodxを作成
	if (mGenusMode == 1)
	{
		mGenusdata.set_tempindex(tTempIndex);
		mGenusdata.make_foodx(mR);
	}
	cout << "memo3" << endl;

	//アイテム集合の木を作成し，深さ優先探索．着目しているアイテム集合をIとする．
	//一番長い処理がここ
	for (auto it : root->child)
	{
		cout << "pattern: " << it->patternStr << endl;
		mPattern = (it->pattern);
		search_tree(it);
	}

	cout << "memo4" << endl;

	//Lの中で下から(R・α) + 1 番目の値未満の最大の値が補正後の有意水準δになる
	sort(mL.begin(), mL.end()); // 昇順ソート

	double tMax = mL[round(mR * mAlpha + 1)];

	int it = round(mR * mAlpha + 1) - 1;

	while (it >= 0)
	{
		if (mL[it] < tMax)
		{
			mDelta = mL[it];
			break;
		}
		else
			it--;
	}

	cout << "delta:" << mDelta << endl;

	//mLの出力
	it = round(mR * mAlpha + 1) - 1;
	while (it >= 0)
	{
		cout << it << ": " << mL[it] << endl;
		it--;
	}
	cout << endl;

	for (auto it : root->child)
	{
		search_tree_final(it);
	}
}