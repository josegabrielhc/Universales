#include "Wall.h"
#include "GameObject.h"
#include "Camera.h"

Wall::Wall(float centerX, float centerY, string file, WallStyle lStyle, Point lTile, int lRoom){
	editing = false;
	tile = lTile;
    roomID = lRoom;
    wall.Open(file);
	style = lStyle;
    switch (lStyle) {
    case UPPER_RIGHT:
        box = Rect(centerX,centerY-wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
        break;
    case UPPER_LEFT:
        box = Rect(centerX,centerY-wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
        break;
    case UPPER_CORNER:
        box = Rect(centerX-wall.GetWidth()/2, centerY-wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
        break;
    case LEFT_CORNER:
        box = Rect(centerX-wall.GetWidth()/2, centerY-wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
        break;
	case RIGHT_CORNER:
		box = Rect(centerX - wall.GetWidth()/2, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
	case INF_CORNER:
		box = Rect(centerX - wall.GetWidth() / 2, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
    default:
        break;
    }

}

Wall::~Wall(){

}

void Wall::Update(float dt, vector<unique_ptr<GameObject>> *objectArray){
	editTimer.Update(dt);
}

void Wall::Render(int cameraX, int cameraY){
	wall.Render(box.x + Camera::pos.x, box.y + Camera::pos.y);
}

bool Wall::IsDead(){
	return false;
}

void Wall::NotifyCollision(GameObject &other){

}

bool Wall::Is(string type){
    return type == "Wall";
}

bool Wall::IsCharacter(){
	return false;
}

string Wall::Type(){
	switch (style)
	{
	case UPPER_RIGHT:
		return "UPPER_RIGHT";
		break;
	case UPPER_LEFT:
		return "UPPER_LEFT";
		break;
	case LOWER_RIGHT:
		return "LOWER_RIGHT";
		break;
	case LOWER_LEFT:
		return "LOWER_LEFT";
		break;
	case UPPER_CORNER:
		return "UPPER_CORNER";
		break;
	case LEFT_CORNER:
		return "LEFT_CORNER";
		break;
	case RIGHT_CORNER:
		return "RIGHT_CORNER";
		break;
	case INF_CORNER:
		return "INF_CORNER";
		break;
	default:
		return "Wall";
		break;
	}
}

void Wall::Editing(bool editing) {
	this->editing = editing;
}

void Wall::AddObjective(float x, float y, Point tile){
}

void Wall::AddObjective(vector<int> path){
}

int Wall::GetHunger(){
	return 0;
}

void Wall::SetHunger(int hunger){
	
}

string Wall::GetChoice(){
	string a;
	return a;
}

void Wall::MoveTo(int x, int y){
	int centerX = x;
	int centerY = y;
	switch (style) {
	case UPPER_RIGHT:
		box = Rect(centerX, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
	case UPPER_LEFT:
		box = Rect(centerX, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
	case UPPER_CORNER:
		box = Rect(centerX - wall.GetWidth() / 2, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
	case LEFT_CORNER:
		box = Rect(centerX - wall.GetWidth() / 2, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
	case RIGHT_CORNER:
		box = Rect(centerX - wall.GetWidth() / 2, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
	case INF_CORNER:
		box = Rect(centerX - wall.GetWidth() / 2, centerY - wall.GetHeight(), wall.GetWidth(), wall.GetHeight());
		break;
	default:
		break;
	}
}

bool Wall::SettlePos(vector<int> obstacleMap){
	return false;
}

ActionCharacter Wall::GetAction(){
	return DECIDING_ROOM;
}

int Wall::GetObjectIndex(){
	return -1;
}

vector<int> Wall::GetHeuristic(int i){
	vector<int> a;
	return a;
}
vector<int> Wall::GetAttributes(){
	vector<int> a;
	return a;
}

void Wall::UseObject(vector<unique_ptr<GameObject>> *objectArray, int index){

}

vector<Point> Wall::GetAccessPoints(){
	vector<Point> a;
	return a;
}

vector<string> Wall::GetTextAttributes(){
	vector<string> a;
	return a;
}

Rect Wall::GetBox(){
	return box;
}

Point Wall::GetTile(){
	return tile;
}

void Wall::Save(ofstream &file){

}