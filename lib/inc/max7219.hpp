// Header: MAX7219 Cpp Driver Header File
// File Name: max7219.hpp
// Author: Turgay Hopal
// Date: 30.12.2023

#include <iostream>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>

#define	NUM_0				0x00
#define	NUM_1				0x01
#define	NUM_2				0x02
#define	NUM_3				0x03
#define	NUM_4				0x04
#define	NUM_5				0x05
#define	NUM_6				0x06
#define	NUM_7				0x07
#define	NUM_8				0x08
#define	NUM_9				0x09
#define	MINUS				0x0A
#define	LETTER_E			0x0B
#define	LETTER_H			0x0C
#define	LETTER_L			0x0D
#define	LETTER_P			0x0E
#define	BLANK				0x0F

#define	REG_NO_OP 			0x00
#define	REG_DIGIT_0 		0x01
#define	REG_DIGIT_1 		0x02
#define	REG_DIGIT_2 		0x03
#define	REG_DIGIT_3 		0x04
#define	REG_DIGIT_4 		0x05
#define	REG_DIGIT_5 		0x06
#define	REG_DIGIT_6 		0x07
#define	REG_DIGIT_7 		0x08
#define	REG_DECODE_MODE 	0x09
#define	REG_INTENSITY 		0x0A
#define	REG_SCAN_LIMIT 		0x0B
#define	REG_SHUTDOWN 		0x0C
#define	REG_DISPLAY_TEST 	0x0F

#define NUMBER_OF_DIGITS	8

typedef struct max7219_config_s
{
    const char *spi_device_path;
    uint8_t spi_mode;
    uint8_t spi_bits;
    uint32_t spi_speed;
} max7219_config_t;

typedef enum max7219_digit_e{
  DIGIT_1 = 0x01,
  DIGIT_2,
  DIGIT_3,
  DIGIT_4,
  DIGIT_5,
  DIGIT_6,
  DIGIT_7,
  DIGIT_8,
} max7219_digit_t;

class MAX7219
{
private:
    int spi_fd;
    max7219_config_t config;
    uint8_t decode;
    uint8_t intensivity;
    uint8_t init_spi(void);
    uint8_t write_spi(uint8_t register_addr, uint8_t data);
public:
    MAX7219(uint8_t intensivity);
    ~MAX7219();
    void set_intensivity(uint8_t intensivity);
    void clear(void);
    void turn_on(void);
    void turn_off(void);
    void decode_on(void);
    void decode_off(void);
    void print_digit(max7219_digit_t digit, uint8_t number, bool point_st);
    void print_number(uint8_t position, int value);
    void print_char(max7219_digit_t digit, uint8_t data);
};
