#include "DialogueBox.h"
#include "GameObject.h"
#include "Game.h"
#include "InputManager.h"
#include "Log.h"
#include "SpriteRenderer.h"
#include "Sprite.h"
#include "GlobalSounds.h"

#include <sstream>

static const int TEXT_PADDING = 10;

DialogueBox::DialogueBox(GameObject& associated,
                         Rect box,
                         std::string fontFile,
                         int fontSize,
                         SDL_Color textColor,
                         PortraitMode portraitMode)
    : Component(associated),
      dialogBox{static_cast<int>(box.x), static_cast<int>(box.y),
                static_cast<int>(box.w), static_cast<int>(box.h)},
      backgroundColor{0, 0, 0, 255},
      borderColor{255, 255, 255, 255},
      borderThickness(2.0f),
      font(nullptr),
      textColor(textColor),
      fontSize(fontSize),
      fontFile(fontFile),
      fullText(""),
      currentPage(0),
      displayedText(""),
      typingTimer(0.0f),
      typingInterval(0.03f),
      selectedOption(0),
      optionColor{200, 200, 200, 255},
      optionSelectedColor{255, 255, 0, 255},
      currentState(State::IDLE),
      finished(false),
      borderVisible(true),
      continuePrompt("Aperte ESPAÇO para continuar"),
      typingSound(),
      typingSoundPlaying(false)
{
    Log::info("DIALOGUE_BOX - Created");

    std::string portraitFile = (portraitMode == PortraitMode::THINKING || portraitMode == PortraitMode::RAT_QUIZ)
        ? "game/assets/img/kid_large_thinking.png"
        : "game/assets/img/kid_large.png";
    SpriteRenderer *kidLargeSpriteRenderer = new SpriteRenderer(this->associated, portraitFile);
    kidLargeSpriteRenderer->SetScale(0.25, 0.25);
    kidLargeSpriteRenderer->SetPosition(100, 100);
    this->associated.AddComponent(kidLargeSpriteRenderer);

    if (portraitMode == PortraitMode::RAT_QUIZ) {
        ratPortrait = new Sprite("game/assets/img/rat_smiling.png");
        ratPortrait->SetScale(0.25, 0.25);
        ratPortrait->cameraFollower = true;
        ratPortraitW = ratPortrait->GetWidth();
        ratPortraitH = ratPortrait->GetHeight();
        ratPortraitX = Game::GetInstance().GetWindowWidth() - ratPortraitW - 100;
        ratPortraitY = 10;
    }
}

DialogueBox::~DialogueBox()
{
    Log::info("DIALOGUE_BOX - Destroyed");
    StopTypingSound();
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (ratPortrait) {
        delete ratPortrait;
        ratPortrait = nullptr;
    }
}

void DialogueBox::StopTypingSound()
{
    if (typingSoundPlaying) {
        try {
            typingSound.Stop();
        } catch (const std::runtime_error& e) {
            Log::error(std::string("DIALOGUE_BOX - Could not stop typing sound: ") + e.what());
        }
        typingSoundPlaying = false;
    }
}

void DialogueBox::Start()
{
    Log::info("DIALOGUE_BOX - Started");

    font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (!font) {
        Log::error("DIALOGUE_BOX - Failed to load font: " + fontFile +
                   " Error: " + TTF_GetError());
        return;
    }

    currentState = State::TYPING;
    typingTimer = 0.0f;

    try {
        typingSound.Open("game/assets/music/typing.mp3");
    } catch (const std::runtime_error& e) {
        Log::error(std::string("DIALOGUE_BOX - Could not open typing sound: ") + e.what());
    }
}

void DialogueBox::Update(float dt)
{
    InputManager& inputManager = InputManager::GetInstance();

    switch (currentState) {
    case State::IDLE:
        break;

    case State::TYPING:
        if (!typingSoundPlaying) {
            try {
                typingSound.Play(-1);
                typingSoundPlaying = true;
            } catch (const std::runtime_error& e) {
                Log::error(std::string("DIALOGUE_BOX - Could not play typing sound: ") + e.what());
            }
        }
        UpdateTyping(dt);
        if (inputManager.KeyPress(SPACE_KEY)) {
            displayedText = fullText;
            StopTypingSound();
            GlobalSounds::Button().Play(0);
            currentState = State::TEXT_SHOWN;
        }
        break;

    case State::TEXT_SHOWN:
        if (inputManager.KeyPress(SPACE_KEY) || inputManager.KeyPress(SDLK_RETURN)) {
            GlobalSounds::Button().Play(0);
            if (currentPage < static_cast<int>(pages.size()) - 1) {
                currentPage++;
                displayedText = fullText;
                currentState = State::TEXT_SHOWN;
            } else if (!options.empty()) {
                currentState = State::OPTIONS_SHOWN;
                selectedOption = 0;
            } else {
                finished = true;
            }
        }
        break;

    case State::OPTIONS_SHOWN:
        if (inputManager.KeyPress(UP_ARROW_KEY)) {
            NavigateUp();
        }
        if (inputManager.KeyPress(DOWN_ARROW_KEY)) {
            NavigateDown();
        }
        if (inputManager.KeyPress(SPACE_KEY)) {
            GlobalSounds::Button().Play(0);
            finished = true;
        }
        break;

    case State::SELECTION_MADE:
        break;
    }
}

void DialogueBox::UpdateTyping(float dt)
{
    typingTimer += dt;

    while (typingTimer >= typingInterval &&
           displayedText.length() < fullText.length()) {
        displayedText += fullText[displayedText.length()];
        typingTimer -= typingInterval;
    }

    if (displayedText.length() >= fullText.length()) {
        displayedText = fullText;
        StopTypingSound();
        currentState = State::TEXT_SHOWN;
    }
}

void DialogueBox::Render()
{
    RenderBackground();
    RenderBorder();

    if (ratPortrait) {
        ratPortrait->Render(ratPortraitX, ratPortraitY, ratPortraitW, ratPortraitH, 0.0f);
    }

    if (!font) return;

    if (currentState == State::OPTIONS_SHOWN) {
        RenderOptions();
    } else {
        RenderText();
    }

    RenderContinuePrompt();
}

/* -------------------------------------------------------------------------- */
/* Rendering helpers                                                          */
/* -------------------------------------------------------------------------- */

void DialogueBox::RenderBackground()
{
    SDL_Renderer* renderer = Game::GetRenderer();
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                           backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(renderer, &dialogBox);
}

void DialogueBox::RenderBorder()
{
    if (!borderVisible) return;

    SDL_Renderer* renderer = Game::GetRenderer();
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g,
                           borderColor.b, borderColor.a);

    int t = static_cast<int>(borderThickness);
    SDL_Rect top    = {dialogBox.x, dialogBox.y, dialogBox.w, t};
    SDL_Rect bottom = {dialogBox.x, dialogBox.y + dialogBox.h - t, dialogBox.w, t};
    SDL_Rect left   = {dialogBox.x, dialogBox.y, t, dialogBox.h};
    SDL_Rect right  = {dialogBox.x + dialogBox.w - t, dialogBox.y, t, dialogBox.h};

    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
}

void DialogueBox::RenderText()
{
    std::vector<std::string> lines = WrapText(displayedText);
    int lineH = fontSize + 4;
    int maxLines = (dialogBox.h - TEXT_PADDING * 2) / lineH;
    if (maxLines < 1) maxLines = 1;

    int total = static_cast<int>(lines.size());
    int start = total - maxLines;
    if (start < 0) start = 0;

    for (int i = start; i < total; i++) {
        int lineIndex = i - start;
        if (lineIndex >= maxLines) break;

        RenderSingleLine(lines[i],
                         dialogBox.x + TEXT_PADDING,
                         dialogBox.y + TEXT_PADDING + lineIndex * lineH,
                         textColor);
    }
}

void DialogueBox::RenderOptions()
{
    int lineH = fontSize + 8;
    int y = dialogBox.y + TEXT_PADDING;

    for (size_t i = 0; i < options.size(); i++) {
        SDL_Color color = (static_cast<int>(i) == selectedOption)
                          ? optionSelectedColor
                          : optionColor;

        std::string prefix = (static_cast<int>(i) == selectedOption) ? "> " : "  ";
        RenderSingleLine(prefix + options[i],
                         dialogBox.x + TEXT_PADDING,
                         y,
                         color);
        y += lineH;
    }
}

void DialogueBox::RenderContinuePrompt()
{
    if (!font) return;

    int w = 0, h = 0;
    TTF_SizeUTF8(font, continuePrompt.c_str(), &w, &h);
    int x = dialogBox.x + dialogBox.w - w - TEXT_PADDING;
    int y = dialogBox.y + dialogBox.h - h - TEXT_PADDING;

    SDL_Color promptColor = textColor;
    promptColor.a = 180;
    RenderSingleLine(continuePrompt, x, y, promptColor);
}

void DialogueBox::RenderSingleLine(const std::string& line, int x, int y, SDL_Color color)
{
    SDL_Renderer* renderer = Game::GetRenderer();

    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, line.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

/* -------------------------------------------------------------------------- */
/* Text helpers                                                               */
/* -------------------------------------------------------------------------- */

std::vector<std::string> DialogueBox::WrapText(const std::string& text)
{
    std::vector<std::string> lines;
    int maxWidth = dialogBox.w - TEXT_PADDING * 2;

    std::istringstream stream(text);
    std::string word;
    std::string line;

    while (stream >> word) {
        std::string test = line.empty() ? word : line + " " + word;
        int w, h;
        TTF_SizeUTF8(font, test.c_str(), &w, &h);

        if (w > maxWidth && !line.empty()) {
            lines.push_back(line);
            line = word;
        } else {
            line = test;
        }
    }
    if (!line.empty()) {
        lines.push_back(line);
    }

    return lines;
}

/* Navigation */

void DialogueBox::NavigateUp()
{
    if (options.empty()) return;
    selectedOption--;
    if (selectedOption < 0)
        selectedOption = static_cast<int>(options.size()) - 1;
}

void DialogueBox::NavigateDown()
{
    if (options.empty()) return;
    selectedOption++;
    if (selectedOption >= static_cast<int>(options.size()))
        selectedOption = 0;
}

/* -------------------------------------------------------------------------- */
/* Public setters / getters                                                   */
/* -------------------------------------------------------------------------- */

void DialogueBox::SetText(std::string text)
{
    fullText = text;
    displayedText = "";
    currentPage = 0;

    if (currentState != State::IDLE) {
        currentState = State::TYPING;
        typingTimer = 0.0f;
        typingSoundPlaying = false;
    }
}

void DialogueBox::SetOptions(std::vector<std::string> opts)
{
    options = std::move(opts);
    selectedOption = 0;
}

bool DialogueBox::IsBusy() const
{
    return currentState != State::IDLE && !finished;
}

int DialogueBox::GetSelectedOption() const
{
    return selectedOption;
}

bool DialogueBox::IsFinished() const
{
    return finished;
}
