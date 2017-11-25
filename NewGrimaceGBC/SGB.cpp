
#include "Class.h"

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

void GGBC::SGBCheckByte() {
    
    SGB_ReadCommandBits = 0;
    SGB_CommandBytes[SGB_NoPacketsSent][SGB_ReadCommandBytes] = 
        SGB_CommandBits[0] | (SGB_CommandBits[1] << 1) | 
        (SGB_CommandBits[2] << 2) | (SGB_CommandBits[3] << 3) | 
        (SGB_CommandBits[4] << 4) | (SGB_CommandBits[5] << 5) | 
        (SGB_CommandBits[6] << 6) | (SGB_CommandBits[7] << 7);
    SGB_ReadCommandBytes++;
    
    if ((SGB_ReadCommandBytes == 1) && (SGB_NoPacketsSent == 0)) {
        // This is first command byte
        SGB_NoPacketsToSend = SGB_CommandBytes[0][0] & 0x07;
        SGB_Command = (SGB_CommandBytes[0][0] >> 3) & 0x1f;
    }
    
}

void GGBC::SGBCheckPackets() {
    
    unsigned int ByteIndex;
    unsigned int SentDataSets;

    unsigned int CtrlCode, DataGroups, GroupNo, PacketNo, ByteNo;
    unsigned int p, c, dp;
    GLfloat PalCol;
    
    unsigned int xLeft, xRight, yTop, yBottom;
    unsigned int x, y;
    unsigned int x_max, y_max;

    switch (SGB_Command) {
        case SGBCOM_PAL01:
            SentDataSets = (SGB_CommandBytes[0][2]<<8) + SGB_CommandBytes[0][1];
            SGB_Palettes[0] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[1] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[2] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SGB_Palettes[12] = SGB_Palettes[0];
            SGB_Palettes[13] = SGB_Palettes[1];
            SGB_Palettes[14] = SGB_Palettes[2];
            SGB_Palettes[24] = SGB_Palettes[0];
            SGB_Palettes[25] = SGB_Palettes[1];
            SGB_Palettes[26] = SGB_Palettes[2];
            SGB_Palettes[36] = SGB_Palettes[0];
            SGB_Palettes[37] = SGB_Palettes[1];
            SGB_Palettes[38] = SGB_Palettes[2];
            SentDataSets = (SGB_CommandBytes[0][4]<<8) + SGB_CommandBytes[0][3];
            SGB_Palettes[3] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[4] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[5] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][6]<<8) + SGB_CommandBytes[0][5];
            SGB_Palettes[6] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[7] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[8] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][8]<<8) + SGB_CommandBytes[0][7];
            SGB_Palettes[9] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[10] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[11] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][10]<<8) + SGB_CommandBytes[0][9];
            SGB_Palettes[15] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[16] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[17] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][12]<<8) + SGB_CommandBytes[0][11];
            SGB_Palettes[18] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[19] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[20] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][14]<<8) + SGB_CommandBytes[0][13];
            SGB_Palettes[21] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[22] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[23] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            break;
        case SGBCOM_PAL23:
            SentDataSets = (SGB_CommandBytes[0][2]<<8) + SGB_CommandBytes[0][1];
            SGB_Palettes[0] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[1] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[2] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SGB_Palettes[12] = SGB_Palettes[0];
            SGB_Palettes[13] = SGB_Palettes[1];
            SGB_Palettes[14] = SGB_Palettes[2];
            SGB_Palettes[24] = SGB_Palettes[0];
            SGB_Palettes[25] = SGB_Palettes[1];
            SGB_Palettes[26] = SGB_Palettes[2];
            SGB_Palettes[36] = SGB_Palettes[0];
            SGB_Palettes[37] = SGB_Palettes[1];
            SGB_Palettes[38] = SGB_Palettes[2];
            SentDataSets = (SGB_CommandBytes[0][4]<<8) + SGB_CommandBytes[0][3];
            SGB_Palettes[27] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[28] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[29] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][6]<<8) + SGB_CommandBytes[0][5];
            SGB_Palettes[30] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[31] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[32] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][8]<<8) + SGB_CommandBytes[0][7];
            SGB_Palettes[33] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[34] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[35] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][10]<<8) + SGB_CommandBytes[0][9];
            SGB_Palettes[39] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[40] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[41] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][12]<<8) + SGB_CommandBytes[0][11];
            SGB_Palettes[42] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[43] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[44] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][14]<<8) + SGB_CommandBytes[0][13];
            SGB_Palettes[45] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[46] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[47] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            break;
        case SGBCOM_PAL03:
            SentDataSets = (SGB_CommandBytes[0][2]<<8) + SGB_CommandBytes[0][1];
            SGB_Palettes[0] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[1] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[2] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SGB_Palettes[12] = SGB_Palettes[0];
            SGB_Palettes[13] = SGB_Palettes[1];
            SGB_Palettes[14] = SGB_Palettes[2];
            SGB_Palettes[24] = SGB_Palettes[0];
            SGB_Palettes[25] = SGB_Palettes[1];
            SGB_Palettes[26] = SGB_Palettes[2];
            SGB_Palettes[36] = SGB_Palettes[0];
            SGB_Palettes[37] = SGB_Palettes[1];
            SGB_Palettes[38] = SGB_Palettes[2];
            SentDataSets = (SGB_CommandBytes[0][4]<<8) + SGB_CommandBytes[0][3];
            SGB_Palettes[3] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[4] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[5] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][6]<<8) + SGB_CommandBytes[0][5];
            SGB_Palettes[6] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[7] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[8] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][8]<<8) + SGB_CommandBytes[0][7];
            SGB_Palettes[9] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[10] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[11] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][10]<<8) + SGB_CommandBytes[0][9];
            SGB_Palettes[39] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[40] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[41] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][12]<<8) + SGB_CommandBytes[0][11];
            SGB_Palettes[42] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[43] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[44] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][14]<<8) + SGB_CommandBytes[0][13];
            SGB_Palettes[45] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[46] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[47] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            break;
        case SGBCOM_PAL12:
            SentDataSets = (SGB_CommandBytes[0][2]<<8) + SGB_CommandBytes[0][1];
            SGB_Palettes[0] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[1] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[2] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SGB_Palettes[12] = SGB_Palettes[0];
            SGB_Palettes[13] = SGB_Palettes[1];
            SGB_Palettes[14] = SGB_Palettes[2];
            SGB_Palettes[24] = SGB_Palettes[0];
            SGB_Palettes[25] = SGB_Palettes[1];
            SGB_Palettes[26] = SGB_Palettes[2];
            SGB_Palettes[36] = SGB_Palettes[0];
            SGB_Palettes[37] = SGB_Palettes[1];
            SGB_Palettes[38] = SGB_Palettes[2];
            SentDataSets = (SGB_CommandBytes[0][4]<<8) + SGB_CommandBytes[0][3];
            SGB_Palettes[15] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[16] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[17] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][6]<<8) + SGB_CommandBytes[0][5];
            SGB_Palettes[18] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[19] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[20] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][8]<<8) + SGB_CommandBytes[0][7];
            SGB_Palettes[21] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[22] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[23] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][10]<<8) + SGB_CommandBytes[0][9];
            SGB_Palettes[27] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[28] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[29] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][12]<<8) + SGB_CommandBytes[0][11];
            SGB_Palettes[30] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[31] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[32] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            SentDataSets = (SGB_CommandBytes[0][14]<<8) + SGB_CommandBytes[0][13];
            SGB_Palettes[33] = (GLfloat)(SentDataSets & 0x001f) / 31;
            SGB_Palettes[34] = (GLfloat)(SentDataSets & 0x03e0) / 992;
            SGB_Palettes[35] = (GLfloat)(SentDataSets & 0x7c00) / 31744;
            break;
        case SGBCOM_ATTR_BLK:
            // Get number of data groups
            DataGroups = SGB_CommandBytes[0][1] & 0x07;
            PacketNo = 0;
            ByteNo = 2;
            for (GroupNo = 0; GroupNo < DataGroups; GroupNo++) {
                // Get control code and colour palette for this data group
                CtrlCode = SGB_CommandBytes[PacketNo][ByteNo++] & 0x07;
                dp = SGB_CommandBytes[PacketNo][ByteNo++] & 0x07;
                if (ByteNo >= 16) {ByteNo = 0; PacketNo++;}
                // Get coordinates
                xLeft = SGB_CommandBytes[PacketNo][ByteNo++] & 0x1f;
                yTop = SGB_CommandBytes[PacketNo][ByteNo++] & 0x1f;
                if (ByteNo >= 16) {ByteNo = 0; PacketNo++;}
                xRight = SGB_CommandBytes[PacketNo][ByteNo++] & 0x1f;
                yBottom = SGB_CommandBytes[PacketNo][ByteNo++] & 0x1f;
                if (ByteNo >= 16) {ByteNo = 0; PacketNo++;}
                // Invalid conditions may cause errors in my program:
                if (xLeft > 19) break;
                if (xRight > 19) xRight = 19;
                if (yTop > 17) break;
                if (yBottom > 17) yBottom = 17;
                if (xLeft > xRight) break;
                if (yTop > yBottom) break;
                // Assign the palettes to screen segments
                if (CtrlCode > 3) {
                    // Fill area outside block
                    p = (dp & 0x30) >> 4;
                    for (y = 0; y < 18; y++) {
                        for (x = 0; x < xLeft; x++)
                            SGB_CHRPalettes[y*20+x] = p;
                        for (x = xRight + 1; x < 20; x++)
                            SGB_CHRPalettes[y*20+x] = p;
                    }
                    for (x = xLeft; x <= xRight; x++) {
                        for (y = 0; y < yTop; y++)
                            SGB_CHRPalettes[y*20+x] = p;
                        for (y = yBottom + 1; y < 18; y++)
                            SGB_CHRPalettes[y*20+x] = p;
                    }
                }
                if ((CtrlCode & 0x01) > 0) {
                    // Fill area inside block
                    p = dp & 0x03;
                    for (y = yTop + 1; y < yBottom; y++)
                        for (x = xLeft + 1; x < xRight; x++)
                            SGB_CHRPalettes[y*20+x] = p;
                }
                if (CtrlCode > 0 && CtrlCode != 5) {
                    if (CtrlCode == 1) p = dp & 0x03;
                    else if (CtrlCode == 4) p = (dp & 0x30) >> 4;
                    else p = (dp & 0x0c) >> 2;
                    for (y = yTop; y <= yBottom; y++) {
                        SGB_CHRPalettes[y*20+xLeft] = p;
                        SGB_CHRPalettes[y*20+xRight] = p;
                    }
                    for (x = xLeft; x <= xRight; x++) {
                        SGB_CHRPalettes[yTop*20+x] = p;
                        SGB_CHRPalettes[yBottom*20+x] = p;
                    }
                }
            }
            break;
        case SGBCOM_ATTR_LIN:
            SGB_Command = 0;
            break;
        case SGBCOM_ATTR_DIV:
            if ((SGB_CommandBytes[0][1] & 0x40) != 0x00) {
                y_max = (unsigned int)(SGB_CommandBytes[0][2] & 0x1f);
                dp = (SGB_CommandBytes[0][1] & 0x0c) >> 2;
                if (y_max < 18) {
                    for (y = 0; y < y_max; y++)
                        for (x = 0; x < 20; x++)
                            SGB_CHRPalettes[y*20+x] = dp;
                }
                else {
                    for (y = 0; y < 18; y++)
                        for (x = 0; x < 20; x++)
                            SGB_CHRPalettes[y*20+x] = dp;
                    break;
                }
                dp = (SGB_CommandBytes[0][1] & 0x30) >> 4;
                for (x = 0; x < 20; x++)
                    SGB_CHRPalettes[y_max*20+x] = dp;
                dp = SGB_CommandBytes[0][1] & 0x03;
                for (y = y_max; y < 18; y++)
                    for (x = 0; x < 20; x++)
                        SGB_CHRPalettes[y*20+x] = dp;
            }
            else {
                x_max = (unsigned int)(SGB_CommandBytes[0][2] & 0x1f);
                dp = (SGB_CommandBytes[0][1] & 0x0c) >> 2;
                if (x_max < 20) {
                    for (y = 0; y < 18; y++)
                        for (x = 0; x < x_max; x++)
                            SGB_CHRPalettes[y*20+x] = dp;
                }
                else {
                    for (y = 0; y < 18; y++)
                        for (x = 0; x < 20; x++)
                            SGB_CHRPalettes[y*20+x] = dp;
                    break;
                }
                dp = (SGB_CommandBytes[0][1] & 0x30) >> 4;
                for (y = 0; y < 18; y++)
                    SGB_CHRPalettes[y*20+x_max] = dp;
                dp = SGB_CommandBytes[0][1] & 0x03;
                for (y = 0; y < 18; y++)
                    for (x = x_max; x < 20; x++)
                        SGB_CHRPalettes[y*20+x] = dp;
            }
            break;
        case SGBCOM_ATTR_CHR:
            xLeft = SGB_CommandBytes[0][1] & 0x1f;
            yTop = SGB_CommandBytes[0][2] & 0x1f;
            if (xLeft > 19) break;
            if (yTop > 17) break;
            SentDataSets = SGB_CommandBytes[0][4] & 0x01;
            SentDataSets *= 256;
            SentDataSets += SGB_CommandBytes[0][3];
            CtrlCode = SGB_CommandBytes[0][5] & 0x01; // Write hori/vert
            dp = 0; // Number of bytes used so far
            ByteIndex = 0; // Position within bytes (one of 4 pal no's)
            x = xLeft;
            y = yTop;
            p = 0; // Packet no.
            c = 6; // Byte in packet
            if (CtrlCode == 0x00) { // Horizontal write
                while (dp < SentDataSets) {
                    if (ByteIndex == 0) SGB_CHRPalettes[y*20+x] = (SGB_CommandBytes[p][c] & 0xc0) >> 6;
                    else if (ByteIndex == 1) SGB_CHRPalettes[y*20+x] = (SGB_CommandBytes[p][c] & 0x30) >> 4;
                    else if (ByteIndex == 2) SGB_CHRPalettes[y*20+x] = (SGB_CommandBytes[p][c] & 0x0c) >> 2;
                    else SGB_CHRPalettes[y*20+x] = SGB_CommandBytes[p][c] & 0x03;
                    x++;
                    if (x >= 20) {
                        x = xLeft;
                        y++;
                        if (y >= 18) break;
                    }
                    ByteIndex++;
                    if (ByteIndex >= 4) {
                        ByteIndex = 0;
                        c++;
                        if (c >= 16) {
                            c = 0;
                            p++;
                            if (p >= SGB_NoPacketsSent) break;
                        }
                        dp++;
                    }
                }
            }
            else { // Vertical write
                while (dp < SentDataSets) {
                    if (ByteIndex == 0) SGB_CHRPalettes[y*20+x] = (SGB_CommandBytes[p][c] & 0xc0) >> 6;
                    else if (ByteIndex == 1) SGB_CHRPalettes[y*20+x] = (SGB_CommandBytes[p][c] & 0x30) >> 4;
                    else if (ByteIndex == 2) SGB_CHRPalettes[y*20+x] = (SGB_CommandBytes[p][c] & 0x0c) >> 2;
                    else SGB_CHRPalettes[y*20+x] = SGB_CommandBytes[p][c] & 0x03;
                    y++;
                    if (y >= 18) {
                        y = yTop;
                        x++;
                        if (x >= 20) break;
                    }
                    ByteIndex++;
                    if (ByteIndex >= 4) {
                        ByteIndex = 0;
                        c++;
                        if (c >= 16) {
                            c = 0;
                            p++;
                            if (p >= SGB_NoPacketsSent) break;
                        }
                        dp++;
                    }
                }
            }
            break;
        case SGBCOM_SOUND:
            SGB_Command = 0;
            break;
        case SGBCOM_PAL_SET:
            for (p = 0; p < 4; p++) {
                dp = (unsigned int)(SGB_CommandBytes[0][p*2+2] & 0x01);
                dp *= 256;
                dp += (unsigned int)(SGB_CommandBytes[0][p*2+1]);
                for (c = 0; c < 4; c++) {
                    ByteIndex = p*12 + c*3;
                    GroupNo = dp*12 + c*3;
                    SGB_Palettes[ByteIndex  ] = SGB_SYSPalettes[GroupNo  ];
                    SGB_Palettes[ByteIndex+1] = SGB_SYSPalettes[GroupNo+1];
                    SGB_Palettes[ByteIndex+2] = SGB_SYSPalettes[GroupNo+2];
                }
            }
            break;
        case SGBCOM_PAL_TRN:
            if (IOPorts[0x40] < 128)
                // Won't work if display signal is disabled. Shouldn't ever be a problem.
                break;
            if ((IOPorts[0x40] & 0x10) == 0) {
                // BG character data stored in 0x8800-0x97ff
                dp = 0x0800;
                for (p = 0; p < 512; p++) {
                    for (c = 0; c < 4; c++) {
                        GroupNo = p*12 + c*3;
                        PalCol = (GLfloat)(VRAM[dp] & 0x1f);
                        SGB_SYSPalettes[GroupNo  ] = PalCol / 31;
                        PalCol = (GLfloat)((VRAM[dp++] & 0xe0) >> 5);
                        PalCol += (GLfloat)((VRAM[dp] & 0x03) << 3);
                        SGB_SYSPalettes[GroupNo+1] = PalCol / 31;
                        PalCol = (GLfloat)((VRAM[dp++] & 0x7c) >> 2);
                        SGB_SYSPalettes[GroupNo+2] = PalCol / 31;
                    }
                }
            }
            else {
                // BG character data stored in 0x8000-0x8fff
                dp = 0;
                for (p = 0; p < 512; p++) {
                    for (c = 0; c < 4; c++) {
                        GroupNo = p*12 + c*3;
                        PalCol = (GLfloat)(VRAM[dp] & 0x1f);
                        SGB_SYSPalettes[GroupNo  ] = PalCol / 31;
                        PalCol = (GLfloat)((VRAM[dp++] & 0xe0) >> 5);
                        PalCol += (GLfloat)((VRAM[dp] & 0x03) << 3);
                        SGB_SYSPalettes[GroupNo+1] = PalCol / 31;
                        PalCol = (GLfloat)((VRAM[dp++] & 0x7c) >> 2);
                        SGB_SYSPalettes[GroupNo+2] = PalCol / 31;
                    }
                }
            }
            break;
        case SGBCOM_ICON_EN:
            SGB_Command = 0;
            break;
        case SGBCOM_DATA_SEND:
            SGB_Command = 0;
            break;
        case SGBCOM_MLT_REQ:
            SGB_MultEnabled = SGB_CommandBytes[0][1] & 0x01;
            SGB_NoPlayers = SGB_CommandBytes[0][1] + 0x01;
            SGB_ReadJoypadID = 0x0f;
            break;
        case SGBCOM_CHR_TRN:
            SGB_Command = 0;
            break;
        case SGBCOM_PCT_TRN:
            SGB_Command = 0;
            break;
        case SGBCOM_MASK_EN:
            if (SGB_CommandBytes[0][1] == 0x00)
                SGB_FreezeScreen = 0;
            else if (SGB_CommandBytes[0][1] < 0x04)
                SGB_FreezeScreen = 1;
            else
                SGB_FreezeScreen = 0;
            SGB_FreezeMode = SGB_CommandBytes[0][1];
            break;
        case SGBCOM_PAL_PRI:
            SGB_Command = 0;
            break;
        default:
            break;
    }
    
}
