ENTRY(_start)
SECTIONS
{
	. = 0x20000000;
	.init : { *(.init) }
	.text : { *(.text) }
}

