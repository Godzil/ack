.define _signal
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _signal
NSIG=32
	.sect .text
_signal:
	move.w	4(sp), d0
	ext.l	d0
	cmp.l	#NSIG,d0
	bcc	1f
	move.l	6(sp),d1
	move.l	d0,a0
	add.l	a0,a0
	add.l	a0,a0
	add.l	#dvect,a0
	move.l	(a0),a1
	move.l	d1,(a0)
	beq	2f
	btst	#0,d1
	bne	2f
	move.l	#enter,d1
2:
	move.l	d0,a0
	move.w	#0x30,d0
	trap	#0
	bcs	3f
	btst	#0,d0
	bne	4f
	move.l	a1,d0
4:
	rts
1:
	move.l	#22,d0
3:
	jmp	cerror

enter:
	movem.l	d0/d1/a0/a1,-(sp)
	move.l	16(sp),a0
	move.l	a0,-(sp)
	add.l	a0,a0
	add.l	a0,a0
	add.l	#dvect,a0
	move.l	(a0),a0
	jsr	(a0)
	add.l	#4,sp
	movem.l	(sp)+,d0/d1/a0/a1
	add.l	#4,sp
	rtr
.sect .bss
dvect: .space 4*NSIG
