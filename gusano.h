#include <iostream>
#include <stdlib.h>
#include <vector>

#include <unistd.h>
#include <termios.h>

using namespace std;

////////////////////////////////////////////////////////
vector<vector<char>> tab;
int size_tab=0;
////////////////////////////////////////////////////////
char prop = '.';

string PadZeros(int number, int longitud)
{
  string num_letra = to_string(number);
  for (int i = num_letra.length(); i < longitud; ++i)
    num_letra = "0" + num_letra;
  
  return num_letra;
}

class Gusano{
public:
	vector<pair<int,int>> posiciones;
	char id;
	int size_gus = 3;
	Gusano(int x,int y,char c)
	{
		for(int i=0;i<size_gus;i++)
		{
			pair<int,int> temp(x,y);
			posiciones.push_back(temp);
		}
		id=c;
		pintar(c);
	}
	Gusano(string str_pos, char c)
	{
		for (int i = 0; i < str_pos.length(); i+=4)
		{
			int x = stoi(str_pos.substr(i,2));
			int y = stoi(str_pos.substr(i+2,2));
			pair<int,int> temp(x,y);
			posiciones.push_back(temp);
		}
		id = c;
	}

	void pintar(char c){
		for(auto i:posiciones){
			tab[i.first][i.second]=c;
		}
		auto i=posiciones.begin();
		tab[i->first][i->second]=char(toupper(c)) ;
	}
	void actualizar(int x,int y){
		vector<pair<int,int>> temp;
		pair<int,int> t(x,y);
		temp.push_back(t);
		auto j=posiciones.begin();
		for(int i=0;i<posiciones.size()-1;i++){
			temp.push_back(*j);
			j++;
		}
		posiciones=temp;
	}
	bool Lose()
	{
		return true;		
	} 
	void mover(char e){
		switch(e){
		case 'w':{
			pair<int,int> temp=posiciones.front();
			if(temp.first-1==-1)
				break;
			pintar(prop);
			actualizar(temp.first-1,temp.second);
			pintar(id);
			break;
		}
		case 'a':{
			pair<int,int> temp=posiciones.front();
			if(temp.second-1==-1)
				break;
			pintar(prop);
			actualizar(temp.first,temp.second-1);
			pintar(id);
			break;
		}
		case 's':{
			pair<int,int> temp=posiciones.front();	
			if(temp.first+1==size_tab)
				break;
			pintar(prop);
			actualizar(temp.first+1,temp.second);
			pintar(id);
			break;
		}
		case 'd':{
			pair<int,int> temp=posiciones.front();
			if(temp.second+1==size_tab)
				break;
			pintar(prop);
			actualizar(temp.first,temp.second+1);
			pintar(id);
			break;
		}
		default:
			return;
		}
	}
	string info()
	{
		string str_pos = "";
		for (auto p = posiciones.begin(); p != posiciones.end(); ++p)
		{
			str_pos += PadZeros(p->first, 2) + PadZeros(p->second,2);
		}
		return str_pos;
	}
};
class Snake{
	vector<Gusano> jugadores; //TODO Mejorar a MAP
public:
	//int size_gus = 0;
	Snake(int si){
		size_tab = si;
		tab.resize(size_tab, vector<char>(size_tab));
		for(int i=0;i<size_tab;i++){
			for(int j=0;j<size_tab;j++){
				tab[i][j]=prop;
			}
		}
	}
	void insertar_jugador(int x,int y,char id){
		Gusano temp(x,y,id);
		jugadores.push_back(temp);
	}

	void insertar_jugador(string str_pos, char id){
		Gusano temp(str_pos, id);
		jugadores.push_back(temp);
	}

	void moverjugador(char direc,char id){
		auto i=jugadores.begin();
		for(;i!=jugadores.end();i++){
			if(id==i->id){
				i->mover(direc);
				break;
			}
		}	
	}
	void mostrar(){
		system("clear");
		for(auto i:tab){
			for(auto j:i){
				cout<<j<<"    ";
			}
			cout<<"\n\n";
		}
	}
	string info(char id)
	{
		auto i=jugadores.begin(); //TODO MEJORAR A MAP
		for(;i!=jugadores.end();i++){
			if(id==i->id){
				return i->info();
				break;
			}
		}	
	}	
};

/*
int main(int argc, char *argv[]) {
	Snake s(6);
	s.insertar_jugador(1,1,'j');
	char c=' ';
	struct termios old_tio, new_tio;
	
	// get the terminal settings for stdin
    tcgetattr(STDIN_FILENO,&old_tio);

    // we want to keep the old setting to restore them a the end
    new_tio=old_tio;

    // disable canonical mode (buffered i/o) and local echo
    new_tio.c_lflag &=(~ICANON & ~ECHO);

    // set the new settings immediately 
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

	while(true){
		s.mostrar();
		c = getchar();
		s.moverjugador(c,'j');
		system("clear");
	}

	// restore the former settings 
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
	return 0;
}
*/
