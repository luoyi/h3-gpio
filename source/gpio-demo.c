#include "gpio-h3.h"


void keypad_test(void)
{
	struct gpio_t  gpio_row[5] = {0};
	struct gpio_t  gpio_col[5] = {0};

	struct gpio_bank_t row_bank;
	struct gpio_bank_t col_bank;

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
	gpio_bank_set_output_value(&row_bank, 1);

	do {
		gpio_bank_set_output(&col_bank);
		gpio_bank_set_output_value(&col_bank, 0);
		gpio_bank_set_input(&col_bank);
		gpio_bank_read(&col_bank);
		for ( uint32_t i = 0; i < 4; i++ ) {
			printf("col[%d] read value %d\n", i, gpio_col[i].val);
		}
		sleep(3);
	} while ( 1 );
}

int main(int argc, char ** argv)
{
	struct gpio_t  g;
	gpio_system_init();
	gpio_init(&g, "PA16");
	gpio_init(&g, "PA8");
	gpio_init(&g, "PA21");
	gpio_init(&g, "PA14");
	gpio_demo_test();
//	keypad_test();
	return 0;
}
