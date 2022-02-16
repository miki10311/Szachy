#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
using namespace sf;

int rozmiar = 56;                      // rozmiar potrzebny do wydzielania odpowiedniej części obrazka
Vector2f zmiana(28,28);                // wektor potrzebny do zapamietywania pozycji

Sprite figury[32];                     // ilość figur na planszy na początku
std::string pozycja="";                // string na zapis pozycji

int plansza[8][8] = 
              {-1,-2,-3,-4,-5,-3,-2,-1,
               -6,-6,-6,-6,-6,-6,-6,-6,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                6, 6, 6, 6, 6, 6, 6, 6,
                1, 2, 3, 4, 5, 3, 2, 1};         // tablica z zapisem pozycji na początku

std::string zapisruchow(Vector2f pole)           // funkcja potrzebna do wypisywania ruchów
{
  std::string ruch = "";
  ruch += char(pole.x/rozmiar+97);
  ruch += char(7-pole.y/rozmiar+49);
  return ruch;
}

Vector2f bicie(char a,char b)        // funkcja odpowiedzialna za bicie figur
{                                    // a i b to wspolrzedne figury, ktora bierze udzial przy biciu
  int x = int(a) - 97;
  int y = 7-int(b)+49;
  return Vector2f(x*rozmiar,y*rozmiar);
}

void move(std::string str)             // funkcja odpowiada za poruszanie figurami
{
  Vector2f oldPos = bicie(str[0],str[1]);      // podmieniamy stare wartosci na nowe po biciu
  Vector2f newPos = bicie(str[2],str[3]);

  for(int i=0;i<32;i++)                        // aktualna pozycja
    if (figury[i].getPosition()==newPos) figury[i].setPosition(-100,-100);
      
  for(int i=0;i<32;i++)                        // poprzednia pozycja
    if (figury[i].getPosition()==oldPos) figury[i].setPosition(newPos);

  if (str=="e1g1") if (pozycja.find("e1")==-1) move("h1f1");    // implementacja roszady, czyli wtedy kiedy król nie poruszył się ze swojego pola
  if (str=="e8g8") if (pozycja.find("e8")==-1) move("h8f8");    // to wtedy może zamienić się miejscami z wieżą, tzn poruszyć się o dwa pola
  if (str=="e1c1") if (pozycja.find("e1")==-1) move("a1d1");
  if (str=="e8c8") if (pozycja.find("e8")==-1) move("a8d8");
}

void loadPosition()                   // funkcja odpowiada za to, aby każda figura znajdowała się tam gdzie ma być, tzn żeby było 
{                                     // osiem pionków każdego koloru i bierze z obrazka odpowiedni fragment i go wrzuca na dene miejsce
  int k=0;
  for(int i=0;i<8;i++)
  for(int j=0;j<8;j++)
  {
    int n = plansza[i][j];
    if (!n) continue;
    int x = abs(n)-1;
    int y = n>0?1:0;
    figury[k].setTextureRect( IntRect(rozmiar*x,rozmiar*y,rozmiar,rozmiar) );   // bierzemy tę część obrazka, ktora nas interesuje
    figury[k].setPosition(rozmiar*j,rozmiar*i);
    k++;
  }

  for(int i=0;i<pozycja.length();i+=5)
  move(pozycja.substr(i,4));                                  // zmiana dla każdego kolejnego ruchu
}


int main()
{
  RenderWindow window(VideoMode(504, 504), "PAMSI P3 - Szachy!");   // do pokazywania planszy szachowej

  Texture t1,t2;                                                    // dwie tekstury odpowiadające za planszę i za bierki
  t1.loadFromFile("obrazy/figury.png"); 
  t2.loadFromFile("obrazy/plansza.png");

  for(int i=0;i<32;i++) figury[i].setTexture(t1);                   // zrobienie miejsca na figury na planszy
  Sprite sBoard(t2);                                                // zmienna dla planszy

  loadPosition();                // załadowanie pozycji

  bool isMove=false;             // czy wykonujemy ruch czy nie
  float dx=0, dy=0;              // współrzędne
  Vector2f oldPos,newPos;        // nowa i stara pozycja
  std::string str;               // do zapisu ruchów
  int n=0; 

  while (window.isOpen())
  {
    Vector2i pos = Mouse::getPosition(window) - Vector2i(zmiana);    // wyswietlanie nowej pozycji po kontakcie myszki z oknem

    Event e;                            // Jakaś akcja, u mnie związana z myszką, która dzieje się w aplikacji
    while (window.pollEvent(e))
    {
      if (e.type == Event::Closed)      // Koniec programu gdy okno zamknięte
          window.close();

      if (e.type == Event::KeyPressed)  // Cofnięcie ruchu
      if (e.key.code == Keyboard::BackSpace)
      {
        pozycja.erase(pozycja.length()-6,5);
        loadPosition();
      }

      if (e.type == Event::MouseButtonPressed)    // Łapanie i przenoszenie figur
      if (e.key.code == Mouse::Left)              // z użyciem lewego klawisza myszki
      for(int i=0;i<32;i++)                       // sprawdzanie wszystkich figur
      if (figury[i].getGlobalBounds().contains(pos.x,pos.y))  // zmiana pozycji tej konkretnej figury
      {
        isMove=true; n=i;
        dx=pos.x - figury[i].getPosition().x;
        dy=pos.y - figury[i].getPosition().y;
        oldPos  =  figury[i].getPosition();
      }

      if (e.type == Event::MouseButtonReleased)  // Gdy puszczamy klawisz myszki
      if (e.key.code == Mouse::Left)
      {
        isMove=false;                                                               // Kończymy ruch
        Vector2f p = figury[n].getPosition() + Vector2f(rozmiar/2,rozmiar/2);       // Wyrównanie do kratek planszy
        newPos = Vector2f( rozmiar*int(p.x/rozmiar), rozmiar*int(p.y/rozmiar) );    // Zapisujemy nową pozycję 
        str = zapisruchow(oldPos)+zapisruchow(newPos);                              // Aktualizacja spisu ruchów
        std::cout<<str<<std::endl;                                                  // Wyświetlenie ruchu
        move(str);                                                                  // Wykonanie ruchu (bicie itp)
        if (oldPos!=newPos) pozycja+=str+" ";
        figury[n].setPosition(newPos);                   
      }                       
    }
    if (isMove) figury[n].setPosition(pos.x-dx,pos.y-dy);

  window.clear();                               // pokazanie w oknie aplikacji wszystkiego
  window.draw(sBoard);
  for(int i=0;i<32;i++) figury[i].move(zmiana);
  for(int i=0;i<32;i++) window.draw(figury[i]); window.draw(figury[n]);
  for(int i=0;i<32;i++) figury[i].move(-zmiana);
  window.display();
  }

  return 0;
}
