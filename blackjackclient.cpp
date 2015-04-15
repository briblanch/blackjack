#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <sstream>

using namespace std;

void error(const char *msg) {
    perror(msg);
    exit(0);
}

char *intToString(int number) {
    stringstream ss;
    ss << number;

    return (char *) ss.str().c_str();
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n, server_sum = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    srand(time(NULL));

    std::vector<string> card_vec;
    std::map<string,int> card_map;
    std::map<string,int>::iterator it;

    card_vec.push_back("2");
    card_vec.push_back("3");
    card_vec.push_back("4");
    card_vec.push_back("5");
    card_vec.push_back("6");
    card_vec.push_back("7");
    card_vec.push_back("8");
    card_vec.push_back("9");
    card_vec.push_back("10");
    card_vec.push_back("J");
    card_vec.push_back("K");
    card_vec.push_back("Q");
    card_vec.push_back("A");


    card_map["2"] = 2;
    card_map["3"] = 3;
    card_map["4"] = 4;
    card_map["5"] = 5;
    card_map["6"] = 6;
    card_map["7"] = 7;
    card_map["8"] = 8;
    card_map["9"] = 9;
    card_map["10"] =10;
    card_map["J"] = 10;
    card_map["K"] = 10;
    card_map["Q"] = 10;
    card_map["A"] = 1;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    while(1) {
        char *temp;
        int selectedCard;
        bzero(buffer, 256);

        int randIndex = rand() % card_map.size();
        it = card_map.find(card_vec.at(randIndex));

        selectedCard = it->second;
        temp = intToString(selectedCard);

        card_vec.erase(card_vec.begin()+randIndex);
        card_map.erase(it);

        n = write(sockfd, temp, sizeof(temp));
        if (n < 0) {
            error("Error writing to socket");
        }

        n = read(sockfd, buffer, 255);
        if (n < 0) {
            error("Error reading from socket");
        }

        server_sum += atoi(buffer);
        if (server_sum > 21) {
            cout << "User wins, Casears Palace loses\n";
            break;
        } else {
            cout << "Server sum: " << server_sum << "\n";
        }
    }

    close(sockfd);
    return 0;
}