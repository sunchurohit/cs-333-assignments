#include "types.h"
#include "stat.h"
#include "user.h"

char b[1024];

#define PHYSTOP 0xE000000 
#define KERNBASE 0x80000000
#define t321 0xFFFFFFFF
#define PGSIZE 4096

int getpusz()
{
	uint ans = 0;
	int flag;
	uint pa ;
	for(uint i=0;i < KERNBASE/PGSIZE ; i ++)
	{
		if(get_va_to_pa(PGSIZE*i,&pa,&flag) == 1)
		{
			ans += PGSIZE ;
		}
	}
	return ans ;
	/* todo */
}
int getpksz()
{
	uint ans = 0;
	int flag;
	uint pa;
	for(uint i=KERNBASE/PGSIZE; i <= t321/PGSIZE ; i+= 1)
	{
		if(get_va_to_pa(i*PGSIZE,&pa,&flag))
		{
			ans += PGSIZE;
		}
	}

	return ans ;
	/* todo */
}

int
main(int argc, char *argv[])
{
	char *buf;

	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());


	buf=sbrk(4096);
	buf[0]='\0';
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	
	buf=sbrk(4096*7);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	buf=sbrk(1);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());

	buf=sbrk(2);
	printf(1, "\ngetpsz: %d bytes \n", getpsz());
	printf(1,"getpusz: %d bytes \n",getpusz());
	printf(1,"getpksz: %d bytes\n",getpksz());


	exit();
}