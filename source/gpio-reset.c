#include "gpio-h3.h"

int main(int argc, char ** argv)
{
	struct gpio_t  g;
	gpio_system_init();
	gpio_init(&g, "PA0");
	*(g.reg_ptr++) = 0x77777177;
	*(g.reg_ptr++) = 0x77777777;
	*(g.reg_ptr++) = 0x77777777;
	*(g.reg_ptr++) = 0x77777777;
	return 0;
}
