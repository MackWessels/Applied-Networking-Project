#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main()
{
    int mySocket, nBytes;
    char buffer[1024];
    struct sockaddr_in ServerAddr;

    mySocket = socket(PF_INET, SOCK_DGRAM, 0);
    ServerAddr.sin_family = AF_INET;
    cout << "Please enter server port: ";
    cin.getline(buffer, 1023, '\n');
    ServerAddr.sin_port = htons(atoi(buffer));
    cout << "Please enter the server IP: ";
    memset(buffer, '\0', sizeof(buffer));
    cin.getline(buffer, 1023, '\n');
    ServerAddr.sin_addr.s_addr = inet_addr(buffer);
    memset(ServerAddr.sin_zero, '\0', sizeof(ServerAddr.sin_zero));
    socklen_t addr_size = sizeof ServerAddr;
    do
    {
        cout << "Please enter the filename: ";
        memset(buffer, '\0', sizeof(buffer));
        cin.getline(buffer, 1023, '\n');
        nBytes = strlen(buffer) + 1;

        char filename[1024];
        strcpy(filename, buffer);

        sendto(mySocket, buffer, nBytes, 0, (struct sockaddr *)&ServerAddr, addr_size);
        memset(buffer, '\0', sizeof(buffer));
        nBytes = recvfrom(mySocket, buffer, 1024, 0, (struct sockaddr *)&ServerAddr, &addr_size);
        FILE *file = fopen(filename, "rb");

        if (file != NULL)
        {
            sendto(mySocket, "Start!", 6, 0, (struct sockaddr *)&ServerAddr, addr_size);
            int bytesRead = 0;
            int bytesSize;
            char packet[1024];
            fseek(file, bytesRead, SEEK_SET);
            while (!feof(file))
            {
                memset(buffer, '\0', sizeof(buffer));
                string position = to_string(bytesRead) + " ";
                for (int i = 0; i < position.length(); i++)
                {
                    packet[i] = position[i];
                }

                if ((bytesSize = fread(buffer, 1, 1000, file)) > 0)
                {
                    string size = to_string(bytesSize);
                    int counter = 0;
                    for (int i = position.length(); counter < bytesSize + 1; i++)
                    {
                        packet[i] = buffer[counter];
                        counter++;
                    }

                    sendto(mySocket, size.c_str(), sizeof(bytesSize), 0, (struct sockaddr *)&ServerAddr, addr_size);
                    sendto(mySocket, packet, sizeof(packet), 0, (struct sockaddr *)&ServerAddr, addr_size);
                    cout << "Sending bytes." << endl;
                }
                else
                {
                    break;
                }

                nBytes = recvfrom(mySocket, buffer, 1024, 0, (struct sockaddr *)&ServerAddr, &addr_size);
                if (strtol(buffer, NULL, 0) == bytesRead)
                {
                    bytesRead += bytesSize;
                }
                fseek(file, bytesRead, SEEK_SET);
            }
            sendto(mySocket, "Quit!", 1024, 0, (struct sockaddr *)&ServerAddr, addr_size);
            memset(buffer, '\0', sizeof(buffer));
            nBytes = recvfrom(mySocket, buffer, 1024, 0, (struct sockaddr *)&ServerAddr, &addr_size);
            cout << "Success!" << endl;
        }
        else
        {
            cout << "Can't find file." << endl;
            sendto(mySocket, "Quit!", 1024, 0, (struct sockaddr *)&ServerAddr, addr_size);
            nBytes = recvfrom(mySocket, buffer, 1024, 0, (struct sockaddr *)&ServerAddr, &addr_size);
        }

    } while (strcmp(buffer, "Quit!") != 0);
    close(mySocket);
    return 0;
}