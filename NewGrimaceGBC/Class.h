
#define UNICODE
#include <Windows.h>
#include <gl/GL.h>
#include <stdio.h>
#include "glext.h"

#define GB_FREQ  4194304
#define SGB_FREQ 4295454
#define GBC_FREQ 8400000

#define MBC_NONE 0x00
#define MBC1     0x01
#define MBC2     0x02
#define MBC3     0x03
#define MBC4     0x04
#define MBC5     0x05
#define MMM01    0x11

extern GLuint* ImgData;
extern BOOL OpenGLRunning;
extern HANDLE hEventFrameReady;
extern HANDLE hEventFrameComplete;
extern HANDLE hGLRunning;
extern BOOL DrawToFrame1;

// OpenGL related stuff
class GrimOpenGL {

public:

    // Functions
    static void BeginOpenGL(HDC);
    static void CloseOpenGL();
    static void SetResize(int, int);
    static DWORD WINAPI RunProc(LPVOID);

};

// GBC class
class GGBC {

public:
    
    // Debugging parameters
    int BreakCode = 0;
    int TotalBreakEnables = 0;
    int BreakOnSRAMEnable = 0;
    int BreakOnSRAMDisable = 0;
    int BreakOnPC = 0;
    int BreakOnWrite = 0;
    int BreakOnRead = 0;
    char BreakMsg[25];
    unsigned int BreakLastCallTo = 0;
    unsigned int BreakLastCallAt = 0;
    unsigned int BreakLastCallReturned = 0;
    unsigned int BreakPCAddr = 0x0100;
    unsigned int BreakWriteAddr = 0x0000;
    unsigned int BreakWriteByte = 0;
    unsigned int BreakReadAddr = 0x0000;
    unsigned int BreakReadByte = 0;

    // Over-/under-clocking
    int ClockMultiply = 1;
    int ClockDivide = 1;

    // Block memory accessible by debug window
    unsigned char* ROM;
    unsigned char* WRAM;
    unsigned char* SRAM;
    unsigned char* VRAM;
    unsigned char* IOPorts;
    
    // Sprite data
    unsigned char OAM[160];

    // Colour GB palettes
    GLfloat CGB_BGPalette[8][4][3];
    GLfloat CGB_OBJPalette[8][4][3];

    // ROM stats
    BOOL ROM_Valid;
    unsigned int ROM_BankOffset;
    unsigned int SRAM_BankOffset;
    char ROM_Title[17];
    unsigned int ROM_MBC;
    unsigned char ROM_GBCFlag;
    unsigned char ROM_SGBFlag;
    BOOL ROM_HasSRAM;
    BOOL ROM_HasBattery;
    BOOL ROM_HasTimer;
    BOOL ROM_HasRumble;
    long ROM_ROMSizeBytes;
    long ROM_RAMSizeBytes;
    unsigned char ROM_ROMBankSelectMask;
    unsigned char ROM_RAMBankSelectMask;

    // CPU registers
    unsigned int CPU_PC;
    unsigned int CPU_SP;
    unsigned char CPU_A;
    unsigned char CPU_F;
    unsigned char CPU_B;
    unsigned char CPU_C;
    unsigned char CPU_D;
    unsigned char CPU_E;
    unsigned char CPU_H;
    unsigned char CPU_L;

    // Public members
    BOOL Running;
    int KeyBut, KeyDir;
    BOOL KeyStateChanged;

    // Constructor/deconstructor
    GGBC();
    ~GGBC();

    // Other public functions
    void Begin();
    void LoadROM(const wchar_t*, HWND);
    void Reset();
    void Close();

private:

    // Private functions
    static DWORD WINAPI RunProc(LPVOID);
    int Ex(int);
    unsigned char Read8(unsigned int);
    void Read16(unsigned int, unsigned char*, unsigned char*);
    void Write8(unsigned int, unsigned char);
    void Write16(unsigned int, unsigned char, unsigned char);
    unsigned char ReadIO(unsigned int);
    void WriteIO(unsigned int, unsigned char);
    void TranslatePaletteBG(unsigned int);
    void TranslatePaletteObj1(unsigned int);
    void TranslatePaletteObj2(unsigned int);
    void LatchTimerData();
    void SGBCheckByte();
    void SGBCheckPackets();
    void SGB_ColouriseFrame();

    // Running mutex
    HANDLE hRunning;

    // CPU stats
    int ClocksAcc;
    int ClocksRun;
    int CPU_ClockFreq;
    int GPU_ClockFactor;
    int GPU_TimeInMode;
    int GPU_Mode;
    BOOL CPU_IME;
    BOOL CPU_Halted;
    BOOL CPU_Stopped;
    unsigned int CPU_DividerCount;
    unsigned int CPU_TimerCount;
    unsigned CPU_TimerIncTime;
    BOOL CPU_TimerRunning;
    BOOL Serial_Request;
    BOOL Serial_IsTransferring;
    BOOL Serial_ClockIsExternal;
    int Serial_Timer;

    // ROM stats
    int ROM_MBCMode;
    unsigned char ROM_CartType;
    unsigned char ROM_CheckSum;
    unsigned char ROM_ROMSize;
    unsigned char ROM_RAMSize;
    unsigned char ROM_TimerData[5];
    unsigned int ROM_TimerMode;
    unsigned int ROM_TimerLatch;

    // RAM stats
    BOOL AccessOAM;
    unsigned int WRAM_BankOffset;

    // VRAM stats
    BOOL AccessVRAM;
    unsigned int VRAM_BankOffset;

    // SRAM stats
    FILE* SRAM_File;
    BOOL SRAM_Enable;

    // SGB stats
    BOOL SGB_ReadingCommand;
    unsigned int SGB_CommandBytes[7][16];
    unsigned char SGB_CommandBits[8];
    unsigned int SGB_Command;
    int SGB_ReadCommandBits;
    int SGB_ReadCommandBytes;
    BOOL SGB_FreezeScreen;
    unsigned int SGB_FreezeMode;
    BOOL SGB_MultEnabled;
    unsigned int SGB_NoPlayers;
    unsigned int SGB_NoPacketsSent;
    unsigned int SGB_NoPacketsToSend;
    unsigned int SGB_ReadJoypadID;
    unsigned int *SGB_CHRPalettes;

    // CGB stats
    unsigned char CGB_BGPalData[64];
    unsigned int CGB_BGPalIndex;
    unsigned int CGB_BGPalIncr;
    unsigned char CGB_OBJPalData[64];
    unsigned int CGB_OBJPalIndex;
    unsigned int CGB_OBJPalIncr;

    // Block memory
    unsigned int* TileSet;
    GLfloat *SGB_Palettes;
    GLfloat* SGB_SYSPalettes;

    // Other variables
    unsigned int LastLYCompare;
    BOOL BlankedScreen;
    BOOL NeedClear;

    // Line-processing functions
    void (GGBC::*ReadLine) ();
    void ReadLine_GB ();
    void ReadLine_SGB ();
    void ReadLine_GBC ();

};

// Menu related stuff
class MenuUtil{

public:

    static void AddGGBCMenu(HWND, HINSTANCE);
    static LRESULT CALLBACK Command(HWND, WORD);
    static void LoadROMDetails(GGBC*);
    static void LoadMemoryDetails(int, GGBC*);
    static void LoadBreakDetails(GGBC*);
    static void DecodeBreakPCAddress(GGBC*);
    static void DecodeBreakWriteAddress(GGBC*);
    static void DecodeBreakReadAddress(GGBC*);

};
