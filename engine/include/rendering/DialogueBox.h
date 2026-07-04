#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#define INCLUDE_SDL_TTF
#define INCLUDE_SDL
#include "SDL_include.h"

#include <string>
#include <vector>
#include "Component.h"
#include "Rect.h"
#include "Sound.h"

class DialogueBox : public Component {
public:
    enum class PortraitMode { STILL, THINKING };

private:
    enum class State { IDLE, TYPING, TEXT_SHOWN, OPTIONS_SHOWN, SELECTION_MADE };

    SDL_Rect dialogBox;
    SDL_Color backgroundColor;
    SDL_Color borderColor;
    float borderThickness;

    TTF_Font* font;
    SDL_Color textColor;
    int fontSize;
    std::string fontFile;

    std::string fullText;
    std::vector<std::string> pages;
    int currentPage;
    std::string displayedText;
    float typingTimer;
    float typingInterval;

    std::vector<std::string> options;
    int selectedOption;
    SDL_Color optionColor;
    SDL_Color optionSelectedColor;

    State currentState;
    bool finished;
    bool borderVisible;

    Sound typingSound;
    bool typingSoundPlaying;

    void StopTypingSound();

    void RenderBackground();
    void RenderBorder();
    void RenderText();
    void RenderOptions();
    void BuildPages();
    void UpdateTyping(float dt);
    void UpdateState(float dt);
    void NavigateUp();
    void NavigateDown();
    std::vector<std::string> WrapText(const std::string& text);
    void RenderSingleLine(const std::string& line, int x, int y, SDL_Color color);

public:
    DialogueBox(GameObject& associated,
                Rect box,
                std::string fontFile,
                int fontSize,
                SDL_Color textColor,
                PortraitMode portraitMode = PortraitMode::STILL);

    ~DialogueBox();

    void Update(float dt) override;
    void Render() override;

    void SetText(std::string text);
    void SetOptions(std::vector<std::string> options);
    bool IsBusy() const;
    int GetSelectedOption() const;
    bool IsFinished() const;

    void Start() override;
};

#endif
