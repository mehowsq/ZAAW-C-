#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>

using namespace std;

class ZbiornikPaliwa
{
    public:
 
    unsigned int litryPaliwa;
    
    ZbiornikPaliwa(unsigned int paliwo)
    {
        litryPaliwa=paliwo;
    }

    int pobierz(int paliwo)
    {
       cout << "pobrane paliwo: " << paliwo << endl;
       int temp = litryPaliwa - paliwo;
       if(temp < 0){
           temp = 0;
       }
       litryPaliwa = temp;
       return temp; 
    }
};

class Silnik
{
    public:
    
    unsigned int interwal;
    unsigned int paliwo;
 
    vector<shared_ptr<ZbiornikPaliwa>> zbiornikiPaliwa;
    
    Silnik(int interwal, int paliwo)
    {
        this->interwal = interwal;
        this->paliwo = paliwo;
    }

    
    void pobierzPaliwo(){
        while(zbiornikiPaliwa.size() > 0 ){
            int pozostalePaliwo = zbiornikiPaliwa.back()->pobierz(paliwo);
            if(pozostalePaliwo == 0){
                cout << "Usunięte zbiornik" << endl;
                zbiornikiPaliwa.pop_back();
            }
            chrono::milliseconds timespan(interwal*1000);
            this_thread::sleep_for(timespan);
        }
    }
    
    thread uruchomSilnik(){
      return thread( [this] { this->pobierzPaliwo(); } );
    
    }
 
    void dodajZbiornik(shared_ptr<ZbiornikPaliwa> zp)
    {
        zbiornikiPaliwa.push_back(zp);
    }
};

int main()
{
    
    shared_ptr<ZbiornikPaliwa> zp0( new ZbiornikPaliwa(21));
    shared_ptr<ZbiornikPaliwa> zp1( new ZbiornikPaliwa(24));
    shared_ptr<ZbiornikPaliwa> zp2( new ZbiornikPaliwa(35));
    shared_ptr<ZbiornikPaliwa> zp3( new ZbiornikPaliwa(13));
    shared_ptr<ZbiornikPaliwa> zp4( new ZbiornikPaliwa(34));
    shared_ptr<ZbiornikPaliwa> zp5( new ZbiornikPaliwa(66));
    shared_ptr<ZbiornikPaliwa> zp6( new ZbiornikPaliwa(34));
    shared_ptr<ZbiornikPaliwa> zp7( new ZbiornikPaliwa(32));
    shared_ptr<ZbiornikPaliwa> zp8( new ZbiornikPaliwa(21));
    shared_ptr<ZbiornikPaliwa> zp9( new ZbiornikPaliwa(38));
    
    
    Silnik silnik1(2, 5);
    silnik1.dodajZbiornik(zp0);
    silnik1.dodajZbiornik(zp1);
    silnik1.dodajZbiornik(zp2);
    silnik1.dodajZbiornik(zp3);
    silnik1.dodajZbiornik(zp4);
    
    Silnik silnik2(3, 8);
    silnik2.dodajZbiornik(zp5);
    silnik2.dodajZbiornik(zp6);
    silnik2.dodajZbiornik(zp7);
    silnik2.dodajZbiornik(zp8);
    silnik2.dodajZbiornik(zp9);
    
    Silnik silnik3(4, 3);
    silnik3.dodajZbiornik(zp0);
    silnik3.dodajZbiornik(zp2);
    silnik3.dodajZbiornik(zp4);
    silnik3.dodajZbiornik(zp6);
    silnik3.dodajZbiornik(zp8);

    vector<thread> threads;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    
    threads.push_back(silnik1.uruchomSilnik());
    threads.push_back(silnik2.uruchomSilnik());
    threads.push_back(silnik3.uruchomSilnik());
    
    for(auto& thread : threads){
        thread.join();
    }
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<chrono::seconds> (end - begin).count() << "[s]" << endl;
    
    return 0;
}
