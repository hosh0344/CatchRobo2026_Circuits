# キャチロボ2026で使用した回路 <br>

## 1. DCMD2026

ブラシ付きDCモータドライバ基板

STM32G431を使用し、ハードウェアタイマーによりエンコーダーパルスを自動でカウントすることができる。CAN、リミットスイッチなどに対応しており、パターン的には20A程度流せるように設計している。

<img width="573" height="383" alt="{023DE596-0431-440E-A475-77CB5A5AA967}" src="https://github.com/user-attachments/assets/28314eae-a597-4837-a002-586c2e877398" />

## 2. PWSP2026

従来のPWSPをマイナーチェンジした基板

AND回路による緊停を撤廃し、直接PMOSのゲートをハイインピーダンスにすることによってロジック部に依存しない方式に変更した。CANはオプションで搭載しているが、基本的には使わない方針である。

<img width="493" height="351" alt="{26B5F7B8-E7FF-49B4-9FE5-0DB93D81116B}" src="https://github.com/user-attachments/assets/ab98d2fd-3d0f-4990-aa24-940b5b1776dd" />

## 3. MainController2026

STM32G474RET6を用いたコントローラ基板

各基板とCANを行うための中央制御基板として用いた。CANは2系統に対応。各タクトスイッチにはシュミットトリガICによるチャタリング防止機能がついているほか、中央のOLEDにはCANの状態監視機能を実装しているため、デバッグ等にも用いることができる。あと純粋にかっこいい。

<img width="564" height="316" alt="{3861A9B9-AB5E-49AF-85AC-EDFF892A3663}" src="https://github.com/user-attachments/assets/a14d38b3-f0e5-4329-828d-ad7fac966b7c" />

## 4. EndEffecterBoard2026

基板データは[こちら](https://github.com/hosh0344/EndEffecterBoard2026)

3つのポンプと3つのサーボ、FEETECH社のSTS3215を制御できるほか、DCMD同様にハードウェアエンコーダカウントができる。

<img width="1161" height="797" alt="image" src="https://github.com/user-attachments/assets/8dbe5f78-3ac2-423f-93dd-b20832d3aa04" />


