#include "gpio-h3.h"
#include <time.h>

#define NANO_SECOND_MULTIPLIER  1000000 
#define INTERVAL_MS  (100 * NANO_SECOND_MULTIPLIER)

void keypad_test(void)
{
	struct gpio_t  gpio_row[5] = {0};
	struct gpio_t  gpio_col[5] = {0};

	struct gpio_bank_t row_bank;
	struct gpio_bank_t col_bank;

	struct timespec tim, tim2;

	tim.tv_sec = 0;
	tim.tv_nsec = INTERVAL_MS;


	gpio_init(&gpio_row[0], "PA9");
	gpio_init(&gpio_row[1], "PA16");
	gpio_init(&gpio_row[2], "PA8");
	gpio_init(&gpio_row[3], "PA21");

	gpio_init(&gpio_col[0], "PA14");
	gpio_init(&gpio_col[1], "PA15");
	gpio_init(&gpio_col[2], "PA13");
	gpio_init(&gpio_col[3], "PA7");

	for ( uint32_t i = 0; i < 4; i++ ) {
		row_bank.gpio[i] = &gpio_row[i];
		col_bank.gpio[i] = &gpio_col[i];
	}
	row_bank.size = col_bank.size = 4;

	gpio_bank_set_output(&row_bank);

	do {
		for ( uint32_t i = 0; i < row_bank.size; i++ ) {
			gpio_bank_set_output_value(&row_bank, 0);
			gpio_set_output_value(row_bank.gpio[i], 1);

			gpio_bank_set_output(&col_bank);
			gpio_bank_set_output_value(&col_bank, 0);
			gpio_bank_set_input(&col_bank);
			gpio_bank_read(&col_bank);
			for ( uint32_t j = 0; j < 4; j++ ) {
				if ( gpio_col[j].val ) {
					printf("row[%d] col[%d] read value %d\n", i, j, gpio_col[j].val);
				}
			}
		}
		nanosleep(&tim , &tim2);
	} while ( 1 );
}

int main(int argc, char ** argv)
{
	/*
	struct gpio_t  g;
	gpio_init(&g, "PA16");
	gpio_set_output(&g);
	gpio_set_output_value(&g, 0);
	gpio_set_output_value(&g, 1);
	gpio_set_func(&g, 3);
	gpio_init(&g, "PA8");
	gpio_set_input(&g);
	gpio_set_func(&g, 3);
	gpio_init(&g, "PA21");
	gpio_set_output(&g);
	gpio_set_output_value(&g, 0);
	gpio_set_output_value(&g, 1);
	gpio_set_func(&g, 2);
	gpio_init(&g, "PA14");
	gpio_demo_test();
	*/
	gpio_system_init();
	keypad_test();
	return 0;
}
