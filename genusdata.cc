#include "genusdata.h"
#include <stdexcept>
#include <sstream>

void Genusdata::init(const string aFilename, uint aR, uint aKind, uint aSpecies, vector<double> aY)
{
    // category実装前
    ifstream tFile(aFilename.c_str());
    if (!tFile)
    {
        cerr << "Error: cannot open genus file" << endl;
        exit(1);
    }

    string tLine;
    while (getline(tFile, tLine))
    {
        mGenus.push_back(tLine);
    }
    mSpecies = aSpecies;
    vector<string> allergen_list, nonallergen_list;

    if(mGenus.size() != aY.size()){
        std::stringstream errmsg;
        errmsg << "Number of instances in the data file is " << aY.size() << ", but the genus file \"" << aFilename << "\" contains " << mGenus.size() << " instances";
        std::cerr << errmsg.str() << std::endl;
        throw new std::runtime_error(errmsg.str());
    }

    if (mSpecies)
    {
        cout << "mSpecies: " << mSpecies << endl;
        // 202009 categoryがspeciesの場合
        // 一回全部読んでallergen, nonallergenの全カテゴリを記録, mFoodxを作成
        for (int i = 0; i < mGenus.size(); i++)
        {
            string genus = mGenus[i];
            double index = aY[i];
            if (index == 1)
            {
                auto it_find = find(allergen_list.begin(), allergen_list.end(), genus);

                if (it_find == allergen_list.end())
                {
                    allergen_list.push_back(genus);
                }
            }
            if (index == -1)
            {
                auto it_find = find(nonallergen_list.begin(), nonallergen_list.end(), genus);

                if (it_find == nonallergen_list.end())
                {
                    nonallergen_list.push_back(genus);
                }
            }
        }
        for (int i = 0; i < mGenus.size(); i++)
        {
            string genus = mGenus[i];
            auto it_al_find = find(allergen_list.begin(), allergen_list.end(), genus);
            auto it_nonal_find = find(nonallergen_list.begin(), nonallergen_list.end(), genus);

            if (it_al_find != allergen_list.end() && it_nonal_find != nonallergen_list.end())
            {
                mFoodx.push_back(i);
                mPairedList.push_back(genus);
            }
            else
            {
                mNonFoodx.push_back(i);
            }
        }
    }
    else
    {
        //mFoodx
        uint i = 0;
        for (auto it : mGenus)
        {
            if (it == "Food")
                mFoodx.push_back(i);
            i++;
        }
        //mNonFoodx
        i = 0;
        for (auto it : mGenus)
        {
            if (it != "Food")
                mNonFoodx.push_back(i);
            i++;
        }
    }

    //debug: allergen_list, の中身を出力
    cout << "allergen_list:" << endl;
    for (auto it : allergen_list)
    {
        cout << it << ", ";
    }
    cout << endl;
    cout << "nonallergen_list:" << endl;
    for (auto it : nonallergen_list)
    {
        cout << it << ", ";
    }
    cout << endl;
    cout << "mPairedList:" << endl;
    for (auto it : mPairedList)
    {
        cout << it << ", ";
    }
    cout << endl;
    //debug: mFoodxの中身を出力
    cout << "mFoodx:" << endl;
    for (auto it : mFoodx)
    {
        cout << it << ", ";
    }
    cout << endl
         << "mNonFoodx:" << endl;
    for (auto it : mNonFoodx)
    {
        cout << it << ", ";
    }
    cout << endl;

    //mKind
    mKind = aKind;

    //mTempIndexの初期化
    for (uint i = 0; i < aR; i++)
    {
        vector<uint> tTempIndex;
        for (uint j = 0; j < mGenus.size(); j++)
        {
            tTempIndex.push_back(j);
        }
        mTempIndex.push_back(tTempIndex);
    }
}

void Genusdata::set_tempindex(const vector<vector<uint>> aTempIndex)
{
    mTempIndex = aTempIndex;
}

void Genusdata::make_foodx(uint aR)
{
    for (uint i = 0; i < aR; i++)
    {
        vector<uint> tTempFoodx;
        vector<uint> tTempNonFoodx;

        for (uint j = 0; j < mGenus.size(); j++)
        {
            //mTempIndex[i]にi回目シャッフルに関してシャッフル後のインデックスが入っているので, これをjで上からなぞればmGenus[mTempIndex[i][j]]がシャッフル後j番目の属
            //TempIndex廃止して大人しくmTempGenusをvector<vector<string>>で作れば簡潔にはなる
            if (mSpecies == 1)
            {
                // 202009 categoryがspeciesの場合
                string genus = mGenus[mTempIndex[i][j]];
                auto it_find = find(mPairedList.begin(), mPairedList.end(), genus);
                if (it_find != mPairedList.end())
                {
                    tTempFoodx.push_back(j);
                }
                else
                {
                    tTempNonFoodx.push_back(j);
                }
            }
            else
            {
                // 元の実装
                // if (i == 0)
                //     cout << "mGenus[mTempIndex[" << i << "][" << j << "]]" << mGenus[mTempIndex[i][j]] << endl;
                if (mGenus[mTempIndex[i][j]] == "Food")
                {
                    tTempFoodx.push_back(j);
                }
                else
                {
                    tTempNonFoodx.push_back(j);
                }
            }
        }
        mTempFoodx.push_back(tTempFoodx);
        mTempNonFoodx.push_back(tTempNonFoodx);
    }
    // デバッグ mTempFoodx, mTempNonFoodxを表示
    // for (int i = 0; i < aR; i++)
    // {
    //     cout << "mTempFoodx[" << i << "]: " << mTempFoodx[i].size() << endl;
    //     for (int j = 0; j < mTempFoodx[i].size(); j++)
    //     {
    //         cout << mTempFoodx[i][j] << ", ";
    //     }
    //     cout << endl;
    //     cout << "mTempNonFoodx[" << i << "]: " << mTempNonFoodx[i].size() << endl;
    //     for (int j = 0; j < mTempNonFoodx[i].size(); j++)
    //     {
    //         cout << mTempNonFoodx[i][j] << ", ";
    //     }
    //     cout << endl;
    // }
}

//ゲッター群
vector<string> Genusdata::get_genus() const
{
    return mGenus;
}

vector<uint> Genusdata::get_foodx() const
{
    return mFoodx;
}

vector<uint> Genusdata::get_nonfoodx() const
{
    return mNonFoodx;
}

vector<vector<uint>> Genusdata::get_tempindex() const
{
    return mTempIndex;
}
vector<vector<uint>> Genusdata::get_tempfoodx() const
{
    return mTempFoodx;
}
vector<uint> Genusdata::get_tempfoodx(uint i) const
{
    return mTempFoodx[i];
}
vector<vector<uint>> Genusdata::get_tempnonfoodx() const
{
    return mTempNonFoodx;
}
vector<uint> Genusdata::get_tempnonfoodx(uint i) const
{
    return mTempNonFoodx[i];
}

uint Genusdata::get_mkind() const
{
    return mKind;
}