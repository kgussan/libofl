obj2h 
ジオメトリデータ変換ツール
小口貴弘　kgussan@gmail.com
================================================================

環境：cygwin / linux (ubuntu)

入力：obj 
	wave front obj のジオメトリファイルを入力し、整形する。

出力：Cヘッダファイル
	内容は以下のようになっている。
		コメント：// データサイズ総量
		char *ofl_pathname : 元データのファイル名称
		static const uint32_t ofl_numtriangle : トライアングル数
		float ofl_pos : 位置データ
		float ofl_norm : 法線
		float ofl_tex : テクスチャ
	以上。

使い方
	使用するプログラムで #include するだけでよい。
	その際にnamespace を設定すると複数ファイルを使っても
	変数がぶつかることはない。
	namespace model_a {
		#include "output_file_a.h"
	};
	namespace model_b {
		#include "output_file_b.h"
	};

