[GLOBAL get_Eip]
get_Eip:
  pop eax
  jmp eax

[GLOBAL get_Ebp]
get_Ebp:
  mov eax, ebp
  ret

[GLOBAL get_Esp]
get_Esp:
  mov eax, esp
  ret

[GLOBAL get_Eax]
get_Eax:
  ret

[GLOBAL get_Eflags]
get_Eflags:
  pushf
  pop eax
  ret

[GLOBAL set_Eflags]
set_Eflags:
  push dword [esp + 4]
  popf
  ret
