#include <iostream>
#include <string>
using namespace std;

class sword {
protected:
    int cost;
    string name;
public:
    virtual void showInfo() = 0;
    virtual ~sword() = default;
};
class klaymore : public sword {
public:
    katana() {
        cost = 120;
        name = "Klaymore";
    }
    void showInfo() override {
        cout << "It's - " << name << ". It costs - " << cost << endl;
    }
};
class katana : public sword {
public:
    katana() {
        cost = 100;
        name = "Katana";
    }
    void showInfo() override {
        cout << "It's - " << name << ". It costs - " << cost << endl;
    }
};

class ISwordFactory {
public:
    virtual sword* CreateSword() = 0;
    virtual ~ISwordFactory() = default;
};

class JapanFactory : public ISwordFactory {
public:
    sword* CreateSword() override {
        return new Katana;
    }
};

class ScandinavFactory : public ISwordFactory {
public:
    sword* CreateSword() override {
        return new Klaymore;        
    }
}; 
int main(){
     
}