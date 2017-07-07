#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H


#define SBI(reg, bit) ((reg)|=(1<<(bit)))
#define CBI(reg, bit) ((reg)&=~(1<<(bit)))
#define LIMIT(value, min, max) (((value)<(min))?(min):((value)>(max))?(max):(value))
#define BIT_SET(reg, bit, value) ((reg)=((reg)&~(1<<(bit)))|((value)<<(bit)))

#define BITS_MASK(sbit, nbit) (((2<<(nbit))-1)<<(sbit))
#define BITS_SET(reg, sbit, nbit, value) ((reg)=((reg)&~BITS_MASK(sbit, nbit))|(((value)<<(sbit))&BITS_MASK(sbit, nbit)))

#define BV(reg, bit) (((reg)>>(bit))&1)

#define	CONCAT_(a, b)	a ## b
#define	CONCAT(a, b)	CONCAT_(a, b)


#endif
