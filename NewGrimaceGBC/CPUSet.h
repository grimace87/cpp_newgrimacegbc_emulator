
switch (instr) {
case 0x00: // nop
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x01: // ld BC, nn
    PC_Inc = 3;
    ClocksSub = 12;
    CPU_B = lsb;
    CPU_C = msb;
    break;

case 0x02: // ld (BC), A
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_B << 8) + (unsigned int)CPU_C, CPU_A);
    break;

case 0x03: // inc BC
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_C++;
    if (CPU_C == 0x00) CPU_B++;
    break;

case 0x04: // inc B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_B += 0x01;
    if (CPU_B == 0x00) CPU_F |= 0x80;
    if ((CPU_B & 0x0f) == 0x00) CPU_F |= 0x20;
    break;

case 0x05: // dec B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    if ((CPU_B & 0x0f) == 0x00) CPU_F |= 0x20;
    CPU_B -= 0x01;
    if (CPU_B == 0x00) CPU_F |= 0x80;
    break;

case 0x06: // ld B, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_B = msb;
    break;

case 0x07: // rlc A (rotate bit 7 to bit 0, and copy bit 7 to carry flag)
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_A & 0x80; // True if bit 7 is set
    CPU_F = 0x00;
    CPU_A = CPU_A << 1;
    if (TempByte != 0) {
        CPU_F |= 0x10; // Set carry
        CPU_A |= 0x01;
    }
    break;

case 0x08: // ld (nn), SP
    PC_Inc = 3;
    ClocksSub = 20;
    Write16 ( ((unsigned int)lsb << 8) + (unsigned int)msb,
        (unsigned char)(CPU_SP & 0x00ff),
        (unsigned char)((CPU_SP >> 8) & 0x00ff)
    );
    break;

case 0x09: // add HL, BC
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_F &= 0x80;
    CPU_L += CPU_C;
    if (CPU_L < CPU_C) {
        CPU_H++;
        if (CPU_H == 0x00)
            CPU_F |= 0x10;
        if ((CPU_H & 0x0f) == 0x00)
            CPU_F |= 0x20;
    }
    CPU_H += CPU_B;
    if (CPU_H < CPU_B) CPU_F |= 0x10;
    if ((CPU_H & 0x0f) < (CPU_B & 0x0f)) CPU_F |= 0x20;
    break;

case 0x0a: // ld A, (BC)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = Read8 (((unsigned int)CPU_B << 8) + (unsigned int)CPU_C);
    break;

case 0x0b: // dec BC
    PC_Inc = 1;
    ClocksSub = 8;
    if (CPU_C == 0x00) CPU_B--;
    CPU_C--;
    break;

case 0x0c: // inc C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_C += 0x01;
    if (CPU_C == 0x00) CPU_F |= 0x80;
    if ((CPU_C & 0x0f) == 0x00) CPU_F |= 0x20;
    break;

case 0x0d: // dec C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    if ((CPU_C & 0x0f) == 0x00) CPU_F |= 0x20;
    CPU_C -= 0x01;
    if (CPU_C == 0x00) CPU_F |= 0x80;
    break;

case 0x0e: // ld C, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_C = msb;
    break;

case 0x0f: // rrc A (8-bit rotation right - bit 0 is moved to carry also)
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_A & 0x01;
    CPU_F = 0x00;
    CPU_A = CPU_A >> 1;
    CPU_A = CPU_A & 0x7f; // Clear msb in case sign bit preserved by compiler
    if (TempByte != 0) {
        CPU_F = 0x10;
        CPU_A |= 0x80;
    }
    break;

case 0x10: // stop
    PC_Inc = 0;
    ClocksSub = 4;
    if ((IOPorts[0x4d] & 0x01) != 0) {
        PC_Inc = 1;
        IOPorts[0x4d] &= 0x80;
        if (IOPorts[0x4d] == 0x00) { // Switch CPU running speed
            IOPorts[0x4d] = 0x80;
            CPU_ClockFreq = GBC_FREQ;
            GPU_ClockFactor = 2;
        }
        else {
            IOPorts[0x4d] = 0x00;
            CPU_ClockFreq = GB_FREQ;
            GPU_ClockFactor = 1;
        }
    }
    break;

case 0x11: // ld DE, nn
    PC_Inc = 3;
    ClocksSub = 12;
    CPU_D = lsb;
    CPU_E = msb;
    break;

case 0x12: // ld (DE), A
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_D << 8) + (unsigned int)CPU_E, CPU_A);
    break;

case 0x13: // inc DE
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_E++;
    if (CPU_E == 0x00) CPU_D++;
    break;

case 0x14: // inc D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_D += 0x01;
    if (CPU_D == 0x00) CPU_F |= 0x80;
    if ((CPU_D & 0x0f) == 0x00) CPU_F |= 0x20;
    break;

case 0x15: // dec D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    if ((CPU_D & 0x0f) == 0x00) CPU_F |= 0x20;
    CPU_D -= 0x01;
    if (CPU_D == 0x00) CPU_F |= 0x80;
    break;

case 0x16: // ld D, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_D = msb;
    break;

case 0x17: // rl A (rotate carry bit to bit 0 of A)
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10; // True if carry flag was set
    CPU_F = 0x00;
    if ((CPU_A & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
    CPU_A = CPU_A << 1;
    if (TempByte != 0) CPU_A |= 0x01; // Copy carry flag to bit 0
    break;

case 0x18: // jr d
    PC_Inc = 0;
    ClocksSub = 12;
    CPU_PC += 2;
    if (msb >= 0x80) CPU_PC -= 256 - (unsigned int)msb;
    else CPU_PC += (unsigned int)msb;
    break;

case 0x19: // add HL, DE
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_F &= 0x80;
    CPU_L += CPU_E;
    if (CPU_L < CPU_E) {
        CPU_H++;
        if (CPU_H == 0x00)
            CPU_F |= 0x10;
        if ((CPU_H & 0x0f) == 0x00)
            CPU_F |= 0x20;
    }
    CPU_H += CPU_D;
    if (CPU_H < CPU_D) CPU_F |= 0x10;
    if ((CPU_H & 0x0f) < (CPU_D & 0x0f)) CPU_F |= 0x20;
    break;

case 0x1a: // ld A, (DE)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = Read8 (((unsigned int)CPU_D << 8) + (unsigned int)CPU_E);
    break;

case 0x1b: // dec DE
    PC_Inc = 1;
    ClocksSub = 8;
    if (CPU_E == 0x00) CPU_D--;
    CPU_E--;
    break;

case 0x1c: // inc E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_E += 0x01;
    if (CPU_E == 0x00) CPU_F |= 0x80;
    if ((CPU_E & 0x0f) == 0x00) CPU_F |= 0x20;
    break;

case 0x1d: // dec E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    if ((CPU_E & 0x0f) == 0x00) CPU_F |= 0x20;
    CPU_E -= 0x01;
    if (CPU_E == 0x00) CPU_F |= 0x80;
    break;

case 0x1e: // ld E, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_E = msb;
    break;

case 0x1f: // rr A (9-bit rotation right of A through carry)
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    if (CPU_A & 0x01) CPU_F = 0x10;
    else CPU_F = 0x00;
    CPU_A = CPU_A >> 1;
    CPU_A = CPU_A & 0x7f;
    if (TempByte != 0x00)
        CPU_A |= 0x80;
        
    break;

case 0x20: // jr NZ, d
    if ((CPU_F & 0x80) != 0) {
        PC_Inc = 2;
        ClocksSub = 8;
    }
    else {
        PC_Inc = 0;
        ClocksSub = 12;
        CPU_PC += 2;
        if (msb >= 0x80) CPU_PC -= 256 - (unsigned int)msb;
        else CPU_PC += (unsigned int)msb;
    }
    break;

case 0x21: // ld HL, nn
    PC_Inc = 3;
    ClocksSub = 12;
    CPU_H = lsb;
    CPU_L = msb;
    break;

case 0x22: // ldi (HL), A
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_A);
    CPU_L++;
    if (CPU_L == 0x00) CPU_H++; // L overflowed into H
    break;

case 0x23: // inc HL
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_L++;
    if (CPU_L == 0x00) CPU_H++;
    break;

case 0x24: // inc H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_H += 0x01;
    if (CPU_H == 0x00) CPU_F |= 0x80;
    if ((CPU_H & 0x0f) == 0x00) CPU_F |= 0x20;
    break;

case 0x25: // dec H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    if ((CPU_H & 0x0f) == 0x00) CPU_F |= 0x20;
    CPU_H -= 0x01;
    if (CPU_H == 0x00) CPU_F |= 0x80;
    break;

case 0x26: // ld H, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_H = msb;
    break;

case 0x27: // daa (Decimal Adjust Accumulator - do BCD correction)
    PC_Inc = 1;
    ClocksSub = 4;
    if ((CPU_F & 0x40) == 0x00) {
        if (((CPU_A & 0x0f) > 0x09) || ((CPU_F & 0x20) != 0x00)) // If lower 4 bits are non-decimal or H is set, add 0x06
            CPU_A += 0x06;
        TempByte = CPU_F & 0x10;
        CPU_F &= 0x40; // Reset C, H and Z flags
        if ((CPU_A > 0x9f) || (TempByte != 0x00)) { // If upper 4 bits are non-decimal or C was set, add 0x60
            CPU_A += 0x60;
            CPU_F |= 0x10; // Sets the C flag if this second addition was needed
        }
    }
    else {
        if (((CPU_A & 0x0f) > 0x09) || ((CPU_F & 0x20) != 0x00)) // If lower 4 bits are non-decimal or H is set, add 0x06
            CPU_A -= 0x06;
        TempByte = CPU_F & 0x10;
        CPU_F &= 0x40; // Reset C, H and Z flags
        if ((CPU_A > 0x9f) || (TempByte != 0x00)) { // If upper 4 bits are non-decimal or C was set, add 0x60
            CPU_A -= 0x60;
            CPU_F |= 0x10; // Sets the C flag if this second addition was needed
        }
    }
    if (CPU_A == 0x00)
        CPU_F |= 0x80;
    break;

case 0x28: // jr Z, d
    if ((CPU_F & 0x80) != 0x00) {
        PC_Inc = 0;
        ClocksSub = 12;
        CPU_PC += 2;
        if (msb >= 0x80) CPU_PC -= 256 - (unsigned int)msb;
        else CPU_PC += (unsigned int)msb;
    }
    else {
        PC_Inc = 2;
        ClocksSub = 8;
    }
    break;

case 0x29: // add HL, HL
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_F &= 0x80;
    if ((CPU_H & 0x80) != 0x00) CPU_F |= 0x10;
    if ((CPU_H & 0x08) != 0x00) CPU_F |= 0x20;
    if ((CPU_L & 0x80) != 0x00) {
        CPU_H += CPU_H + 1;
        CPU_L += CPU_L;
    }
    else {
        CPU_H *= 2;
        CPU_L *= 2;
    }
    break;

case 0x2a: // ldi A, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    CPU_L++;
    if (CPU_L == 0x00) CPU_H++;
    break;

case 0x2b: // dec HL
    PC_Inc = 1;
    ClocksSub = 8;
    if (CPU_L == 0x00) CPU_H--;
    CPU_L--;
    break;

case 0x2c: // inc L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_L += 0x01;
    if (CPU_L == 0x00) CPU_F |= 0x80;
    if ((CPU_L & 0x0f) == 0x00) CPU_F |= 0x20;
    break;

case 0x2d: // dec L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    if ((CPU_L & 0x0f) == 0x00) CPU_F |= 0x20;
    CPU_L -= 0x01;
    if (CPU_L == 0x00) CPU_F |= 0x80;
    break;

case 0x2e: // ld L, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_L = msb;
    break;

case 0x2f: // cpl A (complement - bitwise NOT)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = ~CPU_A;
    CPU_F |= 0x60;
    break;

case 0x30: // jr NC, d
    if ((CPU_F & 0x10) != 0x00) {
        PC_Inc = 2;
        ClocksSub = 8;
    }
    else {
        PC_Inc = 0;
        ClocksSub = 12;
        CPU_PC += 2;
        if (msb >= 0x80) CPU_PC -= 256 - (unsigned int)msb;
        else CPU_PC += (unsigned int)msb;
    }
    break;

case 0x31: // ld SP, nn
    PC_Inc = 3;
    ClocksSub = 12;
    CPU_SP = ((unsigned int)lsb << 8) + (unsigned int)msb;
    break;

case 0x32: // ldd (HL), A
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_A);
    if (CPU_L == 0x00) CPU_H --;
    CPU_L --;
    break;

case 0x33: // inc SP
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_SP++;
    CPU_SP &= 0xffff;
    break;

case 0x34: // inc (HL)
    PC_Inc = 1;
    ClocksSub = 12;
    CPU_F &= 0x10;
    TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
    TempByte = Read8 (TempAddr) + 1;
    if (TempByte == 0x00) CPU_F |= 0x80;
    if ((TempByte & 0x0f) == 0x00) CPU_F |= 0x20;
    Write8 (TempAddr, TempByte);
    break;

case 0x35: // dec (HL)
    PC_Inc = 1;
    ClocksSub = 12;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
    TempByte = Read8 (TempAddr);
    if ((TempByte & 0x0f) == 0x00) CPU_F |= 0x20;
    TempByte--;
    if (TempByte == 0x00) CPU_F |= 0x80;
    Write8 (TempAddr, TempByte);
    break;

case 0x36: // ld (HL), n
    PC_Inc = 2;
    ClocksSub = 12;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, msb);
    break;

case 0x37: // SCF (set carry flag)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x80;
    CPU_F |= 0x10;
    break;

case 0x38: // jr C, n
    if ((CPU_F & 0x10) != 0x00) {
        PC_Inc = 0;
        ClocksSub = 12;
        CPU_PC += 2;
        if (msb >= 0x80) CPU_PC -= 256 - (unsigned int)msb;
        else CPU_PC += (unsigned int)msb;
    }
    else {
        PC_Inc = 2;
        ClocksSub = 8;
    }
    break;

case 0x39: // add HL, SP
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_F &= 0x80;
    TempByte = (unsigned char)(CPU_SP & 0xff);
    CPU_L += TempByte;
    if (CPU_L < TempByte) CPU_H++;
    TempByte = (unsigned char)(CPU_SP >> 8);
    CPU_H += TempByte;
    if (CPU_H < TempByte) CPU_F |= 0x10;
    TempByte = TempByte & 0x0f;
    if ((CPU_H & 0x0f) < TempByte) CPU_F |= 0x20;
    break;

case 0x3a: // ldd A, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    if (CPU_L == 0x00) CPU_H--;
    CPU_L--;
    break;

case 0x3b: // dec SP
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_SP--;
    CPU_SP &= 0xffff;
    break;

case 0x3c: // inc A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A++;
    CPU_F &= 0x10;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if ((CPU_A & 0x0f) == 0x00) CPU_F |= 0x20;
    break;

case 0x3d: // dec A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0x10;
    CPU_F |= 0x40;
    if ((CPU_A & 0x0f) == 0x00) CPU_F |= 0x20;
    CPU_A -= 0x01;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x3e: // ld A, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_A = msb;
    break;

case 0x3f: // ccf (invert carry flags)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F &= 0xb0;
    TempByte = CPU_F & 0x30;
    TempByte = TempByte ^ 0x30;
    CPU_F &= 0x80;
    CPU_F |= TempByte;
    break;

case 0x40: // ld B, B
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x41: // ld B, C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_B = CPU_C;
    break;

case 0x42: // ld B, D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_B = CPU_D;
    break;

case 0x43: // ld B, E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_B = CPU_E;
    break;

case 0x44: // ld B, H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_B = CPU_H;
    break;

case 0x45: // ld B, L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_B = CPU_L;
    break;

case 0x46: // ld B, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_B = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    break;

case 0x47: // ld B, A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_B = CPU_A;
    break;

case 0x48: // ld C, B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_C = CPU_B;
    break;

case 0x49: // ld C, C
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x4a: // ld C, D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_C = CPU_D;
    break;

case 0x4b: // ld C, E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_C = CPU_E;
    break;

case 0x4c: // ld C, H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_C = CPU_H;
    break;

case 0x4d: // ld C, L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_C = CPU_L;
    break;

case 0x4e: // ld C, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_C = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    break;

case 0x4f: // ld C, A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_C = CPU_A;
    break;

case 0x50: // ld D, B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_D = CPU_B;
    break;

case 0x51: // ld D, C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_D = CPU_C;
    break;

case 0x52: // ld D, D
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x53: // ld D, E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_D = CPU_E;
    break;

case 0x54: // ld D, H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_D = CPU_H;
    break;

case 0x55: // ld D, L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_D = CPU_L;
    break;

case 0x56: // ld D, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_D = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    break;

case 0x57: // ld D, A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_D = CPU_A;
    break;

case 0x58: // ld E, B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_E = CPU_B;
    break;

case 0x59: // ld E, C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_E = CPU_C;
    break;

case 0x5a: // ld E, D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_E = CPU_D;
    break;

case 0x5b: // ld E, E
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x5c: // ld E, H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_E = CPU_H;
    break;

case 0x5d: // ld E, L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_E = CPU_L;
    break;

case 0x5e: // ld E, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_E = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    break;

case 0x5f: // ld E, A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_E = CPU_A;
    break;

case 0x60: // ld H, B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_H = CPU_B;
    break;

case 0x61: // ld H, C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_H = CPU_C;
    break;

case 0x62: // ld H, D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_H = CPU_D;
    break;
        
case 0x63: // ld H, E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_H = CPU_E;
    break;

case 0x64: // ld H, H
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x65: // ld H, L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_H = CPU_L;
    break;

case 0x66: // ld H, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_H = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    break;

case 0x67: // ld H, A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_H = CPU_A;
    break;

case 0x68: // ld L, B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_L = CPU_B;
    break;

case 0x69: // ld L, C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_L = CPU_C;
    break;

case 0x6a: // ld L, D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_L = CPU_D;
    break;

case 0x6b: // ld L, E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_L = CPU_E;
    break;

case 0x6c: // ld L, H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_L = CPU_H;
    break;

case 0x6d: // ld L, L
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x6e: // ld L, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_L = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    break;

case 0x6f: // ld L, A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_L = CPU_A;
    break;

case 0x70: // ld (HL), B
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_B);
    break;

case 0x71: // ld (HL), C
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_C);
    break;

case 0x72: // ld (HL), D
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_D);
    break;

case 0x73: // ld (HL), E
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_E);
    break;

case 0x74: // ld (HL), H
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_H);
    break;

case 0x75: // ld (HL), L
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_L);
    break;

case 0x76: // halt (NOTE THAT THIS GETS INTERRUPTED EVEN WHEN INTERRUPTS ARE DISABLED)
    CPU_Halted = TRUE;
    PC_Inc = 0;
    ClocksSub = 4;
    break;

case 0x77: // ld (HL), A
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L, CPU_A);
    break;

case 0x78: // ld A, B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_B;
    break;

case 0x79: // ld A, C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_C;
    break;

case 0x7a: // ld A, D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_D;
    break;

case 0x7b: // ld A, E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_E;
    break;

case 0x7c: // ld A, H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_H;
    break;

case 0x7d: // ld A, L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_L;
    break;

case 0x7e: // ld A, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    break;

case 0x7f: // ld A, A
    PC_Inc = 1;
    ClocksSub = 4;
    break;

case 0x80: // add B (add B to A)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    CPU_A += CPU_B;
    if (CPU_A == 0x00) CPU_F = 0x80;
    if ((CPU_B & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    if (CPU_B > CPU_A) CPU_F |= 0x10;
    break;

case 0x81: // add C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    CPU_A += CPU_C;
    if (CPU_A == 0x00) CPU_F = 0x80;
    if ((CPU_C & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    if (CPU_C > CPU_A) CPU_F |= 0x10;
    break;

case 0x82: // add D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    CPU_A += CPU_D;
    if (CPU_A == 0x00) CPU_F = 0x80;
    if ((CPU_D & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    if (CPU_D > CPU_A) CPU_F |= 0x10;
    break;

case 0x83: // add E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    CPU_A += CPU_E;
    if (CPU_A == 0x00) CPU_F = 0x80;
    if ((CPU_E & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    if (CPU_E > CPU_A) CPU_F |= 0x10;
    break;

case 0x84: // add H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    CPU_A += CPU_H;
    if (CPU_A == 0x00) CPU_F = 0x80;
    if ((CPU_H & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    if (CPU_H > CPU_A) CPU_F |= 0x10;
    break;

case 0x85: // add L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    CPU_A += CPU_L;
    if (CPU_A == 0x00) CPU_F = 0x80;
    if ((CPU_L & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    if (CPU_L > CPU_A) CPU_F |= 0x10;
    break;

case 0x86: // add (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    TempByte = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F = 0x80;
    if (TempByte > CPU_A) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x87: // add A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    if ((CPU_A & 0x08) != 0x00) CPU_F |= 0x20;
    if ((CPU_A & 0x80) != 0x00) CPU_F |= 0x10;
    CPU_A += CPU_A;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x88: // adc A, B (add B + carry to A)
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_B;
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x89: // adc A, C
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_C;
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x8a: // adc A, D
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_D;
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x8b: // adc A, E
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_E;
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x8c: // adc A, H
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_H;
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x8d: // adc A, L
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_L;
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x8e: // adc A, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    TempByte = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x8f: // adc A, A
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_A;
    if ((CPU_F & 0x10) != 0x00) {
        CPU_F = 0x00;
        if (TempByte == 0xff) CPU_F |= 0x10;
        TempByte++;
    }
    else CPU_F = 0x00;
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0x90: // sub B (sub B from A)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if (CPU_B > CPU_A) CPU_F |= 0x10;
    if ((CPU_B & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_B;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0x91: // sub C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if (CPU_C > CPU_A) CPU_F |= 0x10;
    if ((CPU_C & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_C;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0x92: // sub D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if (CPU_D > CPU_A) CPU_F |= 0x10;
    if ((CPU_D & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_D;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0x93: // sub E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if (CPU_E > CPU_A) CPU_F |= 0x10;
    if ((CPU_E & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_E;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0x94: // sub H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if (CPU_H > CPU_A) CPU_F |= 0x10;
    if ((CPU_H & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_H;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0x95: // sub L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if (CPU_L > CPU_A) CPU_F |= 0x10;
    if ((CPU_L & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_L;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0x96: // sub (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    TempByte = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    CPU_F = 0x40;
    if (TempByte > CPU_A) CPU_F |= 0x10;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= TempByte;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0x97: // sub A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0xc0;
    CPU_A = 0x00;
    break;

case 0x98: // sbc A, B (A = A - (B+carry))
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    CPU_F = 0x40;
    if (CPU_B > CPU_A) CPU_F = 0x50;
    if ((CPU_B & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_B;
    if (TempByte != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x99: // sbc A, C
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    CPU_F = 0x40;
    if (CPU_C > CPU_A) CPU_F = 0x50;
    if ((CPU_C & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_C;
    if (TempByte != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x9a: // sbc A, D
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    CPU_F = 0x40;
    if (CPU_D > CPU_A) CPU_F = 0x50;
    if ((CPU_D & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_D;
    if (TempByte != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x9b: // sbc A, E
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    CPU_F = 0x40;
    if (CPU_E > CPU_A) CPU_F = 0x50;
    if ((CPU_E & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_E;
    if (TempByte != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x9c: // sbc A, H
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    CPU_F = 0x40;
    if (CPU_H > CPU_A) CPU_F = 0x50;
    if ((CPU_H & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_H;
    if (TempByte != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x9d: // sbc A, L
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    CPU_F = 0x40;
    if (CPU_L > CPU_A) CPU_F = 0x50;
    if ((CPU_L & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= CPU_L;
    if (TempByte != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x9e: // sbc A, (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    TempByte = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    TempByte2 = CPU_F & 0x10;
    CPU_F = 0x40;
    if (TempByte > CPU_A) CPU_F = 0x50;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= TempByte;
    if (TempByte2 != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0x9f: // sbc A, A
    PC_Inc = 1;
    ClocksSub = 4;
    TempByte = CPU_F & 0x10;
    CPU_F = 0x40;
    CPU_A = 0;
    if (TempByte != 0x00) {
        if (CPU_A == 0) {
            CPU_A = 0xff;
            CPU_F = 0x70;
        }
        else CPU_A--;
    }
    if (CPU_A == 0x00) CPU_F |= 0x80;
    break;

case 0xa0: // and B (and B against A)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A & CPU_B;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa1: // and C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A & CPU_C;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa2: // and D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A & CPU_D;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa3: // and E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A & CPU_E;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa4: // and H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A & CPU_H;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa5: // and L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A & CPU_L;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa6: // and (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = CPU_A & Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa7: // and A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xa8: // xor B (A = A XOR B)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A ^ CPU_B;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xa9: // xor C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A ^ CPU_C;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xaa: // xor D)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A ^ CPU_D;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xab: // xor E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A ^ CPU_E;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xac: // xor H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A ^ CPU_H;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xad: // xor L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A ^ CPU_L;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xae: // xor (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = CPU_A ^ Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xaf: // xor A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = 0x00;
    CPU_F = 0x80;
    break;

case 0xb0: // or B (or B against A)
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A | CPU_B;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb1: // or C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A | CPU_C;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb2: // or D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A | CPU_D;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb3: // or E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A | CPU_E;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb4: // or H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A | CPU_H;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb5: // or L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_A = CPU_A | CPU_L;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb6: // or (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = CPU_A | Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb7: // or A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xb8: // cp B
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if ((CPU_B & 0x0f) > (CPU_A & 0x0f)) CPU_F = 0x60;
    if (CPU_B > CPU_A) CPU_F |= 0x10;
    if (CPU_A == CPU_B) CPU_F |= 0x80;
    break;

case 0xb9: // cp C
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if ((CPU_C & 0x0f) > (CPU_A & 0x0f)) CPU_F = 0x60;
    if (CPU_C > CPU_A) CPU_F |= 0x10;
    if (CPU_A == CPU_C) CPU_F |= 0x80;
    break;

case 0xba: // cp D
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if ((CPU_D & 0x0f) > (CPU_A & 0x0f)) CPU_F = 0x60;
    if (CPU_D > CPU_A) CPU_F |= 0x10;
    if (CPU_A == CPU_D) CPU_F |= 0x80;
    break;

case 0xbb: // cp E
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if ((CPU_E & 0x0f) > (CPU_A & 0x0f)) CPU_F = 0x60;
    if (CPU_E > CPU_A) CPU_F |= 0x10;
    if (CPU_A == CPU_E) CPU_F |= 0x80;
    break;

case 0xbc: // cp H
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if ((CPU_H & 0x0f) > (CPU_A & 0x0f)) CPU_F = 0x60;
    if (CPU_H > CPU_A) CPU_F |= 0x10;
    if (CPU_A == CPU_H) CPU_F |= 0x80;
    break;

case 0xbd: // cp L
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0x40;
    if ((CPU_L & 0x0f) > (CPU_A & 0x0f)) CPU_F = 0x60;
    if (CPU_L > CPU_A) CPU_F |= 0x10;
    if (CPU_A == CPU_L) CPU_F |= 0x80;
    break;

case 0xbe: // cp (HL)
    PC_Inc = 1;
    ClocksSub = 8;
    TempByte = Read8 (((unsigned int)CPU_H << 8) + (unsigned int)CPU_L);
    CPU_F = 0x40;
    if (TempByte > CPU_A) CPU_F |= 0x10;
    if (CPU_A == TempByte) CPU_F |= 0x80;
    if ((TempByte & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    break;

case 0xbf: // cp A
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_F = 0xc0;
    break;

case 0xc0: // ret NZ
    if ((CPU_F & 0x80) != 0x00) {
        PC_Inc = 1;
        ClocksSub = 8;
    }
    else {
        if (TotalBreakEnables > 0) BreakLastCallReturned = 1;
        PC_Inc = 0;
        ClocksSub = 20;
        Read16 (CPU_SP, &TempByte, &TempByte2);
        CPU_PC = ((unsigned int)TempByte2 << 8) + (unsigned int)TempByte;
        CPU_SP += 2;
    }
    break;

case 0xc1: // pop BC
    PC_Inc = 1;
    ClocksSub = 12;
    Read16 (CPU_SP, &CPU_C, &CPU_B);
    CPU_SP += 2;
    break;

case 0xc2: // j NZ, nn
    if ((CPU_F & 0x80) != 0x00) {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    else {
        PC_Inc = 0;
        ClocksSub = 16;
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    break;

case 0xc3: // jump to nn
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    break;

case 0xc4: // call NZ, nn
    if ((CPU_F & 0x80) != 0x00) {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    else {
        if (TotalBreakEnables > 0) {
            BreakLastCallAt = CPU_PC;
            BreakLastCallTo = ((unsigned int)lsb << 8) + (unsigned int)msb;
            BreakLastCallReturned = 0;
        }
        PC_Inc = 0;
        ClocksSub = 24;
        CPU_SP -= 2;
        CPU_PC += 3;
        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    break;

case 0xc5: // push BC
    PC_Inc = 1;
    ClocksSub = 16;
    CPU_SP -= 2;
    Write16 (CPU_SP, CPU_C, CPU_B);
    break;

case 0xc6: // add A, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_A += msb;
    CPU_F = 0x00;
    if (CPU_A == 0) CPU_F = 0x80;
    if (CPU_A < msb) CPU_F |= 0x10;
    if ((CPU_A & 0x0f) < (msb & 0x0f)) CPU_F |= 0x20;
    break;

case 0xc7: // rst 0 (call routine at 0x0000)
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x00;
    break;

case 0xc8: // ret Z
    if ((CPU_F & 0x80) != 0x00) {
        if (TotalBreakEnables > 0) BreakLastCallReturned = 1;
        PC_Inc = 0;
        ClocksSub = 20;
        Read16 (CPU_SP, &TempByte, &TempByte2);
        CPU_PC = ((unsigned int)TempByte2 << 8) + (unsigned int)TempByte;
        CPU_SP += 2;
    }
    else {
        PC_Inc = 1;
        ClocksSub = 8;
    }
    break;

case 0xc9: // return
    if (TotalBreakEnables > 0) BreakLastCallReturned = 1;
    PC_Inc = 0;
    ClocksSub = 16;
    Read16 (CPU_SP, &TempByte, &TempByte2);
    CPU_PC = ((unsigned int)TempByte2 << 8) + (unsigned int)TempByte;
    CPU_SP += 2;
    break;

case 0xca: // j Z, nn
    if ((CPU_F & 0x80) != 0x00) {
        PC_Inc = 0;
        ClocksSub = 16;
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    else {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    break;

case 0xcb: // extended instructions
    switch (msb) {

    case 0x00: // rlc B
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_B & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        CPU_B = CPU_B << 1;
        if (TempByte != 0) {
            CPU_B |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (CPU_B == 0x00) CPU_F |= 0x80;
        break;

    case 0x01: // rlc C
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_C & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        CPU_C = CPU_C << 1;
        if (TempByte != 0) {
            CPU_C |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (CPU_C == 0x00) CPU_F |= 0x80;
        break;

    case 0x02: // rlc D
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_D & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        CPU_D = CPU_D << 1;
        if (TempByte != 0) {
            CPU_D |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (CPU_D == 0x00) CPU_F |= 0x80;
        break;

    case 0x03: // rlc E
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_E & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        CPU_E = CPU_E << 1;
        if (TempByte != 0) {
            CPU_E |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (CPU_E == 0x00) CPU_F |= 0x80;
        break;

    case 0x04: // rlc H
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_H & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        CPU_H = CPU_H << 1;
        if (TempByte != 0) {
            CPU_H |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (CPU_H == 0x00) CPU_F |= 0x80;
        break;

    case 0x05: // rlc L
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_L & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        CPU_L = CPU_L << 1;
        if (TempByte != 0) {
            CPU_L |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (CPU_L == 0x00) CPU_F |= 0x80;
        break;

    case 0x06: // rlc (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte2 = Read8 (TempAddr);
        TempByte = TempByte2 & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        TempByte2 = TempByte2 << 1;
        if (TempByte != 0) {
            TempByte2 |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (TempByte2 == 0x00) CPU_F |= 0x80;
        Write8 (TempAddr, TempByte2);
        break;

    case 0x07: // rlc A
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_A & 0x80; // True if bit 7 is set
        CPU_F = 0x00; // Reset all other flags
        CPU_A = CPU_A << 1;
        if (TempByte != 0) {
            CPU_A |= 0x01;
            CPU_F = 0x10; // Set carry
        }
        if (CPU_A == 0x00) CPU_F |= 0x80;
        break;

    case 0x08: // rrc B
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_B & 0x01;
        CPU_F = 0x00;
        CPU_B = CPU_B >> 1;
        CPU_B &= 0x7f;
        if (TempByte != 0) {
            CPU_F = 0x10;
            CPU_B |= 0x80;
        }
        if (CPU_B == 0x00) CPU_F |= 0x80;
        break;

    case 0x09: // rrc C
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_C & 0x01;
        CPU_F = 0x00;
        CPU_C = CPU_C >> 1;
        CPU_C &= 0x7f;
        if (TempByte != 0) {
            CPU_F = 0x10;
            CPU_C |= 0x80;
        }
        if (CPU_C == 0x00) CPU_F |= 0x80;
        break;

    case 0x0a: // rrc D
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_D & 0x01;
        CPU_F = 0x00;
        CPU_D = CPU_D >> 1;
        CPU_D &= 0x7f;
        if (TempByte != 0) {
            CPU_F = 0x10;
            CPU_D |= 0x80;
        }
        if (CPU_D == 0x00) CPU_F |= 0x80;
        break;

    case 0x0b: // rrc E
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_E & 0x01;
        CPU_F = 0x00;
        CPU_E = CPU_E >> 1;
        CPU_E &= 0x7f;
        if (TempByte != 0) {
            CPU_F = 0x10;
            CPU_E |= 0x80;
        }
        if (CPU_E == 0x00) CPU_F |= 0x80;
        break;

    case 0x0c: // rrc H
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_H & 0x01;
        CPU_F = 0x00;
        CPU_H = CPU_H >> 1;
        CPU_H &= 0x7f;
        if (TempByte != 0) {
            CPU_F = 0x10;
            CPU_H |= 0x80;
        }
        if (CPU_H == 0x00) CPU_F |= 0x80;
        break;

    case 0x0d: // rrc L
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_L & 0x01;
        CPU_F = 0x00;
        CPU_L = CPU_L >> 1;
        CPU_L &= 0x7f;
        if (TempByte != 0) {
            CPU_F = 0x10;
            CPU_L |= 0x80;
        }
        if (CPU_L == 0x00) CPU_F |= 0x80;
        break;

    case 0x0e: // rrc (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte = Read8 (TempAddr);
        TempByte2 = TempByte & 0x01;
        CPU_F = 0x00;
        TempByte = TempByte >> 1;
        TempByte &= 0x7f;
        if (TempByte2 != 0) {
            CPU_F = 0x10;
            TempByte |= 0x80;
        }
        if (TempByte == 0x00) CPU_F |= 0x80;
        Write8 (TempAddr, TempByte);
        break;

    case 0x0f: // rrc A
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_A & 0x01;
        CPU_F = 0x00;
        CPU_A = CPU_A >> 1;
        CPU_A &= 0x7f;
        if (TempByte != 0) {
            CPU_F = 0x10;
            CPU_A |= 0x80;
        }
        if (CPU_A == 0x00) CPU_F |= 0x80;
        break;

    case 0x10: // rl B (rotate carry bit to bit 0 of B)
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((CPU_B & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        CPU_B = CPU_B << 1;
        if (TempByte != 0) CPU_B |= 0x01; // Copy carry flag to bit 0
        if (CPU_B == 0x00) CPU_F |= 0x80;
        break;

    case 0x11: // rl C
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((CPU_C & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        CPU_C = CPU_C << 1;
        if (TempByte != 0) CPU_C |= 0x01; // Copy carry flag to bit 0
        if (CPU_C == 0x00) CPU_F |= 0x80;
        break;

    case 0x12: // rl D
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((CPU_D & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        CPU_D = CPU_D << 1;
        if (TempByte != 0) CPU_D |= 0x01; // Copy carry flag to bit 0
        if (CPU_D == 0x00) CPU_F |= 0x80;
        break;

    case 0x13: // rl E
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((CPU_E & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        CPU_E = CPU_E << 1;
        if (TempByte != 0) CPU_E |= 0x01; // Copy carry flag to bit 0
        if (CPU_E == 0x00) CPU_F |= 0x80;
        break;

    case 0x14: // rl H
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((CPU_H & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        CPU_H = CPU_H << 1;
        if (TempByte != 0) CPU_H |= 0x01; // Copy carry flag to bit 0
        if (CPU_H == 0x00) CPU_F |= 0x80;
        break;

    case 0x15: // rl L
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((CPU_L & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        CPU_L = CPU_L << 1;
        if (TempByte != 0) CPU_L |= 0x01; // Copy carry flag to bit 0
        if (CPU_L == 0x00) CPU_F |= 0x80;
        break;

    case 0x16: // rl (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte2 = Read8 (TempAddr);
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((TempByte2 & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        TempByte2 = TempByte2 << 1;
        if (TempByte != 0) TempByte2 |= 0x01; // Copy carry flag to bit 0
        if (TempByte2 == 0x00) CPU_F |= 0x80;
        Write8 (TempAddr, TempByte2);
        break;

    case 0x17: // rl A
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_F & 0x10; // True if carry flag was set
        if ((CPU_A & 0x80) != 0) CPU_F = 0x10; // Copy bit 7 to carry bit
        else CPU_F = 0x00;
        CPU_A = CPU_A << 1;
        if (TempByte != 0) CPU_A |= 0x01; // Copy carry flag to bit 0
        if (CPU_A == 0x00) CPU_F |= 0x80;
        break;

    case 0x18: // rr B (9-bit rotation incl carry bit)
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_B & 0x01;
        TempByte2 = CPU_F & 0x10;
        CPU_B = CPU_B >> 1;
        CPU_B = CPU_B & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) CPU_B |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (CPU_B == 0x00) CPU_F |= 0x80;
        break;

    case 0x19: // rr C
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_C & 0x01;
        TempByte2 = CPU_F & 0x10;
        CPU_C = CPU_C >> 1;
        CPU_C = CPU_C & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) CPU_C |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (CPU_C == 0x00) CPU_F |= 0x80;
        break;

    case 0x1a: // rr D
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_D & 0x01;
        TempByte2 = CPU_F & 0x10;
        CPU_D = CPU_D >> 1;
        CPU_D = CPU_D & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) CPU_D |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (CPU_D == 0x00) CPU_F |= 0x80;
        break;

    case 0x1b: // rr E
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_E & 0x01;
        TempByte2 = CPU_F & 0x10;
        CPU_E = CPU_E >> 1;
        CPU_E = CPU_E & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) CPU_E |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (CPU_E == 0x00) CPU_F |= 0x80;
        break;

    case 0x1c: // rr H
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_H & 0x01;
        TempByte2 = CPU_F & 0x10;
        CPU_H = CPU_H >> 1;
        CPU_H = CPU_H & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) CPU_H |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (CPU_H == 0x00) CPU_F |= 0x80;
        break;

    case 0x1d: // rr L
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_L & 0x01;
        TempByte2 = CPU_F & 0x10;
        CPU_L = CPU_L >> 1;
        CPU_L = CPU_L & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) CPU_L |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (CPU_L == 0x00) CPU_F |= 0x80;
        break;

    case 0x1e: // rr (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte3 = Read8 (TempAddr);
        TempByte = TempByte3 & 0x01;
        TempByte2 = CPU_F & 0x10;
        TempByte3 = TempByte3 >> 1;
        TempByte3 = TempByte3 & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) TempByte3 |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (TempByte3 == 0x00) CPU_F |= 0x80;
        Write8 (TempAddr, TempByte3);
        break;

    case 0x1f: // rr A
        PC_Inc = 2;
        ClocksSub = 8;
        TempByte = CPU_A & 0x01;
        TempByte2 = CPU_F & 0x10;
        CPU_A = CPU_A >> 1;
        CPU_A = CPU_A & 0x7f;
        CPU_F = 0x00;
        if (TempByte2 != 0x00) CPU_A |= 0x80;
        if (TempByte != 0x00) CPU_F |= 0x10;
        if (CPU_A == 0x00) CPU_F |= 0x80;
        break;

    case 0x20: // sla B (shift B left arithmetically)
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_B & 0x80) != 0x00) CPU_F = 0x10;
        CPU_B = CPU_B << 1;
        if (CPU_B == 0x00) CPU_F |= 0x80;
        break;

    case 0x21: // sla C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_C & 0x80) != 0x00) CPU_F = 0x10;
        CPU_C = CPU_C << 1;
        if (CPU_C == 0x00) CPU_F |= 0x80;
        break;

    case 0x22: // sla D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_D & 0x80) != 0x00) CPU_F = 0x10;
        CPU_D = CPU_D << 1;
        if (CPU_D == 0x00) CPU_F |= 0x80;
        break;

    case 0x23: // sla E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_E & 0x80) != 0x00) CPU_F = 0x10;
        CPU_E = CPU_E << 1;
        if (CPU_E == 0x00) CPU_F |= 0x80;
        break;

    case 0x24: // sla H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_H & 0x80) != 0x00) CPU_F = 0x10;
        CPU_H = CPU_H << 1;
        if (CPU_H == 0x00) CPU_F |= 0x80;
        break;

    case 0x25: // sla L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_L & 0x80) != 0x00) CPU_F = 0x10;
        CPU_L = CPU_L << 1;
        if (CPU_L == 0x00) CPU_F |= 0x80;
        break;

    case 0x26: // sla (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte = Read8 (TempAddr);
        CPU_F = 0x00;
        if ((TempByte & 0x80) != 0x00) CPU_F = 0x10;
        TempByte = TempByte << 1;
        if (TempByte == 0x00) CPU_F |= 0x80;
        Write8 (TempAddr, TempByte);
        break;

    case 0x27: // sla A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_A & 0x80) != 0x00) CPU_F = 0x10;
        CPU_A = CPU_A << 1;
        if (CPU_A == 0x00) CPU_F |= 0x80;
        break;

    case 0x28: // sra B (shift B right arithmetically - preserve sign bit)
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_B & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = CPU_B & 0x80;
        CPU_B = CPU_B >> 1;
        CPU_B |= TempByte;
        if (CPU_B == 0x00) CPU_F |= 0x80;
        break;

    case 0x29: // sra C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_C & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = CPU_C & 0x80;
        CPU_C = CPU_C >> 1;
        CPU_C |= TempByte;
        if (CPU_C == 0x00) CPU_F |= 0x80;
        break;

    case 0x2a: // sra D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_D & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = CPU_D & 0x80;
        CPU_D = CPU_D >> 1;
        CPU_D |= TempByte;
        if (CPU_D == 0x00) CPU_F |= 0x80;
        break;

    case 0x2b: // sra E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_E & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = CPU_E & 0x80;
        CPU_E = CPU_E >> 1;
        CPU_E |= TempByte;
        if (CPU_E == 0x00) CPU_F |= 0x80;
        break;

    case 0x2c: // sra H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_H & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = CPU_H & 0x80;
        CPU_H = CPU_H >> 1;
        CPU_H |= TempByte;
        if (CPU_H == 0x00) CPU_F |= 0x80;
        break;

    case 0x2d: // sra L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_L & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = CPU_L & 0x80;
        CPU_L = CPU_L >> 1;
        CPU_L |= TempByte;
        if (CPU_L == 0x00) CPU_F |= 0x80;
        break;

    case 0x2e: // sra (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        CPU_F = 0x00;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte = Read8 (TempAddr);
        if ((TempByte & 0x01) != 0x00) CPU_F = 0x10;
        TempByte2 = TempByte & 0x80;
        TempByte = TempByte >> 1;
        TempByte |= TempByte2;
        if (TempByte == 0x00) CPU_F |= 0x80;
        Write8 (TempAddr, TempByte);
        break;

    case 0x2f: // sra A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_A & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = CPU_A & 0x80;
        CPU_A = CPU_A >> 1;
        CPU_A |= TempByte;
        if (CPU_A == 0x00) CPU_F |= 0x80;
        break;

    case 0x30: // swap B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        TempByte = (CPU_B << 4);
        CPU_B = CPU_B >> 4;
        CPU_B &= 0x0f;
        CPU_B |= TempByte;
        if (CPU_B == 0x00) CPU_F = 0x80;
        break;

    case 0x31: // swap C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        TempByte = (CPU_C << 4);
        CPU_C = CPU_C >> 4;
        CPU_C &= 0x0f;
        CPU_C |= TempByte;
        if (CPU_C == 0x00) CPU_F = 0x80;
        break;

    case 0x32: // swap D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        TempByte = (CPU_D << 4);
        CPU_D = CPU_D >> 4;
        CPU_D &= 0x0f;
        CPU_D |= TempByte;
        if (CPU_D == 0x00) CPU_F = 0x80;
        break;

    case 0x33: // swap E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        TempByte = (CPU_E << 4);
        CPU_E = CPU_E >> 4;
        CPU_E &= 0x0f;
        CPU_E |= TempByte;
        if (CPU_E == 0x00) CPU_F = 0x80;
        break;

    case 0x34: // swap H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        TempByte = (CPU_H << 4);
        CPU_H = CPU_H >> 4;
        CPU_H &= 0x0f;
        CPU_H |= TempByte;
        if (CPU_H == 0x00) CPU_F = 0x80;
        break;

    case 0x35: // swap L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        TempByte = (CPU_L << 4);
        CPU_L = CPU_L >> 4;
        CPU_L &= 0x0f;
        CPU_L |= TempByte;
        if (CPU_L == 0x00) CPU_F = 0x80;
        break;

    case 0x36: // swap (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        CPU_F = 0x00;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte = Read8 (TempAddr);
        TempByte2 = (TempByte << 4);
        TempByte = TempByte >> 4;
        TempByte &= 0x0f;
        TempByte |= TempByte2;
        if (TempByte == 0x00) CPU_F = 0x80;
        Write8 (TempAddr, TempByte);
        break;

    case 0x37: // swap A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        TempByte = (CPU_A << 4);
        CPU_A = CPU_A >> 4;
        CPU_A &= 0x0f;
        CPU_A |= TempByte;
        if (CPU_A == 0x00) CPU_F = 0x80;
        break;

    case 0x38: // srl B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_B & 0x01) != 0x00) CPU_F = 0x10;
        CPU_B = CPU_B >> 1;
        CPU_B &= 0x7f;
        if (CPU_B == 0x00) CPU_F |= 0x80;
        break;

    case 0x39: // srl C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_C & 0x01) != 0x00) CPU_F = 0x10;
        CPU_C = CPU_C >> 1;
        CPU_C &= 0x7f;
        if (CPU_C == 0x00) CPU_F |= 0x80;
        break;

    case 0x3a: // srl D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_D & 0x01) != 0x00) CPU_F = 0x10;
        CPU_D = CPU_D >> 1;
        CPU_D &= 0x7f;
        if (CPU_D == 0x00) CPU_F |= 0x80;
        break;

    case 0x3b: // srl E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_E & 0x01) != 0x00) CPU_F = 0x10;
        CPU_E = CPU_E >> 1;
        CPU_E &= 0x7f;
        if (CPU_E == 0x00) CPU_F |= 0x80;
        break;

    case 0x3c: // srl H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_H & 0x01) != 0x00) CPU_F = 0x10;
        CPU_H = CPU_H >> 1;
        CPU_H &= 0x7f;
        if (CPU_H == 0x00) CPU_F |= 0x80;
        break;

    case 0x3d: // srl L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_L & 0x01) != 0x00) CPU_F = 0x10;
        CPU_L = CPU_L >> 1;
        CPU_L &= 0x7f;
        if (CPU_L == 0x00) CPU_F |= 0x80;
        break;

    case 0x3e: // srl (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        CPU_F = 0x00;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        TempByte = Read8 (TempAddr);
        if ((TempByte & 0x01) != 0x00) CPU_F = 0x10;
        TempByte = TempByte >> 1;
        TempByte &= 0x7f;
        if (TempByte == 0x00) CPU_F |= 0x80;
        Write8 (TempAddr, TempByte);
        break;

    case 0x3f: // srl A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F = 0x00;
        if ((CPU_A & 0x01) != 0x00) CPU_F = 0x10;
        CPU_A = CPU_A >> 1;
        CPU_A &= 0x7f;
        if (CPU_A == 0x00) CPU_F |= 0x80;
        break;

    case 0x40: // Test bit 0 of B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x41: // Test bit 0 of C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x42: // Test bit 0 of D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x43: // Test bit 0 of E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x44: // Test bit 0 of H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x45: // Test bit 0 of L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x46: // Test bit 0 of (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x47: // Test bit 0 of A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x01) == 0x00) CPU_F |= 0x80;
        break;

    case 0x48: // bit 1, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x49: // bit 1, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x4a: // bit 1, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x4b: // bit 1, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x4c: // bit 1, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x4d: // bit 1, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x4e: // bit 1, (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x4f: // bit 1, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x02) == 0x00) CPU_F |= 0x80;
        break;

    case 0x50: // bit 2, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x51: // bit 2, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x52: // bit 2, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x53: // bit 2, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x54: // bit 2, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x55: // bit 2, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x56: // bit 2, (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x57: // bit 2, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x04) == 0x00) CPU_F |= 0x80;
        break;

    case 0x58: // bit 3, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x59: // bit 3, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x5a: // bit 3, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x5b: // bit 3, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x5c: // bit 3, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x5d: // bit 3, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x5e: // bit 3, (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x5f: // bit 3, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x08) == 0x00) CPU_F |= 0x80;
        break;

    case 0x60: // bit 4, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x61: // bit 4, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x62: // bit 4, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x63: // bit 4, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x64: // bit 4, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x65: // bit 4, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x66: // bit 4, (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x67: // bit 4, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x10) == 0x00) CPU_F |= 0x80;
        break;

    case 0x68: // bit 5, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x69: // bit 5, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x6a: // bit 5, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x6b: // bit 5, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x6c: // bit 5, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x6d: // bit 5, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x6e: // bit 5, (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x6f: // bit 5, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x20) == 0x00) CPU_F |= 0x80;
        break;

    case 0x70: // bit 6, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x71: // bit 6, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x72: // bit 6, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x73: // bit 6, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x74: // bit 6, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x75: // bit 6, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x76: // bit 6, (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x77: // bit 6, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x40) == 0x00) CPU_F |= 0x80;
        break;

    case 0x78: // bit 7, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_B & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x79: // bit 7, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_C & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x7a: // bit 7, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_D & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x7b: // bit 7, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_E & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x7c: // bit 7, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_H & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x7d: // bit 7, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_L & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x7e: // bit 7, (HL)
        PC_Inc = 2;
        ClocksSub = 12;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((Read8(((unsigned int)CPU_H << 8) + (unsigned int)CPU_L) & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x7f: // bit 7, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_F &= 0x30;
        CPU_F |= 0x20;
        if ((CPU_A & 0x80) == 0x00) CPU_F |= 0x80;
        break;

    case 0x80: // res 0, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0xfe;
        break;

    case 0x81: // res 0, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0xfe;
        break;

    case 0x82: // res 0, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0xfe;
        break;

    case 0x83: // res 0, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0xfe;
        break;

    case 0x84: // res 0, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0xfe;
        break;

    case 0x85: // res 0, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0xfe;
        break;

    case 0x86: // res 0, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0xfe);
        break;

    case 0x87: // res 0, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0xfe;
        break;

    case 0x88: // res 1, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0xfd;
        break;

    case 0x89: // res 1, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0xfd;
        break;

    case 0x8a: // res 1, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0xfd;
        break;

    case 0x8b: // res 1, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0xfd;
        break;

    case 0x8c: // res 1, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0xfd;
        break;

    case 0x8d: // res 1, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0xfd;
        break;

    case 0x8e: // res 1, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0xfd);
        break;

    case 0x8f: // res 1, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0xfd;
        break;

    case 0x90: // res 2, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0xfb;
        break;

    case 0x91: // res 2, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0xfb;
        break;

    case 0x92: // res 2, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0xfb;
        break;

    case 0x93: // res 2, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0xfb;
        break;

    case 0x94: // res 2, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0xfb;
        break;

    case 0x95: // res 2, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0xfb;
        break;

    case 0x96: // res 2, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0xfb);
        break;

    case 0x97: // res 2, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0xfb;
        break;

    case 0x98: // res 3, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0xf7;
        break;

    case 0x99: // res 3, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0xf7;
        break;

    case 0x9a: // res 3, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0xf7;
        break;

    case 0x9b: // res 3, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0xf7;
        break;

    case 0x9c: // res 3, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0xf7;
        break;

    case 0x9d: // res 3, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0xf7;
        break;

    case 0x9e: // res 3, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0xf7);
        break;

    case 0x9f: // res 3, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0xf7;
        break;

    case 0xa0: // res 4, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0xef;
        break;

    case 0xa1: // res 4, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0xef;
        break;

    case 0xa2: // res 4, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0xef;
        break;

    case 0xa3: // res 4, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0xef;
        break;

    case 0xa4: // res 4, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0xef;
        break;

    case 0xa5: // res 4, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0xef;
        break;

    case 0xa6: // res 4, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0xef);
        break;

    case 0xa7: // res 4, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0xef;
        break;

    case 0xa8: // res 5, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0xdf;
        break;

    case 0xa9: // res 5, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0xdf;
        break;

    case 0xaa: // res 5, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0xdf;
        break;

    case 0xab: // res 5, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0xdf;
        break;

    case 0xac: // res 5, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0xdf;
        break;

    case 0xad: // res 5, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0xdf;
        break;

    case 0xae: // res 5, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0xdf);
        break;

    case 0xaf: // res 5, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0xdf;
        break;

    case 0xb0: // res 6, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0xbf;
        break;

    case 0xb1: // res 6, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0xbf;
        break;

    case 0xb2: // res 6, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0xbf;
        break;

    case 0xb3: // res 6, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0xbf;
        break;

    case 0xb4: // res 6, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0xbf;
        break;

    case 0xb5: // res 6, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0xbf;
        break;

    case 0xb6: // res 6, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0xbf);
        break;

    case 0xb7: // res 6, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0xbf;
        break;

    case 0xb8: // res 7, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B &= 0x7f;
        break;

    case 0xb9: // res 7, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C &= 0x7f;
        break;

    case 0xba: // res 7, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D &= 0x7f;
        break;

    case 0xbb: // res 7, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E &= 0x7f;
        break;

    case 0xbc: // res 7, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H &= 0x7f;
        break;

    case 0xbd: // res 7, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L &= 0x7f;
        break;

    case 0xbe: // res 7, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) & 0x7f);
        break;

    case 0xbf: // res 7, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A &= 0x7f;
        break;

    case 0xc0: // set 0, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x01;
        break;

    case 0xc1: // set 0, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x01;
        break;

    case 0xc2: // set 0, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x01;
        break;

    case 0xc3: // set 0, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x01;
        break;

    case 0xc4: // set 0, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x01;
        break;

    case 0xc5: // set 0, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x01;
        break;

    case 0xc6: // set 0, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x01);
        break;

    case 0xc7: // set 0, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x01;
        break;

    case 0xc8: // set 1, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x02;
        break;

    case 0xc9: // set 1, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x02;
        break;

    case 0xca: // set 1, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x02;
        break;

    case 0xcb: // set 1, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x02;
        break;

    case 0xcc: // set 1, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x02;
        break;

    case 0xcd: // set 1, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x02;
        break;

    case 0xce: // set 1, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x02);
        break;

    case 0xcf: // set 1, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x02;
        break;

    case 0xd0: // set 2, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x04;
        break;

    case 0xd1: // set 2, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x04;
        break;

    case 0xd2: // set 2, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x04;
        break;

    case 0xd3: // set 2, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x04;
        break;

    case 0xd4: // set 2, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x04;
        break;

    case 0xd5: // set 2, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x04;
        break;

    case 0xd6: // set 2, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x04);
        break;

    case 0xd7: // set 2, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x04;
        break;

    case 0xd8: // set 3, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x08;
        break;

    case 0xd9: // set 3, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x08;
        break;

    case 0xda: // set 3, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x08;
        break;

    case 0xdb: // set 3, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x08;
        break;

    case 0xdc: // set 3, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x08;
        break;

    case 0xdd: // set 3, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x08;
        break;

    case 0xde: // set 3, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x08);
        break;

    case 0xdf: // set 3, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x08;
        break;

    case 0xe0: // set 4, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x10;
        break;

    case 0xe1: // set 4, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x10;
        break;

    case 0xe2: // set 4, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x10;
        break;

    case 0xe3: // set 4, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x10;
        break;

    case 0xe4: // set 4, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x10;
        break;

    case 0xe5: // set 4, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x10;
        break;

    case 0xe6: // set 4, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x10);
        break;

    case 0xe7: // set 4, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x10;
        break;

    case 0xe8: // set 5, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x20;
        break;

    case 0xe9: // set 5, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x20;
        break;

    case 0xea: // set 5, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x20;
        break;

    case 0xeb: // set 5, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x20;
        break;

    case 0xec: // set 5, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x20;
        break;

    case 0xed: // set 5, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x20;
        break;

    case 0xee: // set 5, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x20);
        break;

    case 0xef: // set 5, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x20;
        break;

    case 0xf0: // set 6, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x40;
        break;

    case 0xf1: // set 6, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x40;
        break;

    case 0xf2: // set 6, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x40;
        break;

    case 0xf3: // set 6, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x40;
        break;

    case 0xf4: // set 6, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x40;
        break;

    case 0xf5: // set 6, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x40;
        break;

    case 0xf6: // set 6, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x40);
        break;

    case 0xf7: // set 6, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x40;
        break;

    case 0xf8: // set 7, B
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_B |= 0x80;
        break;

    case 0xf9: // set 7, C
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_C |= 0x80;
        break;

    case 0xfa: // set 7, D
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_D |= 0x80;
        break;

    case 0xfb: // set 7, E
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_E |= 0x80;
        break;

    case 0xfc: // set 7, H
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_H |= 0x80;
        break;

    case 0xfd: // set 7, L
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_L |= 0x80;
        break;

    case 0xfe: // set 7, (HL)
        PC_Inc = 2;
        ClocksSub = 16;
        TempAddr = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
        Write8 (TempAddr, Read8(TempAddr) | 0x80);
        break;

    case 0xff: // set 7, A
        PC_Inc = 2;
        ClocksSub = 8;
        CPU_A |= 0x80;
        break;

    }
    break;

case 0xcc: // call Z, nn
    if ((CPU_F & 0x80) != 0x00) {
        if (TotalBreakEnables > 0) {
            BreakLastCallAt = CPU_PC;
            BreakLastCallTo = ((unsigned int)lsb << 8) + (unsigned int)msb;
            BreakLastCallReturned = 0;
        }
        PC_Inc = 0;
        ClocksSub = 24;
        CPU_SP -= 2;
        CPU_PC += 3;
        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    else {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    break;

case 0xcd: // call nn
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = ((unsigned int)lsb << 8) + (unsigned int)msb;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 24;
    CPU_SP -= 2;
    CPU_PC += 3;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    break;

case 0xce: // adc A, n
    PC_Inc = 2;
    ClocksSub = 8;
    TempByte = msb;
    TempByte2 = CPU_F & 0x10;
    CPU_F = 0x00;
    if (TempByte2 != 0x00) {
        if (TempByte == 0xff) CPU_F = 0x10;
        TempByte++;
    }
    CPU_A += TempByte;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    if (CPU_A < TempByte) CPU_F |= 0x10;
    TempByte = TempByte & 0x0f;
    TempByte2 = CPU_A & 0x0f;
    if (TempByte > TempByte2) CPU_F |= 0x20;
    break;

case 0xcf: // rst 8 (call 0x0008)
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0x8;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x0008;
    break;

case 0xd0: // ret NC
    if ((CPU_F & 0x10) != 0x00) {
        PC_Inc = 1;
        ClocksSub = 8;
    }
    else {
        if (TotalBreakEnables > 0) BreakLastCallReturned = 1;
        PC_Inc = 0;
        ClocksSub = 20;
        Read16 (CPU_SP, &TempByte, &TempByte2);
        CPU_PC = ((unsigned int)TempByte2 << 8) + (unsigned int)TempByte;
        CPU_SP += 2;
    }
    break;

case 0xd1: // pop DE
    PC_Inc = 1;
    ClocksSub = 12;
    Read16 (CPU_SP, &CPU_E, &CPU_D);
    CPU_SP += 2;
    break;

case 0xd2: // j NC, nn
    if ((CPU_F & 0x10) != 0x00) {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    else {
        PC_Inc = 0;
        ClocksSub = 16;
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    break;

case 0xd3: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xd4: // call NC, nn
    if ((CPU_F & 0x10) != 0x00) {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    else {
        if (TotalBreakEnables > 0) {
            BreakLastCallAt = CPU_PC;
            BreakLastCallTo = ((unsigned int)lsb << 8) + (unsigned int)msb;
            BreakLastCallReturned = 0;
        }
        PC_Inc = 0;
        ClocksSub = 24;
        CPU_SP -= 2;
        CPU_PC += 3;
        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    break;

case 0xd5: // push DE
    PC_Inc = 1;
    ClocksSub = 16;
    CPU_SP -= 2;
    Write16 (CPU_SP, CPU_E, CPU_D);
    break;

case 0xd6: // sub A, n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_F = 0x40;
    if (msb > CPU_A) CPU_F |= 0x10;
    if ((msb & 0x0f) > (CPU_A & 0x0f)) CPU_F |= 0x20;
    CPU_A -= msb;
    if (CPU_A == 0x00) CPU_F = 0xc0;
    break;

case 0xd7: // rst 10
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0x10;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x0010;
    break;

case 0xd8: // ret C
    if ((CPU_F & 0x10) != 0x00) {
        if (TotalBreakEnables > 0) BreakLastCallReturned = 1;
        PC_Inc = 0;
        ClocksSub = 20;
        Read16 (CPU_SP, &TempByte, &TempByte2);
        CPU_PC = ((unsigned int)TempByte2 << 8) + (unsigned int)TempByte;
        CPU_SP += 2;
    }
    else {
        PC_Inc = 1;
        ClocksSub = 8;
    }
    break;

case 0xd9: // reti
    if (TotalBreakEnables > 0) BreakLastCallReturned = 1;
    PC_Inc = 0;
    ClocksSub = 16;
    Read16 (CPU_SP, &TempByte, &TempByte2);
    CPU_PC = ((unsigned int)TempByte2 << 8) + (unsigned int)TempByte;
    CPU_SP += 2;
    CPU_IME = TRUE;
    break;

case 0xda: // j C, nn (abs jump if carry)
    if ((CPU_F & 0x10) != 0x00) {
        PC_Inc = 0;
        ClocksSub = 16;
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    else {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    break;

case 0xdb: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xdc: // call C, nn
    if ((CPU_F & 0x10) != 0x00) {
        if (TotalBreakEnables > 0) {
            BreakLastCallAt = CPU_PC;
            BreakLastCallTo = ((unsigned int)lsb << 8) + (unsigned int)msb;
            BreakLastCallReturned = 0;
        }
        PC_Inc = 0;
        ClocksSub = 24;
        CPU_SP -= 2;
        CPU_PC += 3;
        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
        CPU_PC = ((unsigned int)lsb << 8) + (unsigned int)msb;
    }
    else {
        PC_Inc = 3;
        ClocksSub = 12;
    }
    break;

case 0xdd: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xde: // sbc A, n
    PC_Inc = 2;
    ClocksSub = 8;
    TempByte = CPU_A;
    TempByte2 = CPU_F & 0x10;
    CPU_F = 0x40;
    CPU_A -= msb;
    if (TempByte2 != 0x00) {
        if (CPU_A == 0x00) CPU_F |= 0x30;
        CPU_A--;
    }
    if (CPU_A > TempByte) CPU_F |= 0x10;
    if (CPU_A == 0x00) CPU_F |= 0x80;
    TempByte2 = TempByte & 0x0f;
    TempByte = CPU_A & 0x0f;
    if (TempByte > TempByte2) CPU_F |= 0x20;
    break;

case 0xdf: // rst 18
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0x18;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x0018;
    break;

case 0xe0: // ldh (n), A (load to IO port n - ff00 + n)
    PC_Inc = 2;
    ClocksSub = 12;
    Write8 (0xff00 + (unsigned int)msb, CPU_A);
    break;

case 0xe1: // pop HL
    PC_Inc = 1;
    ClocksSub = 12;
    Read16 (CPU_SP, &CPU_L, &CPU_H);
    CPU_SP += 2;
    break;

case 0xe2: // ldh (C), A (load to IO port C - ff00 + C)
    PC_Inc = 1;
    ClocksSub = 8;
    Write8 (0xff00 + (unsigned int)CPU_C, CPU_A);
    break;

case 0xe3: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xe4: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xe5: // push HL
    PC_Inc = 1;
    ClocksSub = 16;
    CPU_SP -= 2;
    Write16 (CPU_SP, CPU_L, CPU_H);
    break;

case 0xe6: // and n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_A = CPU_A & msb;
    CPU_F = 0x20;
    if (CPU_A == 0x00) CPU_F = 0xa0;
    break;

case 0xe7: // rst 20
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0x20;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x0020;
    break;

case 0xe8: // add SP, d
    PC_Inc = 2;
    ClocksSub = 16;
    CPU_F = 0x00;
    if (msb >= 0x80) {
        TempAddr = 256 - (unsigned int)msb;
        CPU_SP -= TempAddr;
        if ((CPU_SP & 0x0000ffff) > (TempAddr & 0x0000ffff)) CPU_F |= 0x10;
        if ((CPU_SP & 0x000000ff) > (TempAddr & 0x000000ff)) CPU_F |= 0x20;
    }
    else {
        TempAddr = (unsigned int)msb;
        CPU_SP += TempAddr;
        if ((CPU_SP & 0x0000ffff) < (TempAddr & 0x0000ffff)) CPU_F |= 0x10;
        if ((CPU_SP & 0x000000ff) < (TempAddr & 0x000000ff)) CPU_F |= 0x20;
    }
    break;

case 0xe9: // j HL
    PC_Inc = 0;
    ClocksSub = 4;
    CPU_PC = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
    break;

case 0xea: // ld (nn), A
    PC_Inc = 3;
    ClocksSub = 16;
    Write8 (((unsigned int)lsb << 8) + (unsigned int)msb, CPU_A);
    break;

case 0xeb: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xec: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xed: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xee: // xor n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_A = CPU_A ^ msb;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xef: // rst 28
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0x28;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x0028;
    break;

case 0xf0: // ldh A, (n)
    PC_Inc = 2;
    ClocksSub = 12;
    CPU_A = Read8 (0xff00 + (unsigned int)msb);
    break;

case 0xf1: // pop AF
    PC_Inc = 1;
    ClocksSub = 12;
    Read16 (CPU_SP, &CPU_F, &CPU_A);
    CPU_F &= 0xf0;
    CPU_SP += 2;
    break;

case 0xf2: // ldh A, C
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_A = Read8 (0xff00 + (unsigned int)CPU_C);
    break;

case 0xf3: // di
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_IME = FALSE;
    break;

case 0xf4: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xf5: // push AF
    PC_Inc = 1;
    ClocksSub = 16;
    CPU_SP -= 2;
    Write16 (CPU_SP, CPU_F, CPU_A);
    break;

case 0xf6: // or n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_A = CPU_A | msb;
    CPU_F = 0x00;
    if (CPU_A == 0x00) CPU_F = 0x80;
    break;

case 0xf7: // rst 30
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0x30;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x0030;
    break;

case 0xf8: // ld HL, SP+d
    PC_Inc = 2;
    ClocksSub = 12;
    CPU_F = 0x00;
    TempAddr = CPU_SP;
    if (msb >= 0x80) {
        TempAddr -= 256 - (unsigned int)msb;
        if (TempAddr > CPU_SP) CPU_F |= 0x10;
        if ((TempAddr & 0x00ffffff) > (CPU_SP & 0x00ffffff)) CPU_F |= 0x20;
    }
    else {
        TempAddr += (unsigned int)msb;
        if (CPU_SP > TempAddr) CPU_F |= 0x10;
        if ((CPU_SP & 0x00ffffff) > (TempAddr & 0x00ffffff)) CPU_F |= 0x20;
    }
    CPU_H = (unsigned char)(TempAddr >> 8);
    CPU_L = (unsigned char)(TempAddr & 0xff);
    break;

case 0xf9: // ld SP, HL
    PC_Inc = 1;
    ClocksSub = 8;
    CPU_SP = ((unsigned int)CPU_H << 8) + (unsigned int)CPU_L;
    break;

case 0xfa: // ld A, (nn)
    PC_Inc = 3;
    ClocksSub = 16;
    CPU_A = Read8(((unsigned int)lsb << 8) + (unsigned int)msb);
    break;

case 0xfb: // ei
    PC_Inc = 1;
    ClocksSub = 4;
    CPU_IME = TRUE;
    break;

case 0xfc: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xfd: // REMOVED INSTRUCTION
    PC_Inc = 0;
    ClocksSub = ClocksAcc;
    Running = FALSE;
    break;

case 0xfe: // cp n
    PC_Inc = 2;
    ClocksSub = 8;
    CPU_F = 0x40;
    if ((msb & 0x0f) > (CPU_A & 0x0f)) CPU_F = 0x60;
    if (msb > CPU_A) CPU_F |= 0x10;
    if (CPU_A == msb) CPU_F |= 0x80;
    break;

case 0xff: // rst 38
    if (TotalBreakEnables > 0) {
        BreakLastCallAt = CPU_PC;
        BreakLastCallTo = 0x38;
        BreakLastCallReturned = 0;
    }
    PC_Inc = 0;
    ClocksSub = 16;
    CPU_SP -= 2;
    CPU_PC++;
    Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
    CPU_PC = 0x0038;
    break;
}
