/*
Text (herda de Component)
+ TextStyle : enum (SOLID, SHADED, BLENDED)
+ Text (associated : GameObject&,
fontFile : std::string,
fontSize : int,
style : TextStyle,
text : std::string,
color : SDL_Color)
+ ~Text ()
+ Update (dt : float) : void
+ Render () : void
+ SetText (text : std::string) : void
+ SetColor (color : SDL_Color) : void
+ SetStyle (style : TextStyle) : void
+ SetFontFile (fontFile : std::string) : void
+ SetFontSize (fontSize : int) : void
- RemakeTexture () : void
- font : TTF_Font*
- texture : SDL_Texture*
- text : std::string
- style : TextStyle
- fontFile : std::string
- fontSize : int
- color : SDL_Color
*/

#ifndef TEXT_H
#define TEXT_H

#define INCLUDE_SDL_TTF
#define INCLUDE_SDL
#include "SDL_include.h"

#include <string>
#include "Component.h"

class Text : public Component {
public:
  enum TextStyle { SOLID, SHADED, BLENDED };

  Text(GameObject &associated,
       std::string fontFile,
       int fontSize,
       TextStyle style,
       std::string text,
       SDL_Color color);
       
  ~Text();

  void Update(float dt) override;
  void Render() override;

  void SetText(std::string text);
  void SetColor(SDL_Color color);
  void SetStyle(TextStyle style);
  void SetFontFile(std::string fontFile);
  void SetFontSize(int fontSize);
  void Show();
  void Hide();

private:
  TTF_Font *font;
  SDL_Texture *texture;
  std::string text;
  TextStyle style;
  std::string fontFile;
  int fontSize;
  SDL_Color color;
  bool hidden = false;

  void RemakeTexture();
};

#endif
