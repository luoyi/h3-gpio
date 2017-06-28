#include "gpio-h3.h"

static const off_t GPIO_REG_BASE=0x01C20000;
static const size_t GPIO_REG_OFF=0x800;
static const size_t GPIO_REG_LEN=0x1800;

static uint32_t*   p_gpio=NULL;

int gpio_system_init(void)
{
	if ( NULL == p_gpio ) {
		int fd = open("/dev/mem", O_RDWR|O_SYNC);

		if (fd < 0) {
			fprintf(stderr, "Could not open /dev/mem\n");
			return -1;
		}

		unsigned char * ptr = (unsigned char*)mmap(NULL, GPIO_REG_LEN, PROT_READ|PROT_WRITE,
				MAP_SHARED, fd, GPIO_REG_BASE);

		if ( MAP_FAILED == ptr ) {
			fprintf(stderr, "fd = %d, mmap error: %d-%s\n", fd, errno, strerror(errno));
			return -1;
		}
		printf("ptr = 0x%08X\n", ptr);
		ptr += GPIO_REG_OFF;
		p_gpio = (uint32_t*) ptr;
		printf("p_gpio = 0x%08X\n", p_gpio);
	}
	return 0;
}

int gpio_init(struct gpio_t* p, const char * name)
{
	char bank;
	memset(p->name, 0, 5);
	strncpy(p->name, name, 4);
	sscanf(p->name, "P%c%d", &bank, &p->idx);
	p->base_off = bank - 'A';
	p->reg_off  = p->idx / 8;
	p->reg_ptr  = p_gpio + 9 * p->base_off + p->reg_off;
	p->dat_ptr  = p_gpio + 9 * p->base_off + 4;
	p->reg_idx  = p->idx % 8;
	p->reg_clear_mask = ~(( p->reg_idx * 4 )<<0xF);
	p->data_mask = ~((p->idx)<<1);
	printf("bank = %c, idx = %d, base_off= %d, reg_off = %d, reg_idx = %d, reg=0x%x\n", bank, p->idx, 
			p->base_off, p->reg_off, p->reg_idx, *(p->reg_ptr));
	return 0;
}

int gpio_set_func(struct gpio_t* p, uint32_t i)
{
	uint32_t v = *p->reg_ptr;
	uint32_t mask = (( p->reg_idx * 4 )<<i);
	mask |= p->reg_clear_mask;
	v &= p->reg_clear_mask;
	v |= mask;
	*p->reg_ptr = v;
	__sync_synchronize();
}
int gpio_set_input(struct gpio_t* p)
{
	*p->reg_ptr &= p->reg_clear_mask;
	__sync_synchronize();
}
int gpio_set_output(struct gpio_t* p)
{
	gpio_set_func(p, 1);
}
int gpio_set_output_value(struct gpio_t* p, const uint32_t v)
{
	uint32_t dv = *p->dat_ptr;
	if ( v != 1 || v != 0 ) {
		return -1;
	}
	uint32_t mask = p->idx << v;
	mask |=  p->data_mask;
	dv &= p->data_mask;
	dv |= mask;
	*p->dat_ptr = dv;
	__sync_synchronize();
	return 0;
}
int gpio_read_input(struct gpio_t* p, uint32_t* v)
{
	uint32_t mask = p->idx << 1;
	if ( 0 != *p->dat_ptr & mask ) {
		*v = 1;
	} else {
		*v = 0;
	}
	__sync_synchronize();
	return 0;
}
int gpio_set_disable(struct gpio_t* p)
{
	gpio_set_func(p, 7);
}

void gpio_demo_test(void)
{
	printf("pa0 = 0x%x\n", *p_gpio++);
	printf("pa1 = 0x%x\n", *p_gpio++);
	printf("pa2 = 0x%x\n", *p_gpio++);
	printf("pa3 = 0x%x\n", *p_gpio++);
}
