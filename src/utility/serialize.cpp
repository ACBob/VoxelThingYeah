#include "serialize.hpp"

#include <cstring>

CSerializer::CSerializer() {
    m_chBuffer[0] = '\0';
    m_nBufferSize = 0;
    m_nBufferPos = 0;
}

CSerializer::CSerializer(char *pBuffer, int iBufferSize) {
    m_chBuffer[0] = '\0';
    m_nBufferSize = iBufferSize;
    m_nBufferPos = 0;
    strncpy(m_chBuffer, pBuffer, iBufferSize);
}

CSerializer::~CSerializer() {
}

char *CSerializer::GetBuffer() {
    return m_chBuffer;
}

#define SERIALIZE_WRITE(type) \
    void CSerializer::Write(type value) { \
        if (m_nBufferPos + sizeof(type) > m_nBufferSize) \
            return; \
        memcpy(m_chBuffer + m_nBufferPos, &value, sizeof(type)); \
        m_nBufferPos += sizeof(type); \
    }

SERIALIZE_WRITE(bool);
SERIALIZE_WRITE(char);
SERIALIZE_WRITE(unsigned char);
SERIALIZE_WRITE(short);
SERIALIZE_WRITE(unsigned short);
SERIALIZE_WRITE(int);
SERIALIZE_WRITE(unsigned int);
SERIALIZE_WRITE(long);
SERIALIZE_WRITE(unsigned long);
SERIALIZE_WRITE(float);

void CSerializer::Write(char *value) {
    int length = strlen(value);
    Write(length);
    Write(value, length);
}

void CSerializer::Write(char *value, int length) {
    if (m_nBufferPos + length > m_nBufferSize)
        return;
    memcpy(m_chBuffer + m_nBufferPos, value, length);
    m_nBufferPos += length;
}

#define SERIALIZE_READ(type) \
    bool CSerializer::Read(type &value) { \
        if (m_nBufferPos + sizeof(type) > m_nBufferSize) \
            return false; \
        memcpy(&value, m_chBuffer + m_nBufferPos, sizeof(type)); \
        m_nBufferPos += sizeof(type); \
        return true; \
    }

SERIALIZE_READ(bool);
SERIALIZE_READ(char);
SERIALIZE_READ(unsigned char);
SERIALIZE_READ(short);
SERIALIZE_READ(unsigned short);
SERIALIZE_READ(int);
SERIALIZE_READ(unsigned int);
SERIALIZE_READ(long);
SERIALIZE_READ(unsigned long);
SERIALIZE_READ(float);

bool CSerializer::Read(char *value, int length) {
    if (m_nBufferPos + length > m_nBufferSize)
        return false;
    memcpy(value, m_chBuffer + m_nBufferPos, length);
    m_nBufferPos += length;
    return true;
}

bool CSerializer::Read(char *value) {
    int length;
    if (!Read(length))
        return false;
    value = new char[length + 1];
    value[length] = '\0';
    return Read(value, length);
}