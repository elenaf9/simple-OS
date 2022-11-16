.section .init
.global _start
_start:
    bl main
.Lend:
b .Lend
