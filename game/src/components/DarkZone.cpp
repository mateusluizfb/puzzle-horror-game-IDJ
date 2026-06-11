#include "DarkZone.h"
#include "Character.h"
#include "Collider.h"

DarkZone::DarkZone(GameObject& associated) : Component(associated) {
    associated.AddComponent(new Collider(associated));
}

void DarkZone::Update(float dt) {
    damageTimer.Update(dt);
}
void DarkZone::Render() {}

void DarkZone::NotifyCollision(GameObject& other) {
    Character* character = other.GetComponent<Character>();
    
    // Se quem pisou foi o jogador e passou 1 segundo desde o ultimo dano
    if (other.tag == "player") {
        if (damageTimer.Get() > 1.0f) { 
            character->TakeDamage();
            damageTimer.Restart();
        }
    }
}
