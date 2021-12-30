#include "serialize.hpp"

#include <cstring>

CSerializer::CSerializer() {
    m_nBufferSize = 0;
    m_nBufferPos = 0;

    m_chBuffer = new char[m_nBufferSize];
}

CSerializer::CSerializer(char *pBuffer, int iBufferSize) {
    m_nBufferSize = iBufferSize;
    m_nBufferPos = 0;

    m_chBuffer = pBuffer;
}

CSerializer::~CSerializer() {
    delete[] m_chBuffer;
}

char *CSerializer::GetBuffer() {
    return m_chBuffer;
}

void CSerializer::Resize(int iNewSize) {
    if (iNewSize < m_nBufferSize)
    {
        // Shrink and eradicate any excess data
        m_nBufferSize = iNewSize;
        m_chBuffer[m_nBufferSize] = '\0';
    }

    if (iNewSize > m_nBufferSize) {
        m_nBufferSize = iNewSize;
        m_chBuffer = (char *)realloc(m_chBuffer, m_nBufferSize);
    }
    if (m_nBufferPos > m_nBufferSize) {
        m_nBufferPos = m_nBufferSize;
    }

    m_chBuffer[m_nBufferPos] = '\0';
}

#define SERIALIZE_WRITE(type) \
    void CSerializer::Write(type value) { \
        if (m_nBufferPos + sizeof(type) > m_nBufferSize) \
            if (m_nBufferPos + sizeof(type) > MAX_SERIALIZE_BUFFER_SIZE) \
                return; \
            else \
                Resize(m_nBufferPos + sizeof(type)); \
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
        if (m_nBufferPos + length > MAX_SERIALIZE_BUFFER_SIZE)
            return;
        else
            Resize(m_nBufferPos + length);
    memcpy(m_chBuffer + m_nBufferPos, value, length);
    m_nBufferPos += length;
}

void CSerializer::Write(std::string value) {
    Write(value.c_str());
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

bool CSerializer::Read(std::string &value) {
    char *value_;
    if (!Read(value_))
        return false;
    value = value_;
    delete[] value_;
    return true;
}