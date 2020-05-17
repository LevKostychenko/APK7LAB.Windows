#include "SerialPort.h"

SerialPort::SerialPort(const char* portName)
{
    this->connected = false;

    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (this->handler == INVALID_HANDLE_VALUE) 
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) 
        {
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        }
        else
        {
            printf("ERROR!!!");
        }
    }
    else 
    {
        DCB com_parameters = { 0 };

        if (!GetCommState(this->handler, &com_parameters))
        {
            printf("failed to get current serial parameters");
        }
        else 
        {
            com_parameters.BaudRate = CBR_115200;
            com_parameters.ByteSize = 8;
            com_parameters.StopBits = ONESTOPBIT;
            com_parameters.Parity = NOPARITY;
            com_parameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &com_parameters))
            {
                printf("ALERT: could not set Serial port parameters\n");
            }
            else 
            {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
}

SerialPort::SerialPort(const SerialPort& port)
{
    this->handler = port.handler;
    this->errors = port.errors;
    this->connected = port.connected;
}

SerialPort::~SerialPort()
{
    if (this->connected) 
    {
        this->connected = false;
    }
}

int SerialPort::readSerialPort(char* buffer, unsigned int buf_size)
{
    DWORD bytes_read;
    unsigned int to_read = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0) 
    {
        if (this->status.cbInQue > buf_size) 
        {
            to_read = buf_size;
        }
        else
        {
            to_read = this->status.cbInQue;
        }
    }

    if (ReadFile(this->handler, buffer, to_read, &bytes_read, NULL))
    {
        return bytes_read;
    }

    return 0;
}

bool SerialPort::writeSerialPort(char* buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) 
    {
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    }
    else
    {
        return true;
    }
}

bool SerialPort::isConnected()
{
    return this->connected;
}