/*
	5V�d����
		�p�C���b�g�����v������ 6uA
		�p�C���b�g�����v�_���� 20uA

	�P4�}���K���d�r 450mAh
		450mAh / 20uA = 22500h = 937days
		�x���炳�Ȃ���Γd�r������N�ȏ�
*/
#include<avr/io.h>
#include<avr/sleep.h>
#include<avr/interrupt.h>
#include<avr/wdt.h>
#include"common.h"
#include <util/delay.h>


// �o��
#define BUZZER_PORTx B
#define BUZZER_PINx  3
// �v���A�b�v����
#define SENSOR_PORTx B
#define SENSOR_PINx  1
// �v���A�b�v���o�͂Ƃ��Ďg�p
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

	// �ȃG�l�̂��ߕK�v���̂ݒʓd����
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

	// �f�W�^�����͋֎~
//	SBI(DIDR0, ADC0D); // PB5/ADC0/RESET
	SBI(DIDR0, ADC2D); // PB4/ADC2
	SBI(DIDR0, ADC3D); // PB3/ADC3/CLKI
	SBI(DIDR0, ADC1D); // PB2/ADC1/SCK
//	SBI(DIDR0, AIN1D); // PB1/AIN1/MISO
	SBI(DIDR0, AIN0D); // PB0/AIN0/MOSI

	// BOD��~
	SBI(BODCR, 1); // BODS=1 bit
	SBI(BODCR, 0); // BODSE=0 bit
	SBI(BODCR, 1); // BODS=1 bit
	CBI(BODCR, 0); // BODSE=0 bit

	// �^�C�}�J�E���^��~
	SBI(PRR, PRTIM0);

	// �R���p���[�^��~
	SBI(ACSR, ACD);

	// ������d����~
	CBI(ACSR, ACBG);
	// ADC��~
	CBI(ADCSRA, ADEN);
	SBI(PRR, PRADC);

	// �X���[�v���[�h�ݒ�
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// WDT�ݒ�
	cli();
	wdt_reset();
	wdt_disable();
	CBI(MCUSR, WDRF);
	WDTCR|=(1<<WDCE)|(1<<WDE); // WDT�ύX����
	WDTCR=0b01000011; // ���荞�݋���,���Z�b�g����,�N���b�N����(0.125sec)
	wdt_reset();
	sei();

	buzzer_set(0);
	pilot_set(0);
}


// 0.125�b���ƂɌĂ΂��
void loop(void)
{
	static uint16_t count=0;
	uint16_t sec;
	uint8_t buzzer=0, sensor;

	// ���[�h�X�C�b�`�̏�Ԃ��擾
	sensor=sensor_get();
	if(sensor)
	{
		if(count<30000) count++;

		sec=(count>>3); // count 8�ɂ� 1�b
		if(sec<500) // 10���ȉ��͉������Ȃ�
		{
		}
		else if(sec<750) // 10���`15���͗\���x��
		{
			if(!(sec&0x3f))
			{
				buzzer=!(count&0x03);
			}
		}
		else // 15���ȏ�͌x��
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
