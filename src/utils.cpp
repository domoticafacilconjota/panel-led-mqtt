#include <utils.h>
#include <constants.h>

// Función para contar el número de dígitos.
int countDigit(long long n) { 
    int count = 0; 
    while (n != 0) { 
        n = n / 10; 
        ++count; 
    } 
    return count; 
}

// Sí, las matemáticas del colegio, sirven para algo.
int alignToCenter (int totalWidth, int fontWidth, int digits) {
    int startAt = (totalWidth / 2) - ((fontWidth * digits) / 2);

    return startAt;
}

static byte lastByte;  // Último caracter del buffer
byte utf8ascii(byte ascii) {
    if (ascii < 128) {
    	lastByte = 0;
        return(ascii);
    }

    byte last = lastByte;
    lastByte = ascii;

    switch (last) {
        case 0xC2: return  (ascii);  break;
        case 0xC3: return  (ascii | 0xC0);  break;
        case 0x82: if(ascii == 0xAC) return (0x80);
    }

    return  (0);
}

String utf8ascii(String stringToConvert) {      
        String stringToReturn = "";
        char charTemp;
        for (int i=0; i < stringToConvert.length(); i++) {
                charTemp = utf8ascii(stringToConvert.charAt(i));
                if (charTemp != 0) stringToReturn += charTemp;
        }
        return stringToReturn;
}

void utf8ascii(char* charToConvert) {      
        int k = 0;
        char charTemp;
        for (int i = 0; i < strlen(charToConvert); i++) {
                charTemp = utf8ascii(charToConvert[i]);
                if (charTemp != 0)
                     charToConvert[k++] = charTemp;
        }
        charToConvert[k] = 0;
}