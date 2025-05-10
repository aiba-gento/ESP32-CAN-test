#include <Arduino.h>
#include <CAN.h>

bool rx_flag = false; // 受信フラグ
int rx_data[8];       // 受信データ格納用配列
int rx_data_len = 0;  // 受信データの長さ
int rx_data_id = 0;   // 受信データのID

void rx_callback(int packet_size)
{
    // 受信データのIDを取得
    rx_data_id = CAN.packetId();
    // 受信データの長さを取得
    rx_data_len = CAN.packetDlc();
    // 受信データを配列に格納
    for (int i = 0; i < packet_size; i++)
    {
        rx_data[i] = CAN.read();
    }
    // 受信フラグを立てる
    rx_flag = true;
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // シリアル通信が開始されるまで待機

    CAN.setPins(32, 33); // RXピンとTXピンの設定（基板によって異なるが、今回はHTMDv1.5に合わせる）
    CAN.begin(1000E3);   // CAN通信の初期化(1Mbps)
    volatile uint32_t *pREG_IER = (volatile uint32_t *)0x3ff6b010;
    *pREG_IER &= ~(uint8_t)0x10;
    CAN.onReceive(rx_callback); // 受信コールバック関数の設定
}

void loop()
{
    // 受信フラグが立っている場合
    if (rx_flag)
    {
        // 受信データのIDを表示
        Serial.print("ID: ");
        Serial.println(rx_data_id, HEX);
        // 受信データの長さを表示
        Serial.print("Length: ");
        Serial.println(rx_data_len);
        // 受信データを表示
        Serial.print("Data: ");
        for (int i = 0; i < rx_data_len; i++)
        {
            Serial.print(rx_data[i]);
            Serial.print(" ");
        }
        Serial.println();
        // 受信フラグを下ろす
        rx_flag = false;
    }
    delay(100); // 100ms待機
}