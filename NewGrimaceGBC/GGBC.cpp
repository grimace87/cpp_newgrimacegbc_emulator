
#include "Class.h"

#define GPU_HBLANK    0x00
#define GPU_VBLANK    0x01
#define GPU_SCAN_OAM  0x02
#define GPU_SCAN_VRAM 0x03

#define SGBCOM_PAL01    0x00
#define SGBCOM_PAL23    0x01
#define SGBCOM_PAL03    0x02
#define SGBCOM_PAL12    0x03
#define SGBCOM_ATTR_BLK 0x04
#define SGBCOM_ATTR_LIN 0x05
#define SGBCOM_ATTR_DIV 0x06
#define SGBCOM_ATTR_CHR 0x07
#define SGBCOM_SOUND    0x08
#define SGBCOM_SOU_TRN  0x09
#define SGBCOM_PAL_SET  0x0a
#define SGBCOM_PAL_TRN  0x0b
#define SGBCOM_ATRC_EN  0x0c
#define SGBCOM_TEST_EN  0x0d
#define SGBCOM_ICON_EN  0x0e
#define SGBCOM_DATA_SEND 0x0f
#define SGBCOM_DATA_TRN 0x10
#define SGBCOM_MLT_REQ  0x11
#define SGBCOM_JUMP     0x12
#define SGBCOM_CHR_TRN  0x13
#define SGBCOM_PCT_TRN  0x14
#define SGBCOM_ATTR_TRN 0x15
#define SGBCOM_ATTR_SET 0x16
#define SGBCOM_MASK_EN  0x17
#define SGBCOM_OBJ_TRN  0x18
#define SGBCOM_PAL_PRI  0x19

BOOL DrawToFrame1;
SYSTEMTIME SysTime;

const unsigned char OfficialNintendoLogo[48] = {
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d,
    0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99,
    0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e
};

void saveLastFileName(const wchar_t* path, int buffSize) {

	// Attempt to read running path
	wchar_t localPath[512];
	int size = GetModuleFileNameW(NULL, localPath, buffSize);
	if (size == 0) return;

	// FInd last '\' character and put the config filename after that
	int foundIndex = -1;
	for (int t = 0; t < buffSize; t++) {
		if (localPath[t] == L'\\')
			foundIndex = t;
		else if (localPath[t] == L'\0')
			break;
	}
	if (foundIndex < 0) return;
	if (foundIndex > buffSize - 9) return;

	// Append characters
	foundIndex++;
	const wchar_t* configFile = L"conf.gcf";
	for (int i = 0; i < 9; i++)
		localPath[foundIndex + i] = configFile[i];

	// Open file and write to it
	FILE* file;
	_wfopen_s(&file, localPath, L"w");
	if (file == NULL) return;
	int length = lstrlenW(path) + 1;
	size_t read = fwrite((void*) path, 2, length, file);
	fclose(file);

}

GGBC::GGBC() {

    // Flag not running and no ROM loaded
    Running = FALSE;
    ROM_Valid = FALSE;
    SRAM_File = NULL;

    // Allocate necessary memory
    ROM = new unsigned char [256*16384];
    WRAM = new unsigned char [4*4096];
    SRAM = new unsigned char [8*4096];
    VRAM = new unsigned char [2*8192];
    IOPorts = new unsigned char [256];
    TileSet = new unsigned int [2*384*8*8]; // 2 VRAM banks, 384 tiles, 8 rows, 8 pixels per row
	SGB_MonoData = new unsigned int[160 * 152];
    SGB_Palettes = new GLuint[4*4];
    SGB_SYSPalettes  = new GLuint [512*4]; // 512 palettes, 4 colours per palette, RGB
    SGB_CHRPalettes = new unsigned int[18*20];

    // Clear things that should be cleared
	ZeroMemory(TileSet, 2 * 384 * 8 * 8 * sizeof(unsigned int));
	ZeroMemory(SGB_CHRPalettes, 18 * 20 * sizeof(unsigned int));

}

GGBC::~GGBC() {

    // Release memory
    delete[] ROM;
    delete[] WRAM;
    delete[] SRAM;
    delete[] VRAM;
    delete[] IOPorts;
    delete[] TileSet;
	delete[] SGB_MonoData;
    delete[] SGB_SYSPalettes;
    delete[] SGB_Palettes;
    delete[] SGB_CHRPalettes;

}

void GGBC::Begin() {
    
    hRunning = CreateMutex(NULL, TRUE, NULL);
    Reset();
    // Begin the running thread
    if (Running)
        CreateThread(NULL, 0, RunProc, (void*)this, NULL, NULL);

}

void GGBC::Close() {
    
    // Tell thread to end and wait for it to finish
    Running = FALSE;
    WaitForSingleObject(hRunning, 5000);
    CloseHandle(hRunning);

}

DWORD WINAPI GGBC::RunProc(LPVOID GGBCObj) {

    // Cast the pointer to this object to the right type
    GGBC *ggbc = (GGBC*)GGBCObj;

    // Get timing information
    UINT64 PerfFreq;
    UINT64 LastCounts, NewCounts;
    float TimeChange;
    int NoClocks = 0;
    if (!QueryPerformanceFrequency ((LARGE_INTEGER*)&PerfFreq))
        PerfFreq = 1000; // Fallback value in case of error
    if (!QueryPerformanceCounter ((LARGE_INTEGER*)&LastCounts))
        LastCounts = (UINT64)timeGetTime ();

    while (ggbc->Running) {
        
        // Execute as many instructions in time since last update as a gameboy would:
        if (!QueryPerformanceCounter ((LARGE_INTEGER*)&NewCounts)) {
            NewCounts = (UINT64)timeGetTime ();
            TimeChange = (float)(NewCounts - LastCounts) / 1000; // Convert ms to s
        }
        else
            TimeChange = (float)(NewCounts - LastCounts) / (float)PerfFreq;
        LastCounts = NewCounts;
        NoClocks += (int)(TimeChange * (float)(ggbc->CPU_ClockFreq * ggbc->ClockMultiply / ggbc->ClockDivide));

        // Limit clocks performed to prevent stalling a slow computer:
        if (NoClocks > 1000000) NoClocks = 1000000;

        // Execute this many clock cycles
        NoClocks -= ggbc->Ex(NoClocks);

    }

    return 0;

}

bool GGBC::LoadROM(const wchar_t *FileName, HWND hWnd) {

    FILE *FilePtr;
    unsigned char NintendoLogo[49];
    int n;
    unsigned char TestByte;
    unsigned int Sum;
    wchar_t BattFileName[256];
    
    unsigned char NewTimerData[16];
    ZeroMemory (NewTimerData, 16);
    
    _wfopen_s (&FilePtr, FileName, L"rb");
    if (FilePtr == NULL) {
        MessageBox (hWnd, L"Couldn't open ROM file.", L"Error", MB_OK);
        ROM_Valid = FALSE;
        return false;
    }

    // Read ROM header. Starts at 0x0100 with NOP and a jump (usually to 0x0150, after the header) (total 4 bytes)
    // Nintendo logo at 0x0104 (48 bytes)
    fseek (FilePtr, 0x0104, SEEK_SET);
    fread (NintendoLogo, 1, 48, FilePtr);
    for (n = 0; n < 48; n++) {
        if (NintendoLogo[n] != OfficialNintendoLogo[n]) {
            MessageBox (hWnd, L"ROM has an invalid header, but still might run.", L"Error", MB_OK);
            //ROM_Valid = FALSE;
            //fclose (FilePtr);
            //return;
			break;
        }
    }
    ROM_Valid = TRUE;

    // Game title at 0x0134 (15 bytes)
    fread (ROM_Title, 1, 16, FilePtr);
    // Note that last byte of title is colour compatibility
    if ((ROM_Title[15] == 0x80) || (ROM_Title[15] == 0xc0)) {
        ROM_GBCFlag = TRUE;
        ROM_Title[11] = '\0';
    }
    else {
        ROM_GBCFlag = FALSE;
        ROM_Title[16] = '\0';
    }
    // Next two bytes are licensee codes.
    // Then is an SGB flag, cartridge type, and cartridge ROM and RAM sizes.
    fseek (FilePtr, 0x0146, SEEK_SET);
    fread (&ROM_SGBFlag, 1, 1, FilePtr);
    fread (&ROM_CartType, 1, 1, FilePtr);
    fread (&ROM_ROMSize, 1, 1, FilePtr);
    fread (&ROM_RAMSize, 1, 1, FilePtr);
    if (ROM_SGBFlag != 0x03) ROM_SGBFlag = 0x00;
    ROM_HasSRAM = FALSE;
    ROM_HasBattery = FALSE;
    ROM_HasTimer = FALSE;
    ROM_HasRumble = FALSE;
    switch (ROM_CartType) {

        case 0x00: ROM_MBC = MBC_NONE; break;
        case 0x08: ROM_MBC = MBC_NONE; ROM_HasSRAM = TRUE; break;
        case 0x09: ROM_MBC = MBC_NONE; ROM_HasSRAM = TRUE; ROM_HasBattery = TRUE; break;

        case 0x01: ROM_MBC = MBC1; break;
        case 0x02: ROM_MBC = MBC1; ROM_HasSRAM = TRUE; break;
        case 0x03: ROM_MBC = MBC1; ROM_HasSRAM = TRUE; ROM_HasBattery = TRUE; break;

        case 0x05: ROM_MBC = MBC2; break;
        case 0x06: ROM_MBC = MBC2; ROM_HasBattery = TRUE; break;

        //case 0x0b: case 0x0c: case 0x0d: rom.MBC = MMM01; break;

        case 0x0f: ROM_MBC = MBC3; ROM_HasBattery = TRUE; ROM_HasTimer = TRUE; break;
        case 0x10: ROM_MBC = MBC3; ROM_HasSRAM = TRUE; ROM_HasBattery = TRUE; ROM_HasTimer = TRUE; break;
        case 0x11: ROM_MBC = MBC3; break;
        case 0x12: ROM_MBC = MBC3; ROM_HasSRAM = TRUE; break;
        case 0x13: ROM_MBC = MBC3; ROM_HasSRAM = TRUE; ROM_HasBattery = TRUE; break;

        //case 0x15: case 0x16: case 0x17: rom.MBC = MBC4; break;

        case 0x19: ROM_MBC = MBC5; break;
        case 0x1a: ROM_MBC = MBC5; ROM_HasSRAM = TRUE; break;
        case 0x1b: ROM_MBC = MBC5; ROM_HasSRAM = TRUE; ROM_HasBattery = TRUE; break;
        case 0x1c: ROM_MBC = MBC5; ROM_HasRumble = TRUE; break;
        case 0x1d: ROM_MBC = MBC5; ROM_HasSRAM = TRUE; ROM_HasRumble = TRUE; break;
        case 0x1e: ROM_MBC = MBC5; ROM_HasSRAM = TRUE; ROM_HasBattery = TRUE; ROM_HasRumble = TRUE; break;

        default:
            MessageBox (hWnd, L"Cartridge type is not supported.", L"Error", MB_OK);
            ROM_Valid = FALSE;
            fclose (FilePtr);
            return false;
    }

    switch (ROM_ROMSize) {
        case 0x00: ROM_ROMSizeBytes = 32768; ROM_ROMBankSelectMask = 0x0; break;
        case 0x01: ROM_ROMSizeBytes = 65536; ROM_ROMBankSelectMask = 0x03; break;
        case 0x02: ROM_ROMSizeBytes = 131072; ROM_ROMBankSelectMask = 0x07; break;
        case 0x03: ROM_ROMSizeBytes = 262144; ROM_ROMBankSelectMask = 0x0f; break;
        case 0x04: ROM_ROMSizeBytes = 524288; ROM_ROMBankSelectMask = 0x1f; break;
        case 0x05: ROM_ROMSizeBytes = 1048576; ROM_ROMBankSelectMask = 0x3f; break;
        case 0x06: ROM_ROMSizeBytes = 2097152; ROM_ROMBankSelectMask = 0x7f; break;
        case 0x07: ROM_ROMSizeBytes = 4194304; ROM_ROMBankSelectMask = 0xff; break;
        case 0x52: ROM_ROMSizeBytes = 1179648; ROM_ROMBankSelectMask = 0x7f; break;
        case 0x53: ROM_ROMSizeBytes = 1310720; ROM_ROMBankSelectMask = 0x7f; break;
        case 0x54: ROM_ROMSizeBytes = 1572864; ROM_ROMBankSelectMask = 0x7f; break;
        default:
            MessageBox (hWnd, L"ROM has invalid size indicator.", L"Error", MB_OK);
            ROM_Valid = FALSE;
            fclose (FilePtr);
            return false;
    }
    switch (ROM_RAMSize) {
        case 0x00:
            ROM_RAMSizeBytes = 0;
            if (ROM_MBC == MBC2)
                ROM_RAMSizeBytes = 512; // MBC2 includes 512x4 bits of RAM (and still says 0x00 for RAM size)
            break;
        case 0x01: ROM_RAMSizeBytes = 2048; break;
        case 0x02: ROM_RAMSizeBytes = 8192; break;
        case 0x03: ROM_RAMSizeBytes = 32768; break;
        default:
            MessageBox (hWnd, L"ROM has invalid external\nRAM size indicator.", L"Error", MB_OK);
            ROM_Valid = FALSE;
            fclose (FilePtr);
            return false;
    }

    if (ROM_HasBattery) {
        n = 0;
        while (FileName[n] != '.' && n < 250) {
             BattFileName[n] = (wchar_t)FileName[n];
            n++;
        }
        BattFileName[n++] = '.';
        BattFileName[n++] = 'g';
        BattFileName[n++] = 's';
        BattFileName[n++] = 'v';
        BattFileName[n] = 0x00;
        _wfopen_s (&SRAM_File, BattFileName, L"rb+");
        if (SRAM_File == NULL) {
            _wfopen_s (&SRAM_File, BattFileName, L"wb");
            if (SRAM_File == NULL) {
                MessageBox (hWnd, L"Save file does not exist and\ncould not be created.", L"Error", MB_OK);
                ROM_Valid = FALSE;
                fclose (FilePtr);
                return false;
            }
            else
                MessageBox (hWnd, L"New .gsv save file was created.", L"Note", MB_OK);
            for (n = 0; n < ROM_RAMSizeBytes; n++)
                SRAM[n] = 0x00;
            if (ROM_HasTimer) {
                fwrite (SRAM, 1, ROM_RAMSizeBytes, SRAM_File);
                // Prepare new data to append to file
                GetSystemTime (&SysTime);
                NewTimerData[0] = (unsigned char)SysTime.wSecond;
                NewTimerData[1] = (unsigned char)SysTime.wMinute;
                NewTimerData[2] = (unsigned char)SysTime.wHour;
                NewTimerData[3] = (unsigned char)SysTime.wDay;
                NewTimerData[4] = (unsigned char)SysTime.wMonth;
                NewTimerData[5] = (unsigned char)SysTime.wYear;
                // Append data at the end
                fwrite (NewTimerData, 1, 16, SRAM_File);
            }
            else
                fwrite (SRAM, 1, ROM_RAMSizeBytes + 16, SRAM_File);
            rewind (SRAM_File);
        }
        else {
            fread (SRAM, 1, ROM_RAMSizeBytes, SRAM_File);
            if (ROM_HasTimer) {
                // Check if the file already has the extra space at the end
                fseek (SRAM_File, 0, SEEK_END);
                if (ftell (SRAM_File) < ROM_RAMSizeBytes + 16) {
                    // Prepare new data to append to file
                    GetSystemTime (&SysTime);
                    NewTimerData[0] = (unsigned char)SysTime.wSecond;
                    NewTimerData[1] = (unsigned char)SysTime.wMinute;
                    NewTimerData[2] = (unsigned char)SysTime.wHour;
                    NewTimerData[3] = (unsigned char)SysTime.wDay;
                    NewTimerData[4] = (unsigned char)SysTime.wMonth;
                    NewTimerData[5] = (unsigned char)SysTime.wYear;
                    // Reopen file for appending data at the end, and write the new data. Reopen in normal edit mode
                    fclose (SRAM_File);
                    _wfopen_s (&SRAM_File, BattFileName, L"ab+");
                    fwrite (NewTimerData, 1, 16, SRAM_File);
                    fclose (SRAM_File);
                    _wfopen_s (&SRAM_File, BattFileName, L"rb+");
                    MessageBox (hWnd, L"Amended save file to include\ntimer data.", L"Note", MB_OK);
                }
            }
            rewind (SRAM_File);
        }
    }

    // 0x4a - 0x4c contain a japanese designatioon, an old licensee number and a version number.
    // 0x4d is the header checksum. Game won't work if it checks incorrectly.
    fseek (FilePtr, 0x014d, SEEK_SET);
    fread (&ROM_CheckSum, 1, 1, FilePtr);
    // Check the checksum. Using bytes 0x0134 to 0x014c, apply x = x - mem[addr] - 1 (starting x = 0)
    Sum = 0;
    fseek (FilePtr, 0x0134, SEEK_SET);
    for (n = 0; n < 25; n++) {
        fread (&TestByte, 1, 1, FilePtr);
        Sum = Sum - (unsigned int)TestByte - 1;
    }
    if (Sum % 256 != (unsigned int)ROM_CheckSum) {
        MessageBox (hWnd, L"Checksum does not match, but the game may still run.", L"Error", MB_OK);
        //ROM_Valid = FALSE;
        //fclose (FilePtr);
        //return;
    }
    // Bytes 0x4e and 0x4f are a global checksum (all bytes in ROM except these two). Is never checked.

    // Load ROM data into memory.
    fseek (FilePtr, 0, SEEK_END);
    long ROMFileSize = ftell (FilePtr);
    rewind (FilePtr);
    if (ROM_ROMSizeBytes > ROMFileSize) {
        MessageBox (hWnd, L"ROM appears incomplete.", L"Error", MB_OK);
        ROM_Valid = FALSE;
        fclose (FilePtr);
        return false;
    }
    if (ROMFileSize > 4194304) {
        MessageBox (hWnd, L"ROM file is unusually large.\nOmitting extra data.", L"Note", MB_OK);
        ROMFileSize = 4194304;
    }
    fread (ROM, 1, ROMFileSize, FilePtr);
    fclose (FilePtr);

	// Save that this was opened
	saveLastFileName(FileName, 512);

    return true;

}

void GGBC::Reset() {
    
    // Make sure a valid ROM is loaded
    if (ROM_Valid == FALSE) {
        Running = FALSE;
        return;
    }

    // Initialise control variables
    CPU_IME = 0;
    CPU_Halted = 0;
    CPU_Stopped = 0;
    GPU_Mode = GPU_SCAN_OAM;
    GPU_TimeInMode = 0;
    KeyBut = 0x0f;
    KeyDir = 0x0f;
    KeyStateChanged = FALSE;
    AccessOAM = TRUE;
    AccessVRAM = TRUE;
    Serial_Request = FALSE;
    Serial_ClockIsExternal = FALSE;
    BreakCode = 0;
    NeedClear = FALSE;

    // Initialise emulated memory
    ROM_BankOffset = 0x4000;
    WRAM_BankOffset = 0x1000;
    VRAM_BankOffset = 0x0000;
    SRAM_BankOffset = 0x0000;

    // Resetting IOPorts may avoid graphical glitches when switching to a colour game. Clearing VRAM may help too.
    ZeroMemory(IOPorts, 256);
    ZeroMemory(VRAM, 16384);

    // Set up registers
    CPU_PC = 0x0100;
    CPU_SP = 0xfffe;
    CPU_F = 0xb0;
    CPU_B = 0x00;
    CPU_C = 0x13;
    CPU_D = 0x00;
    CPU_E = 0xd8;
    CPU_H = 0x01;
    CPU_L = 0x4d;
    IOPorts[5] = 0x00;
    IOPorts[6] = 0x00;
    IOPorts[7] = 0x00;
    IOPorts[16] = 0x80;
    IOPorts[17] = 0xbf;
    IOPorts[18] = 0xf3;
    IOPorts[20] = 0xbf;
    IOPorts[22] = 0x3f;
    IOPorts[23] = 0x00;
    IOPorts[25] = 0xbf;
    IOPorts[26] = 0x7f;
    IOPorts[27] = 0xff;
    IOPorts[28] = 0x9f;
    IOPorts[30] = 0xbf;
    IOPorts[32] = 0xff;
    IOPorts[33] = 0x00;
    IOPorts[34] = 0x00;
    IOPorts[35] = 0xbf;
    IOPorts[36] = 0x77;
    IOPorts[37] = 0xf3;
    IOPorts[64] = 0x91;
    IOPorts[66] = 0x00;
    IOPorts[67] = 0x00;
    IOPorts[69] = 0x00;
    IOPorts[71] = 0xfc;
    IOPorts[72] = 0xff;
    IOPorts[73] = 0xff;
    IOPorts[74] = 0x00;
    IOPorts[75] = 0x00;
    IOPorts[85] = 0xff;
    IOPorts[255] = 0x00;

    // Set things specific to the type of emulated device
    if (ROM_GBCFlag) {
        
        ROM_SGBFlag = FALSE;
        CPU_ClockFreq = GB_FREQ;
        GPU_ClockFactor = 1;
        ReadLine = &GGBC::ReadLine_GBC;
        CPU_A = 0x11;
        IOPorts[38] = 0xf1;
        
        SGB_FreezeScreen = FALSE;
        SGB_MultEnabled = 0;

    }
    else if (ROM_SGBFlag) {
        
        ROM_GBCFlag = FALSE;
        CPU_ClockFreq = SGB_FREQ;
        GPU_ClockFactor = 1;
        ReadLine = &GGBC::ReadLine_SGB;
        CPU_A = 0x01;
        IOPorts[38] = 0xf0;
        
        SGB_ReadingCommand = FALSE;
        SGB_FreezeScreen = FALSE;
        SGB_MultEnabled = FALSE;
        SGB_NoPacketsSent = 0;
        SGB_NoPacketsToSend = 0;
        SGB_ReadJoypadID = 0x0c;

        CGB_BGPalIndex = 0;
        CGB_OBJPalIndex = 0;

    }
    else {

        ROM_SGBFlag = FALSE;
        ROM_GBCFlag = FALSE;
        CPU_ClockFreq = GB_FREQ;
        GPU_ClockFactor = 1;
        ReadLine = &GGBC::ReadLine_GB;
        CPU_A = 0x01;
        IOPorts[38] = 0xf1;

        SGB_FreezeScreen = 0;
        SGB_MultEnabled = 0;

    }

    // Other stuff:
    Serial_Timer = 0;
    CPU_DividerCount = 0;
    CPU_TimerCount = 0;
    CPU_TimerIncTime = 1024;
    CPU_TimerRunning = FALSE;
    ROM_MBCMode = 0;
    SRAM_Enable = FALSE;
    ROM_TimerMode = 0x00;
    ROM_TimerLatch = 0x00;

    LastLYCompare = 1; // Will prevent LYC causing interrupts immediately
    BlankedScreen = FALSE;

    Running = TRUE;

}

int GGBC::Ex(int ClocksAdd) {

    // Assign memory
    unsigned char instr, msb, lsb, interruptable;
    unsigned int PC_Inc;
    int ClocksSub;
    int StartClocksAcc;
    int DisplayEnabled;
    unsigned char TempByte, TempByte2, TempByte3;
    unsigned int TempAddr, TempAddr2;

    // Increment clocks accumulator
    ClocksAcc += ClocksAdd;
    StartClocksAcc = ClocksAcc;

    // Sort keys if required
    if (KeyStateChanged) {
        // Adjust value in keypad register
        if ((IOPorts[0x00] & 0x30) == 0x20) {
            IOPorts[0x00] &= 0xf0;
            IOPorts[0x00] |= KeyDir;
        }
        else if ((IOPorts[0x00] & 0x30) == 0x10) {
            IOPorts[0x00] &= 0xf0;
            IOPorts[0x00] |= KeyBut;
        }
        // Set interrupt request flag
        IOPorts[0x0f] |= 0x10;
        KeyStateChanged = FALSE;
    }

    while (ClocksAcc > 0) {

        if (BreakCode > 0) {
            Running = FALSE;
            break;
        }

        instr = Read8 (CPU_PC);
        msb = Read8 (CPU_PC + 1);
        lsb = Read8 (CPU_PC + 2);

        // Run appropriate RunOp function, depending on opcode.
        // Pass three bytes in case all are needed. Sets PC increment and clocks taken.
        #include "CPUSet.h"

        CPU_PC += PC_Inc;
        CPU_PC &= 0xffff; // Clamp to 16 bits
        ClocksAcc -= ClocksSub; // Dependent on instruction run
        ClocksRun += ClocksSub;

        // Check for interrupts:
        if ((CPU_IME == TRUE) || (CPU_Halted == TRUE)) {
            msb = IOPorts[0xff];
            if (msb != 0x00) {
                lsb = IOPorts[0x0f];
                if (lsb != 0x00) {
                    interruptable = msb & lsb;
                    if ((interruptable & 0x01) != 0x00) { // VBlank
                        if (CPU_Halted) CPU_PC++;
                        CPU_Halted = FALSE;
                        if (CPU_IME == FALSE) {
                            instr = Read8 (CPU_PC);
                            goto end_of_int_check;
                        }
                        CPU_IME = FALSE;
                        IOPorts[0x0f] &= 0x1e;
                        CPU_SP -= 2; // Pushing PC onto stack
                        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
                        CPU_PC = 0x0040;
                        instr = Read8 (CPU_PC);
                        goto end_of_int_check;
                    }
                    if ((interruptable & 0x02) != 0x00) { // LCD Stat
                        if (CPU_Halted) CPU_PC++;
                        CPU_Halted = FALSE;
                        if (CPU_IME == FALSE) {
                            instr = Read8 (CPU_PC);
                            goto end_of_int_check;
                        }
                        CPU_IME = FALSE;
                        IOPorts[0x0f] &= 0x1d;
                        CPU_SP -= 2;
                        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
                        CPU_PC = 0x0048;
                        instr = Read8 (CPU_PC);
                        goto end_of_int_check;
                    }
                    if ((interruptable & 0x04) != 0x00) { // Timer
                        if (CPU_Halted) CPU_PC++;
                        CPU_Halted = FALSE;
                        if (CPU_IME == FALSE) {
                            instr = Read8 (CPU_PC);
                            goto end_of_int_check;
                        }
                        CPU_IME = FALSE;
                        IOPorts[0x0f] &= 0x1b;
                        CPU_SP -= 2;
                        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
                        CPU_PC = 0x0050;
                        instr = Read8 (CPU_PC);
                        goto end_of_int_check;
                    }
                    if ((interruptable & 0x08) != 0x00) { // Serial
                        if (CPU_Halted) CPU_PC++;
                        CPU_Halted = FALSE;
                        if (CPU_IME == FALSE) {
                            instr = Read8 (CPU_PC);
                            goto end_of_int_check;
                        }
                        CPU_IME = FALSE;
                        IOPorts[0x0f] &= 0x17;
                        CPU_SP -= 2;
                        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
                        CPU_PC = 0x0058;
                        instr = Read8 (CPU_PC);
                        goto end_of_int_check;
                    }
                    if ((interruptable & 0x10) != 0x00) { // Joypad
                        if (CPU_Halted || CPU_Stopped) CPU_PC++;
                        CPU_Halted = FALSE;
                        if (CPU_IME == FALSE) {
                            instr = Read8 (CPU_PC);
                            goto end_of_int_check;
                        }
                        CPU_IME = FALSE;
                        IOPorts[0x0f] &= 0x0f;
                        CPU_SP -= 2;
                        Write16 (CPU_SP, (unsigned char)(CPU_PC & 0xff), (unsigned char)(CPU_PC >> 8));
                        CPU_PC = 0x0060;
                        instr = Read8 (CPU_PC);
                        goto end_of_int_check;
                    }
                }
            }
        }
        end_of_int_check:

        if (BreakOnPC) {
            if (CPU_PC == BreakPCAddr)
                BreakCode = 3;
        }
        
        DisplayEnabled = IOPorts[0x0040] & 0x80;

        // Permanent compare of LY and LYC
        if ((IOPorts[0x44] == IOPorts[0x45]) && (DisplayEnabled != 0)) {
            IOPorts[0x41] |= 0x04; // Set coincidence flag
            // Request interrupt if this signal goes low to high
            if (((IOPorts[0x41] & 0x40) != 0x00) && (LastLYCompare == 0))
                IOPorts[0x0f] |= 0x02;
            LastLYCompare = 1;
        }
        else {
            IOPorts[0x41] &= 0xfb; // Clear coincidence flag
            LastLYCompare = 0;
        }

        // Handling of timers
        CPU_DividerCount += ClocksSub;
        if (CPU_DividerCount >= 256) {
            CPU_DividerCount -= 256;
            IOPorts[0x04]++;
        }
        if (CPU_TimerRunning != 0) {
            CPU_TimerCount++;
            if (CPU_TimerCount >= CPU_TimerIncTime) {
                CPU_TimerCount -= CPU_TimerIncTime;
                IOPorts[0x05]++;
                if (IOPorts[0x05] == 0x00) {
                    IOPorts[0x05] = IOPorts[0x06];
                    IOPorts[0x0f] |= 0x04;
                }
            }
        }

        // Handle serial port timeout
        if (Serial_IsTransferring) {
            if (Serial_ClockIsExternal == FALSE) {
                Serial_Timer -= ClocksSub;
                if (Serial_Timer <= 0) {
                    Serial_IsTransferring = 0;
                    IOPorts[0x02] &= 0x03; // Clear the transferring indicator
                    IOPorts[0x0f] |= 0x08; // Request a serial interrupt
                    /*if (ggbc2.Running != 0) {
                        mem.RequestSerial = 1;
                        gpu.TimeInMode += clocks_sub / GPU_ClockFactor; // Don't skip the LCD timer
                        StartClockAcc -= clocks_acc;
                        clocks_acc = 0;
                        return StartClockAcc;
                    }
                    else*/
                        IOPorts[1] = 0xff;
                }
            }
            else {
                if (Serial_Timer == 1) {
                    Serial_Timer = 0;
                    /*if (ggbc2.Running != 0) {
                        mem.RequestSerial = 1;
                        gpu.TimeInMode += clocks_sub / GPU_ClockFactor; // Don't skip the LCD timer
                        StartClockAcc -= clocks_acc;
                        clocks_acc = 0;
                        return StartClockAcc;
                    }*/
                }
            }
        }

        // Handle GPU timings
        if (DisplayEnabled != 0) {
            GPU_TimeInMode += ClocksSub / GPU_ClockFactor; // GPU clock factor accounts for double speed mode
            switch (GPU_Mode) {
                case GPU_HBLANK:
                    // Spends 204 cycles here, then moves to next line. After 144th hblank, move to vblank.
                    if (GPU_TimeInMode >= 204) {
                        GPU_TimeInMode -= 204;
                        IOPorts[0x0044]++;
                        if (IOPorts[0x0044] == 144) {
                            GPU_Mode = GPU_VBLANK;
                            IOPorts[0x0041] &= 0xfc; // address 0xff41 - LCD status register
                            IOPorts[0x0041] |= GPU_VBLANK;
                            if (DisplayEnabled != 0)
                                IOPorts[0x000f] |= 0x01; // Set interrupt request for VBLANK
                            AccessOAM = TRUE;
                            AccessVRAM = TRUE;
                            // Request status int if condition met
                            if ((IOPorts[0x0041] & 0x10) != 0x00)
                                IOPorts[0x000f] |= 0x02;
                            // This is where stuff can be drawn - on the beginning of the vblank
                            if ((SGB_FreezeScreen == FALSE) && (DisplayEnabled != 0)) {
                                if (ROM_SGBFlag)
                                    SGB_ColouriseFrame ();
                                if (DrawToFrame1) {
                                    DrawToFrame1 = FALSE;
                                    SetEvent(hEventFrameReady);
									OutputDebugString(L"Frame ready.");
                                }
                                StartClocksAcc -= ClocksAcc;
                                ClocksAcc = 0;
                                return StartClocksAcc;
                            }
                        }
                        else {
                            GPU_Mode = GPU_SCAN_OAM;
                            IOPorts[0x0041] &= 0xfc;
                            IOPorts[0x0041] |= GPU_SCAN_OAM;
                            if (DisplayEnabled) {
                                AccessOAM = FALSE;
                                AccessVRAM = TRUE;
                            }
                            // Request status int if condition met:
                            if ((IOPorts[0x0041] & 0x20) != 0x00)
                                IOPorts[0x000f] |= 0x02;
                        }
                    }
                    break;
                case GPU_VBLANK:
                    if (GPU_TimeInMode >= 456) { // 10 of these lines in vblank
                        GPU_TimeInMode -= 456;
                        IOPorts[0x0044]++;
                        if (IOPorts[0x0044] >= 154) {
                            GPU_Mode = GPU_SCAN_OAM;
                            IOPorts[0x0041] &= 0xfc;
                            IOPorts[0x0041] |= GPU_SCAN_OAM;
                            IOPorts[0x0044] = 0;
                            if (DisplayEnabled) {
                                AccessOAM = FALSE;
                                AccessVRAM = TRUE;
                            }
                            // Request status int if condition met:
                            if ((IOPorts[0x0041] & 0x20) != 0x00)
                                IOPorts[0x000f] |= 0x02;
                            // Check whether the pixel buffer is free to draw to
                            int Temp = WaitForSingleObject(hEventFrameComplete, 0);
							if (Temp == WAIT_OBJECT_0) {
								DrawToFrame1 = TRUE;
								OutputDebugString(L"Starting drawing to frame.");
							}
                        }
                    }
                    break;
                case GPU_SCAN_OAM:
                    if (GPU_TimeInMode >= 80) {
                        GPU_TimeInMode -= 80;
                        GPU_Mode = GPU_SCAN_VRAM;
                        IOPorts[0x0041] &= 0xfc;
                        IOPorts[0x0041] |= GPU_SCAN_VRAM;
                        if (DisplayEnabled) {
                            AccessOAM = FALSE;
                            AccessVRAM = FALSE;
                        }
                    }
                    break;
                case GPU_SCAN_VRAM:
                    if (GPU_TimeInMode >= 172) {
                        GPU_TimeInMode -= 172;
                        GPU_Mode = GPU_HBLANK;
                        IOPorts[0x0041] &= 0xfc;
                        IOPorts[0x0041] |= GPU_HBLANK;
                        AccessOAM = TRUE;
                        AccessVRAM = TRUE;
                        // Request status int if condition met
                        if ((IOPorts[0x0041] & 0x08) != 0x00)
                            IOPorts[0x000f] |= 0x02;
                        // Run DMA if applicable
                        if (IOPorts[0x55] < 0xff) { // H-blank DMA currently active
                            TempAddr = (IOPorts[0x51] << 8) + IOPorts[0x52]; // DMA source
                            if ((TempAddr & 0xe000) == 0x8000)
                                goto EndDMAOp; // Don't do transfers within VRAM
                            if (TempAddr >= 0xe000)
                                goto EndDMAOp; // Don't take source data from these addresses either
                            TempAddr2 = (IOPorts[0x53] << 8) + IOPorts[0x54] + 0x8000; // DMA destination
                            for (int count = 0; count < 16; count++) {
                                Write8 (TempAddr2, Read8 (TempAddr));
                                TempAddr++;
                                TempAddr2++;
                                TempAddr2 &= 0x9fff; // Keep it within VRAM
                            }
                            EndDMAOp:
                            //if (ClockFreq == GBC_FREQ) clocks_acc -= 64;
                            //else clocks_acc -= 32;
                            IOPorts[0x55]--;
                            if (IOPorts[0x55] < 0x80)
                                IOPorts[0x55] = 0xff; // End the DMA
                        }
                        // Process current line's graphics
                        if (DrawToFrame1)
                            (*this.*ReadLine) ();
                    }
                    break;
                default: // Error that should never happen:
                    Running = 0;
                    break;
            }
        }
        else {
            IOPorts[0x0044] = 0;
            GPU_TimeInMode = 0;
            GPU_Mode = GPU_SCAN_OAM;
            if (BlankedScreen == FALSE) {
                AccessOAM = TRUE;
                AccessVRAM = TRUE;
                
                ///////////////////////////////////
                // SIGNAL DRAWING HERE
                ///////////////////////////////////

                BlankedScreen = TRUE;
                StartClocksAcc -= ClocksAcc;
                ClocksAcc = 0;
                return StartClocksAcc;
            }
        }

    }
    
    StartClocksAcc -= ClocksAcc;
    ClocksAcc = 0;
    return StartClocksAcc;
    
}

unsigned char GGBC::Read8 (unsigned int addr) {

    addr &= 0xffff;
    
    if (BreakOnRead) {
        if (addr == BreakReadAddr) {
            if (BreakCode != 5) {
                BreakCode = 5;
                BreakReadByte = (unsigned int)Read8 (addr);
            }
        }
    }

    if (addr < 0x4000) return ROM[addr];
    else if (addr < 0x8000) return ROM[ROM_BankOffset + (addr & 0x3fff)];
    else if (addr < 0xa000) {
        if (AccessVRAM) return VRAM[VRAM_BankOffset + (addr & 0x1fff)];
        else return 0xff;
    }
    else if (addr < 0xc000) {
        if (SRAM_Enable) {
            if (ROM_HasTimer == FALSE)
                return SRAM[SRAM_BankOffset + (addr & 0x1fff)];
            else {
                /*if (ROM_TimerMode > 0)
                    return rom.TimerData[(unsigned int)rom.TimerMode - 0x08];
                else */if (SRAM_BankOffset < 0x8000)
                    return SRAM[SRAM_BankOffset + (addr & 0x1fff)];
                else
                    return 0;
            }
        }
        else return 0x00;
    }
    else if (addr < 0xd000) return WRAM[addr & 0x0fff];
    else if (addr < 0xe000) return WRAM[WRAM_BankOffset + (addr & 0x0fff)];
    else if (addr < 0xf000) return WRAM[addr & 0x0fff];
    else if (addr < 0xfe00) return WRAM[WRAM_BankOffset + (addr & 0x0fff)];
    else if (addr < 0xfea0) {
        if (AccessOAM) return OAM[(addr & 0x00ff) % 160];
        else return 0xff;
    }
    else if (addr < 0xff00) return 0xff;
    else if (addr < 0xff80) return ReadIO(addr & 0x7f);
    else return IOPorts[(addr & 0xff)];

}

void GGBC::Read16 (unsigned int addr, unsigned char *msb, unsigned char *lsb) {

    addr &= 0xffff;
    if (addr < 0x4000) { *msb = ROM[addr]; *lsb = ROM[addr + 1]; return;}
    else if (addr < 0x8000) { *msb = ROM[ROM_BankOffset + (addr & 0x3fff)]; *lsb = ROM[ROM_BankOffset + ((addr + 1) & 0x3fff)]; return;}
    else if (addr < 0xa000) {
        if (AccessVRAM) { *msb = VRAM[VRAM_BankOffset + (addr & 0x1fff)]; *lsb = VRAM[VRAM_BankOffset + ((addr + 1) & 0x1fff)]; return;}
        else { *msb = 0xff; *lsb = 0xff; return;}
    }
    else if (addr < 0xc000) {
        if (SRAM_Enable) {*msb = SRAM[SRAM_BankOffset + (addr & 0x1fff)]; *lsb = SRAM[SRAM_BankOffset + ((addr + 1) & 0x1fff)]; return;}
        else { *msb = 0xff; *lsb = 0xff; return;}
    }
    else if (addr < 0xd000) { *msb = WRAM[addr & 0x0fff]; *lsb = WRAM[(addr + 1) & 0x0fff]; return;}
    else if (addr < 0xe000) { *msb = WRAM[WRAM_BankOffset + (addr & 0x0fff)]; *lsb = WRAM[WRAM_BankOffset + ((addr + 1) & 0x0fff)]; return;}
    else if (addr < 0xf000) { *msb = WRAM[addr & 0x0fff]; *lsb = WRAM[(addr + 1) & 0x0fff]; return;}
    else if (addr < 0xfe00) { *msb = WRAM[WRAM_BankOffset + (addr & 0x0fff)]; *lsb = WRAM[WRAM_BankOffset + ((addr + 1) & 0x0fff)]; return;}
    else if (addr < 0xfea0) {
        if (AccessOAM) { *msb = OAM[(addr & 0x00ff) % 160]; *lsb = OAM[((addr + 1) & 0x00ff) % 160]; return;}
        else { *msb = 0xff; *lsb = 0xff; return;}
    }
    else if (addr < 0xff00) { *msb = 0xff; *lsb = 0xff; return;}
    else if (addr < 0xff80) {
        *msb = ReadIO(addr & 0x7f);
        *lsb = ReadIO((addr + 1) & 0x7f);
        return;
    }
    else { *msb = IOPorts[addr & 0xff]; *lsb = IOPorts[(addr + 1) & 0xff]; return;}

}

void GGBC::Write8 (unsigned int addr, unsigned char byte) {

    addr &= 0xffff;
    
    if (BreakOnWrite) {
        if (addr == BreakWriteAddr) {
            BreakWriteByte = (unsigned int)byte;
            BreakCode = 4;
        }
    }

    if (addr < 0x8000) {
        switch (ROM_MBC) {
            case MBC1:
                if (addr < 0x2000) {
                    byte = byte & 0x0f; // Only 4 bits are used. Writing 0xa enables SRAM.
                    if (byte == 0x0a) SRAM_Enable = TRUE;
                    else SRAM_Enable = FALSE;
                    return;
                }
                else if (addr < 0x4000) { // Set low 5 bits of bank number
                    ROM_BankOffset &= 0xfff80000;
                    byte = byte & 0x1f;
                    if (byte == 0x00) byte++;
                    ROM_BankOffset |= ((unsigned int)byte * 0x4000);
                    return;
                }
                else if (addr < 0x6000) {
                    byte &= 0x03;
                    if (ROM_MBCMode != 0)
                        SRAM_BankOffset = (unsigned int)byte * 0x2000; // Select RAM bank
                    else {
                        ROM_BankOffset &= 0xffe7c000;
                        ROM_BankOffset |= (unsigned int)byte * 0x80000;
                    }
                    return;
                }
                else {
                    if (ROM_RAMSizeBytes > 8192) ROM_MBCMode = byte & 0x01;
                    else ROM_MBCMode = 0;
                    return;
                }
            case MBC2:
                if (addr < 0x1000) {
                    byte = byte & 0x0f; // Only 4 bits are used. Writing 0xa enables SRAM.
                    if (byte == 0x0a) SRAM_Enable = TRUE;
                    else SRAM_Enable = FALSE;
                    return;
                }
                else if (addr < 0x2100) 
                    return;
                else if (addr < 0x21ff) {
                    byte &= 0x0f;
                    byte &= ROM_ROMBankSelectMask;
                    if (byte == 0) byte++;
                    ROM_BankOffset = (unsigned int)byte * 0x4000;
                    return;
                }
                return;
            case MBC3:
                if (addr < 0x2000) {
                    byte = byte & 0x0f;
                    if (byte == 0x0a) {
                        SRAM_Enable = TRUE; // Also enables timer registers
                        if (BreakOnSRAMEnable) BreakCode = 1;
                    }
                    else {
                        SRAM_Enable = FALSE;
                        if (BreakOnSRAMDisable) BreakCode = 2;
                    }
                    return;
                }
                else if (addr < 0x4000) {
                    byte &= ROM_ROMBankSelectMask;
                    if (byte == 0) byte++;
                    ROM_BankOffset = (unsigned int)byte * 0x4000;
                    return;
                }
                else if (addr < 0x6000) {
                    byte &= 0x0f;
                    if (byte < 0x04) {
                        SRAM_BankOffset = (unsigned int)byte * 0x2000;
                        ROM_TimerMode = 0;
                    }
                    else if ((byte >= 0x08) && (byte < 0x0d))
                        ROM_TimerMode = (unsigned int)byte;
                    else
                        ROM_TimerMode = 0;
                    return;
                }
                else {
                    byte &= 0x01;
                    if ((ROM_TimerLatch == 0x00) && (byte == 0x01))
                        LatchTimerData ();
                    ROM_TimerLatch = (unsigned int)byte;
                    return;
                }
                break;
            case MBC5:
                if (addr < 0x2000) {
                    byte = byte & 0x0f;
                    if (byte == 0x0a) SRAM_Enable = TRUE;
                    else SRAM_Enable = FALSE;
                    return;
                }
                else if (addr < 0x3000) { // Set lower 8 bits of 9-bit reg in MBC5
                    ROM_BankOffset &= 0x00400000;
                    ROM_BankOffset |= (unsigned int)byte * 0x4000;
                    if (ROM_BankOffset == 0) ROM_BankOffset = 0x4000; // Only exclusion with MBC5 is bank 0
                    return;
                }
                else if (addr < 0x4000) { // Set bit 9
                    byte &= 0x01;
                    ROM_BankOffset &= 0x003fc000;
                    if (byte != 0x00) ROM_BankOffset |= 0x00400000;
                    if (ROM_BankOffset == 0) ROM_BankOffset = 0x00004000; // Only exclusion with MBC5 is bank 0
                    return;
                }
                else if (addr < 0x6000) { // Set 4-bit RAM bank register
                    byte &= 0x0f;
                    SRAM_BankOffset = (unsigned int)byte * 0x2000;
                    return;
                }
                // Writing to 0x6000 - 0x7fff does nothing
                return;
        }
    }
    else if (addr < 0xa000) {
        if (addr == 156*16)
            addr &= 0x1fff;
        if (AccessVRAM) {
            addr = addr & 0x1fff;
            VRAM[VRAM_BankOffset + addr] = byte;
            if (addr < 0x1800) { // Pre-calculate pixels in tile set
                addr = addr & 0x1ffe;
                unsigned int Byte1 = 0xff&(unsigned int)VRAM[VRAM_BankOffset + addr];
                unsigned int Byte2 = 0xff&(unsigned int)VRAM[VRAM_BankOffset + addr + 1];
                addr = addr * 4;
                if (VRAM_BankOffset != 0) addr += 24576;
                TileSet[addr++] = ((Byte2 >> 6) & 0x02) +  (Byte1 >> 7);
                TileSet[addr++] = ((Byte2 >> 5) & 0x02) + ((Byte1 >> 6) & 0x01);
                TileSet[addr++] = ((Byte2 >> 4) & 0x02) + ((Byte1 >> 5) & 0x01);
                TileSet[addr++] = ((Byte2 >> 3) & 0x02) + ((Byte1 >> 4) & 0x01);
                TileSet[addr++] = ((Byte2 >> 2) & 0x02) + ((Byte1 >> 3) & 0x01);
                TileSet[addr++] = ((Byte2 >> 1) & 0x02) + ((Byte1 >> 2) & 0x01);
                TileSet[addr++] =  (Byte2 & 0x02) +       ((Byte1 >> 1) & 0x01);
                TileSet[addr] =   ((Byte2 << 1) & 0x02) +  (Byte1 & 0x01);
            }
        }
    }
    else if (addr < 0xc000) {
        if (SRAM_Enable) {
            if (ROM_HasTimer) {
                if (ROM_TimerMode > 0) {
                    LatchTimerData ();
                    fseek (SRAM_File, ROM_RAMSizeBytes + (unsigned int)ROM_TimerMode, SEEK_SET);
                    fwrite (&byte, 1, 1, SRAM_File);
                    ROM_TimerData[(int)(ROM_TimerMode - 0)] = byte;
                }
                else if (SRAM_BankOffset < 0x8000) {
                    SRAM[SRAM_BankOffset + (addr & 0x1fff)] = byte;
                    if (ROM_HasBattery) {
                        addr &= 0x1fff;
                        addr += SRAM_BankOffset;
                        fseek (SRAM_File, addr, SEEK_SET);
                        fwrite (&byte, 1, 1, SRAM_File);
                    }
                }
            }
            else {
                if (ROM_MBC == MBC2) byte &= 0x0f;
                SRAM[SRAM_BankOffset + (addr & 0x1fff)] = byte;
                if (ROM_HasBattery) {
                    addr &= 0x1fff;
                    addr += SRAM_BankOffset;
                    fseek (SRAM_File, addr, SEEK_SET);
                    fwrite (&byte, 1, 1, SRAM_File);
                }
            }
        }
    }
    else if (addr < 0xd000)
        WRAM[addr & 0x0fff] = byte;
    else if (addr < 0xe000)
        WRAM[WRAM_BankOffset + (addr & 0x0fff)] = byte;
    else if (addr < 0xf000)
        WRAM[addr & 0x0fff] = byte;
    else if (addr < 0xfe00)
        WRAM[WRAM_BankOffset + (addr & 0x0fff)] = byte;
    else if (addr < 0xfea0) {
        if (AccessOAM)
            OAM[(addr & 0x00ff) % 160] = byte;
    }
    else if (addr < 0xff00) return; // Unusable
    else if (addr < 0xff80) WriteIO (addr & 0x007f, byte);
    else IOPorts[addr & 0x00ff] = byte;

}

void GGBC::Write16 (unsigned int addr, unsigned char msb, unsigned char lsb) {

    addr &= 0xffff;
    if (addr < 0x8000) {
        Write8 (addr, msb);
        Write8 (addr+1, lsb);
    }
    else if (addr < 0x9fff) {
        if (AccessVRAM) {
            VRAM[VRAM_BankOffset + (addr & 0x1fff)] = msb;
            VRAM[VRAM_BankOffset + ((addr + 1) & 0x1fff)] = lsb;
        }
    }
    else if (addr < 0xbfff) { if (SRAM_Enable) {
        if (ROM_MBC == MBC2) {
            msb &= 0x0f;
            lsb &= 0x0f;
        }
        SRAM[SRAM_BankOffset + (addr & 0x1fff)] = msb;
        SRAM[SRAM_BankOffset + ((addr + 1) & 0x1fff)] = lsb;
        if (ROM_HasBattery) {
            addr &= 0x1fff;
            addr += SRAM_BankOffset;
            fseek (SRAM_File, addr, SEEK_SET);
            fwrite (&msb, 1, 1, SRAM_File);
            fwrite (&lsb, 1, 1, SRAM_File);
        }
    } }
    else if (addr < 0xcfff) { WRAM[addr & 0x0fff] = msb; WRAM[(addr + 1) & 0x0fff] = lsb; }
    else if (addr < 0xdfff) { WRAM[WRAM_BankOffset + (addr & 0x0fff)] = msb; WRAM[WRAM_BankOffset + ((addr + 1) & 0x0fff)] = lsb; }
    else if (addr < 0xefff) { WRAM[addr & 0x0fff] = msb; WRAM[(addr + 1) & 0x0fff] = lsb; }
    else if (addr < 0xfdff) { WRAM[WRAM_BankOffset + (addr & 0x0fff)] = msb; WRAM[WRAM_BankOffset + ((addr + 1) & 0x0fff)] = lsb; }
    else if (addr < 0xfe9f) {
        if (AccessOAM) {
            OAM[(addr & 0x00ff) % 160] = msb;
            OAM[(addr & 0x00ff + 1) % 160] = lsb;
        }
    }
    else if (addr < 0xfeff) { return; } // Unusable
    else if (addr < 0xff7f) { WriteIO (addr & 0x007f, msb); WriteIO ((addr + 1) & 0x007f, lsb); }
    else { IOPorts[addr & 0x00ff] = msb; IOPorts[(addr + 1) & 0x00ff] = lsb; }

}

unsigned char GGBC::ReadIO (unsigned int addr) {

    unsigned char Byte;
    switch (addr) {

        case 0x00: // Used for keypad status
            Byte = IOPorts[0] & 0x30;
            if (Byte == 0x20)
                return KeyDir; // Note that only bits 0-3 are read here
            else if (Byte == 0x10)
                return KeyBut;
            else if ((SGB_MultEnabled != 0x00) && (Byte == 0x30))
                return SGB_ReadJoypadID;
            else
                return 0x0f;
        case 0x01: // Serial data
            return IOPorts[1];
        case 0x02: // Serial control
            return IOPorts[2];
        case 0x11: // NR11
            return IOPorts[0x11] & 0xc0;
        case 0x13: // NR13
            return 0;
        case 0x14: // NR14
            return IOPorts[0x14] & 0x40;
        case 0x16: // NR21
            return IOPorts[0x16] & 0xc0;
        case 0x18: // NR23
            return 0;
        case 0x19: // NR24
            return IOPorts[0x19] & 0x40;
        case 0x1a: // NR30
            return IOPorts[0x1a] & 0x80;
        case 0x1c: // NR32
            return IOPorts[0x14] & 0x60;
        case 0x1d: // NR33
            return 0;
        case 0x1e: // NR34
            return IOPorts[0x1e] & 0x40;
        case 0x23: // NR44
            return IOPorts[0x23] & 0x40;
        case 0x69: // CBG background palette data (using address set by 0xff68)
            if (ROM_GBCFlag == 0) return 0;
            return CGB_BGPalData[CGB_BGPalIndex];
        case 0x6b: // CBG sprite palette data (using address set by 0xff6a)
            if (ROM_GBCFlag == 0) return 0;
            return CGB_OBJPalData[CGB_OBJPalIndex];
        default:
            return IOPorts[addr];

    }
}

void GGBC::WriteIO (unsigned int addr, unsigned char data) {

    unsigned char Byte;
    unsigned int Word, count;

    switch (addr) {

        case 0x00:
            Byte = data & 0x30;
            if (ROM_SGBFlag) {
                if (Byte == 0x00) {
                    if (SGB_ReadingCommand == FALSE) {
                        // Begin command packet transfer:
                        SGB_ReadingCommand = TRUE;
                        SGB_ReadCommandBits = 0;
                        SGB_ReadCommandBytes = 0;
                        SGB_NoPacketsSent = 0;
                        SGB_NoPacketsToSend = 1; // Will get amended later if needed
                    }
                    IOPorts[0] = Byte;
                }
                else if (Byte == 0x20) {
                    IOPorts[0] = Byte;
                    IOPorts[0] |= KeyDir;
                    if (SGB_ReadingCommand) {
                        // Transfer a '0'
                        if (SGB_ReadCommandBytes >= 16) {
                            SGB_NoPacketsSent++;
                            SGB_ReadCommandBytes = 0;
                            if (SGB_NoPacketsSent >= SGB_NoPacketsToSend) {
                                SGBCheckPackets ();
                                SGB_ReadingCommand = FALSE;
                            }
                            break;
                        }
                        SGB_CommandBits[SGB_ReadCommandBits] = 0;
                        SGB_ReadCommandBits++;
                        if (SGB_ReadCommandBits >= 8)
                            SGBCheckByte ();
                        if (SGB_NoPacketsSent >= SGB_NoPacketsToSend) {
                            SGBCheckPackets ();
                            SGB_ReadingCommand = FALSE;
                            SGB_NoPacketsSent = 0;
                            SGB_NoPacketsToSend = 0;
                        }
                    }
                }
                else if (Byte == 0x10) {
                    IOPorts[0] = Byte;
                    IOPorts[0] |= KeyBut;
                    if (SGB_ReadingCommand) {
                        // Transfer a '1' 
                        if (SGB_ReadCommandBytes >= 16) {
                            // Error in transmission - 1 at end of packet
                            SGB_ReadingCommand = FALSE;
                            break;
                        }
                        SGB_CommandBits[SGB_ReadCommandBits] = 1;
                        SGB_ReadCommandBits++;
                        if (SGB_ReadCommandBits >= 8)
                            SGBCheckByte ();
                    }
                }
                else if ((SGB_MultEnabled != 0x00) && (SGB_ReadingCommand == 0)) {
                    if (IOPorts[0] < 0x30) {
                        SGB_ReadJoypadID--;
                        if (SGB_ReadJoypadID < 0x0c) SGB_ReadJoypadID = 0x0f;
                    }
                    IOPorts[0] = Byte;
                }
                else
                    IOPorts[0] = Byte;
            }
            else {
                IOPorts[0] = Byte;
                if (Byte == 0x20)
                    IOPorts[0] |= KeyDir;
                else if (Byte == 0x10)
                    IOPorts[0] |= KeyBut;
            }
            return;
        case 0x01: // Serial data
            Byte = data;
            if ((Serial_IsTransferring == FALSE) || (Serial_ClockIsExternal == TRUE))
                IOPorts[1] = data;
            return;
        case 0x02: // Serial transfer control
            if ((data & 0x80) != 0x00) {
                Byte = data;
                IOPorts[2] = data & 0x83;
                Serial_IsTransferring = TRUE;
                if ((data & 0x01) != 0) {
                    // Attempt to send a byte
                    Serial_ClockIsExternal = FALSE;
                    Serial_Timer = 512 * 1;
                    if (ROM_GBCFlag == 0x00)
                        IOPorts[2] |= 0x02;
                    else if ((data & 0x02) != 0x00)
                        Serial_Timer /= 32;
                }
                else {
                    // Listen for a transfer
                    Serial_ClockIsExternal = TRUE;
                    Serial_Timer = 1;
                }
            }
            else {
                IOPorts[2] = data & 0x83;
                Serial_IsTransferring = FALSE;
                Serial_Request = FALSE;
            }
            return;
        case 0x04: // Divider register (writing resets to 0)
            IOPorts[0x04] = 0x00;
            return;
        case 0x07: // Timer control
            Byte = data & 0x04;
            if (Byte != 0x00) CPU_TimerRunning = TRUE;
            else CPU_TimerRunning = FALSE;
            switch (data & 0x03) {
                case 0: CPU_TimerIncTime = 1024; break;
                case 1: CPU_TimerIncTime = 16; break;
                case 2: CPU_TimerIncTime = 64; break;
                case 3: CPU_TimerIncTime = 256; break;
            }
            IOPorts[0x07] = data & 0x07;
            return;
        case 0x40: // LCD ctrl
            if (data < 128) {
                AccessVRAM = TRUE;
                AccessOAM = TRUE;
                IOPorts[0x44] = 0;
                if (IOPorts[0x40] >= 0x80) {
                    // Try to clear the screen. Be prepared to wait because frame rate is irrelevant when LCD is disabled.
                    if (WaitForSingleObject(hEventFrameComplete, 200) == WAIT_OBJECT_0) {
                        for (addr = 0; addr < 160*144; addr++)
                            ImgData[addr] = 0x000000ff;
                        DrawToFrame1 = FALSE;
                        SetEvent(hEventFrameReady);
						OutputDebugString(L"Frame marked as ready.");
                    }
                }
            }
            else {
                if (IOPorts[0x40] < 0x80) {
                    // Draw to frame now if the memory is free
					if (WaitForSingleObject(hEventFrameComplete, 0) == WAIT_OBJECT_0) {
						DrawToFrame1 = TRUE;
						OutputDebugString(L"LCD enabled - starting new frame.");
					}
                }
            }
            IOPorts[0x40] = data;
            return;
        case 0x41: // LCD status
            IOPorts[0x41] &= 0x07; // Bits 0-2 are read-only. Bit 7 doesn't exist.
            IOPorts[0x41] |= (data & 0x78);
            return;
        case 0x44: // LCD Line No (read-only)
            return;
        case 0x46: // Launch OAM DMA transfer
            IOPorts[0x46] = data;
            if (data < 0x80) return; // Cannot copy from ROM in this way
            Word = ((unsigned int)data) << 8;
            for (count = 0; count < 160; count++) {
                OAM[count] = Read8 (Word);
                Word++;
            }
            return;
        case 0x47: // Mono palette
            IOPorts[0x47] = data;
            TranslatePaletteBG((unsigned int)data);
            return;
        case 0x48: // Mono palette
            IOPorts[0x48] = data;
            TranslatePaletteObj1((unsigned int)data);
            return;
        case 0x49: // Mono palette
            IOPorts[0x49] = data;
            TranslatePaletteObj2((unsigned int)data);
            return;
        case 0x4d: // KEY1 (changes clock speed)
            if (ROM_GBCFlag == FALSE) // Only works for GBC
                return;
            if ((data & 0x01) != 0x00)
                IOPorts[0x4d] |= 0x01;
            else
                IOPorts[0x4d] &= 0x80;
            return;
        case 0x4f: // VRAM bank
            if (ROM_GBCFlag) {
                IOPorts[0x4f] = data & 0x01; // 1-bit register
                VRAM_BankOffset = (unsigned int)(data & 0x01) * 0x2000;
            }
            return;
        case 0x51: // HDMA1
            IOPorts[0x51] = data;
            return;
        case 0x52: // HDMA2
            IOPorts[0x52] = data & 0xf0;
            return;
        case 0x53: // HDMA3
            IOPorts[0x53] = data & 0x1f;
            return;
        case 0x54: // HDMA4
            IOPorts[0x54] = data;
            return;
        case 0x55: // HDMA5 (initiates DMA transfer from ROM or RAM to VRAM)
            if (ROM_GBCFlag == FALSE)
                return;
            if ((data & 0x80) == 0x00) { // General purpose DMA
                if (IOPorts[0x55] != 0xff) { // H-blank DMA already running
                    IOPorts[0x55] = data; // Can be used to halt H-blank DMA
                    return;
                }
                Word = (IOPorts[0x51] << 8) + IOPorts[0x52]; // DMA source
                if ((Word & 0xe000) == 0x8000) return; // Don't do transfers within VRAM
                if (Word >= 0xe000) return; // Don't take source data from these addresses either
                unsigned int Word2 = (IOPorts[0x53] << 8) + IOPorts[0x54] + 0x8000; // DMA destination
                unsigned int BytesToTransfer = data & 0x7f;
                BytesToTransfer++;
                BytesToTransfer *= 16;
                for (count = 0; count < BytesToTransfer; count++) {
                    Write8 (Word2, Read8 (Word));
                    Word++;
                    Word2++;
                    Word2 &= 0x9fff; // Keep it within VRAM
                }
                //if (ClockFreq == GBC_FREQ) clocks_acc -= BytesToTransfer * 4;
                //else clocks_acc -= BytesToTransfer * 2;
                IOPorts[0x55] = 0xff;
            }
            else // H-blank DMA
                IOPorts[0x55] = data;
            return;
        case 0x56: // Infrared
            IOPorts[0x56] = (data & 0xc1) | 0x02; // Setting bit 2 indicates 'no light received'
            return;
        case 0x68: // CGB background palette index
            IOPorts[0x68] = data & 0xbf; // There is no bit 6
            CGB_BGPalIndex = data & 0x3f;
            if ((data & 0x80) != 0x00)
                CGB_BGPalIncr = 1;
            else
                CGB_BGPalIncr = 0;
            return;
        case 0x69: // CBG background palette data (using address set by 0xff68)
            CGB_BGPalData[CGB_BGPalIndex] = data;
            Word = CGB_BGPalData[CGB_BGPalIndex | 0x01];
            Word = Word << 8;
            Word |= CGB_BGPalData[CGB_BGPalIndex & 0xfe];
            CGB_BGPalette[CGB_BGPalIndex/8][(CGB_BGPalIndex/2)&0x03][0] = (GLfloat)(Word & 0x1f) / 31.0f;
            CGB_BGPalette[CGB_BGPalIndex/8][(CGB_BGPalIndex/2)&0x03][1] = (GLfloat)((Word>>5) & 0x1f) / 31.0f;
            CGB_BGPalette[CGB_BGPalIndex/8][(CGB_BGPalIndex/2)&0x03][2] = (GLfloat)((Word>>10) & 0x1f) / 31.0f;
            if (CGB_BGPalIncr) {
                CGB_BGPalIndex++;
                CGB_BGPalIndex &= 0x3f;
                IOPorts[0x68]++;
                IOPorts[0x68] &= 0xbf;
            }
            return;
        case 0x6a: // CGB sprite palette index
            IOPorts[0x6a] = data & 0xbf; // There is no bit 6
            CGB_OBJPalIndex = data & 0x3f;
            if ((data & 0x80) != 0x00)
                CGB_OBJPalIncr = 1;
            else
                CGB_OBJPalIncr = 0;
            return;
        case 0x6b: // CBG sprite palette data (using address set by 0xff6a)
            CGB_OBJPalData[CGB_OBJPalIndex] = data;
            Word = CGB_OBJPalData[CGB_OBJPalIndex | 0x01];
            Word = Word << 8;
            Word |= CGB_OBJPalData[CGB_OBJPalIndex & 0xfe];
            CGB_OBJPalette[CGB_OBJPalIndex/8][(CGB_OBJPalIndex/2)&0x03][0] = (GLfloat)(Word & 0x1f) / 31.0f;
            CGB_OBJPalette[CGB_OBJPalIndex/8][(CGB_OBJPalIndex/2)&0x03][1] = (GLfloat)((Word>>5) & 0x1f) / 31.0f;
            CGB_OBJPalette[CGB_OBJPalIndex/8][(CGB_OBJPalIndex/2)&0x03][2] = (GLfloat)((Word>>10) & 0x1f) / 31.0f;
            if (CGB_OBJPalIncr) {
                CGB_OBJPalIndex++;
                CGB_OBJPalIndex &= 0x3f;
                IOPorts[0x6a]++;
                IOPorts[0x6a] &= 0xbf;
            }
            return;
        case 0x70: // WRAM bank
            if (ROM_GBCFlag == FALSE) return;
            IOPorts[0x70] = data & 0x07;
            if (IOPorts[0x70] == 0x00) IOPorts[0x70]++;
            WRAM_BankOffset = (unsigned int)IOPorts[0x70] * 0x2000;
            return;
        default:
            IOPorts[addr] = data;
            return;

    }
}

void GGBC::LatchTimerData() {

}
