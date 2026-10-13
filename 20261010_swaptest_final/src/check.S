	.macro CHECK_FUNC name, padding
	.section	.text.\name,"ax",@progbits
	.align	1
	.global	\name
	.type	\name, @function
\name:
\name\()_LFB3:
	pushm	r0-r3, r4-r7, lr
\name\()_LCFI0:
	sub     sp, 28
\name\()_LCFI1:
	nop
	mov	r0, 0x77              #  s1
	lda.w	r7, \padding
	ld.w	r11, r7[512]	      #  data[0]  (t1, 1. clen)
	mov     r10, r11	        #  t1 = data[0]
	ld.w	r11, r7[512]	      #  data[0]  (t1, 2. clen)
	eor     r10, r10, r11	    #  t1 ^= data[0]
	ld.w	r11, r7[512]	      #  data[0]  (t1, 3. clen)
	eor     r10, r10, r11	    #  t1 ^= data[0]  (vysledek se dale nepouziva)
	stdsp	sp[12], r10	        #  ulozeni t1
	ld.w	r9, r7[512]	        #  data[0]  (t2, 1. clen)
	nop
	bfextu	r8, r0, 0, 8	    #  byte(s1,0) - index (=0x77)
	add     r8, r7, r8 << 2	  #  adresa data[byte(s1,0)]
	ld.w	r10, r8[512]	      #  data[byte(s1,0)] - KRITICKE cteni (t2, 2. clen)
	eor     r9, r10	          #  t2 = data[0] ^ data[byte(s1,0)]
	mov	r0, r9	              #  navratova hodnota = t2
	rjmp	9f
	.cpool
	.align	2
9:
	mov	r12, r0               # navratova hodnota = t2 (r12 neni v pushm/popm)

	sub	sp, -28	# Reset Frame Pointer
	popm	r0-r3, r4-r7, pc
\name\()_LFE3:
	.size	\name, .-\name
	.endm

	.irpc letter, ABCDEFGHIJKLMNOPQRSTUVWX
	.irp digit, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
	CHECK_FUNC check\letter\digit, data_padding_\letter
	.endr
	.endr
