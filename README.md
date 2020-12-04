# ロボットシステム学
## デバイスドライバーの改造
実行環境
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
下の回路図のように組んでください


改造点
