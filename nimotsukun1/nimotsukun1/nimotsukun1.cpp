/* 
	2019/08/19
	「倉庫番」のようなゲーム
*/

#include "pch.h"
#include <cstdio>
#include <fstream>
#include <iostream>

/* 
	MAP上に配置するもの
	改行などのMAP表示の調整に使うもの
*/
//床
static const char MAP_FLOOR = ' ';

//壁
static const char MAP_WALL = '#';

//荷物
static const char MAP_BOX = 'o';

//ゴールの位置まできた荷物
static const char MAP_AFTER_GOAL_BOX = 'O';

//ゴール位置
static const char MAP_GOAL = '.';

//プレイヤー
static const char MAP_PLAYER = 'p';

//プレイヤーがゴールの上に乗った時の大文字
static const char MAP_PLAYER_ON_GOAL = 'P';

//\マーク
static const char MAP_FIRST_CODE = '\\';

//改行コード
static const char MAP_NEW_LINE_CODE = '\n';

/*
	二次元配列の中の整数を定数にしておく
*/
//床
static const int NUMBER_FLOOR = 0;

//壁
static const int NUMBER_WALL = 1;

//荷物
static const int NUMBER_BOX = 2;

//ゴール位置
static const int NUMBER_GOAL = 4;

//プレイヤー
static const int NUMBER_PLAYER = 5;

//ゴール済み荷物
static const int NUMBER_AFTER_GOAL_BOX = 6;

//プレイヤー大文字
static const int NUMBER_PLAYER_ON_GOAL = 7;

//改行コード
static const int NUMBER_NEW_LINE_CODE = 10;

/*
	入力するキーボードのキー
	w: up
	z: down
	a: left
	s: right
*/
static const char KEY_UPMOVE = 'w';
static const char KEY_DOWNMOVE = 'z';
static const char KEY_LEFTMOVE = 'a';
static const char KEY_RIGHTMOVE = 's';

/*
	マップを出力
	（仕様）
	# 壁
	. 荷物を運ぶ目標地点
	o 荷物（目標地点まで行くと O になる。）
	p プレイヤー（目標地点まで行くと P になる。）
*/
const char stageMap[] = "\
#######\n\
# ..p #\n\
# oo  #\n\
#     #\n\
#######";

const int stageWidth = 8;
const int stageHeight = 5;

// マップのコピー（出力用）
char stageMapCopy[ stageWidth*stageHeight+1 ];

/*
	マップデータ
	0：なにもなし
	1：壁
	2：荷物
	3：スタート
	4：ゴール
	5：プレイヤー
	6：ゴール済み荷物
	7：大文字プレイヤー
	10:改行
*/
int stageMapData[stageHeight][stageWidth] = {
	{1,1,1,1,1,1,1,10,},
	{1,0,4,4,3,0,1,10,},
	{1,0,2,2,0,0,1,10,},
	{1,0,0,0,0,0,1,10,},
	{1,1,1,1,1,1,1},
};

//プレイヤーの操作方法を出力
const char controlMenutext[] = "w:up, z:down, a:left, s:right";

//プレイヤーにクリアしたことを出力
const char clearMenutext[] = "おめでとう！荷物を無事に運べました♪";

//クラス
class GameClass
{
private:
	//非公開関数と変数
	int mapClearNum = 0;		// クリアに必要な荷物の数をカウント用
	int mapClear = 0;			// クリア状態（0：プレイ中、1：クリア）
	bool flgGoled = false;		// ゴールの上にいたか状態（false：いない、true:いた）

	//移動する方向X(左右）
	int xPosition = 4;			// キーボード入力前の座標
	int xNextPosition = 4;		// キーボード入力後の座標の一つ先

	//移動する方向y（上下）
	int yPosition = 1;			// キーボード入力前の座標
	int yNextPosition = 1;		// キーボード入力後の座標の一つ先

public:
	//公開関数と変数
	GameClass();									//コンストラクタ
	~GameClass();									//デストラクタ
	char getInput();								//ユーザのキーボード入力を受け取る
	void updateGame(char playerInputKey);			//入力した値で、荷物が動く、プレイヤーが動く、クリア判定
	void draw();									//結果を描画する
	int clearInfo();								//ゲームのクリア条件
	void upperLowerChk(int keyChk);					//大文字、小文字に変換する
	void wallBaggagesChk(int numberFloorObject, char playerInputKey);	//荷物の進行先に、壁や荷物がないかチェック
};

struct moveDistance
{
	//getInputで使用
	int xkey = 0;				// x方向の移動量
	int ykey = 0;				// y方向の移動量
	int yWallChkPosition = 1;	// キーボード入力後の座標
	int xWallChkPosition = 4;	// キーボード入力後の座標
	bool drawFlg = true;		//入力値が操作に必要なものか判定する

}distance;

// 初期化（コンストラクタ）
GameClass::GameClass()
{
	// クリアに必要な荷物数を初期化
	mapClearNum = 0;

	// stageMapの要素数
	int arraySize = sizeof stageMap;
	
	for (int i = 0; i < arraySize; i++)
	{
		stageMapCopy[i] = stageMap[i];

		// 荷物の数を数えてクリア条件判定に使う
		if (stageMap[i] == MAP_BOX)
		{
			++mapClearNum;
		}

	}

	// クリア状態初期化
	mapClear = 0;

}

// デストラクタ
GameClass::~GameClass()
{
	//終了

}

/*
	【移動】
	・wzasキーを押したら移動する (w: up, z: down, a: left, s: right)
	・XとYなどの場所を渡す
	・移動量を渡す
*/
char GameClass::getInput()
{
	char playerInputKey;

	// 入力待ち
	scanf_s("%c", &playerInputKey, 1);

	// 入力した値を受け取って、どっちに進むつもりか判定
	switch (playerInputKey)
	{
		case KEY_UPMOVE : 

			--distance.yWallChkPosition;
			distance.xkey = 0;
			distance.ykey = -1;
			distance.drawFlg = true;
			break;
		case KEY_DOWNMOVE : 

			++distance.yWallChkPosition;
			distance.xkey = 0;
			distance.ykey = 1;
			distance.drawFlg = true;
			break;
		case KEY_LEFTMOVE : 

			--distance.xWallChkPosition;
			distance.xkey = -1;
			distance.ykey = 0;
			distance.drawFlg = true;
			break;
		case KEY_RIGHTMOVE : 

			++distance.xWallChkPosition;
			distance.xkey = 1;
			distance.ykey = 0;
						distance.drawFlg = true;
			break;
		case MAP_NEW_LINE_CODE :

			//コマンド入力後にEnterを押したかどちらか(Enterを誤って押した場合も含む）
			distance.drawFlg = false;
			break;
		default:

			//updateしないようにする
			distance.drawFlg = false;
			break;
	}
	return playerInputKey;
}

/*
	【移動判定】

	床（なにもない）
		→プレイヤーのみなので進行できる
	壁
		→進行できないので、xpとypを元の値に戻す
	荷物
		→荷物とプレイヤーが進行できるかチェックする処理へ
	ゴール
		→プレイヤーを大文字の「P」にする
	ゴール済み荷物
		→なにもしない
*/
void GameClass::updateGame(char playerInputKey)
{
	if (distance.drawFlg)
	{
		switch (stageMapData[distance.yWallChkPosition][distance.xWallChkPosition])
		{
		case NUMBER_FLOOR:

			// 荷物の先に壁や荷物がないかチェック
			wallBaggagesChk(NUMBER_FLOOR, playerInputKey);

			break;
		case NUMBER_WALL:

			// 元の座標を代入してプレイヤーの位置を戻す
			distance.xWallChkPosition = xPosition;
			distance.yWallChkPosition = yPosition;

			break;
		case NUMBER_BOX:

			// 荷物の先に壁や荷物がないかチェック
			wallBaggagesChk(NUMBER_BOX, playerInputKey);

			break;
		case NUMBER_GOAL:

			// 荷物の先に壁や荷物がないかチェック
			wallBaggagesChk(NUMBER_GOAL, playerInputKey);

			break;
		case NUMBER_AFTER_GOAL_BOX:

			// ゴール後は荷物は動かせないのでなにもしない
			break;
		default:

			// あてはまらない場合はなにもしない
			break;
		}
	}
}

/* 
	描画（マップ、文言）

*/
void GameClass::draw()
{
	if (distance.drawFlg)
	{
		// ２次元配列の要素数
		int arrayHeight = sizeof(stageMapData) / sizeof(stageMapData[0]);	   // 行数
		int arrayWidth = sizeof(stageMapData[0]) / sizeof(stageMapData[0][0]); // 列数

		// 配列の中でマップに表示するマークに変換
		for (int i = 0; i < arrayHeight; i++)
		{

			for (int j = 0; j < arrayWidth; j++)
			{

				if (stageMapData[i][j] == NUMBER_FLOOR)
				{
					// 床
					stageMapCopy[(arrayWidth * i + j)] = MAP_FLOOR;
				}
				else if (stageMapData[i][j] == NUMBER_WALL)
				{
					// 壁
					stageMapCopy[(arrayWidth * i + j)] = MAP_WALL;
				}
				else if (stageMapData[i][j] == NUMBER_BOX)
				{
					// 荷物
					stageMapCopy[(arrayWidth * i + j)] = MAP_BOX;
				}
				else if (stageMapData[i][j] == NUMBER_GOAL)
				{
					// ゴール位置
					stageMapCopy[(arrayWidth * i + j)] = MAP_GOAL;
				}
				else if (stageMapData[i][j] == NUMBER_PLAYER)
				{
					// プレイヤー
					stageMapCopy[(arrayWidth * i + j)] = MAP_PLAYER;
				}
				else if (stageMapData[i][j] == NUMBER_AFTER_GOAL_BOX)
				{
					// ゴール後の荷物
					stageMapCopy[(arrayWidth * i + j)] = MAP_AFTER_GOAL_BOX;
				}
				else if (stageMapData[i][j] == NUMBER_PLAYER_ON_GOAL)
				{
					// ゴール後のプレイヤー
					stageMapCopy[(arrayWidth * i + j)] = MAP_PLAYER_ON_GOAL;
				}
				else if (stageMapData[i][j] == NUMBER_NEW_LINE_CODE)
				{
					//改行
					stageMapCopy[(arrayWidth * i + j)] = MAP_NEW_LINE_CODE;
				}

			}
		}

		printf(stageMapCopy);

		// 生成したマップ出力時に改行
		printf("%c", MAP_NEW_LINE_CODE);

		// 操作方法、クリア後の文言
		if (mapClear == 0)
		{
			printf("%s", controlMenutext);
			printf("%c", MAP_NEW_LINE_CODE);
		}
		else if (mapClear == 1)
		{
			printf("%s", clearMenutext);
			printf("%c", MAP_NEW_LINE_CODE);
		}
	
	}

}

// クリア判定
int GameClass::clearInfo()
{
	// ゴールした荷物を数えるカウンタ
	int cnt = 0;
	// ２次元配列表示
	int c_height = sizeof(stageMapData) / sizeof(stageMapData[0]);		// 行数
	int c_width = sizeof(stageMapData[0]) / sizeof(stageMapData[0][0]);	// 列数

	for (int i = 0; i < c_height; i++)
	{
		for (int j = 0; j < c_width; j++)
		{
			if (stageMapData[i][j] == NUMBER_AFTER_GOAL_BOX)
			{
				cnt++;
			}
		}
	}

	// NUMBER_AFTER_GOAL_BOXがクリア条件個数分あればクリア
	if (cnt == mapClearNum)
	{
		mapClear = 1;
	}

	return mapClear;

}

/*
	大文字、小文字チェック(関数使う）
*/
void GameClass::upperLowerChk(int keyChk)
{
	if (keyChk == NUMBER_FLOOR)
	{
		// プレイヤーがいた床をゴールに戻す
		stageMapData[yPosition][xPosition] = NUMBER_GOAL;
		// ゴールからプレイヤーが離れたら小文字にする
		stageMapData[distance.yWallChkPosition][distance.xWallChkPosition] = NUMBER_PLAYER;

	}
	else if (keyChk == NUMBER_GOAL)
	{
		if(!flgGoled)
		{
			// プレイヤーがいた場所を床にする
			stageMapData[yPosition][xPosition] = NUMBER_FLOOR;
		}
		else
		{
			// プレイヤーがいた場所をゴールにする
			stageMapData[yPosition][xPosition] = NUMBER_GOAL;
		}

		// ゴールにプレイヤーがきたら大文字にする
		stageMapData[distance.yWallChkPosition][distance.xWallChkPosition] = NUMBER_PLAYER_ON_GOAL;

	}
}

// 壁と荷物が進行先にないかチェック
void GameClass::wallBaggagesChk(int numberFloorObject, char playerInputKey)
{
	// 普通の進行できる場合
	if (numberFloorObject == NUMBER_FLOOR || numberFloorObject == NUMBER_GOAL)
	{
		if (numberFloorObject == NUMBER_GOAL)
		{
			// ゴールがあった場合

			// 座標を代入
			xNextPosition = distance.xWallChkPosition;
			yNextPosition = distance.yWallChkPosition;

			// 大文字、小文字判定
			upperLowerChk(NUMBER_GOAL);
			
			// プレイヤー現在位置を更新
			xPosition += distance.xkey;
			yPosition += distance.ykey;

			flgGoled = true;

		}
		else
		{
			if (flgGoled)
			{
				// ゴールの上にプレイヤーがいた
				upperLowerChk(NUMBER_FLOOR);
				flgGoled = false;
			}
			else
			{
				// 移動前のプレイヤー削除
				stageMapData[yPosition][xPosition] = NUMBER_FLOOR;
			}

			// プレイヤー現在位置を更新
			xPosition += distance.xkey;
			yPosition += distance.ykey;

			// 座標を代入
			xNextPosition = distance.xWallChkPosition;
			yNextPosition = distance.yWallChkPosition;

			// プレイヤーの位置を更新
			stageMapData[distance.yWallChkPosition][distance.xWallChkPosition] = NUMBER_PLAYER;
		}

	}
	else if (numberFloorObject == NUMBER_BOX)
	{

		// 荷物があった場合
		// チェック前の座標を代入
		int xNextPosition = distance.xWallChkPosition;
		int	yNextPosition = distance.yWallChkPosition;

		switch (playerInputKey)
		{
			case KEY_UPMOVE:
				--yNextPosition;
				break;
			case KEY_DOWNMOVE:
				++yNextPosition;
				break;
			case KEY_LEFTMOVE:
				--xNextPosition;
				break;
			case KEY_RIGHTMOVE:
				++xNextPosition;
				break;
			default:
				// あてはまらない場合はなにもしない
				break;

		}

		// もう一つ先の座標に荷物か壁がないかチェック
		if (stageMapData[yNextPosition][xNextPosition] == NUMBER_FLOOR ||
			stageMapData[yNextPosition][xNextPosition] == NUMBER_GOAL)
		{
			//進行ができるので荷物とプレイヤー座標を変える

			//プレイヤーは一つ前の座標
			stageMapData[distance.yWallChkPosition][distance.xWallChkPosition] = NUMBER_PLAYER;

			if (stageMapData[yNextPosition][xNextPosition] == NUMBER_GOAL)
			{
				//荷物がゴールの上にきた
				stageMapData[yNextPosition][xNextPosition] = NUMBER_AFTER_GOAL_BOX;
			}
			else
			{
				//荷物が今の座標に移動
				stageMapData[yNextPosition][xNextPosition] = NUMBER_BOX;
			}

			//元のプレイヤーは消す
			stageMapData[yPosition][xPosition] = NUMBER_FLOOR;

			if (flgGoled)
			{
				// ゴールの上にプレイヤーがいた
				upperLowerChk(NUMBER_FLOOR);
				flgGoled = false;
			}

			// プレイヤー現在位置を更新
			xPosition += distance.xkey;
			yPosition += distance.ykey;

		}
		else
		{
			// 進行ができない場合の後処理（元の座標を代入）
			distance.xWallChkPosition = xPosition;
			distance.yWallChkPosition = yPosition;

		}

	}

}

int main()
{
	/*
	ステージマップの読み込みを試してコメントアウト

	std::ifstream inputFile("stageData.txt", std::ifstream::binary);
	inputFile.seekg(0, std::ifstream::end);
	int fileSize = static_cast<int>(inputFile.tellg());
	inputFile.seekg(0, std::ifstream::beg);
	char* fileImage = new char[fileSize];
	inputFile.read(fileImage, fileSize);
	std::cout.write(fileImage, fileSize);
	*/

	GameClass ob1;
	char getInputKey;

	// メインループ
	while (true)
	{
		// クリアしているかチェック
		ob1.clearInfo();

		// マップ描画
		ob1.draw();

		// キーボードの入力
		getInputKey = ob1.getInput();

		// 入力された値でマップの更新
		ob1.updateGame(getInputKey);

	}

	return 0;

}