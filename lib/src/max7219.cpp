// Header: MAX7219 Cpp Driver Source File
// File Name: max7219.cpp
// Author: Turgay Hopal
// Date: 30.12.2023

#include "../inc/max7219.hpp"

static uint8_t SYMBOLS[] = {
    0x7E, // numeric 0
    0x30, // numeric 1
    0x6D, // numeric 2
    0x79, // numeric 3
    0x33, // numeric 4
    0x5B, // numeric 5
    0x5F, // numeric 6
    0x70, // numeric 7
    0x7F, // numeric 8
    0x7B, // numeric 9
    0x01, // minus
    0x4F, // letter E
    0x37, // letter H
    0x0E, // letter L
    0x67, // letter P
    0x00, // blank
    0x63, // % üst	(16)
    0x1D, // % alt	(17)
    0x1F, // letter b (18)
    0x3D, // letter d (19)
    0x0A, // -
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x49};

static uint16_t getSymbol(uint8_t number)
{
    return SYMBOLS[number];
}

MAX7219::MAX7219(uint8_t intensivity)
{
    this->config.spi_bits = 8;
    this->config.spi_device_path = "/dev/spidev0.0";
    this->config.spi_mode = SPI_MODE_0;
    this->config.spi_speed = 1000000;

    this->intensivity = intensivity;
    this->decode = 0x00;

    this->init_spi();

    this->turn_on();
    this->write_spi(REG_SCAN_LIMIT, NUMBER_OF_DIGITS - 1);
    this->write_spi(REG_DISPLAY_TEST, 0); // Disable Self Test
    this->set_intensivity(intensivity);
    this->decode_off();
    this->clear();
}

MAX7219::~MAX7219() {
    if (this->spi_fd >= 0) {
        close(this->spi_fd);
    }
}

uint8_t MAX7219::init_spi(void)
{
    this->spi_fd = open(this->config.spi_device_path, O_RDWR);
    if (this->spi_fd < 0)
    {
        std::cerr << "Error opening SPI device" << std::endl;
        return 0;
    }

    if (ioctl(this->spi_fd, SPI_IOC_WR_MODE, &this->config.spi_mode) < 0 ||
        ioctl(this->spi_fd, SPI_IOC_WR_BITS_PER_WORD, &this->config.spi_bits) < 0 ||
        ioctl(this->spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &this->config.spi_speed) < 0)
    {
        std::cerr << "Error setting SPI parameters" << std::endl;
        close(this->spi_fd);
        return 0;
    }
    
    return 1;
}

uint8_t MAX7219::write_spi(uint8_t register_addr, uint8_t data)
{

    if (this->spi_fd < 0) {
        std::cerr << "SPI device not available." << std::endl;
        return 0;
    }

    uint8_t tx_buffer[2];

    tx_buffer[0] = register_addr;
    tx_buffer[1] = data;

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx_buffer,
        .len = 2,
        .delay_usecs = 0,
        .bits_per_word = this->config.spi_bits};

    if (ioctl(this->spi_fd, SPI_IOC_MESSAGE(1), &transfer) < 0)
    {
        std::cerr << "Error transmitting SPI data" << std::endl;
        close(this->spi_fd);
        return 0;
    }

    return 1;
}

void MAX7219::set_intensivity(uint8_t intensivity)
{
    if (intensivity > 0x0F)
    {
        return;
    }
    else
    {
        this->write_spi(REG_INTENSITY, intensivity);
    }
}
void MAX7219::clear(void)
{
    uint8_t clear = 0x00;

    if (this->decode == 0xFF)
    {

        clear = BLANK;
    }

    for (int i = 1; i <= 8; ++i)
    {
        this->write_spi(i, clear);
    }
}

void MAX7219::turn_on(void)
{
    this->write_spi(REG_SHUTDOWN, 0x01);
}

void MAX7219::turn_off(void)
{
    this->write_spi(REG_SHUTDOWN, 0x00);
}

void MAX7219::decode_on(void)
{
    this->decode = 0xFF;
    this->write_spi(REG_DECODE_MODE, this->decode);
}

void MAX7219::decode_off(void)
{
    this->decode = 0x00;
    this->write_spi(REG_DECODE_MODE, this->decode);
}

void MAX7219::print_digit(max7219_digit_t digit, uint8_t number, bool point_st)
{
    if (digit > NUMBER_OF_DIGITS)
    {
        return;
    }

    if (point_st)
    {
        if (this->decode == 0x00)
        {
            this->write_spi(digit, getSymbol(number) | (1 << 7));
        }
        else
        {
            this->write_spi(digit, number | (1 << 7));
        }
    }
    else
    {
        if (this->decode == 0x00)
        {
            this->write_spi(digit, getSymbol(number) & (~(1 << 7)));
        }
        else
        {
            this->write_spi(digit, number & (~(1 << 7)));
        }
    }
}

void MAX7219::print_number(uint8_t position, int value)
{
    this->write_spi(REG_DECODE_MODE, 0xFF);

	int32_t i;

	if (value < 0)
	{
		if(position > 0)
		{
			this->write_spi(position, MINUS);
			position--;
		}
		value = -value;
	}

	i = 1;

	while ((value / i) > 9)
	{
		i *= 10;
	}

	if(position > 0)
	{
		this->write_spi(position, value/i);
		position--;
	}

	i /= 10;

	while (i > 0)
	{
		if(position > 0)
		{
			this->write_spi(position, (value % (i * 10)) / i);
			position--;
		}

		i /= 10;
	}

	this->write_spi(REG_DECODE_MODE, this->decode);

}

void MAX7219::print_char(max7219_digit_t digit, uint8_t data)
{
    this->write_spi(digit, getSymbol(data));
}
