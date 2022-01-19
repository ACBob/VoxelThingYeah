#include "serialize.hpp"

#include <cstring>

/*
    Data Types:
        - Boolean, 0x00 = false, 0x01 = true
        - Byte, 0x00 - 0xFF
        - Unsigned Byte, 0x00 - 0xFF
        - Short, 0x0000 - 0xFFFF
        - Unsigned Short, 0x0000 - 0xFFFF
        - Int, 0x00000000 - 0xFFFFFFFF
        - Unsigned Int, 0x00000000 - 0xFFFFFFFF
        - Long, 0x0000000000000000 - 0xFFFFFFFFFFFFFFFF
        - Unsigned Long, 0x0000000000000000 - 0xFFFFFFFFFFFFFFFF
        - Single-Precision 32-bit IEEE 754 floating point
        - UTF-8 String, up to 2^16-1 characters
            Given as a length, followed by the string
            Does NOT include the null terminator
*/

CSerializer::CSerializer() {
    m_nBufferSize = 0;
    m_nBufferPos = 0;

    m_chBuffer = nullptr;
}

CSerializer::CSerializer(char *pBuffer, int iBufferSize) {
    m_nBufferSize = iBufferSize;
    m_nBufferPos = 0;

    m_chBuffer = new char[m_nBufferSize];
    memcpy(m_chBuffer, pBuffer, m_nBufferSize);
}

CSerializer::CSerializer(char *pBuffer) {
    m_nBufferSize = strlen(pBuffer);
    m_nBufferPos = 0;

    m_chBuffer = new char[m_nBufferSize];
    memcpy(m_chBuffer, pBuffer, m_nBufferSize);
}

CSerializer::CSerializer(std::string sBuffer) {
    m_nBufferSize = sBuffer.length();
    m_nBufferPos = 0;

    m_chBuffer = new char[m_nBufferSize];
    memcpy(m_chBuffer, sBuffer.c_str(), m_nBufferSize);
}

CSerializer::~CSerializer() {
    if (m_chBuffer != nullptr) {
        delete[] m_chBuffer;
    }
}

char *CSerializer::GetBuffer() {
    return m_chBuffer;
}

int CSerializer::GetBufferSize() {
    return m_nBufferSize;
}

void CSerializer::Resize(int iNewSize) {
    if (m_chBuffer == nullptr) {
        // We're actually creating a new buffer
        m_chBuffer = new char[iNewSize];
        memset(m_chBuffer, 0, iNewSize);
        m_nBufferPos = 0;
        m_nBufferSize = iNewSize;
        return;
    }

    if (iNewSize > MAX_SERIALIZE_BUFFER_SIZE)
        return;

    char *pNewBuffer = new char[iNewSize];
    memcpy(pNewBuffer, m_chBuffer, m_nBufferSize);
    delete[] m_chBuffer;
    m_chBuffer = pNewBuffer;
    m_nBufferSize = iNewSize;

    if (m_nBufferPos > m_nBufferSize) {
        m_nBufferPos = m_nBufferSize;
    }
}

#define SERIALIZE_WRITE(type, name) \
    void CSerializer::Write##name(type value) { \
        if (m_nBufferPos + sizeof(type) > m_nBufferSize) { \
            Resize(m_nBufferSize + sizeof(type)); \
        } \
        memcpy(m_chBuffer + m_nBufferPos, &value, sizeof(type)); \
        m_nBufferPos += sizeof(type); \
    }

SERIALIZE_WRITE(bool, Bool);
SERIALIZE_WRITE(char, Char);
SERIALIZE_WRITE(unsigned char, UChar);
SERIALIZE_WRITE(short, Short);
SERIALIZE_WRITE(unsigned short, UShort);
SERIALIZE_WRITE(int, Int);
SERIALIZE_WRITE(unsigned int, UInt);
SERIALIZE_WRITE(long, Long);
SERIALIZE_WRITE(unsigned long, ULong);
SERIALIZE_WRITE(float, Float);

void CSerializer::WriteString(char *pString) {
    int iStringLength = strlen(pString);
    WriteInt(iStringLength);
    if (m_nBufferPos + iStringLength > m_nBufferSize) {
        Resize(m_nBufferSize + iStringLength);
    }
    memcpy(m_chBuffer + m_nBufferPos, pString, iStringLength);
    m_nBufferPos += iStringLength;
}

void CSerializer::WriteBytes(char *value, int length) {
    if (m_nBufferPos + length > m_nBufferSize) {
        Resize(m_nBufferSize + length);
    }
    memcpy(m_chBuffer + m_nBufferPos, value, length);
    m_nBufferPos += length;
}

void CSerializer::WriteBytes(const char *value, int length) {
    if (m_nBufferPos + length > m_nBufferSize) {
        Resize(m_nBufferSize + length);
    }
    memcpy(m_chBuffer + m_nBufferPos, value, length);
    m_nBufferPos += length;
}

void CSerializer::WriteSTDString(std::string value) {
    WriteShort(value.length());
    WriteBytes(value.c_str(), value.length());
}

#define SERIALIZE_READ(type, name) \
    bool CSerializer::Read##name (type &value) { \
        if (m_nBufferPos + sizeof(type) > m_nBufferSize) { \
            return false; \
        } \
        memcpy(&value, m_chBuffer + m_nBufferPos, sizeof(type)); \
        m_nBufferPos += sizeof(type); \
        return true; \
    }

SERIALIZE_READ(bool, Bool);
SERIALIZE_READ(char, Char);
SERIALIZE_READ(unsigned char, UChar);
SERIALIZE_READ(short, Short);
SERIALIZE_READ(unsigned short, UShort);
SERIALIZE_READ(int, Int);
SERIALIZE_READ(unsigned int, UInt);
SERIALIZE_READ(long, Long);
SERIALIZE_READ(unsigned long, ULong);
SERIALIZE_READ(float, Float);

bool CSerializer::ReadString(char *value) {
    unsigned short iStringLength;
    if (!ReadUShort(iStringLength)) {
        return false;
    }

    if (m_nBufferPos + iStringLength > m_nBufferSize) {
        return false;
    }

    memcpy(value, m_chBuffer + m_nBufferPos, iStringLength);
    value[iStringLength] = '\0';
    m_nBufferPos += iStringLength;
    return true;
}

bool CSerializer::ReadSTDString(std::string &value) {
    unsigned short iStringLength;
    if (!ReadUShort(iStringLength)) {
        return false;
    }

    if (m_nBufferPos + iStringLength > m_nBufferSize) {
        return false;
    }

    value.assign(m_chBuffer + m_nBufferPos, iStringLength);
    m_nBufferPos += iStringLength;
    return true;
}

bool CSerializer::ReadBytes(char *value, int length) {
    if (m_nBufferPos + length > m_nBufferSize) {
        return false;
    }

    memcpy(value, m_chBuffer + m_nBufferPos, length);
    m_nBufferPos += length;
    return true;
}