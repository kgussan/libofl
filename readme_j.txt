OFL ライブラリ
(Object File Loader library)
小口　貴弘  kgussan@gmail.com
================================================================

概要
	Wavefront Obj ファイルからジオメトリ情報を読み込みます。
	読み込んだデータをアスキー・バイナリ形式で入出力ができます。
	また、幾何情報について、
	アフィン変換、法線・従法線・接線の作成、テクスチャ座標の作成など
	変形・属性追加が可能です。
	ライブラリに加えて、Wavefront Obj ファイルをwindows 上で表示するサンプルと
	windows コマンドでさまざまな形式に変換可能なツールがついています。

環境・コンパイル方法
  基礎ライブラリ動作確認
	windows 7 / ubuntu
  サンプル
  	visual studio 環境：Microsoft Visual Studio 2008 / 2010
	GLUT / freeglut をインストール
　ツール
　　windows コマンド

サンプル・ツール
・dispObj サンプル
	$ dispObj WAVEFRONT_OBJ_FILE
	とすることでwavefront objファイルを軽量に表示することができます。
	実行時、右クリックからカレントディレクトリのモデルデータを参照できます。
	nate robinsさんのプログラム構成を踏襲しています。
・simple サンプル
　　単純に表示をするだけのサンプルです。
	さまざまなプラットフォームで試すときに便利かと思います。

・C/C++ヘッダ変換ツール
	wavefront objファイルをC/C++ヘッダに変換してつかうことができます。
	$ obj2h WAVEFRONT_OBJ_FILE
	とすることで標準出力にC/C++形式でデータを出力します。

	出力ヘッダの使用例サンプルコード

	#include <stdio.h>
	#include <stdint.h>
	namespace nsA{
		#include "../obj_a.h"
	}
	namespace nsB{
		#include "../obj_b.h"
	}
	namespace nsC{
		#include "../obj_c.h"
	}

	int main(void){
		printf("hello world\n");
		printf("%f %f %f \n", nsA::ofl_pos[0], nsA::ofl_pos[1], nsA::ofl_pos[2]);
		printf("%f %f %f \n", nsB::ofl_pos[0], nsB::ofl_pos[1], nsB::ofl_pos[2]);
		printf("%f %f %f \n", nsC::ofl_pos[0], nsC::ofl_pos[1], nsC::ofl_pos[2]);
		return 0;
	}

フィードバック
	ご意見やご提案がありましたらメールいただければ幸いです。


バージョン
	version.txt を参照してください。

更新履歴
	1.6.0	tool のobj2gb2を追加。バイナリフォーマットにtangent, binormalを追加。
			元のgbデータのフォーマットではヘッダだけが更新されている。
	1.4.0	バージョンを統一。
			バイナリフォーマットを読み書きするライブラリを追加した。
			oflReadGb
	1.1.1	バイナリフォーマットへ変換するツールを追加
			（tool/obj2gb）
			検証プログラムの追加。visual studio対応。
	1.0.0	バグを修正。プロジェクトで使用したバージョンへリファクタリングvisual studio プロジェクトに対応。
	0.5.0	バグを修正
	0.4.1	mtlがなくとも大丈夫なように変更した。
	0.4.0	windows対応
	0.3.6	アスペクト比に応じて描画領域を調整するようにした。オブジェクトがゆがまなくなった。
	0.3.5	キーボードズームに対応。( z/Z )
	0.3.4	バグ修正。メッセージ整形。
	0.3.3	ofl2h のコメント追加。
	0.3.2	ofl2h の修正。容量計算を修正。
	0.3.1	ofl2h の修正。ofl2h サンプルの法線情報のスムージング処理バグ修正
			README VERSION ファイルの廃止。このreadme とversion ファイルに統合。
	0.3.0	マテリアル単位での分解に対応。プリントバグ修正
	0.2.0	ジオメトリデータ扱いのトレーニング等。
	0.1.0	動作

ライセンス
	nate robinsさん（h26年現在ディズニー社）のローダを元にしており、
	OpenGL に依存しないライブラリと、現状の環境でも動作するようにしたサンプルです。

	私が追記した部分について、制限のゆるいMITライセンスを適用します。
	平成26年6月における、wikipediaによる日本語要約のとおりです。
	１．このソフトウェアを誰でも無償で無制限に扱って良い。
	　　ただし、著作権表示および本許諾表示をソフトウェアのすべての複製または重要な部分に記載しなければならない。
	２．作者または著作権者は、ソフトウェアに関してなんら責任を負わない。

	元の部分にはnate さんの指定するライセンスを記載することで許諾を得ています。

	詳細はライセンスをよく読んでいただければと思いますが、
	私の解釈では、基本的にこの条文はライセンス文言を記載すればよいゆるいものです。
	商用・非商用問わず、何かに使うときには以下のライセンスをreadme 等に
	貼り付けておけばOKと思います。
	気に入ってもらえたらどんどん使ってください。


	----------------------------------------------------------------

	The MIT License (MIT)

	Copyright (c) 2014 KOGUCHI Takahiro

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

	A part of this software is based on glm library by Nate Robins under 
	the following license.

	----------------------------------------------------------------

	LICENCE

	Copyright 2010 Nate Robins. All rights reserved.
	Redistribution and use in source and binary forms, with or without modification, 
	are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice, 
	this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, 
	this list of conditions and the following disclaimer in the documentation and/or 
	other materials provided with the distribution.
	THIS SOFTWARE IS PROVIDED BY NATE ROBINS ``AS IS'' AND ANY EXPRESS OR IMPLIED 
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NATE ROBINS 
	OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	The views and conclusions contained in the software and documentation are those 
	of the authors and should not be interpreted as representing official policies, 
	either expressed or implied, of Nate Robins.
