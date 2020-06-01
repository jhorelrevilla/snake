#include <iostream>
#include <stdlib.h>
#include <vector>
using namespace std;

////////////////////////////////////////////////////////
vector<vector<char>> tab;
int size=0;
////////////////////////////////////////////////////////
struct Casilla{
	char prop = '+';
};
class Gusano{
public:
	vector<pair<int,int>> posiciones;
	char p;
	Gusano(int x,int y,char c){
		for(int i=0;i<3;i++){
			pair<int,int> temp(x,y);
			posiciones.push_back(temp);
		}
		p=c;
	}
	void pintar(char c){
		for(auto i:posiciones){
			tab[i.first][i.second]=c;
		}
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
	bool Lose(){
		
	} 
	void mover(char e){
		switch(e){
		case 'w':{
			pair<int,int> temp=posiciones.front();
			if(temp.first-1==-1)
				break;
			pintar('+');
			actualizar(temp.first-1,temp.second);
			pintar(p);
			break;
		}
		case 'a':{
			pair<int,int> temp=posiciones.front();
			if(temp.second-1==-1)
				break;
			pintar('+');
			actualizar(temp.first,temp.second-1);
			pintar(p);
			break;
		}
		case 's':{
			pair<int,int> temp=posiciones.front();	
			if(temp.first+1==size)
				break;
			pintar('+');
			actualizar(temp.first+1,temp.second);
			pintar(p);
			break;
		}
		case 'd':{
			pair<int,int> temp=posiciones.front();
			if(temp.second+1==size)
				break;
			pintar('+');
			actualizar(temp.first,temp.second+1);
			pintar(p);
			break;
		}
		default:
			return;
		}
	}
	
};
class Snake{
	vector<Gusano> jugadores;
public:
	Snake(int si){
		size = si;
		tab.resize(size, vector<char>(size));
		for(int i=0;i<size;i++){
			for(int j=0;j<size;j++){
				tab[i][j]='+';
			}
		}
	}
	void insertar_jugador(int x,int y,char p){
		Gusano temp(x,y,p);
		jugadores.push_back(temp);
	}
	void moverjugador(char d,char p){
		auto i=jugadores.begin();
		for(;i!=jugadores.end();i++){
			if(p==i->p){
				i->mover(d);
				break;
			}
		}
		
	}
	void mostrar(){
		for(auto i:tab){
			for(auto j:i){
				cout<<j<<"    ";
			}
			cout<<"\n\n";
		}
	}
};
int main(int argc, char *argv[]) {
	Snake s(6);
	s.insertar_jugador(1,1,'j');
	char c=' ';
	while(true){
		s.mostrar();
		cin>>c;
		s.moverjugador(c,'j');
		system("CLS");
	}
	return 0;
}

