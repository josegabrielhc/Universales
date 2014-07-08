#include "CheatState.h"

CheatState::CheatState(State *state, StateData *data) :text("font/enhanced_dot_digital-7.ttf", 30, Text::TEXT_BLENDED, "-", WHITE, 100)
{
	SDL_Surface *surface = NULL;
	int w, h;
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 400, 40, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
	texture = SDL_CreateTextureFromSurface(Game::GetInstance().GetRenderer(), surface);
	SDL_FreeSurface(surface);
	SDL_QueryTexture(texture, 0, 0, &w, &h);
	box.SetRect(box.GetXrect(), box.GetYrect(), w, h);
	this->state = state;
	this->data = data;
	text.SetPos(512, 300, true, true);
}


CheatState::~CheatState()
{
}

void CheatState::Update(float dt){
	Input();

}
void CheatState::Render(){
	state->Render();
	SDL_Rect dstrec, clipRect;
	dstrec.x = 512 - box.GetWrect()/2;
	dstrec.y = 300 - box.GetHrect()/2;
	dstrec.w = box.GetWrect();
	dstrec.h = box.GetHrect();
	clipRect.x = 0;
	clipRect.y = 0;
	clipRect.w = box.GetWrect();
	clipRect.h = box.GetHrect();
	SDL_RenderCopy(Game::GetInstance().GetRenderer(), texture, &clipRect, &dstrec);
	text.Render(0, 0);
}

void CheatState::Input(){
	if (InputManager::GetInstance().KeyPress(SDLK_ESCAPE)){
		requestDelete = true;
	}
	if (msg.size() <= 21){
		for (char c = 32; c <= 122; c++){
			if (InputManager::GetInstance().KeyPress(c)){
				msg += c;
				text.SetText(msg);
				text.SetPos(512, 300, true, true);
			}
		}
	}
	if (InputManager::GetInstance().KeyPress(SDLK_BACKSPACE)){
		if (msg.size() > 0){
			msg.pop_back();
			text.SetText(msg);
			text.SetPos(512, 300, true, true);
		}
	}
	if (InputManager::GetInstance().KeyPress(SDLK_RETURN)){
		ChechCheat();
		requestDelete = true;
	}
}
void CheatState::ChechCheat(){
	if (msg == "gibemonipls"){
		data->money += 1000;
	}
	if (msg == "fabulous"){
		data->fame += 64;
	}
}