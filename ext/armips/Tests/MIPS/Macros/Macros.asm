.n64
.create "output.bin",0

; li

li		a0,0x123456			; standard li
li 		a0,-0x123456		; negative value, lower half MSB set
li		a0,0xFFFFF123		; li that turns into addiu from r0
li		a0,-0xEDD			; should give the same result
li		a0,0x120000			; only lui
li		a0,0x7FF0			; li that turns into ori from r0

li		a0,1.0				; float
li		a0,-1.0
li		a0,325.0

li.s	f8,325.0

; load/store
lb		a0,0x123456
lbu		a0,0xFFFFF123
lh		a0,0x12000000
lhu		a0,0x7FF0
lw		a0,0x7FF0
lwu 	a0,0xFFE0
ld 		a0,0xFFE0
ll		a0,0xFFE0
lld		a0,0xFFE0
lwc1	f8,0xFFE0
ldc1	f8,0xFFE0

.resetdelay

sb		a0,0x123456
sh		a0,0xFFFFF123
sw		a0,0x7FF0
sd		a0,0xFFE0
sc		a0,0xFFE0
scd		a0,0xFFE0
swc1	f8,0xFFE0
sdc1	f8,0xFFE0

lb.u	a0,0x1234			; should turn into nop
lb.l	a0,0x123456			; should just be the second instruction
lb.u	a0,0x123456			; should just be first instruction


; load/store unaligned
ulh		a0,0x1234(a1)
ulhu	a0,0x1234(a1)
ulw		a0,0x1234(a1)
uld		a0,0x1234(a1)
ush		a0,0x1234(a1)
usw		a0,0x1234(a1)
usd		a0,0x1234(a1)

; absolute value

abs 	a0,a1
dabs 	a0,a1

; branch macros

bne		a0, 0x1234, 0x4444
beq		a0, 0x1234, 0x4444
bnel	a0, 0x1234, 0x4444
beql	a0, 0x1234, 0x4444

bne		a0, 0x12345, 0x4444
beq		a0, 0x12345, 0x4444
bnel	a0, 0x12345, 0x4444
beql	a0, 0x12345, 0x4444

blt		a0, a1, 0x4444
bltu	a0, a1, 0x4444
bgt		a0, a1, 0x4444
bgtu	a0, a1, 0x4444
bge		a0, a1, 0x4444
bgeu	a0, a1, 0x4444
ble		a0, a1, 0x4444
bleu	a0, a1, 0x4444
bltl	a0, a1, 0x4444
bltul	a0, a1, 0x4444
bgtl	a0, a1, 0x4444
bgtul	a0, a1, 0x4444
bgel	a0, a1, 0x4444
bgeul	a0, a1, 0x4444
blel	a0, a1, 0x4444
bleul	a0, a1, 0x4444

blt		a0, 0x1234, 0x4444
bltu	a0, 0x1234, 0x4444
bgt		a0, 0x1234, 0x4444
bgtu	a0, 0x1234, 0x4444
bge		a0, 0x1234, 0x4444
bgeu	a0, 0x1234, 0x4444
ble		a0, 0x1234, 0x4444
bleu	a0, 0x1234, 0x4444
bltl	a0, 0x1234, 0x4444
bltul	a0, 0x1234, 0x4444
bgtl	a0, 0x1234, 0x4444
bgtul	a0, 0x1234, 0x4444
bgel	a0, 0x1234, 0x4444
bgeul	a0, 0x1234, 0x4444
blel	a0, 0x1234, 0x4444
bleul	a0, 0x1234, 0x4444

blt		a0, 0x12345, 0x4444
bltu	a0, 0x12345, 0x4444
bgt		a0, 0x12345, 0x4444
bgtu	a0, 0x12345, 0x4444
bge		a0, 0x12345, 0x4444
bgeu	a0, 0x12345, 0x4444
ble		a0, 0x12345, 0x4444
bleu	a0, 0x12345, 0x4444
bltl	a0, 0x12345, 0x4444
bltul	a0, 0x12345, 0x4444
bgtl	a0, 0x12345, 0x4444
bgtul	a0, 0x12345, 0x4444
bgel	a0, 0x12345, 0x4444
bgeul	a0, 0x12345, 0x4444
blel	a0, 0x12345, 0x4444
bleul	a0, 0x12345, 0x4444


; set macros

sne		a0, a1, a2
seq		a0, a1, a2
sge		a0, a1, a2
sgeu	a0, a1, a2
sle		a0, a1, a2
sleu	a0, a1, a2

sne		a0, a1, 0x1234
seq		a0, a1, 0x1234
slt		a0, a1, 0x1234
sltu	a0, a1, 0x1234
sgt		a0, a1, 0x1234
sgtu	a0, a1, 0x1234
sge		a0, a1, 0x1234
sgeu	a0, a1, 0x1234
sle		a0, a1, 0x1234
sleu	a0, a1, 0x1234

sne		a0, a1, 0x12345
seq		a0, a1, 0x12345
slt		a0, a1, 0x12345
sltu	a0, a1, 0x12345
sgt		a0, a1, 0x12345
sgtu	a0, a1, 0x12345
sge		a0, a1, 0x12345
sgeu	a0, a1, 0x12345
sle		a0, a1, 0x12345
sleu	a0, a1, 0x12345

; rotate macros
rol		a0,a1,a2
rol		a0,a1,12
ror		a0,a1,a2
ror		a0,a1,12

.close
