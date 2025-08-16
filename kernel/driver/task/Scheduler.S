[GLOBAL cpu_Idle]
[GLOBAL context_Switch]
[GLOBAL resume_Thread]
[GLOBAL switch_To_User_Mode]

[EXTERN interrupt_Exit]

cpu_Idle:
  hlt
  ret

context_Switch:
  push eax
  push ecx
  push edx
  push ebx
  push ebp
  push esi
  push edi
  mov eax, [esp + 32]
  mov [eax], esp

  ; jump to next thread's kernel stack and recover its execution
  mov eax, [esp + 36]
  mov esp, [eax]

resume_Thread:
  pop edi
  pop esi
  pop ebp
  pop ebx
  pop edx
  pop ecx
  pop eax

  sti
  ret

switch_To_User_Mode:
  add esp, 8
  jmp interrupt_Exit