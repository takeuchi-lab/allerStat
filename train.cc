#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

#include "database.h"

#include "fastWY.h"

//for G option
#include "genusdata.h"

using namespace std;
using uint = unsigned int;
//for G option
using Genus = vector<string>;

inline void exit_with_help()
{
	cout << "-- FastWY method for sequential pattern mining --\n"
			"Usage: train [-options] input_file\n"
			"options:\n"
			"    -m : minimum supportSum (default 1)\n"
			"         If you want to decide on the percentage of the whole, please enter a value less than 1.\n"
			"    -M : minimum length of pattern (default 1)\n"
			"    -L : maximum length of pattern (default 10)\n"
			"    -F : name of reslut file (default output/result.csv)\n"
			"    -p : maximum interval of event (default 0|-1:none)\n"
			"    -C : whether to do CloSpan (default 0:do not|1:do)\n"
			"    -R : Repeat resampling in FastWY (default 10000)\n"
			"    -a : significance level (default 0.05)\n"
			"    -s : the Mode of counting supportSum(default 0)\n"
			"     	: 0 is 0 or 1 per record,1 is the number of pattern\n"
			"    -w : whether to consider WildCard (default 0:No | 1:Yes)\n"
			"    -S : double or single-side test (default 0:double | 1:upper-side | 2:lower-side)\n"
			"	 -A : (全パターン出力)\n"
			"	 -G : (属も考慮 ファイル名を指定)\n"
			"	 -K : (GオプションのNonFood-Aに関して種類数の閾値kを指定 (default 2))\n"
			"	 -Z : (アレルゲンに関して, sup-が0となるようなパターンのみを抽出 (default 0: disable | 1: enable))"
			"    -T : (カテゴリが種の場合(202009): 1\n"
			"    -r : (SEEDの値を設定(randomのr)) default : srandなし\n "
		 << endl;

	exit(1);
}

int main(int argc, char **argv)
{

	//default
	double tMinsup = 1;
	uint tMinpat = 1;
	uint tMaxpat = 10;
	uint tR = 10000;
	double tAlpha = 0.05;
	uint tWild = 0;
	uint tSide = 0;

	int tInterval = 0;
	uint tSupMode = 0;
	uint tCloSpan = 0;
	uint tGenusMode = 0;
	uint tKind = 2;
	uint tZeroMode = 0;
	uint tSpecies = 0;
	uint tRand = 0;
	string tFilename = "result.csv";
	string tAllFilename = "allResult.csv";
	string tGenusFilename = "genericName.csv";

	int tI;
	for (tI = 1; tI < argc; tI++)
	{
		if (argv[tI][0] != '-')
		{
			break;
		}
		if (++tI >= argc)
		{
			exit_with_help();
		}
		switch (argv[tI - 1][1])
		{
		case 'm':
			tMinsup = atof(argv[tI]);
			break;
		case 'M':
			tMinpat = atoi(argv[tI]);
			break;
		case 'L':
			tMaxpat = atoi(argv[tI]);
			break;
		case 'F':
			tFilename = argv[tI];
			break;
		case 'p':
			tInterval = atoi(argv[tI]);
			break;
		case 's':
			tSupMode = atoi(argv[tI]);
			break;
		case 'C':
			tCloSpan = atoi(argv[tI]);
			break;
		case 'R':
			tR = atoi(argv[tI]);
			break;
		case 'a':
			tAlpha = atof(argv[tI]);
			break;
		case 'w':
			tWild = atof(argv[tI]);
			break;
		case 'S':
			tSide = atof(argv[tI]);
			break;
		case 'A':
			tAllFilename = argv[tI];
			break;
		case 'G':
			tGenusFilename = argv[tI];
			tGenusMode = 1;
			break;
		case 'K':
			tKind = atoi(argv[tI]);
			break;
		case 'Z':
			tZeroMode = atoi(argv[tI]);
			break;
		case 'T':
			tSpecies = atoi(argv[tI]);
			break;
		case 'r':
			tRand = atoi(argv[tI]);
			srand(tRand);
			cout << "srand(" << tRand << ")" << endl;
			break;
		default:
			cerr << "Error unknown option: -" << argv[tI - 1][1] << endl;
			exit_with_help();
			break;
		}
	}

	if (tI >= argc)
	{
		cerr << "Error please input filename" << endl;
		exit_with_help();
	}

	if (tWild == 1 && tInterval != 0)
	{
		cerr << "Error: If you want to consider WildCard, please set Interval value to 0" << endl;
		exit_with_help();
	}

	//read data
	Database tDatabase;
	tDatabase.read(argv[tI]);
	vector<vector<Event>> tTransaction = tDatabase.get_transaction();
	vector<double> tY = tDatabase.get_y();
	//for G option
	Genusdata tGenusdata;
	if (tGenusMode == 1)
	{
		tGenusdata.init(tGenusFilename, tR, tKind, tSpecies, tY);
		// vector<string> tGenus = tGenusdata.get_genus();

		// for (auto itr = tGenus.begin(); itr != tGenus.end(); ++itr)
		// {
		// 	cout << *itr << endl;
		// }
	}

	chrono::system_clock::time_point start, end; // 型は auto で可
	start = chrono::system_clock::now();		 // 計測開始時間

	//make fastWY
	FastWY tFastWY(tMinsup, tMinpat, tMaxpat, tR, tAlpha, tCloSpan, tSupMode, tInterval, tWild, tSide, tGenusMode, tGenusdata, tZeroMode);

	tFastWY.init(tTransaction, tY);

	cout << "init finish" << endl;

	//FastWY実行
	tFastWY.main();
	cout << "FastWY finish" << endl;

	end = chrono::system_clock::now();												   // 計測終了時間
	double elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count(); //処理に要した時間をミリ秒に変換
	elapsed *= 0.001;

	cout << "****************************" << endl;
	cout << "time(sec):" << elapsed << endl;

	//write outcome
	tFastWY.printSigPattern(tFilename);

	tFastWY.printTree(tAllFilename);
	cout << "print finish" << endl;

	return 0;
}
