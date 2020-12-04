# ロボットシステム学
## デバイスドライバーの改造
授業内で作ったデバイスドライバを改造したものです

## 実行環境
以下の環境で動作確認しました
- Ubuntu18.04 LTS
- Raspberry Pi4 Model B

## インストール方法&使い方
~~~
git clone https://github.com/syutyo/device_driver.git
cd myled
make
sudo insmod myled.ko
sudo chmod 666 /dev/myled0
echo (0 or 1 or 2) > /dev/myled0
~~~
### 必要なもの
- ジャンパー線 : 4本
- LED : 二個
- 200Ω程度の抵抗

下の回路図のように組んでください

GPIO 18 と　GPIO 25 を使ってください,LEDのカソードのほうをGNDにつけてください
![IMG_1250 (2)](https://user-images.githubusercontent.com/72371137/101174205-e88cef00-3686-11eb-8e5b-4b6a4f3e5973.JPG)

## 改造内容
- echo 0 のとき　両方のLEDの消灯
- echo 1 のとき　両方のLEDの点灯
- echo 2 のとき　片方ずつ、明るさを変化させ、どちらかが必ずついている状態にする。

動画のリンク
https://youtu.be/kiuZPdknLSE
