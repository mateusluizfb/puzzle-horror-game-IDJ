#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component.h"
#include "Rect.h"
#include "Vec2.h"

/*

Collider (herda de Component)
+ Collider (associated : GameObject&,
scale : Vec2 = {1, 1},
offset : Vec2 = {0, 0})
+ box : Rect
+ Update (dt : float) : void
+ Render () : void
+ SetScale (scale : Vec2)
+ SetOffset (offset : Vec2)
- scale : Vec2
- offset : Vec2

*/

class Collider : public Component {
private:
  Rect box;
  Vec2 scale;
  Vec2 offset;
  bool isComposite = false;

public:
  Collider(GameObject &associated, Vec2 scale = Vec2(1, 1), Vec2 offset = Vec2(0, 0));

  void Update(float dt);
  void Render();
  void SetScale(Vec2 scale);
  void SetOffset(Vec2 offset);
  Rect& GetBox();
  void SetComposite(bool composite) { isComposite = composite; }
  bool GetIsComposite() const { return isComposite; }
};

#endif
