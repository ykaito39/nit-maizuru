#encoding: UTF-8

#32*48のデータをシリアル通信できる形に分割するプログラム
#dataは(M,N)型;M行N列

#----------------------------------------------------------
#送信は、先ず通信開始コード<START>が送られることで始まる。
#次に8ビットごとにマップデータを送信し、通信終了時に<END>を送る。
#マップデータ内に<START>や<END>がある場合は<ESC><0x11>と言った具合に
#<ESC>でエスケープしてから送信される。

#受信側にここらへんの仕様をコミットしたら使えるかも
#----------------------------------------------------------


import sys
import os
import serial
import time

#地図データ取得(仮)
#島田師作成の8*12LEDマトリクス板に対応
def Get_original_map_data(file):
    try:
        original_map_file = os.open(file)
        i += 1
        for l in original_map_file.readline(): #行ごとの読み取り
            original_map_data[i] = '0b' + l
            i += 1
            
            original_map_file.close()
            
    except:
        original_map_data      = [0b11000111,
                                  0b10011011,
                                  0b11111011,
                                  0b11000111,
                                  0b11111011,
                                  0b00111011,
                                  0b11000011,
                                  0b01101011,
                                  0b11010011,
                                  0b01011010,
                                  0b11111011,
                                  0b01101011]
        
    return original_map_data


#------------------------------------------------------------
#ここから上:関数
#ここから下:中身
#------------------------------------------------------------

#ヘッダ・フッタ・エスケープシーケンス
START   = 0x11
END     = 0x22
ESC     = 0x33

#行ごとにデータを読み込む
original_map_data = Get_original_map_data("map.txt") 

M = len(original_map_data)
N = 1 #LED横幅/8を手動で入力してくれぇ...
data = [[0 for j in range(N)] for i in range(M)]

#データ分割
for i in range(M):
    print("["+str(i)+"]", end="")
    for j in range(N):
        data[i][j] = (original_map_data[i] >> j*8) & 0xff
        print(hex(data[i][j]), end=" ")
    print("\n", end="")


#シリアル通信を行なう
#Windowsのみ対応
sel = serial.Serial("COM12") #Arduinoのポートを"COM1"みたいに直打ちしてね(ハート)

#データ送るよー
#一つのマップデータを送るときのフォーマット<START><MAP_DATA><END>
sel.write(START) #<START>の送信

for d in data:   #<MAP_DATA>の送信
    for d_hoge in d: #2次元目の取り出し
        if (d_hoge == START) and (d_hoge == END):
            sel.write(ESC)
            time.sleep(0.001)
            sel.write(d_hoge)

sel.write(END)   #おわり

sel.close()
