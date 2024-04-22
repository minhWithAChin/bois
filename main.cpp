#include <SFML/Graphics.hpp>
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
float BOID::swarmFollow;
float BOID::sepRangeSq;
int BOID::maxNbRange;
int BOID::zpR;
bool BOID::normMov;
bool BOID::wrap;
bool BOID::dynRange;

    const int breite = 1800;
    const int hoehe = 960; // Breite und Höhe des Fensters
    const int nGes = 1000;
    const int zellenGes = 1080;
    const int predGes = 0;
    int n = 0;
    int colour_mode = 4;
    bool mouseAvoid = true;
    BOID* pZellen[zellenGes];
    BOID* pBoid[nGes];
    BOID* pMouseBoid;

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

int main(){ //Mainsetup
    sf::RenderWindow window(sf::VideoMode(breite, hoehe), "BOIS");
    sf::RenderWindow* pWindow = &window;
    window.setFramerateLimit(60);
    if(nGes > 600)
        window.setFramerateLimit(60);
    sf::Clock uhr;
    sf::Time delta_t = uhr.restart();
sf::Vector2f v(0, 0);
    UI params(v, 7, 10, 5, 12, 24);

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
        BOID::sepStrength = 126;
        BOID::aliStrength = 0.25;//0.125;
        BOID::cohStrength = 1;
        BOID::fleeStrength = 512;
        BOID::swarmFollow = 0.125;
        BOID::sepRangeSq = 15*15;
        BOID::maxNbRange = 40;
        BOID::zpR = breite/(BOID::maxNbRange);
        BOID::normMov = false;
        BOID::wrap = true;
        BOID::dynRange = true;


/*    params.nameInsert("margin");
    params.nameInsert("borStrength");
    params.nameInsert("vMax");
    params.nameInsert("ranStrength");
    params.nameInsert("sepStrength");*/
    params.nameInsert("aliStrength");
/*    params.nameInsert("cohStrength");
    params.nameInsert("sepRange");
    params.nameInsert("maxNbRange");
    params.nameInsert("normalVelo");*/
    params.nameInsert("normMov");
    params.nameInsert("wrap");
    params.nameInsert("dynRange");
    params.nameInsert("colourMode");
    params.nameInsert("Anzahl");
    params.nameInsert("mouseAvoid");

/*    params.updateWert(0, BOID::margin);
    params.updateWert(1, BOID::borStrength);
    params.updateWert(2, BOID::vMax);
    params.updateWert(3, BOID::ranStrength);
    params.updateWert(4, BOID::sepStrength);*/
    params.updateWert(0, BOID::aliStrength);
/*    params.updateWert(6, BOID::cohStrength);
    params.updateWert(7, std::sqrt(BOID::sepRangeSq) );
    params.updateWert(8, BOID::maxNbRange);
    params.updateWert(9, BOID::v_rua);*/
    params.updateWert(1, BOID::normMov);
    params.updateWert(2, BOID::wrap);
    params.updateWert(3, BOID::dynRange);
    params.updateMode(4, colour_mode);
    params.updateWert(6, mouseAvoid);
    params.placeLines();
/*
    for(int i = 0; i < nGes; i++){
        pBoid[i] = new BOID(i);
        }
        n = nGes;
        params.updateWert(5,n);
*/
    pMouseBoid = new BOID(1001, true);

 std::srand(delta_t.asMilliseconds()+1000);
    //Ende Mainsetup
    while(window.isOpen()){
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
                             n = 0; break;
                    default: break;
                }
                params.updateWert(0, BOID::aliStrength);
                params.updateWert(1, BOID::normMov);
                params.updateWert(2, BOID::wrap);
                params.updateWert(3, BOID::dynRange);
                params.updateMode(4, colour_mode);
            }
            if(event.type == sf::Event::MouseButtonReleased){
                mouseAvoid = !mouseAvoid;
                params.updateWert(6, mouseAvoid);
            }
            if(mouseAvoid){
                (*pMouseBoid).tri.setPosition((sf::Vector2f)sf::Mouse::getPosition(*pWindow));
                (*pMouseBoid).spatialHash();
            }
        }
        //Mainloop

        if(n < nGes){ // create loop
            pBoid[n] = new BOID(n,(nGes-n)<=predGes);
            n++;
            params.updateWert(5,n);
        }

        for(int i = 0; i < zellenGes; i++){ //zellen leeren
            pZellen[i] = nullptr;
        }

        for(int i = 0; i < n; i++){ // Insert in grid loop
            gridInsert(pBoid[i]);
        }
        gridInsert(pMouseBoid);

        for(int i = 0; i < n; i++){ // nachbarcheckloop
            nachbarCheck(pBoid[i]);
        }
        nachbarCheck(pMouseBoid);

        delta_t = uhr.restart();
        window.clear(sf::Color(175,175,175,255)); //175
        for(int i = 0; i < n; i++){ // bewegen + renderloop
            (*pBoid[i]).bewegen(rand(), delta_t.asSeconds(), colour_mode);
            (*pBoid[i]).drawBoid(pWindow);
            (*pMouseBoid).drawBoid(pWindow);
        }
        params.draw(pWindow );
        window.display();
        //Ende Mainloop
    }
    for(int i = 0; i < n; i++){
        delete pBoid[i];

    }
    delete pMouseBoid;
    return 0;
}
