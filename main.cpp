//
//  main.cpp
//  cs423project1
//
//  Created by Eric Wilson on 3/7/16.
//  Copyright © 2016 Eric Wilson. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#define SERVERPORT "23456"
#define SERVERIP "204.76.188.23"


std::string NAME;
int MSGNUMBER = 10000;
std::string ecrypt(std::string str );
std::string decrypt(std::string str);
void *recvMessage(void *name);
void welcomeMessage(char buf[]);
void incMessageNum(std::string & msgNum);
void sendMessage(int sockfd, struct addrinfo*, std::string &msgNumber, std::string myName);

int main(int argc, const char * argv[]) {
    
    int sockfd;
    struct addrinfo server_struct, *servinfo, *p;
    int return_value;
    int recv_byte;
    char buf[100];
    char sendStr[] = "12345;1;I1@V";
    std::string msgNumber = "12345";
    //std::string getName;
    int recvThreadReturn;
    pthread_t recvThread;
    char menuOption;
    
    memset(&server_struct, 0, sizeof server_struct);
    server_struct.ai_family = AF_UNSPEC;
    server_struct.ai_socktype = SOCK_DGRAM;
    
    if ((return_value = getaddrinfo(SERVERIP, SERVERPORT, &server_struct, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(return_value));
        return 1;
    }
    
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    
    //prompt user input
    std::cout << "Please enter your name: ";
    std::getline(std::cin, NAME);
    
    //build string to send
    std::string sendString = std::to_string(MSGNUMBER) + ";1;" + ecrypt(NAME);
    
    //send login message to server
    if ((recv_byte = sendto(sockfd, sendString.c_str(), sendString.length(), 0,
                            servinfo->ai_addr, servinfo->ai_addrlen)) == -1){
        perror("UDP_Client: sento error");
        exit(1);
    }
    
    incMessageNum(msgNumber);
    
    printf("UDP_Client: sent %d bytes to %s\n", recv_byte, SERVERIP);
    
    //recieve ack from server (main thread because the program cannot continue without it.
    std::cout << "Waiting for ack message from server..." << "\n";
    if ((recv_byte = recvfrom(sockfd, buf, 100-1 , 0, NULL, NULL)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    //display welcome message
    welcomeMessage(buf);
    recvThreadReturn = pthread_create( &recvThread, NULL, recvMessage, (void*) &sockfd);
    
    //Menu message
    std::cout << "x - Exit\ns - Send Message\nPlease enter menu option.\n";
    //MENU
    while(menuOption != 'x'){
        //wait for user command (can be entered any time with no prompt)
        std::cin >> menuOption;
        
        if(menuOption == 's'){
            sendMessage(sockfd, servinfo, msgNumber, NAME);
        }
    }
    //signoff
    //build string
    std::string signOffString = std::to_string(++MSGNUMBER) + ";3;" + ecrypt(NAME);
    if ((recv_byte = sendto(sockfd, signOffString.c_str(), signOffString.length(), 0,
                            servinfo->ai_addr, servinfo->ai_addrlen)) == -1){
        perror("UDP_Client: sento error");
        exit(1);
    }
    
    freeaddrinfo(servinfo);
    close(sockfd);
    
    return 0;
}

//encrypt function
std::string ecrypt(std::string str ){
    char encr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 'b', '.', 0, 'c', '[', 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        'R', 'u', ',', 'q', '\t', 'Y', '\n', '\'', 'n', 's', 'v', 'e', 'H', 'o', 'N', 'M',
        'r', '=', '0', ';', 'z', '/', '`', 'E', '\"', 'k', '&', '5', '>', 'i', 'p', ')',
        '$', '!', '2', 'O', '(', 'I', 'J', '%', 'Z', 'g', '\\', '{', 'h', '7', 'S', 'P',
        'a', ' ', 'W', 'x', 'y', 'T', '+', '8', '-', 'L', '9', 'f', '#', 'F', '\r', 'B',
        '3', 'D', ']', 'V', '?', '*', 'G', '6', 'w', '@', '}', '|', 'C', 'l', '_', 'j',
        'K', '^', '1', 't', 'Q', '<', 'U', 'd', 'm', ':', 'A', 'X', '\f', '4', '~', 0,
        0, 0};
    
    char decr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, '$', '&', 0, '|', '^', 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        'Q', 'A', '8', '\\', '@', 'G', ':', '\'', 'D', '?', 'e', 'V', '\"', 'X', '\n', '5',
        '2', 'r', 'B', '`', '}', ';', 'g', 'M', 'W', 'Z', 'y', '3', 'u', '1', '<', 'd',
        'i', 'z', '_', 'l', 'a', '7', ']', 'f', ',', 'E', 'F', 'p', 'Y', '/', '.', 'C',
        'O', 't', ' ', 'N', 'U', 'v', 'c', 'R', '{', '%', 'H', '\r', 'J', 'b', 'q', 'n',
        '6', 'P', '\t', '\f', 'w', '+', '[', 'I', 'L', '=', 'o', '9', 'm', 'x', '(', '-',
        '>', '#', '0', ')', 's', '!', '*', 'h', 'S', 'T', '4', 'K', 'k', 'j', '~', 0,
        0, 0};
    
    std::string returnString = "";
    std::string::iterator itr;
    
    for (itr = str.begin(); itr != str.end(); itr++){
        returnString.push_back(encr[(int)(*itr)]);
    }
    
    return returnString;
    
}

//decrypt function
std::string decrypt(std::string str){
    char encr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 'b', '.', 0, 'c', '[', 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        'R', 'u', ',', 'q', '\t', 'Y', '\n', '\'', 'n', 's', 'v', 'e', 'H', 'o', 'N', 'M',
        'r', '=', '0', ';', 'z', '/', '`', 'E', '\"', 'k', '&', '5', '>', 'i', 'p', ')',
        '$', '!', '2', 'O', '(', 'I', 'J', '%', 'Z', 'g', '\\', '{', 'h', '7', 'S', 'P',
        'a', ' ', 'W', 'x', 'y', 'T', '+', '8', '-', 'L', '9', 'f', '#', 'F', '\r', 'B',
        '3', 'D', ']', 'V', '?', '*', 'G', '6', 'w', '@', '}', '|', 'C', 'l', '_', 'j',
        'K', '^', '1', 't', 'Q', '<', 'U', 'd', 'm', ':', 'A', 'X', '\f', '4', '~', 0,
        0, 0};
    
    char decr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, '$', '&', 0, '|', '^', 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        'Q', 'A', '8', '\\', '@', 'G', ':', '\'', 'D', '?', 'e', 'V', '\"', 'X', '\n', '5',
        '2', 'r', 'B', '`', '}', ';', 'g', 'M', 'W', 'Z', 'y', '3', 'u', '1', '<', 'd',
        'i', 'z', '_', 'l', 'a', '7', ']', 'f', ',', 'E', 'F', 'p', 'Y', '/', '.', 'C',
        'O', 't', ' ', 'N', 'U', 'v', 'c', 'R', '{', '%', 'H', '\r', 'J', 'b', 'q', 'n',
        '6', 'P', '\t', '\f', 'w', '+', '[', 'I', 'L', '=', 'o', '9', 'm', 'x', '(', '-',
        '>', '#', '0', ')', 's', '!', '*', 'h', 'S', 'T', '4', 'K', 'k', 'j', '~', 0,
        0, 0};
    
    std::string returnString = "";
    std::string::iterator itr;
    
    for (itr = str.begin(); itr != str.end(); itr++){
        returnString.push_back(decr[(int)(*itr)]);    }
    
    return returnString;
}

//thread to recv messages
void *recvMessage(void *sockfd){
    int recv_byte;
    char buf[100];
    //TEST
    struct addrinfo server_struct, *servinfo, *p;
    int(return_value);

    
    memset(&server_struct, 0, sizeof server_struct);
    server_struct.ai_family = AF_UNSPEC;
    server_struct.ai_socktype = SOCK_DGRAM;
    if ((return_value = getaddrinfo(SERVERIP, SERVERPORT, &server_struct, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(return_value));
        exit(1);
    }
    
    int sock = *(int*)sockfd;
    
    
    while(1){
        std::cin.clear();
        std::cout.clear();
        memset(buf ,0 ,sizeof(buf));
        std::cout << "Listening for incoming messages...\n";
        //wait for messages
        if ((recv_byte = recvfrom(sock, buf, 100-1 , 0, NULL, NULL)) == -1) {
            perror("recvfrom");
            exit(1);
            //sleep(10);
        }
        std::string message = buf;
        //notify user of recvd message
        /*std::cout << "Message " << message.substr(0, message.find_first_of(';'))
        << " recieved from server.\n" ;*/
        
        //get message type
        char msgtype = message.at(message.find_first_of(';')+1);
        std::string msgnum = message.substr(0,message.find_first_of(';'));
        //std::cout << "Message type: " << msgtype << '\n';
        //std::cout << "Message number: " << msgnum << '\n';
        
        //handle "keep alive" messages
        if(msgtype == '4'){
            std::cout << "Sending ack to server...\n";
            //build message
            std::string strName = NAME;
            std::string replyMessage = "ack;" + msgnum + ";" + ecrypt(strName);
            int msglength = replyMessage.length();
            
            //send built replyMessage to server
            if ((recv_byte = sendto(sock, replyMessage.c_str(), msglength, 0,
                                    servinfo->ai_addr, servinfo->ai_addrlen)) == -1){
                perror("UDP_Client: sento error");
                exit(1);
            }
            
        }
        //handle notification messages
        //users logging in and logging off
        else if (msgtype == '1' || msgtype == '3'){
            //output in GREEN
            std::cout << "\033[1;32m " + decrypt(message.substr(message.find_first_of(';')+3)) + "\033[0m";
        }
        //messages from users
        else if (msgtype == '2'){
            //output in BLUE
            std::cout << "\033[1;34m " + decrypt(message.substr(message.find_first_of(';')+2)) + "\033[0m\n";
        }
        //handle error messages
        else if (msgnum == "Error"){
            //output in RED
            std::cout << "\033[1;31m " + decrypt(message.substr(message.find_first_of(';')+2)) + "\033[0m\n";
        }
        
    }
}

//handle welcome message
void welcomeMessage(char buf[]){
    std::string encryptedMessage = buf;
    
    //erase header portion of encrypted string
    encryptedMessage.erase(0,encryptedMessage.find_first_of(';')+1);
    encryptedMessage.erase(0,encryptedMessage.find_first_of(';')+1);
    
    //output welcome message
    std::cout << decrypt(encryptedMessage) << '\n';
}

void incMessageNum(std::string & msgNum){
    //convert to integer
    int msgInt = atoi(msgNum.c_str());
    //increment
    msgInt++;
    //convert back to string
    std::string tmp = std::to_string(msgInt);
    //add leading 0's
    while (tmp.size() < 5){
        tmp.insert(0, "0");
    }
}

void sendMessage(int sockfd, struct addrinfo* servinfo, std::string &msgNumber, std::string myName){
    //build string
    int recv_byte;
    std::cin.clear();
    std::cin.ignore(1);
    std::string buddyName;
    std::string messageBody;
    
    //prompt user input
    std::cout << "Who would you like to send to?\n";
    std::getline(std::cin, buddyName);
    std::cout << "What would you like to say?\n";
    std::getline(std::cin, messageBody);
    //build message to send
    std::string encryptedText = ecrypt(myName + "\n" + buddyName + "\n" + messageBody);
    std::string sendString = std::to_string(++MSGNUMBER) + ";2;" + encryptedText;
    
    if ((recv_byte = sendto(sockfd, sendString.c_str(), sendString.length(), 0,
                            servinfo->ai_addr, servinfo->ai_addrlen)) == -1){
        perror("UDP_Client: sento error");
        exit(1);
    }
}
