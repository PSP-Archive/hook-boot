HOOK BOOT Ver.0.94

APIをフックしてモニタする、パッチも当てる。
カーネルモードで動く、とても危険なプログラムなので、
自己責任で使うこと

-- HOOK BOOTってなに？

BOOT.BINにパッチをあてて、APIをフックする。

１．APIを画面やファイルでモニタできる。

２．UMDエミュレーションでMSから起動できる。
　　バッテリのセーブによる長時間プレイと、高速アクセスの為に！

3.インポートライブラリの情報がみれる。－＞解析

4.実行時、カーネルとユーザメモリにパッチする。

-- 履歴

V0.94
   see readme.txt

V0.93
  -sceIoDreadでパスを置き換えなかった問題修正
  -キャッシュクリア制御追加
   (SYSCALL HOOKとファイルログでフリーズの回避）
  -UMDコールバックのフックを変更、sceCreateCallbackのフックは不要
  -sceIoWriteのハンドル1の書き込みをログ出力(SYSTEM LOG)
  -フックAPIsceIoSync and sceIoChstatの追加
  -sceIoDevctlにパス置き換え追加

V0.92B
  -サイズ縮小
  -CODED ARMSフィクス

V0.92A
  -KERNEL_VER parameter未設定時、カーネルバージョンパッチをバイパス.
  -フック関数バグとり　sceIoLseek,sceIoLseekAsync,sceIoGetAsyncStat

V0.92
  -インポートライブラリのダイナミックバージョンフックをサポート
　　平たくいえば1.00で1.50ソフトが起動する。
　　'CODEDARMS/boot.ini'の"IMPORT_VER"を見なさい。
  -"WAIT_RUN=[wait_mode]"パラメータの追加
  -sceIoLseek と sceIoLseekAsync のフック復帰
  -sceKernelLoadExec のフック追加（でも、はずれ）
V0.91
 readme.txtの"HISTRY"を参照

V0.90
 readme.txtの"HISTRY"を参照


V0.10 1st release

-- 技術

1.MODULE patch mode
　1) ロード前
　　ターゲットのNIDを潰して、エントリーポイントに埋め込む
　2)ロード後
  　NIDからエントリーポイントを探してフックする。

2.SYSTEM CALL hook mode
  SYSTEMCALLのジャンプベクターテーブルを変更してフックします。

-- 制限

ＵＭＤエミュレーションは開発中につき、不完全

1.MODULE patch mode
  暗号化されていないELFのみサポート
　シングルモジュールのみ対応、サブモジュールはフック不可

2.SYSTEM CALL hook mode
  事前パッチなしの直接起動可
　暗号化~PSP fileサポート (EBOOT.BIN).
　いくつか問題あり、（sceCreateCallback不動）

-- 使い方

PuzzleBobbleの場合、

1.MS_ROOT_DIR内のファイルをMSのルートにコピー
2.UMDファイルを'/PSP/GAME/PBOBBLE/UMD0/'にコピー
3.'/PSP/HOOKBOOT/EBOOT.PBP'を'/PSP/GAME/PBOBBLE/'にコピー
  (Formware 1.50はPBP toolを使う)
4.'/PSP/GAME/PBOBBLE/EBOOT.PBP'を実行

補足：
　UMDファイルコピー後、PCにMSマウントしてPSP/HOOKBOOTから
"makepbp [gameディレクトリ]"を実行すると、ゲームのアイコン、
背景、サウンドを組み込んだEBOOT.PBPが作れる。
　(1.50の方は、もう一手間必要)

-- UMDエミュレーション

1.MSからKernelModuleのロードを可能にします。
2.UMDドライブの状態をエミュレーションします。

-- 将来の計画

1.FTPクライントによる、UMDエミュレーション

2.完全な、バージョンアップカーネルパッチ
　これは、とりあえず、完了

