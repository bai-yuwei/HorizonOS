[GLOBAL atomic_Exchange]

atomic_Exchange:
  mov ecx, [esp + 4]
  mov eax, [esp + 8]
  xchg [ecx], eax
  ret
