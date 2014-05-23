/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
 ENTRY( main ) 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0x08048000;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(4K) :
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) :
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) :
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) :
	{
		*(COMMON)
		*(.bss)
	}
	
	.comment BLOCK(4K) : 
	{
		*(.comment)
	}
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
