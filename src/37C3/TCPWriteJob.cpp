#ifdef PLATFORM_LINUX

#include "37C3/TCPWriteJob.h"

namespace FalconEye
{

int TCPWriteJob::Init()
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "SIZE";
    char buffer[1024] = { 0 };
    if ((Socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(Port);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, Address.c_str(), &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
 
    if ((status
         = connect(Socket_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(Socket_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(Socket_fd, buffer, 1024 - 1); // subtract 1 for the null terminator at the end
    printf("%s\n", buffer);
 
    // closing the connected socket
    //close(client_fd);
    return 0;
}

std::string format_color(Color c)
{
    c = c * 255.0f;
    int int_r = static_cast<int>(c.r);
    int int_g = static_cast<int>(c.g);
    int int_b = static_cast<int>(c.b);
    int int_a = static_cast<int>(c.a);
    std::stringstream stream;
    //stream << std::setfill('0') << std::setw(2) << std::right <<std::hex << int_r << " " << int_g << " " <<int_g << " "<<  int_a;
    
    stream << format("{:02x}{:02x}{:02x}{:02x}\n", int_r, int_g, int_b, int_a);
    
    return stream.str();
}
bool TCPWriteJob::IsInImageBound(int x, int y)
{
    return x >= 0 && x < Image->width() && y >= 0 && y < Image->height();
}
int TCPWriteJob::VirtualExecute()
{
    std::string msg;
    msg = "";
    for (int i = 0; i < Size; ++i)
    {
        for (int j = 0; j < Size; ++j)
        {
            if (IsInImageBound(Start_x+i,Start_y+j))
            {
                Color pixel = (*Image)(Start_x+i, Start_y+j);

                //std::snprintf(buff, sizeof(buff), "PX %i %i 00ff00ff\n", 1000 - i , 1000-j);
                msg.append("PX ");
                msg.append(std::to_string(i+1500));
                msg.append(" ");
                msg.append(std::to_string(j+500));
                //msg.append(" ffffffff\n");
                msg.append(" ");
                msg.append(format_color(pixel));
                if(msg.size() >= 30000)
                {
                    //flush msg
                    //std::cout << "msg len=" << msg.size() << std::endl;
                    send(Socket_fd, msg.c_str(), msg.size(), 0);
                    msg = "";
                }
            }
        }
    }
    //send(Socket_fd, buff, strlen(buff), 0);
    //std::cout << "msg len=" << msg.size() << std::endl;
    send(Socket_fd, msg.c_str(), msg.size(), 0);
    close(Socket_fd);
    return 0;
}

} // en Namespace FalconEye

#endif // End PLATFORM_LINUX