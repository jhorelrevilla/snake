/* Server N-Raya */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <map>


#include "gusano.h"

using namespace std;

std::vector<pair<char, int>> LISTA_CLIENTES;
std::map<int, char> LISTA_MAP;
//Un mapa inverso, otra opcion es un Bimap
std::map<int,char> R_LISTA;

// Variables del Juego
int size_juego = 8;
Snake Juego(size_juego);
///////////
/*
string PadZeros(int number, int longitud)
{
  string num_letra = to_string(number);
  for (int i = 0; i < longitud - num_letra.length()+1; ++i)
    num_letra = "0" + num_letra;
  
  return num_letra;
}
*/
void EnviarMensaje(int socket_client, string mensaje)
{
  int n = write(socket_client, mensaje.c_str(), mensaje.length());
  if (n < 0) perror("ERROR writing to socket");
  
}


void BroadCast(string mensaje, int excepcion = -1)
{
  for (auto &i : LISTA_CLIENTES)
  {
    if(i.second != excepcion)
    {
      EnviarMensaje(i.second, mensaje);
    }
  }
}

void Process_Client_Thread(int socket_client)
{
    
  string msgToChat;
  char buffer[256];
  int n;
  char comando;
  int longitud = 0;
  //char ficha = ' ';
  do
    {
      //Lectura comando
      n = read(socket_client,buffer,1);  
      sleep(0.001);
      if (n < 0)
        perror("ERROR reading from socket");

      else if (n == 1)
      { 
        comando = buffer[0];
        
        switch (comando)
        {
        case 'n': 
        { //Nuevo jugador
          n = read(socket_client, buffer, 1);
          if (n == 1){
            char ficha = buffer[0];
                     
            //Debe enviar la posicion de todos los jugadores actuales
            //al nuevo jugador
            for (pair<char, int> i: LISTA_CLIENTES){
              string str_pos = Juego.info(i.first); //posicion de player i
              string msg = "i" + string(1, i.first) + PadZeros(str_pos.length(),3) + str_pos;
              cout<<"msg "<<msg<<endl;
              EnviarMensaje(socket_client, msg);
            }

            LISTA_CLIENTES.push_back(pair<char,int>(ficha,(int)socket_client));
            LISTA_MAP.insert(pair<int,char>(socket_client, ficha));
            int x= rand() % size_juego;
            int y=  rand() % size_juego;
            srand(time(NULL));
            Juego.insertar_jugador(x,y, ficha); //TODO MEJORAR A RANDOM
            //Debe enviar la posicion del nuevo jugador a todos
            string str_pos = Juego.info(ficha); //posicion de nuevo player
            string msg = "i" + string(1, ficha) + PadZeros(str_pos.length(),3) + str_pos;
            //string msg(1, ficha);
            BroadCast(msg); 
          }
          break;
        }
        
        case 'w':
        case 's':
        case 'a':
        case 'd':
        { 
          char ficha = LISTA_MAP[socket_client];
          Juego.moverjugador(comando, ficha);

          BroadCast(string(1,comando) + string(1,ficha)); 
          break;
        }

        case 'C': //Es un mensaje de chat
          //Leer tamaño mensaje
          n = read(socket_client, buffer, 2);
          if (n == 2)
          {
            string long_mens = buffer;
            int longitud = stoi(buffer);
            //Leer mensaje
            n = read(socket_client, buffer, longitud);
            if (n == longitud)
            {
              msgToChat = buffer;
              msgToChat = msgToChat.substr(0, longitud);
              msgToChat = "C" + long_mens + msgToChat;
              BroadCast(msgToChat);
              bzero(buffer,256);
            }         
          }
          break;
    
        
        default:
          EnviarMensaje(socket_client, "C08Invalida"); //Mensaje de Error
          break;
        }
        
      }
    } while (true);

    shutdown(socket_client, SHUT_RDWR);
    close(socket_client); 
}




int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  
  if(-1 == SocketFD)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(1100);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if(-1 == listen(SocketFD, 10))
  {
    perror("error listen failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  //int last_client = -1;
  for(;;)
  {
    int ClientSD = accept(SocketFD, NULL, NULL);
    
    if(0 > ClientSD)
    {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    std::thread(Process_Client_Thread, ClientSD).detach();

  } 
  
  close(SocketFD);
  return 0;
}