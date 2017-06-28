#include "gpio-h3.h"

int main(int argc, char ** argv)
{
	struct gpio_t  g;
	gpio_system_init();
	gpio_init(&g, "PA10");
	gpio_init(&g, "PC2");
	gpio_init(&g, "PG8");
	gpio_init(&g, "PC21");
	gpio_demo_test();
	return 0;
}
