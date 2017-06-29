#include "comm.h"
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
			FATAL_ERRORF("fd = %d, mmap error: %d-%s\n", fd, errno, strerror(errno));
			return -1;
		}
		DBG_MSG("ptr = 0x%08X", ptr);
		ptr += GPIO_REG_OFF;
		p_gpio = (uint32_t*) ptr;
		DBG_MSG("p_gpio = 0x%08X", p_gpio);
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
	p->reg_clear_mask =  ~( 0xF << ( p->reg_idx * 4 )) ;
	p->data_clear_mask = ~( 1 << (p->idx));
	p->val = 0xffffffff;
	DBG_MSG("bank = %c, idx = %d, base_off= %d, reg_off = %d, reg_idx = %d, reg=0x%x, clear_mask = 0x%x, data_clear_mask = 0x%x", bank, p->idx, 
			p->base_off, p->reg_off, p->reg_idx, *(p->reg_ptr), p->reg_clear_mask, p->data_clear_mask);
	return 0;
}

int gpio_set_func(struct gpio_t* p, uint32_t i)
{
	uint32_t v = *p->reg_ptr;
	uint32_t sv = (i << ( p->reg_idx * 4  ));
	DBG_MSG("gpio_set_func: name = %s, i = %d, v = 0x%08x, sv = 0x%08x, clear_mask = 0x%08x", p->name, i, v, sv, p->reg_clear_mask);
	v &= p->reg_clear_mask;
	v |= sv;
	DBG_MSG("v = 0x%x, sv = 0x%x", v, sv);
	*p->reg_ptr = v;
	__sync_synchronize();
}
int gpio_set_input(struct gpio_t* p)
{
	return gpio_set_func(p, 0);
	/*
	*p->reg_ptr &= p->reg_clear_mask;
	__sync_synchronize();
	*/
}
int gpio_set_output(struct gpio_t* p)
{
	gpio_set_func(p, 1);
}
int gpio_set_output_value(struct gpio_t* p, const uint32_t v)
{
	uint32_t dv = *p->dat_ptr;
	if ( !(v == 1 || v == 0) ) {
		FATAL_ERRORF("v is notvalid %d", v);
		return -1;
	}
	uint32_t mask = v << p->idx;
	DBG_MSG("v = 0x%x, mask = 0x%x, dv=0x%x", v, mask, dv);
	dv &= p->data_clear_mask;
	dv |= mask;
	*p->dat_ptr = dv;
	DBG_MSG("v = 0x%x, mask = 0x%x, dv=0x%x", v, mask, dv);
	__sync_synchronize();
	return 0;
}
int gpio_read_input(struct gpio_t* p, uint32_t* v)
{
	uint32_t mask = 1 << p->idx;
	DBG_MSG("read dat = 0x%08x", *p->dat_ptr);
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

int gpio_bank_set_input(struct gpio_bank_t* pbank)
{
	uint32_t i = 0;
	for ( i = 0; i < pbank->size; i++ ) {
		gpio_set_input(pbank->gpio[i]);
	}
	return 0;
}
int gpio_bank_set_output(struct gpio_bank_t* pbank)
{
	uint32_t i = 0;
	for ( i = 0; i < pbank->size; i++ ) {
		gpio_set_output(pbank->gpio[i]);
	}
	return 0;
}
int gpio_bank_set_output_value(struct gpio_bank_t* pbank, const uint32_t v)
{
	if ( pbank->size == 0 ) {
		return 0;
	}
	if ( ! (v == 1 || v == 0) ) {
		return -1;
	}

	uint32_t dv = *((pbank->gpio[0])->dat_ptr);
	DBG_MSG("bank_output_value %d, read dat = 0x%08x", v, dv);
	__sync_synchronize();
	uint32_t mask = 0xffffffff;
	uint32_t sv   = 0;
	for ( uint32_t i = 0; i < pbank->size; i++ ) {
		DBG_MSG("bank_output_value %d, mask = 0x%08x, clear_mask = 0x%08x", i, mask, pbank->gpio[i]->data_clear_mask );
		sv |= v << pbank->gpio[i]->idx;
		mask &= pbank->gpio[i]->data_clear_mask;
	}
	dv &= mask;
	dv |= sv;
	DBG_MSG("bank_output_value %d, set dat = 0x%08x", v, dv);
	*(pbank->gpio[0]->dat_ptr) = dv;
	__sync_synchronize();

	return 0;
}
int gpio_bank_read(struct gpio_bank_t* pbank)
{
	uint32_t dv = *((pbank->gpio[0])->dat_ptr);
	__sync_synchronize();
	for ( uint32_t i = 0; i < pbank->size; i++ ) {
		uint32_t mask = ( 1 << pbank->gpio[i]->idx );
		pbank->gpio[i]->val = ( dv & mask ) ? 1 : 0;
	}
	return 0;
}
