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
*/

#include <string>

#define MAX_SERIALIZE_BUFFER_SIZE 0xFFFF

class CSerializer
{
    public:
        CSerializer();
        CSerializer(char *pBuffer, int iBufferSize);
        ~CSerializer();

        char *m_chBuffer;
        int m_nBufferSize;
        int m_nBufferPos;

        char *GetBuffer();

        void Resize(int iNewSize);

        void Write(bool value);
        void Write(char value);
        void Write(unsigned char value);
        void Write(short value);
        void Write(unsigned short value);
        void Write(int value);
        void Write(unsigned int value);
        void Write(long value);
        void Write(unsigned long value);
        void Write(float value);
        void Write(char *value);
        void Write(char *value, int length);
        void Write(std::string);

        bool Read(bool &value);
        bool Read(char &value);
        bool Read(unsigned char &value);
        bool Read(short &value);
        bool Read(unsigned short &value);
        bool Read(int &value);
        bool Read(unsigned int &value);
        bool Read(long &value);
        bool Read(unsigned long &value);
        bool Read(float &value);
        bool Read(char *value, int length);
        bool Read(char *value); // Creates a new string of read length
        bool Read(std::string &value);

        CSerializer &operator <<(bool value) { Write(value); return *this; }
        CSerializer &operator <<(char value) { Write(value); return *this; }
        CSerializer &operator <<(unsigned char value) { Write(value); return *this; }
        CSerializer &operator <<(short value) { Write(value); return *this; }
        CSerializer &operator <<(unsigned short value) { Write(value); return *this; }
        CSerializer &operator <<(int value) { Write(value); return *this; }
        CSerializer &operator <<(unsigned int value) { Write(value); return *this; }
        CSerializer &operator <<(long value) { Write(value); return *this; }
        CSerializer &operator <<(unsigned long value) { Write(value); return *this; }
        CSerializer &operator <<(float value) { Write(value); return *this; }
        CSerializer &operator <<(char *value) { Write(value); return *this; }
        CSerializer &operator <<(std::string value) { Write(value); return *this; }

        CSerializer &operator >>(bool &value) { Read(value); return *this; }
        CSerializer &operator >>(char &value) { Read(value); return *this; }
        CSerializer &operator >>(unsigned char &value) { Read(value); return *this; }
        CSerializer &operator >>(short &value) { Read(value); return *this; }
        CSerializer &operator >>(unsigned short &value) { Read(value); return *this; }
        CSerializer &operator >>(int &value) { Read(value); return *this; }
        CSerializer &operator >>(unsigned int &value) { Read(value); return *this; }
        CSerializer &operator >>(long &value) { Read(value); return *this; }
        CSerializer &operator >>(unsigned long &value) { Read(value); return *this; }
        CSerializer &operator >>(float &value) { Read(value); return *this; }
        CSerializer &operator >>(char *value) { Read(value); return *this; }
        CSerializer &operator >>(std::string &value) { Read(value); return *this; }
};