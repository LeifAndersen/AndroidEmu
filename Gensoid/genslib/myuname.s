.global myuname
.global flushcache

myuname:
	swi #0x90007a
	mov pc, lr
	
flushcache:	
	swi #0x9f0002
	mov pc, lr