ENTRY(_start)
EXTERN(ivt)

MEMORY {
	BOOT : ORIGIN = 0x0, LENGTH = 1M   
	I_ROM : ORIGIN = 0x00100000, LENGTH = 128K
	I_RAM : ORIGIN = 0x00200000, LENGTH = 16K
	FLASH : ORIGIN = 0x10000000, LENGTH = 16M
	E_RAM : ORIGIN = 0X20000000, LENGTH = 64M
}
STACK_SIZE = 1024;
SECTIONS
{
	.vector_table : {
		*(.ivt);
	} >I_RAM

	.init : {
		*(.init);
		_end_init = .; 
	} >E_RAM

	.text : {
		*(.text);
		_end_text = .;
	} >E_RAM
    
}

