#include <SFML/Graphics.hpp>
#include <iostream>
#include "./src/ui.cpp"
#include "./src/boid.cpp"

int BOID::wBreite;
int BOID::wHoehe;
int BOID::v_rua;
int BOID::margin;
float BOID::borStrength;
int BOID::vMax;
float BOID::ranStrength;
int BOID::sepStrength;
float BOID::aliStrength;
float BOID::cohStrength;
int BOID::fleeStrength;
float BOID::sepRangeSq;
int BOID::maxNbRange;
int BOID::zpR;
bool BOID::normMov;
bool BOID::wrap;
bool BOID::dynRange;

    const int breite = 1800;
    const int hoehe = 960; // Breite und Höhe des Fensters
    const char helligkeit = 100;
    const int nGes = 500*2;
    const int zellenGes = 1080; //265
    const char predGes = 2;
    int n = 0;
    char nPred = 0;
    int colour_mode = 5;
    bool mouseAvoid = true;
    bool spuren = false;
    BOID* pZellen[zellenGes];
    BOID* pBoid[nGes];
    PRED* pPred[predGes];
    PRED* pMouseBoid;
    sf::RenderWindow window(sf::VideoMode(breite, hoehe), "BOIS");
    sf::RenderWindow* pWindow = &window;
    sf::Vector2f origin(0, 0);
    UI params(origin, 10, 5, 12, 24);

void checkChunk(BOID* chunkID, BOID* boidID){
    if(chunkID != nullptr){
            (*chunkID).istNachbarR(boidID);
        }
}

void nachbarCheck(BOID* pBoi){
    int hash_pos = (*pBoi).hashPos;
    checkChunk(pZellen[hash_pos],pBoi); //position auf numpad 5
    if(hash_pos > BOID::zpR){
        checkChunk(pZellen[hash_pos-1],pBoi); // 4
        checkChunk(pZellen[hash_pos-1-BOID::zpR],pBoi); // 7
        checkChunk(pZellen[hash_pos-BOID::zpR],pBoi); // 8
        checkChunk(pZellen[hash_pos+1-BOID::zpR],pBoi); // 9
        if(hash_pos < zellenGes-2-BOID::zpR){
            checkChunk(pZellen[hash_pos+1],pBoi); // 6
            checkChunk(pZellen[hash_pos-1+BOID::zpR],pBoi); // 1
            checkChunk(pZellen[hash_pos+BOID::zpR],pBoi); // 2
            checkChunk(pZellen[hash_pos+1+BOID::zpR],pBoi); // 3
        }
    }
}

void gridInsert(BOID* pBoi){
    (*pBoi).next = nullptr;
    int hash_pos = (*pBoi).hashPos;
    if(pZellen[hash_pos] == nullptr){
        pZellen[hash_pos] = pBoi;
    } else{
        (*pZellen[hash_pos]).insertPBoid(pBoi);
    }
}

void eventhandler(){
    sf::Event event;
    while(window.pollEvent(event)){
        if(event.type == sf::Event::Closed){
            window.close();
        }
        if(event.type == sf::Event::KeyPressed){
            switch(event.key.code){
                case 75: colour_mode = 0; break;
                case 76: colour_mode = 1; break;
                case 77: colour_mode = 2; break;
                case 78: colour_mode = 3; break;
                case 79: colour_mode = 4; break;
                case 80: colour_mode = 5; break;
                case 81: colour_mode = 6; break;
                case 82: BOID::wrap = !BOID::wrap; break;
                case 83: BOID::dynRange = !BOID::dynRange; break;
                case 84: colour_mode = 7; break;
                case 68: BOID::normMov = false;
                         BOID::aliStrength = 0.25;
                         BOID::margin = 120; break;
                case 67: BOID::normMov = true;
                         BOID::aliStrength = 0.75;
                         BOID::margin = 60; break;
                case 13: for(int i = 0; i < n; i++){
                                delete pBoid[i];
                            }
                            n = 0;
                            for(int i = 0; i < nPred; i++){
                                delete pPred[i];
                            }
                            nPred = 0; break;
                case 69: spuren=!spuren; break;
                default: break;
            }
            params.updateWert(0, BOID::aliStrength);
            params.updateWert(1, BOID::normMov);
            params.updateWert(2, BOID::wrap);
            params.updateWert(3, BOID::dynRange);
            params.updateMode(4, colour_mode);
            params.updateWert(7, spuren);
        }
        if(event.type == sf::Event::MouseButtonReleased){
            switch(event.mouseButton.button){
                case 1:
                    mouseAvoid = !mouseAvoid;
                    params.updateWert(6, mouseAvoid);
                    break;
                default: break;
                }
        }
        if(mouseAvoid){
            (*pMouseBoid).tri.setPosition((sf::Vector2f)sf::Mouse::getPosition(*pWindow));
            (*pMouseBoid).spatialHash();
        }
    }
}

int main(){ //Mainsetup

    window.setFramerateLimit(60);
    if(nGes > 600)
        window.setFramerateLimit(60);
    sf::Clock uhr;
    sf::Time delta_t = uhr.restart();
;

    //BOID* aktiv;
    //sf::Vector2f boidPos[nGes];
    //sf::Vector2f boidV[nGes];

        BOID::wBreite = breite;
        BOID::wHoehe = hoehe;
        BOID::v_rua = 500;
        BOID::margin = 100;
        BOID::borStrength = 18;
        BOID::vMax = 512;
        BOID::ranStrength = 0.125;
        BOID::sepStrength = 200;
        BOID::aliStrength = 0.25;//0.125;
        BOID::cohStrength = 1;
        BOID::fleeStrength = 512;
        BOID::sepRangeSq = 15*15;
        BOID::maxNbRange = 40;
        BOID::zpR = breite/(BOID::maxNbRange);
        BOID::normMov = false;
        BOID::wrap = true;
        BOID::dynRange = true;

    //std::cout << BOID::zpR << std::endl;
    //std::cout << hoehe/(BOID::maxNbRange) << std::endl;
    /*params.nameInsert("margin");
    params.nameInsert("borStrength");
    params.nameInsert("vMax");
    params.nameInsert("ranStrength");
    params.nameInsert("sepStrength");*/
    params.nameInsert("aliStrength");/*
    params.nameInsert("cohStrength");
    params.nameInsert("sepRange");
    params.nameInsert("maxNbRange");*/
    //params.nameInsert("normalVelo");
    params.nameInsert("normMov");
    params.nameInsert("wrap");
    params.nameInsert("dynRange");
    params.nameInsert("colourMode");
    params.nameInsert("Anzahl");
    params.nameInsert("mouseAvoid");
    params.nameInsert("Spuren");

    //params.updateWert(0, BOID::margin);
    //params.updateWert(1, BOID::borStrength);
    //params.updateWert(2, BOID::vMax);
    /*params.updateWert(0, BOID::ranStrength);
    params.updateWert(1, BOID::sepStrength);*/
    params.updateWert(0, BOID::aliStrength);/*
    params.updateWert(3, BOID::cohStrength);
    params.updateWert(4, std::sqrt(BOID::sepRangeSq) );
    params.updateWert(5, BOID::maxNbRange);
    //params.updateWert(9, BOID::v_rua);*/
    params.updateWert(1, BOID::normMov);
    params.updateWert(2, BOID::wrap);
    params.updateWert(3, BOID::dynRange);
    params.updateMode(4, colour_mode);
    params.updateWert(6, mouseAvoid);
    params.updateWert(7, spuren);
    params.placeLines();
/*
    for(int i = 0; i < nGes; i++){
        pBoid[i] = new BOID(i);
        }
        n = nGes;
        //params.updateWert(5,n);
*/
    pMouseBoid = new PRED(1001);

 std::srand(delta_t.asMilliseconds());
    //Ende Mainsetup
    while(window.isOpen()){
        eventhandler();
        //Mainloop

        if(n < nGes){ // create loop
            pBoid[n] = new BOID(n);
            n++;
            params.updateWert(5,n);
        } else{
            if(nPred < predGes){ // create loop
                pPred[((int)nPred)] = new PRED(n+nPred);
                nPred++;
                params.updateWert(5,n+nPred);
            }
        }

        for(int i = 0; i < zellenGes; i++){ //zellen leeren
            pZellen[i] = nullptr;
        }

        for(int i = 0; i < n; i++){ // Insert Boids in grid loop
            gridInsert(pBoid[i]);
        }
        for(int i = 0; i < nPred; i++){ // Insert Preds in grid loop
            gridInsert(pPred[i]);
        }
        if(mouseAvoid)
            gridInsert(pMouseBoid);

        for(int i = 0; i < n; i++){ // nachbarcheckloop for boids
            nachbarCheck(pBoid[i]);
        }
        for(int i = 0; i < nPred; i++){ // nachbarcheckloop for preds
            nachbarCheck(pPred[i]);
        }
        if(mouseAvoid)
            nachbarCheck(pMouseBoid);

        delta_t = uhr.restart();
        if(!spuren)
            window.clear(sf::Color(helligkeit,helligkeit,helligkeit,255)); //175
        for(int i = 0; i < n; i++){ // bewegen + renderloop
            (*pBoid[i]).bewegen(rand(), delta_t.asSeconds(), colour_mode);
            (*pBoid[i]).drawBoid(pWindow);
        }
        for(int i = 0; i < nPred; i++){ // bewegen + renderloop
            (*pPred[i]).bewegen(rand(), delta_t.asSeconds(), colour_mode);
            (*pPred[i]).drawBoid(pWindow);
        }
        if(mouseAvoid){
            (*pMouseBoid).drawBoid(pWindow);}
        params.draw(pWindow );
        window.display();
        //Ende Mainloop
    }
    //delete[] pBoid;
    //delete[] pPred;
    //delete pMouseBoid;
    return 0;
}
