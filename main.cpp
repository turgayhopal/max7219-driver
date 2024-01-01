#include <iostream>
#include "lib/inc/max7219.hpp"

int main(int argc, char const *argv[])
{   
    MAX7219 max7219(0x07);

    int counter = 0;

    while (1) {
       
        max7219.print_digit((max7219_digit_t) 4, (uint8_t)((counter % 10000) / 1000), false);
		max7219.print_digit((max7219_digit_t) 3, (uint8_t)((counter % 1000) / 100), false);
		max7219.print_digit((max7219_digit_t) 2, (uint8_t)((counter % 100) / 10), false);
		max7219.print_digit((max7219_digit_t) 1, (uint8_t)((counter % 10)), false);
		
		if (counter > 9999) {
			counter = 0;
		} else {
			counter++;
		}

		usleep(10000); // 10 ms
      
    }

    return 0;
}
