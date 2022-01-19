// Baob's shitty Serializer. BS.

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

#include <string>

#define MAX_SERIALIZE_BUFFER_SIZE 0xFFFF

class CSerializer
{
    public:
        CSerializer();
        CSerializer(char *pBuffer, int iBufferSize);
        CSerializer(char *pBuffer); // assumes null-termination
        CSerializer(std::string sBuffer); // Copies the string into a new buffer
        ~CSerializer();

        char *m_chBuffer;
        int m_nBufferSize;
        int m_nBufferPos;

        char *GetBuffer();
        int GetBufferSize();

        void Resize(int iNewSize);

        void WriteBool(bool bValue);
        void WriteChar(char value);
        void WriteUChar(unsigned char value);
        void WriteShort(short value);
        void WriteUShort(unsigned short value);
        void WriteInt(int value);
        void WriteUInt(unsigned int value);
        void WriteLong(long value);
        void WriteULong(unsigned long value);
        void WriteFloat(float value);
        void WriteString(char *value); // Like WriteBytes, but writes the length first
        void WriteSTDString(std::string value); // Like WriteString, but uses std::string
        void WriteBytes(char *value, int iLength);
        void WriteBytes(const char *value, int iLength);

        bool ReadBool(bool &bValue);
        bool ReadChar(char &value);
        bool ReadUChar(unsigned char &value);
        bool ReadShort(short &value);
        bool ReadUShort(unsigned short &value);
        bool ReadInt(int &value);
        bool ReadUInt(unsigned int &value);
        bool ReadLong(long &value);
        bool ReadULong(unsigned long &value);
        bool ReadFloat(float &value);
        bool ReadString(char *value);
        bool ReadSTDString(std::string &value);
        bool ReadBytes(char *value, int iLength);

        CSerializer &operator <<(char value) { WriteChar(value); return *this; }
        CSerializer &operator <<(unsigned char value) { WriteUChar(value); return *this; }
        CSerializer &operator <<(short value) { WriteShort(value); return *this; }
        CSerializer &operator <<(unsigned short value) { WriteUShort(value); return *this; }
        CSerializer &operator <<(int value) { WriteInt(value); return *this; }
        CSerializer &operator <<(unsigned int value) { WriteUInt(value); return *this; }
        CSerializer &operator <<(long value) { WriteLong(value); return *this; }
        CSerializer &operator <<(unsigned long value) { WriteULong(value); return *this; }
        CSerializer &operator <<(float value) { WriteFloat(value); return *this; }
        CSerializer &operator <<(char *value) { WriteString(value); return *this; }
        CSerializer &operator <<(std::string value) { WriteSTDString(value); return *this; }
        // There is no operator << for bytes, because it's ambiguous. Especially so since bytes needs length specified.

        CSerializer &operator >>(char &value) { ReadChar(value); return *this; }
        CSerializer &operator >>(unsigned char &value) { ReadUChar(value); return *this; }
        CSerializer &operator >>(short &value) { ReadShort(value); return *this; }
        CSerializer &operator >>(unsigned short &value) { ReadUShort(value); return *this; }
        CSerializer &operator >>(int &value) { ReadInt(value); return *this; }
        CSerializer &operator >>(unsigned int &value) { ReadUInt(value); return *this; }
        CSerializer &operator >>(long &value) { ReadLong(value); return *this; }
        CSerializer &operator >>(unsigned long &value) { ReadULong(value); return *this; }
        CSerializer &operator >>(float &value) { ReadFloat(value); return *this; }
        CSerializer &operator >>(char *value) { ReadString(value); return *this; }
        CSerializer &operator >>(std::string &value) { ReadSTDString(value); return *this; }
        // There is no operator >> for bytes, because it's ambiguous. Especially so since bytes needs length specified.
};