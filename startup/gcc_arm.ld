OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

ENTRY(Reset_Handler)

MEMORY
{
        RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 32K
        FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 64K
}

SECTIONS
{
        .text : ALIGN(4)
        {
                KEEP(*(.isr_vectors))
                *(.text)
                *(.text.*)
                *(.rodata)
                *(.rodata.*)
                *(.glue_7)
                *(.glue_7t)
                _etext = . ;

        } > FLASH

		.stack (NOLOAD): ALIGN(4)
        {
        	*(.stack)
        	*(.stack.*)
        } > RAM
		
        .data : AT (_etext) ALIGN(4)
        {
                _sdata = . ;
				_sidata = _etext ;
				
                *(.data)
                *(.data.*)
                _edata = . ;

        } > RAM

        .bss : ALIGN(4)
        {
                _sbss = . ;
                *(.bss)
                *(.bss.*)
                *(COMMON)
                _ebss = . ;

        } > RAM
        
        ASSERT(!(SIZEOF(.stack) >= (ORIGIN(RAM)+LENGTH(RAM))), "region ram  overflowed with stack")
}
