.386
.model flat,stdcall

CPLUSPLUSFunc PROTO

.DATA
text db 'Hello'

.CODE
AsmFunc proc
	mov eax,10
	invoke CPLUSPLUSFunc
	RETN
AsmFunc ENDP

END