#include <iostream>
#include "centralheating.h"

bool checkIP(std::string ip)
{
    int dots = 0;
    int digits = 0;

    for (unsigned i = 0; i < ip.length(); i++)
    {
        if (isdigit(ip[i]))
            digits++;

        else if (ip[i] == '.')
        {
            dots++;

            if (digits == 0 || digits > 3 || dots > 3)
                return false;

            digits = 0;
        }

        else
            return false;
    }

    if (dots != 3 || (digits > 3))
        return false;

    return true;
}


int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        CentralHeating ch;
        ch.run(false);
    }

    else
    {
        int port = 0;
        std::string ip;

        for (int i = 1; i < argc; i++)
        {
            if (checkIP(argv[i]))
                ip = argv[i];

            else if (isdigit(*argv[i]))
                port = atoi(argv[i]);
        }

        if (ip.empty() || port == 0)
        {
            std::cout << "Usage:\n./CentralHeating\n./CentralHeating <ip> <port>\n";
            return 0;
        }

        CentralHeating ch(port, ip);
        ch.run(false);
    }

    return 0;
}
