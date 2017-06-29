#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

#define MAX_GPIO_BANK_SIZE  30

struct gpio_t {
	char       name[5];
	uint32_t   base_off;
	uint32_t   idx; 
	uint32_t   reg_off;
	uint32_t*  reg_ptr;
	uint32_t*  dat_ptr;
	uint32_t   reg_idx;
	uint32_t   reg_clear_mask;
	uint32_t   data_clear_mask;
	uint32_t   old_val;
	uint32_t   val;
};

struct gpio_bank_t {
	struct gpio_t*  gpio[MAX_GPIO_BANK_SIZE];
	uint32_t  size;
};

int gpio_system_init(void);
void gpio_demo_test(void);

int gpio_init(struct gpio_t* p, const char * name);
int gpio_set_func(struct gpio_t* p, uint32_t i);
int gpio_set_output_value(struct gpio_t* p, const uint32_t v);
int gpio_set_output(struct gpio_t* p);
int gpio_set_input(struct gpio_t* p);

int gpio_bank_set_input(struct gpio_bank_t* pbank);
int gpio_bank_set_output(struct gpio_bank_t* pbank);
int gpio_bank_set_output_value(struct gpio_bank_t* pbank, const uint32_t v);

int gpio_bank_read(struct gpio_bank_t* pbank);

