#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <math.h>

struct BOID{
    int id;
    int hashPos = 0;
    int nachbar = 0;
    int predCount = 0;
    float dir = 0;// Richtung in Radien(2*PI = 360°)
    float nbRange = maxNbRange;// Reichweite ab der man Nachbar ist (muss quadriert werden)
    bool isPred = false;
    int colourMode = 0;// Wie das Dreieck gefärbt werden soll
//    float , gStrength;
    static int wBreite, wHoehe;
    static int v_rua; //Geschwindigkeit (Richtungsunabhängig)
    static int margin;// Reichweite der Abstoßung von den Rändern
    static float borStrength;// Stärke der Abstoßung von den Rändern
    static int vMax;// Maximale Geschwindigkeit in X und Y-Richtung
    static float ranStrength;// Stärke der zufälligen Richtung
    static int sepStrength;// Stärke der Abstoßung voneinander
    static float aliStrength;// Stärke der Richtungsangleichung
    static float cohStrength;// Stärke, in der Mitte seiner Nachbarn zu sein
    static int fleeStrength;// Stärke von der von Jägern geflohen wird
    static float swarmFollow;// wie Stark die Jäger den Schwarm folgen
    static float sepRangeSq;// Reichweite ab der man abgestoßen wird (muss quadriert werden)
    static int maxNbRange;// maximale Nachbar Reichweite
    static int zpR ; // Zellen pro Reihe
    static bool normMov;// Normalized Movement?
    static bool wrap;// Screenwrap?
    static bool dynRange;// Dynamic Nachbar Range

    sf::CircleShape tri{7,3};
    sf::Vector2f vNachbar{0,0};  // Geschwindigket der Nachbarn
    sf::Vector2f v{0,0};        // berechnete Geschwindigkeit von selbst
    sf::Vector2f m;              // Abweichung von Mittelpunkt der Nachbarn
    sf::Vector2f predPos{0,0};
    BOID* next = nullptr;

    BOID( int i, bool is_pred){ //Constructor
        if(is_pred){
            tri.setPointCount(4);
            tri.setScale(1.5f,2.f);
        }
        tri.setOrigin(7.f,7.f);
//        gStrength = 256;    // Anziehungskraft/1 */
        m.x = wBreite/3;
        m.y = wHoehe/3;
        isPred = is_pred;
        tri.setPosition(m);
        id = i;
        nachbar = 0;
        hashPos = 0;
        colourId();
    }

    public: void bewegen(int noise, float delta_t, int colour_mode){ //Bewegen
        /*if(next != nullptr){
            (*next).bewegen(noise, delta_t, colour_mode);
        }*/
        if(nachbar > 0){
            alignment();
                if(nachbar > 1){
                cohesion();
            }
        }
        nbRangeUpdate();
        spatialHash();
        faerben(colour_mode);
        if(wrap){
            bWrap(tri.getPosition());
        } else{
            avoidBorder(tri.getPosition());
        }
        if(isPred){
            vLimitPred();
        } else{
            vLimit();
        }
        bMove(randDir(noise),delta_t);
        nachbar = 0;
        predCount = 0;
        vNachbar.x = 0;
        vNachbar.y = 0;
        m.x = 0;
        m.y = 0;
        predPos.x = 0;
        predPos.y = 0;
//        v.x--;v.y--;
        return;
    }

    public: void spatialHash(){
        hashPos = zpR*((int)tri.getPosition().y/(maxNbRange));
        hashPos = hashPos + (int)tri.getPosition().x/(maxNbRange);
        if(hashPos < 0)
            hashPos=0;
        if(hashPos >= 1080)  // maxZellen - 1
            hashPos = 1079;
    }

    public: void insertPBoid(BOID* pBoid){
        if(next == nullptr){
            next = pBoid;
        }else{
            (*next).insertPBoid(pBoid);
        }
    }

    public: void drawBoid(sf::RenderWindow* renderer){
        (*renderer).draw(tri);/*
        if(next == nullptr){
            return;
        }else{
            (*next).drawBoid(renderer);
        }*/
    }

    private: float dirUpdate(){
        if(v.x != 0)
            return std::atan2(v.y,v.x);
        return 0;
    }

    private: void bMove(float newDir, float delta_t){
        dir = dirUpdate() + newDir;
        tri.setRotation((dir+1.57)*57.3);
        if(normMov){
            v.x = cos(dir)*v_rua;
            v.y = sin(dir)*v_rua;
            tri.move(v*delta_t);
        } else{
            v.x += cos(dir+newDir);
            v.y += sin(dir+newDir);
            tri.move(v*delta_t);
        }
    }

    private: float randDir(int noise){ // gibt random Wert zurück [-0,125; 0,125]
        return  ((noise % 3)-1)*ranStrength;
    }

    private: void rand_v(int noiseX, int noiseY){ // ändert V um random Wert
        //std::cout << ((noise % 3)-1) <<';';
        v.x += ((noiseX % 7)-3)*2*ranStrength;
        v.y -= ((noiseY % 5)-2)*ranStrength;
    }

    private: void nbRangeUpdate(){
        if(dynRange){
            if(nachbar < 6 && nbRange < maxNbRange){
                nbRange++;
            } else{
                if(nachbar > 6)
                    nbRange--;
            }
        } else {
            if( nbRange < maxNbRange)
                nbRange++;
        }
    }

    private: void bWrap(sf::Vector2f pos){ // Wrap wenn es den Screenrand berührt //sollte in Main gehöhren
        if(pos.x > wBreite){
            tri.move(-wBreite, 0);}
        else{
             if(pos.x < 0)
                tri.move(wBreite,0);}

        if(pos.y > wHoehe){
            tri.move(0, -wHoehe);}
        else{
            if(pos.y < 0)
                tri.move(0, wHoehe);}
    }

    private: void avoidBorder(sf::Vector2f pos){ // Wrap wenn es den Screenrand berührt //sollte in Main gehöhren
        if(pos.x > wBreite - margin) {
            v.x -= borStrength;
            if(pos.x > wBreite) {
                v.x = -vMax;}}
        else{
            if(pos.x < margin){
                v.x += borStrength;}
            if(pos.x < 0) {
                v.x = vMax;}}

        if(pos.y > wHoehe - margin){
            v.y -= borStrength;
            if(pos.y > wHoehe) {
                v.y = -vMax;}}
        else{
            if(pos.y < margin){
                v.y += borStrength;}
            if(pos.y < 0) {
                v.y = vMax;}}
    }

    private: void vLimit(){
        if(v.x > vMax ){
            v.x = vMax-10;}
        else{
            if(v.x < -vMax)
                v.x = -vMax+10;}

        if(v.y > vMax){
            v.y = vMax-10;}
        else{
            if(v.y < -vMax)
                v.y = -vMax+10; }
    }

    private: void vLimitPred(){
        if(v.x > vMax/2 ){
            v.x = vMax/2;}
        else{
            if(v.x < -vMax/2)
                v.x = -vMax/2;}

        if(v.y > vMax/2){
            v.y = vMax/2;}
        else{
            if(v.y < -vMax/2)
                v.y = -vMax/2; }
    }

    private: float selfDotP(sf::Vector2f vec){
        return vec.x*vec.x+vec.y*vec.y;
    }

    public: void ist_nachbar(sf::Vector2f pos_nachbar, sf::Vector2f v_nachbar, int id2){ //prüft ob der Boid ein Nachbar ist und zählt
        if(id2 == id)
            return;
        sf::Vector2f deltaPos = tri.getPosition() - pos_nachbar;
        int dotDeltaPos = selfDotP(deltaPos);
        if(dotDeltaPos < nbRange*nbRange){
            nachbar++;
            vNachbar += v_nachbar;
            m += pos_nachbar;
            separation(deltaPos, dotDeltaPos);
        }
    }

    public: void istNachbarR(BOID* pAktiv){ // recursiv
        if(pAktiv != this){
            // code
            sf::Vector2f deltaPos = tri.getPosition() - (*pAktiv).tri.getPosition();
            int dotDeltaPos = selfDotP(deltaPos);
            if((*pAktiv).isPred){
                predCount++;
                predPos += deltaPos;
                avoidPred(deltaPos);
            } else {
                if(dotDeltaPos < nbRange*nbRange){
                    nachbar++;
                    vNachbar += (*pAktiv).v;
                    m -= deltaPos;
                    if(!isPred){
                        separation(deltaPos, dotDeltaPos);
                    }
                }
            }

        }
        if(next == nullptr){
            return;
        } else{
            (*next).istNachbarR(pAktiv);
        }
    }


    private: void faerben(int colour_mode){
        if(colour_mode != colourMode){
            colourMode = colour_mode;
            if(colour_mode == 0){
                colourId();
            }
        }
        switch(colourMode){
            case 1: colourNachbar(); break;
            case 2: colourRange(); break;
            case 3: colourSpeed(); break;
            case 4: colourDirection(); break;
            case 5: colourDirSpeed(); break;
            case 6: colourHash(); break;
            case 7: colourPreds(); break;
        }
    }

    private: void colourNachbar(){
        float t = ((float)nachbar)/5;
        tri.setFillColor(sf::Color(periodic(t),periodic(t+1.5f),periodic(t+3.f),255));
    }

    private: void colourSpeed(){
        float t = (std::abs(v.x)+std::abs(v.y))*3/vMax ;
        tri.setFillColor(sf::Color(periodic(t),periodic(t+1.5f),periodic(t+3.f),255));
    }

    private: void colourDirection(){
        tri.setFillColor(sf::Color(periodic(dir),periodic(dir+1.5f),periodic(dir+3.f),255));
    }

    private: void colourRange(){
        float t = -(nbRange/maxNbRange)*6.5f  ;   //*6.5f ;
        tri.setFillColor(sf::Color(periodic(t),periodic(t+1.5f),periodic(t+3.f),255));
    }

    private: void colourDirSpeed(){
        int t1 = ((v.x+vMax)/vMax)*128;
        int t2 = ((v.y+vMax)/vMax)*128;
        int t3 = (512-t1-t2)/2;
        tri.setFillColor(sf::Color(t3-1,t2-1,t1-1,255));
    }

    private: void colourId(){    // ~ 12/Periode
        float t = ((float)id)/2;
        tri.setFillColor(sf::Color(periodic(t),periodic(t+1.5f),periodic(t+3.f),255));
    }

    private: void colourHash(){    //
        float t = hashPos;
        tri.setFillColor(sf::Color(periodic(t),periodic(t+1.5f),periodic(t+3.f),255));
        /*tri.setFillColor(sf::Color::Green);
        if(hashPos < 50)
            tri.setFillColor(sf::Color::Red);*/

    }

    private: void colourPreds(){
        if(isPred){
            if(nachbar > 0){
                tri.setFillColor(sf::Color::Red);
            } else{
                tri.setFillColor(sf::Color::Blue);
            }
        } else{
            if(predCount > 0){
                tri.setFillColor(sf::Color::Yellow);
            } else{
                tri.setFillColor(sf::Color::Green);
            }
        }
    }

    private: void boidRotate(){
        tri.setRotation(sin(v.y));
        }

    private: void separation(sf::Vector2f deltaPos, int dotDeltaPos){
        if(deltaPos.x !=0 && deltaPos.y != 0){
            if(dotDeltaPos < sepRangeSq && sepStrength > 0){
                v.x += sepStrength/deltaPos.x;
                v.y += sepStrength/deltaPos.y;
            }
        }
    }

    private: void alignment(){
        sf::Vector2f deltaVNachbar((vNachbar.x/nachbar)-v.x, ((vNachbar.y/nachbar)-v.y)); // Berechnet Abweichung vom Durchschnitt der Nachbarn
        if(isPred){
            v.x += deltaVNachbar.x*aliStrength*swarmFollow;
            v.y += deltaVNachbar.y*aliStrength*swarmFollow;
        } else{
            v.x += deltaVNachbar.x*aliStrength;
            v.y += deltaVNachbar.y*aliStrength;
        }
    }

    private: void cohesion(){
        v.x += aliStrength*m.x/nachbar;
        v.y += aliStrength*m.y/nachbar;
    }

    private: void avoidPred(sf::Vector2f deltaPos){
        v.x += fleeStrength/deltaPos.x;
        v.y += fleeStrength/deltaPos.y;
    }

    private: sf::Uint8 periodic(float t){
        return (sf::Uint8)(std::round((std::sin(t)+1)*127));

    }

};
