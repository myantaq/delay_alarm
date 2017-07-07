/*
	5V電源時
		パイロットランプ消灯時 6uA
		パイロットランプ点灯時 20uA

	単4マンガン電池 450mAh
		450mAh / 20uA = 22500h = 937days
		警報を鳴らさなければ電池寿命二年以上
*/
#include<avr/io.h>
#include<avr/sleep.h>
#include<avr/interrupt.h>
#include<avr/wdt.h>
#include"common.h"
#include <util/delay.h>


// 出力
#define BUZZER_PORTx B
#define BUZZER_PINx  3
// プルアップ入力
#define SENSOR_PORTx B
#define SENSOR_PINx  1
// プルアップを出力として使用
#define PILOT_PORTx  B
#define PILOT_PINx   4


#define BUZZER_PORT CONCAT(PORT, BUZZER_PORTx)
#define BUZZER_DDR  CONCAT(DDR , BUZZER_PORTx)
#define SENSOR_PORT CONCAT(PORT, SENSOR_PORTx)
#define SENSOR_PIN  CONCAT(PIN , SENSOR_PORTx)
#define SENSOR_DDR  CONCAT(DDR , SENSOR_PORTx)
#define PILOT_PORT  CONCAT(PORT, PILOT_PORTx)
#define PILOT_DDR   CONCAT(DDR , PILOT_PORTx)


ISR(WDT_vect)
{
}


uint8_t sensor_get(void)
{
	uint8_t ret;

	// 省エネのため必要時のみ通電する
	SBI(SENSOR_PORT,  SENSOR_PINx);
	_delay_us(2);
	ret=BV(SENSOR_PIN, SENSOR_PINx);
	CBI(SENSOR_PORT,  SENSOR_PINx);

	return ret;
}


void buzzer_set(uint8_t buzzer)
{
	if(buzzer) SBI(BUZZER_PORT, BUZZER_PINx);
	else CBI(BUZZER_PORT, BUZZER_PINx);
}


void pilot_set(uint8_t pilot)
{
	if(pilot) SBI(PILOT_PORT, PILOT_PINx);
	else CBI(PILOT_PORT, PILOT_PINx);
}


void setup(void)
{
	DDRB=0b00000000;
	//
	SBI(BUZZER_DDR,  BUZZER_PINx); // PB3
	CBI(BUZZER_PORT, BUZZER_PINx);
	CBI(SENSOR_DDR,  SENSOR_PINx); // PB1
	SBI(SENSOR_PORT, SENSOR_PINx);
	CBI(PILOT_DDR,   PILOT_PINx);  // PB4
	CBI(PILOT_PORT,  PILOT_PINx);

	// デジタル入力禁止
//	SBI(DIDR0, ADC0D); // PB5/ADC0/RESET
	SBI(DIDR0, ADC2D); // PB4/ADC2
	SBI(DIDR0, ADC3D); // PB3/ADC3/CLKI
	SBI(DIDR0, ADC1D); // PB2/ADC1/SCK
//	SBI(DIDR0, AIN1D); // PB1/AIN1/MISO
	SBI(DIDR0, AIN0D); // PB0/AIN0/MOSI

	// BOD停止
	SBI(BODCR, 1); // BODS=1 bit
	SBI(BODCR, 0); // BODSE=0 bit
	SBI(BODCR, 1); // BODS=1 bit
	CBI(BODCR, 0); // BODSE=0 bit

	// タイマカウンタ停止
	SBI(PRR, PRTIM0);

	// コンパレータ停止
	SBI(ACSR, ACD);

	// 内部基準電圧停止
	CBI(ACSR, ACBG);
	// ADC停止
	CBI(ADCSRA, ADEN);
	SBI(PRR, PRADC);

	// スリープモード設定
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// WDT設定
	cli();
	wdt_reset();
	wdt_disable();
	CBI(MCUSR, WDRF);
	WDTCR|=(1<<WDCE)|(1<<WDE); // WDT変更準備
	WDTCR=0b01000011; // 割り込み許可,リセット無し,クロック分周(0.125sec)
	wdt_reset();
	sei();

	buzzer_set(0);
	pilot_set(0);
}


// 0.125秒ごとに呼ばれる
void loop(void)
{
	static uint16_t count=0;
	uint16_t sec;
	uint8_t buzzer=0, sensor;

	// リードスイッチの状態を取得
	sensor=sensor_get();
	if(sensor)
	{
		if(count<30000) count++;

		sec=(count>>3); // count 8につき 1秒
		if(sec<500) // 10分以下は何もしない
		{
		}
		else if(sec<750) // 10分～15分は予備警告
		{
			if(!(sec&0x3f))
			{
				buzzer=!(count&0x03);
			}
		}
		else // 15分以上は警報
		{
			buzzer=!(sec&3);
		}
	}
	else
	{
		count=0;
	}
	buzzer_set(buzzer);
	pilot_set(sensor);
}


int main(void)
{
	setup();

	for(;;)
	{
		loop();
		sleep_mode();
	}
	return 0;
}
