#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <math.h>

struct BOID{
    int id;
    int hashPos = 0;
    int nachbar = 0;
    int predCount = 0;
    float dir = 3;// Richtung in Radien(2*PI = 360°)
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
    static float sepRangeSq;// Reichweite ab der man abgestoßen wird (muss quadriert werden)
    static int maxNbRange;// maximale Nachbar Reichweite
    static int zpR ; // Zellen pro Reihe
    static bool normMov;// Normalized Movement?
    static bool wrap;// Screenwrap?
    static bool dynRange;// Dynamic Nachbar Range
    static bool noMov; 

    sf::CircleShape tri{7,3};
    sf::Vector2f vNachbar{0,0};  // Geschwindigket der Nachbarn
    sf::Vector2f v{0,0};        // berechnete Geschwindigkeit von selbst
    sf::Vector2f m;              // Abweichung von Mittelpunkt der Nachbarn
    sf::Vector2f sepBuffer;              // speichert deltaV der Seperation
    sf::Vector2f avoidBuffer;              // speichert deltaV der Ausweichfunktion  
    BOID* next = nullptr;

    BOID( int i){ //Constructor
        tri.setOrigin(7.f,7.f);
//        gStrength = 256;    // Anziehungskraft/1 */
        m.x = wBreite/3;
        m.y = wHoehe/3;
        tri.setPosition(m);
        id = i;
        nachbar = 0;
        hashPos = 0;
        colourId();
        dir=i%6;
        v.y=(i%3)-1;
        v.x=(i%3)-1;
    }

    virtual void behavior(){    //unique class behaviors
        if(nachbar > 0){
            alignment();
                if(nachbar > 1){
                cohesion();
            }
        }
        nbRangeUpdate();
    }

    public: void bewegen(int noise, float delta_t, int colour_mode){ //Bewegen
        behavior();
        v.x += sepBuffer.x; v.y += sepBuffer.y;
        if(predCount>0){
            v.x += avoidBuffer.x; v.y += avoidBuffer.y;
        }
        vLimit();
        bMove(randDir(noise));
        tri.move(v*delta_t);             //#eigentliche Bewegung
        if(wrap){
            bWrap(tri.getPosition());
        } else{
            avoidBorder(tri.getPosition());
        }
        spatialHash();
        faerben(colour_mode);
        resetVar();
        return;
    }

    void resetVar(){
        nachbar = 0;
        predCount = 0;
        vNachbar = {0,0};
        m.x = 0;
        m.y = 0;
        sepBuffer={0,0};
        avoidBuffer={0,0};
        if(noMov){
            v.x =0 ;v.y=0;
        }
    }

    public: void spatialHash(){
        hashPos = zpR*((int)tri.getPosition().y/((int)maxNbRange));
        hashPos = hashPos + (int)tri.getPosition().x/((int)maxNbRange);
        if(hashPos < 0)
            hashPos=0;
        if(hashPos >= 1080)  // maxZellen - 1
            hashPos = 1079;
        if(hashPos < 0)  // keine negativen Zellen
            hashPos = 0;
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

    float dirUpdate(){
        return std::atan2(v.y,v.x);
    }

    void bMove(float ranDir){
        dir = dirUpdate() + ranDir;
        tri.setRotation((dir+1.57)*57.3); // convert RAD to DEG and Rotates Sprite by 90°
        if(normMov){
            v.x = cos(dir)*v_rua;
            v.y = sin(dir)*v_rua;
        } else{
            v.x += cos(dir);
            v.y += sin(dir);
        }
    }

    float randDir(int noise){ // gibt random Wert zurück [-0,125; 0,125]
        return  ((noise % 3)-1)*ranStrength;
    }

    void rand_v(int noiseX, int noiseY){ // ändert V um random Wert
        //std::cout << ((noise % 3)-1) <<';';
        v.x += ((noiseX % 7)-3)*2*ranStrength;
        v.y -= ((noiseY % 5)-2)*ranStrength;
    }

    void nbRangeUpdate(){
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

    void bWrap(sf::Vector2f pos){ // Wrap wenn es den Screenrand berührt //sollte in Main gehöhren
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

    void avoidBorder(sf::Vector2f pos){ // Wrap wenn es den Screenrand berührt //sollte in Main gehöhren
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

    virtual void vLimit(){
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

    float selfDotP(sf::Vector2f vec){
        return vec.x*vec.x+vec.y*vec.y;
    }
/**
    public: void ist_nachbar(sf::Vector2f pos_nachbar, sf::Vector2f v_nachbar, int id2){ //legacy
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
*/
    public: void istNachbarR(BOID* pAktiv){ // recursiv
        if(pAktiv != this){
            // code
            sf::Vector2f deltaPos = tri.getPosition() - (*pAktiv).tri.getPosition();
            int dotDeltaPos = selfDotP(deltaPos);

                nachbarReaction(deltaPos, dotDeltaPos, (*pAktiv).isPred, (*pAktiv).v);

        }
        if(next == nullptr){
            return;
        } else{
            (*next).istNachbarR(pAktiv);
        }
    }

    virtual void nachbarReaction(sf::Vector2f deltaPos, int dotDeltaPos, bool is_pred, sf::Vector2f v_n){
        if(is_pred){
            predCount++;
            avoidPred(deltaPos);
        } else {
            if(dotDeltaPos < nbRange*nbRange){
                nachbar++;
                vNachbar += v_n;
                m -= deltaPos;
                if(!isPred){
                    separation(deltaPos, dotDeltaPos);
                }
            }
        }
    }

    void faerben(int colour_mode){
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

    void colourNachbar(){
        colourCycle(((float)nachbar)/5);
    }

    void colourSpeed(){
        colourCycle((std::abs(v.x)+std::abs(v.y))*3/vMax);
    }

    void colourDirection(){
        colourCycle(dir);
    }

    void colourRange(){   //*6.5f ;
        colourCycle(-(nbRange/maxNbRange)*7.5f+1.0f);
    }

    virtual void colourDirSpeed(){
        float t1 = (v.x/vMax)*128+127;
        float t2 = (v.y/vMax)*128+127;
        float t3 = (510-t1-t2)/2;
        tri.setFillColor(sf::Color(t3,t2,t1,255));
    }

    void colourId(){    // ~1536/Periode
        colourCycle(((float)id)/256);
    }

    void colourHash(){    //
        colourCycle(hashPos);
        /*tri.setFillColor(sf::Color::Green);
        if(hashPos < 50)
            tri.setFillColor(sf::Color::Red);*/

    }

    void colourCycle(float t){
    tri.setFillColor(sf::Color(periodic(t),periodic(t+1.5f),periodic(t+3.f),255));}

    virtual void colourPreds(){
        if(predCount > 0){
            tri.setFillColor(sf::Color::Yellow);
        } else{
            tri.setFillColor(sf::Color::Green);
        }
    }

    void boidRotate(){
        tri.setRotation(sin(v.y));
        }

    void separation(sf::Vector2f deltaPos, int dotDeltaPos){
        if(deltaPos.x !=0 && deltaPos.y != 0){
            if(dotDeltaPos < sepRangeSq && sepStrength > 0){
                sepBuffer.x += sepStrength/deltaPos.x;
                sepBuffer.y += sepStrength/deltaPos.y;
            }
        }
    }

    void alignment(){
        sf::Vector2f deltaVNachbar((vNachbar.x/nachbar)-v.x, ((vNachbar.y/nachbar)-v.y)); // Berechnet Abweichung vom Durchschnitt der Nachbarn
        v.x += deltaVNachbar.x*aliStrength;
        v.y += deltaVNachbar.y*aliStrength;
    }

    void cohesion(){
        v.x += aliStrength*m.x/nachbar;
        v.y += aliStrength*m.y/nachbar;
    }

    void avoidPred(sf::Vector2f deltaPos){
        avoidBuffer.x += fleeStrength/deltaPos.x;
        avoidBuffer.y += fleeStrength/deltaPos.y;
    }

    sf::Uint8 periodic(float t){
        return (sf::Uint8)(std::round((std::sin(t)+1)*127));

    }

};

struct PRED : BOID{
    sf::Vector2f targetDeltaPos;
    float shortDist;

    PRED(int i):BOID(i){
        tri.setPointCount(4);
        tri.setScale(1.5f,2.f);
        BOID::isPred = true;
    }

    void behavior() override{ //Bewegen
        if(nachbar > 0){
            hunt(targetDeltaPos);
        }

    }

    void nachbarReaction(sf::Vector2f deltaPos, int dotDeltaPos, bool is_pred, sf::Vector2f v_n) override{
        if(is_pred){
            predCount++;
            avoidPred(deltaPos);
            separation(deltaPos, dotDeltaPos);
        } else{
            if(nachbar == 0 || dotDeltaPos < shortDist){
                targetDeltaPos = deltaPos;
                shortDist = dotDeltaPos;
            }
            nachbar++;
        }
    }

    void vLimit() override {
        if(v.x > vMax*0.5 ){
            v.x = vMax*0.5;}
        else{
            if(v.x < -vMax*0.5)
                v.x = -vMax*0.5;}

        if(v.y > vMax*0.5){
            v.y = vMax*0.5;}
        else{
            if(v.y < -vMax*0.5)
                v.y = -vMax*0.5; }
    }

    void colourDirSpeed() override{
        float t1 = (v.x/vMax)*256+127;
        float t2 = (v.y/vMax)*256+127;
        float t3 = (512-t1-t2)/2;
        tri.setFillColor(sf::Color(t3,t2,t1,255));
    }

    void colourPreds() override{
        if(nachbar > 0){
            tri.setFillColor(sf::Color::Red);
        } else{
            tri.setFillColor(sf::Color::Blue);
        }
    }


    void hunt(sf::Vector2f deltaPos){
        v.x -= deltaPos.x;
        v.y -= deltaPos.y;
    }
};
