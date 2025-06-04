#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <string>

// In bytes
#define BUFFER_SIZE 100

void ReadString(char* output);

HANDLE fileHandle;
typedef const wchar_t* win_string;

// https://dev.to/gabbersepp/ipc-between-c-and-c-by-using-named-pipes-4em9
int main()
{
    std::cout << "====C++====\r\n";
    win_string name = TEXT("\\\\.\\pipe\\rsx_mems_pipe");

    // Try to access pipe
    fileHandle = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);

    // Keep trying to connect if failed.
    while (fileHandle == INVALID_HANDLE_VALUE)
    {
        // Open pipe.
        fileHandle = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 
            0, NULL, OPEN_EXISTING, 0, NULL);
        Sleep(100);
    }

    double angle = -15;

    // Update loop
    while (1)
    {
        // Read from pipe.
        char* buffer = new char[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        // The ReadString will halt until a message is received.
        ReadString(buffer);
        std::cout << "recv<< " << buffer << "\r\n";

        // We are asked for the angle.
        if (strcmp(buffer, "Angle request.\0") == 0)
        {
            std::string str = std::string("Angle: ").append(std::to_string(angle)).append("\r\n");
            const char* msg = str.c_str();

            // DO SOME WORK HERE

            // Send data to C#
            WriteFile(fileHandle, msg, strlen(msg), nullptr, NULL);
            std::cout << "send>> " << msg << "\r\n";
            angle++;
        }
        else if (std::string(buffer).rfind("SERVER ERROR NOT", 0) == 0)
        {
            // We have an error.
            std::cout << "We have a server error.";
        }
        else if (std::string(buffer).rfind("Set to ", 0) == 0)
        {
            // The angle returned is the actual angle.
            std::string actualAngleString = std::string(buffer).substr(7);
            double actualAngle = std::stod(actualAngleString); // https://stackoverflow.com/a/30809670

            // Do something with actualAngle
        }

        Sleep(500);
    }
}

// Credit https://dev.to/gabbersepp/ipc-between-c-and-c-by-using-named-pipes-4em9
void ReadString(char* output)
{
    ULONG read = 0;
    int index = 0;
    do
    {
        // ReadFile halts until we have data from C# to read
        (void) ReadFile(fileHandle, output + index++, 1, &read, NULL);
    } while (read > 0 && *(output + index - 1) != 0);
}