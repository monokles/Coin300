#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <3ds.h>

u32 pathData[3] = {MEDIATYPE_NAND, 0xf000000b, 0};
FS_Path CoinPath = {PATH_BINARY, 0xc, (u8*) pathData};
FS_Archive CoinArchive = {ARCHIVE_SHARED_EXTDATA, CoinPath};
u8 byteHigh = (300 >> 8) & 0xFF;
u8 byteLow = 300 & 0xFF;

void initStuff() {
    gfxInitDefault();
    fsInit();
    consoleInit(GFX_TOP, NULL);
}

void drawStuff() {
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void exitStuff() {
    fsExit();
    gfxExit();
}

void waitForInput() {
    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();
        if (hidKeysDown())
            break;
        drawStuff();
    }
}

int main(int argc, char **argv) {
    initStuff();

    printf("Coin300 brought to you by Monokles\n----------------------------------\n");

    FSUSER_OpenArchive(&CoinArchive);
    printf("Opened archive...\n");

    Handle filehandle;
    Result ret;
    ret = FSUSER_OpenFile(&filehandle, CoinArchive, fsMakePath(PATH_ASCII, "/gamecoin.dat"), FS_OPEN_READ, 0);
    if (ret != 0) {
        printf("FS_USER_OpenFile returned an error: %08x\n", (unsigned int) ret);
        FSFILE_Close(filehandle);
        drawStuff();
        waitForInput();
        exitStuff();
        return 0;
    }
    printf("Opened Coin Data...\n");
    
    
    //Read amt coins
    u8 *buffer;
    buffer = (u8*) malloc(0x400000);
    u32 tmpAmt = 0;
    ret = FSFILE_Read(filehandle, &tmpAmt, 0, buffer, 0x14);
    if (ret != 0 || tmpAmt != 0x14) {
        printf("FS_USER_OpenFile returned an error: %08x\n", (unsigned int) ret);
        printf("amtRead / Size: %016x / %016x\n", tmpAmt, (unsigned int) 0x14);
        FSFILE_Close(filehandle);
        drawStuff();
        waitForInput();
        exitStuff();
        return 0;
    }
    printf("Read Coin data...\n");
    FSFILE_Close(filehandle);   
    u16 coins = (buffer[0x5] << 8) | buffer[0x4];
    printf("You have %u coins.\n", (unsigned int) coins);
    printf("Press any key to continue.\n");
    drawStuff();
    waitForInput();
    
    //Set amt to 300
    buffer[0x4] = byteLow;
    buffer[0x5] = byteHigh;
    ret = FSUSER_OpenFile(&filehandle, CoinArchive, fsMakePath(PATH_ASCII, "/gamecoin.dat"), FS_OPEN_WRITE, 0);
    if (ret != 0 ) {
        printf("FS_USER_OpenFile returned an error: %08x\n", (unsigned int) ret);
        FSFILE_Close(filehandle);
        drawStuff();
        waitForInput();
        exitStuff();
        return 0;
    }
    ret = FSFILE_Write(filehandle, &tmpAmt, 0, buffer, 0x14, FS_WRITE_FLUSH);
    FSFILE_Close(filehandle);
    
    printf("You now have 300 coins.\n");
    printf("Press any key to exit.\n");
    waitForInput();
    printf("Exiting...\n");
    drawStuff();
    exitStuff();

    return 0;
}
