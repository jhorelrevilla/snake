
/* Client N-Raya*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

#include <unistd.h>
#include <termios.h>

#include "gusano.h"

using namespace std;

struct termios old_tio, new_tio;

int size_juego = 8;
Snake Juego(size_juego);

string usuario = "";
/*
string PadZeros(int number, int longitud)
{
  string num_letra = to_string(number);
  for (int i = num_letra.length(); i < longitud; ++i)
    num_letra = "0" + num_letra;
  
  return num_letra;
}
*/
string ProtocoloMensaje(string mensaje)
{
  int longitud = mensaje.length();
  return "C" + PadZeros(longitud, 2) + mensaje;
}

void RecepcionMensaje(int SocketFD)
{
  int n;
  char buffer[256];
  int longitud = 0;
  char ficha = ' ';
  for(;;)
  {
    //Lectura Comando
    n = read(SocketFD,buffer,1);
    sleep(0.001);
    if (n < 0) perror("ERROR reading from socket");
    if (n == 1)
    {
      switch (buffer[0]) //LetraComando
      {
        case 'n':
        {
          //Nuevo jugador
          n = read(SocketFD, buffer, 1);
          if (n == 1)
          {
            ficha = buffer[0];
            Juego.insertar_jugador(2,2, ficha); //TODO MEJORAR DEBE RECIBIR POSICION
            tab[2][2]=ficha;
            Juego.mostrar();
            bzero(buffer,256);
          }
          break;
        }

        case 'i':
        {
          //Nuevo jugador
          n = read(SocketFD, buffer, 1);
          if (n == 1)
          {
            ficha = buffer[0];
            //Leer tamano mensaje
            n = read(SocketFD, buffer, 3);
            if (n == 3)
            {
              longitud = stoi(buffer); 
              //Leer posiciones
              n = read(SocketFD, buffer, longitud);
              if ( n == longitud){
                string str_pos = buffer;
                
                int x = stoi(str_pos.substr(0,2));
			          int y = stoi(str_pos.substr(2,2));
			          cout<<"x - y"<<x<<" - "<<y;
                tab[x][y]=char(toupper(ficha));
                Juego.insertar_jugador(str_pos, ficha); 
                Juego.mostrar();
              }
            }   
            bzero(buffer,256);
          }
          break;
        }

        case 'w':
        case 's':
        case 'a':
        case 'd':
        {
          char direc = buffer[0];
          n = read(SocketFD, buffer, 1);
          if (n == 1)
          {
            ficha = buffer[0];
            Juego.moverjugador(direc, ficha);
            //cout << Juego.info(ficha);
            Juego.mostrar();
            bzero(buffer,256);
          }
          break;
        }
        case 'C': //Mensaje
        {
          n = read(SocketFD, buffer, 2);
          if (n == 2)
          {
            longitud = stoi(buffer);
            //Leer mensaje
            n = read(SocketFD, buffer, longitud);
            if (n == longitud){
              cout << buffer << "\n";
            }
            bzero(buffer,256);
          }
          break;
        }        
      }
      
    }
  }
}

void EnvioMensaje(int SocketFD)
{
  string msgToChat = "";
  //char buffer[256];
  char c = ' ';
  int n;
   
  //Iniciar ficha jugador
  cout << "\nIngrese ficha: ";
  getline(cin, msgToChat);
  msgToChat = "n" + msgToChat;
  n = write(SocketFD, msgToChat.c_str(), msgToChat.length());
  //cout << "fichainit: " << msgToChat;
  // Terminal no espera de linea
  tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

  for(;;)
  { 
    //cin.clear(); 
    //cout << "\nIngrese comando: ";
    //getline(cin, msgToChat);
    c = getchar();

    switch (c)
    {
    case 'c':
      //Terminal a espera de linea
      tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

      //Es mensaje a Chat
      //Preparar mensaje
      cout << "\nIngrese comando: ";
      getline(cin, msgToChat);
      msgToChat = ProtocoloMensaje(msgToChat);
      n = write(SocketFD, msgToChat.c_str(), msgToChat.length());

      // Terminal no espera de linea
      tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
      break;
    
    case 'w':
    case 's':
    case 'a':
    case 'd':
    {
      //Posible Comando o jugada
      string msg(1,c);
      n = write(SocketFD, msg.c_str(), msg.length());

      break;
    }
    default:
      break;
    }
    
  }
}

int main(void)
{
  //// Modificacion de Terminal
  //struct termios old_tio, new_tio;
	
	// get the terminal settings for stdin
  tcgetattr(STDIN_FILENO,&old_tio);

  // we want to keep the old setting to restore them a the end
  new_tio=old_tio;

  // disable canonical mode (buffered i/o) and local echo
  new_tio.c_lflag &=(~ICANON & ~ECHO);

  //////////////

  struct sockaddr_in stSockAddr;
  int Res;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  char buffer[256];
  string msgFromChat;
  int n;
    
  if (-1 == SocketFD)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(1100);
  //Res = inet_pton(AF_INET, "51.15.220.108", &stSockAddr.sin_addr);
  Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter does not contain valid ipaddress");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("connect failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  bzero(buffer, 256);

  //Thread de Envio y Recepcion de Mensajes
  std::thread(EnvioMensaje, SocketFD).detach();
  std::thread(RecepcionMensaje, SocketFD).detach();
  //Mantener con vida los threads
  for(;;)
  {}

  shutdown(SocketFD, SHUT_RDWR);
  close(SocketFD);

  // restore the former settings 
  tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
  
  return 0;
}