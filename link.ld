/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
/* ENTRY( start ) */
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0x100000;

	.start BLOCK(64K) : AT(0x100000)
	{
		*(.multiboot)
		*(.start)
	}

	.startdata BLOCK(64K) : AT(0x110000)
	{
		*(.startdata)
	}


	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 0xC0000000 + 0x200000;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(64K) :  AT(ADDR(.text)-0xC0000000)
	{
		*(.text)
	}

	.user BLOCK(4K): AT(ADDR(.user)-0xC0000000)
	{
		*(.user)
	}

	.userdata BLOCK(4K): AT(ADDR(.userdata)-0xC0000000)
	{
		*(.userdata)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : AT(ADDR(.rodata)-0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : AT(ADDR(.data)-0xC0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : AT(ADDR(.bss)-0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
	
	.comment BLOCK(4K) : AT(ADDR(.comment)-0xC0000000)
	{
		*(.comment)
	}
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
