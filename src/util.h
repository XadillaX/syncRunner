#ifdef _WIN32
#include <string>
using namespace std;

bool IsUTF8(const void* pBuffer, long size)
{  
    bool IsUTF8 = true;  
    unsigned char* start = (unsigned char*)pBuffer;  
    unsigned char* end = (unsigned char*)pBuffer + size;  
    while (start < end)
    {  
        if (*start < 0x80) // (10000000): 值小于0x80的为ASCII字符
        {  
            start++;  
        }  
        else if (*start < (0xC0)) // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
        {  
            IsUTF8 = false;  
            break;  
        }  
        else if (*start < (0xE0)) // (11100000): 此范围内为2字节UTF-8字符  
        {  
            if (start >= end - 1)   
                break;  
            if ((start[1] & (0xC0)) != 0x80)  
            {  
                IsUTF8 = false;  
                break;  
            }  
            start += 2;  
        }   
        else if (*start < (0xF0)) // (11110000): 此范围内为3字节UTF-8字符  
        {  
            if (start >= end - 2)   
                break;  
            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)  
            {  
                IsUTF8 = false;  
                break;  
            }  
            start += 3;  
        }   
        else  
        {  
            IsUTF8 = false;  
            break;  
        }  
    }  
    return IsUTF8;  
}

string ACPToUTF8(const char* pStrACP)  
{  
    // 先转成宽字符  
    int nStrLen = MultiByteToWideChar(CP_ACP, 0, pStrACP, -1, NULL, 0);  
    wchar_t* pWStr = new wchar_t[nStrLen + 1];  
    memset(pWStr, 0, nStrLen + 1);  
    MultiByteToWideChar(CP_ACP, 0, pStrACP, -1, pWStr, nStrLen);  
  
    // 再转为UTF8  
    nStrLen = WideCharToMultiByte(CP_UTF8, 0, pWStr, -1, NULL, 0, NULL, NULL);  
    char* pStr = new char[nStrLen + 1];  
    memset(pStr, 0, nStrLen + 1);  
    WideCharToMultiByte(CP_UTF8, 0, pWStr, -1, pStr, nStrLen, NULL, NULL);  
    if(pWStr)  
    {  
        delete[] pWStr;
    }

    string tmp = pStr;
    delete[] pStr;

    return tmp;  
}
#endif
