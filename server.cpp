#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

using namespace std;

int main()
{
    int udpSocket, nBytes;
    char buffer[1024];
    struct sockaddr_in ServerAddr, ClientAddr;
    char *Clientip;

    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    ServerAddr.sin_family = AF_INET;
    cout << "Please enter your listening port: ";
    cin >> buffer;
    ServerAddr.sin_port = htons(atoi(buffer));
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    memset(ServerAddr.sin_zero, '\0', sizeof(ServerAddr.sin_zero));
    bind(udpSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr));
    socklen_t addr_size = sizeof ClientAddr;
    do
    {
        memset(buffer, '\0', sizeof(buffer));
        nBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&ClientAddr, &addr_size);
        char file[1024];
        while (strcmp(buffer, "Quit!") != 0 && strcmp(buffer, "Start!") != 0)
        {
            strcpy(file, buffer);
            sendto(udpSocket, file, nBytes, 0, (struct sockaddr *)&ClientAddr, addr_size);
            memset(buffer, '\0', sizeof(buffer));
            nBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&ClientAddr, &addr_size);
        }
        if (strcmp(buffer, "Quit!") == 0)
        {
            sendto(udpSocket, "Quit!", 5, 0, (struct sockaddr *)&ClientAddr, addr_size);
            cout << "Can't open file." << endl;
        }
        else
        {
            char filename[1024];

            auto unixTimestmp = std::time(nullptr);
            int packet;
            string timestamp = to_string(unixTimestmp);
            strcpy(filename, timestamp.c_str());
            strcat(filename, file);

            FILE *file = fopen(filename, "wb");
            memset(buffer, '\0', sizeof(buffer));
            nBytes = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&ClientAddr, &addr_size);
            cout << "Filename Received: " << buffer << endl;
            cout << filename << endl;
            while (strcmp(buffer, "Quit!") != 0)
            {
                packet = atoi(buffer);
                memset(buffer, '\0', sizeof(buffer));
                nBytes = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&ClientAddr, &addr_size);
                char batch[1000];
                char *header = strtok(buffer, " ");
                int position = strtol(header, NULL, 0);
                string castHeader = to_string(position);

                int counter = 0;
                for (int i = strlen(header) + 1; i < nBytes + 1; i++)
                {
                    batch[counter] = buffer[i];
                    counter++;
                }

                sendto(udpSocket, castHeader.c_str(), 1024, 0, (struct sockaddr *)&ClientAddr, addr_size);
                fseek(file, position, SEEK_SET);
                fwrite(&batch, 1, packet, file);

                cout << "Receiving bytes." << endl;
                memset(buffer, '\0', sizeof(buffer));
                nBytes = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&ClientAddr, &addr_size);
            }
            sendto(udpSocket, "Quit!", 5, 0, (struct sockaddr *)&ClientAddr, addr_size);
            cout << "Success!" << endl;
        }
    } while (strcmp(buffer, "Quit!") != 0);
    close(udpSocket);
    return 0;
}
