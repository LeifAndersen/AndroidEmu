	mov    r0, #0
	mcr    15, 0, r0, cr7, cr10, 4
	mcr    15, 0, r0, cr8, cr7, 0
	mov    pc, lr
	MRC 	 p15, 0, r0, c2, c0, 0
	and r0,r0,#FFFFC000

	
	




