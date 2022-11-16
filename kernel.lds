ENTRY(_start)
SECTIONS
{
	. = 0x20000000;
	.init : { *(.init) }
	. = 0x00200000;
	.text : { *(.text) }
}

