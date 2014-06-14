obj2gb3
ジオメトリデータ変換ツール
小口貴弘
================================================================

更新の必要あり


環境：cygwin / linux (ubuntu)

入力：obj
	（wave front obj / ウェーブフロントオブジェクトファイル）
	objジオメトリファイルを入力し、整形、バイナリデータの出力をする。

出力：gb
	(geometry binary / バイナリファイル)
	
	データ構成は以下のようになっている。

	[header]
	version
	data size
	data offset
	...

	[body]
	pos			(float xyz)
	normal		(float xyz)
	tangent		(float xyz)
	binormal	(float xyz)
	texcoord	(float uv)
	...

	ヘッダ内容は以下のようになっている。

struct oflBinHeader_t{
	char signature[32];			// 32Byte
	uint32_t version;			//  4Byte
	uint32_t numTriangles;		//  4Byte
	uint32_t numVerticies;		//  4Byte
	uint32_t elemNumPos;		//  4Byte,48
	uint32_t elemNumNorm;		//  4Byte
	uint32_t elemNumTangent;	//  4Byte
	uint32_t elemNumBinormal;	//  4Byte
	uint32_t elemNumTex;		//  4Byte,64
	uint32_t sizePos;			//  4Byte
	uint32_t sizeNorm;			//  4Byte
	uint32_t sizeTangent;		//  4Byte
	uint32_t sizeBinormal;		//  4Byte,96
	uint32_t sizeTex;			//  4Byte
	uint32_t offsetToData;		//  4Byte
	uint32_t offsetToPos;		//  4Byte
	uint32_t offsetToNorm;		//  4Byte,128
	uint32_t offsetToTangent;	//  4Byte
	uint32_t offsetToBinormal;	//  4Byte
	uint32_t offsetToTex;		//  4Byte
	//TOTAL : 112Byte
	char padding[144];	//合計でヘッダが256Byteになるようにした。もうちょっとあっても良かったかもしれない。
};


使い方
	使用するプログラムでヘッダ部分のfreadを行った後、
	ヘッダ部分の情報に従い、データ部のfreadを行う。

テスト・検証
	obj2h出力と
	obj2gbをして作成したgbファイルを読んだものの出力を比較して、
	cube.bojのファイル先頭のポジションデータと
	末尾のテクスチャ座標データの一致を確認。

	verificationディレクトリのobj2gbVerificationのプロジェクトを
	コンパイル、ビルドし、引数にたとえば../data/tree_lands.gb
	を入れると内容を取得できていることがわかる。

注意
	マテリアルの情報は出していない。
	numPos, numNorm, numTex は要素数を示す。(3:xyz, 2: uv)

履歴
	2013/03/28 17:45:16	tangent / binormal追加
	2013/02/05 gb出力するグループデータに使う文字列を数字に変更。同時にインデックスと文字列の対応を示すgroup.txtも出力するようにした。
	2013/01/23 12:17:00	gbでグループデータも出力
