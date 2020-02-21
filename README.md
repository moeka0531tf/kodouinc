# プログラム概要

## グローバル変数
【 kououinc.ino 】
- IBI : ビート間の時間を保存するための変数
- Paulse: boolean値, paluseの値が高いとtrueになるフラグ的な役割
- QS: BPMが計算されたらtrueにする変数、フラグ

【 interrupt.ino 】
- thresh : 心拍の瞬間を見つけるための変数、閾値
- T: 脈拍の谷の部分を見つけるための閾値
- P: 脈拍のピークの部分を保持する閾値
- sampleCounter: パルスタイミングの決定
- lastBeatTime: ビート間隔を見つける
- firstBeat: boolean
- secondBeat: boolean
- rate: 要素数10の配列, 過去の10回のIBIの値を保持する

# kodouinc.ino

## void setup()
- アウトプットの出力ピンを設定する(pinMode)
    - 瞬間的な光(blinkPin, 12番)
    - フェードな光(fadePin, 9番)
- シリアル通信のデータ送信レートを指定、現在は一番はやい
- 別ファイルのinterruptSetUp()を実行する

## void loop()

## void ledFadeToBeat()

## void sendDataToProcessing(char symbol, imt data)
- symbolとdataを出力する関数

# interrupt.ino
タイマー割り込みで使用される。
一貫した定期的なタイミングでパルスセンサーをサンプリングする。
コードは500Hz(2ms)でパルスセンサー信号を読み取るように設定されている。

【 メモ 】
interrupt: 割り込みという意味

## interruptSetup()
Timer2を初期化して、2msごとに割り込み処理を行う。

- sei(): グローバル割り込みが有効になるように設定

## ISR(TIMER2_COMPA_vect)
2msごとの割り込み処理で行うことがこの関数内にある
閾値的なところをキャッチする。
この関数はTimer2の値が124になった時をトリガーとする

　【 メモ 】
- ISR(Interrupt Service Routine) : 割り込みサービスルーチン
    - 割り込み受付によって起動されるオペレーティングシステムやシステムドライバのコールバックルーチン
    - 役割 : ハードウェア機器のための処理、システムコールなどのCPUモードの移行
        - CPUモード : CPUが実行できる操作を制限するもの
        
#### プログラム概要

- cli() : 終了するまで割り込みをさせない 
- Signal = analogRead(pulsePin); : パルスセンサーを読む(入力値を読む)
- sampleCounter += 2; : sampleCounter変数によって2msごとに追跡させる
- int N = sampleCounter - lastBeatTime; : 最後のビートからの時間を監視する

脈拍のピークとトラフを見つける

- もしSignalがthreshの閾値よりも小さいかつ、最後のビートからを表す変数Nの時間が3/5よりも大きいかつ、谷として持っている変数Tの値よりも小さい場合、TをそのSignalに更新する
- もし、Signalが脈拍の山の値よりも大きいかつ、閾値よりも大きい場合、PをSignalに更新する

心拍を検索する
パルスがあるたびに信号の急上昇する

- Nの値が250より大きい場合に作動(ノイズ排除)
- もし、Signalが閾値よりも大きく、最後のビートからを表す変数Nの時間が3/5よりも大きいかつPaulseがfalseの場合に作動
    - Paulseをtrueにする
    - Highの値をデジタルピンに出力する
    - IBIの値を更新する
    - lastBeatTimeを更新
- もし初めてbeatが感知された場合そのフラグを解除する
- もし2回目のフラグが感知された場合そのフラグを解除し、rateの全ての要素にIBIの値を入れる
- 最新の10回分のIBIの合計値をrunningTotalという変数に格納し平均を出す
- 60000/runningTotalでBPMを求める
- QSをtrueにする(このinterruptが終わるまでは元に戻らない)

- もしSignalが閾値以下でありPaluseがtrueの場合
    - Lowの値を出力
    - Paluseをfalse
    

# 割り込み処理
- マイコンは同時にひとつの処理しかできない
- ハードウェアからのトリガーを元に優先すべきサブルーチンを実行する割り込み処理が可能
- CTCモードの説明: http://startelc.com/AVR/Avr_100timrMemo.html

1. 割り込み要求(IRQ: InterruptRequest)が周辺回路から発行
2. 次に実行予定であったアドレスをスタックに格納する
3. 現在実行中の命令を完了させる
4. 割り込み処理ルーチン(ISR: Interrupt Service Routine)に制御を移す
5. 割り込み処理完了後、本来実行予定であった2のプロセスに戻る

http://easylabo.com/2015/02/atmel/6684/
