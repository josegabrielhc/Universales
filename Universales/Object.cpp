#include "Object.h"


Object::Object(int x, int y, Attributes attributes, TileMap tileMap): sp(""), tileMap(tileMap){
	attributes.width = 1;
	attributes.height = 1;
	this->attributes = attributes;
	tile = tileMap.GetTile(x, y);
	sp.Open(attributes.sprite);
	box = Rect(x - sp.GetWidth() / 2, y - 3*sp.GetHeight()/4, sp.GetWidth(), sp.GetHeight());
	timer.Restart();
	type = "Editting";

	int counter = 0;
	for (int i = tile.x; i >= tile.x - attributes.width + 1; i--){
		if (attributes.access[counter] == 1){
			Point accessPoint(i, tile.y - attributes.height);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}
	for (int i = tile.y - attributes.height + 1; i <= tile.y; i++){
		if (attributes.access[counter] == 1){
			Point accessPoint(tile.x - attributes.width, i);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}
	for (int i = tile.x - attributes.width + 1; i <= tile.x; i++){
		if (attributes.access[counter] == 1){
			Point accessPoint(i, tile.y + 1);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}
	for (int i = tile.y; i >= tile.y - attributes.height + 1; i--){
		if (attributes.access[counter] == 1){
			Point accessPoint(tile.x + 1, i);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}


}
Object::Object(ifstream &file, TileMap tileMap) : sp(""), tileMap(tileMap){
	int size;
	file.read(reinterpret_cast<char*> (&size), sizeof(int));
	attributes.name.resize(size);
	for (int p = 0; p < attributes.name.size(); p++){
		file.read(reinterpret_cast<char*> (&attributes.name[p]), sizeof(char));
	}
	file.read(reinterpret_cast<char*> (&size), sizeof(int));
	attributes.type.resize(size);
	for (int p = 0; p < size; p++){
		file.read(reinterpret_cast<char*> (&attributes.type[p]), sizeof(char));
	}
	file.read(reinterpret_cast<char*> (&size), sizeof(int));
	attributes.sprite.resize(size);
	for (int p = 0; p < size; p++){
		file.read(reinterpret_cast<char*> (&attributes.sprite[p]), sizeof(char));
	}
	file.read(reinterpret_cast<char*> (&size), sizeof(int));
	attributes.description.resize(size);
	for (int p = 0; p < size; p++){
		file.read(reinterpret_cast<char*> (&attributes.description[p]), sizeof(char));
	}
	file.read(reinterpret_cast<char*> (&attributes.cost), sizeof(int));
	file.read(reinterpret_cast<char*> (&attributes.activeHunger), sizeof(int));
	file.read(reinterpret_cast<char*> (&attributes.activeSatisfaction), sizeof(float));
	file.read(reinterpret_cast<char*> (&attributes.activeMoney), sizeof(int));
	file.read(reinterpret_cast<char*> (&attributes.radius), sizeof(int));
	file.read(reinterpret_cast<char*> (&attributes.passiveHunger), sizeof(int));
	file.read(reinterpret_cast<char*> (&attributes.passiveSatisfaction), sizeof(float));
	file.read(reinterpret_cast<char*> (&attributes.passiveMoney), sizeof(int));
	file.read(reinterpret_cast<char*> (&box), sizeof(Rect));
	file.read(reinterpret_cast<char*> (&tile), sizeof(Point));
	file.read(reinterpret_cast<char*> (&attributes.access), sizeof(attributes.access));
	file.read(reinterpret_cast<char*> (&attributes.width), sizeof(int));
	file.read(reinterpret_cast<char*> (&attributes.height), sizeof(int));
	file.read(reinterpret_cast<char*> (&timer), sizeof(Timer));
	file.read(reinterpret_cast<char*> (&size), sizeof(int));
	accessPoints.resize(size);
	for (int p = 0; p < size; p++){
		file.read(reinterpret_cast<char*> (&accessPoints[p]), sizeof(Point));
	}
	sp.Open(attributes.sprite);
	type = "Object";
}

void Object::Save(ofstream &file){
	int size = attributes.name.size();
	file.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < attributes.name.size(); p++){
		file.write(reinterpret_cast<char*> (&attributes.name[p]), sizeof(char));
	}
	size = attributes.type.size();
	file.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		file.write(reinterpret_cast<char*> (&attributes.type[p]), sizeof(char));
	}
	size = attributes.sprite.size();
	file.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		file.write(reinterpret_cast<char*> (&attributes.sprite[p]), sizeof(char));
	}
	size = attributes.description.size();
	file.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		file.write(reinterpret_cast<char*> (&attributes.description[p]), sizeof(char));
	}
	file.write(reinterpret_cast<char*> (&attributes.cost), sizeof(int));
	file.write(reinterpret_cast<char*> (&attributes.activeHunger), sizeof(int));
	file.write(reinterpret_cast<char*> (&attributes.activeSatisfaction), sizeof(float));
	file.write(reinterpret_cast<char*> (&attributes.activeMoney), sizeof(int));
	file.write(reinterpret_cast<char*> (&attributes.radius), sizeof(int));
	file.write(reinterpret_cast<char*> (&attributes.passiveHunger), sizeof(int));
	file.write(reinterpret_cast<char*> (&attributes.passiveSatisfaction), sizeof(float));
	file.write(reinterpret_cast<char*> (&attributes.passiveMoney), sizeof(int));
	file.write(reinterpret_cast<char*> (&box), sizeof(Rect));
	file.write(reinterpret_cast<char*> (&tile), sizeof(Point));
	file.write(reinterpret_cast<char*> (&attributes.access), sizeof(attributes.access));
	file.write(reinterpret_cast<char*> (&attributes.width), sizeof(int));
	file.write(reinterpret_cast<char*> (&attributes.height), sizeof(int));
	file.write(reinterpret_cast<char*> (&timer), sizeof(Timer));
	size = accessPoints.size();
	file.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		file.write(reinterpret_cast<char*> (&accessPoints[p]), sizeof(Point));
	}
}

Object::~Object(){
}


void Object::Render(int cameraX, int cameraY){
	sp.Render(box.x + Camera::pos.x, box.y + Camera::pos.y);
}

void Object::Update(float dt, vector<unique_ptr<GameObject>> *objectArray){
	timer.Update(dt);
	if (timer.Get() > 5){
		timer.Restart();
		for (int i = 0; i < objectArray->size(); i++){
			if (objectArray->at(i)->IsCharacter()){
				objectArray->at(i)->satisfaction += attributes.passiveSatisfaction;
				objectArray->at(i)->SetHunger(objectArray->at(i)->GetHunger() + attributes.passiveHunger);
				objectArray->at(i)->money += attributes.passiveMoney;
			}
		}
	}

}

void Object::MoveTo(int x, int y){
	type = "Editting";
	tile = tileMap.GetTile(x, y);
	Point pos = tileMap.GetTileCenter(tile);
	box = Rect(pos.x - sp.GetWidth() / 2, pos.y - 3 * sp.GetHeight() / 4, sp.GetWidth(), sp.GetHeight());
	vector<Point> accessPoints;

	int counter = 0;
	for (int i = tile.x; i >= tile.x - attributes.width + 1; i--){
		if (attributes.access[counter] == 1){
			Point accessPoint(i, tile.y - attributes.height);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}
	for (int i = tile.y - attributes.height + 1; i <= tile.y; i++){
		if (attributes.access[counter] == 1){
			Point accessPoint(tile.x - attributes.width, i);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}
	for (int i = tile.x - attributes.width + 1; i <= tile.x; i++){
		if (attributes.access[counter] == 1){
			Point accessPoint(i, tile.y + 1);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}
	for (int i = tile.y; i >= tile.y - attributes.height + 1; i--){
		if (attributes.access[counter] == 1){
			Point accessPoint(tile.x + 1, i);
			accessPoints.emplace_back(accessPoint);
		}
		counter++;
	}
	this->accessPoints = accessPoints;
}

bool Object::SettlePos(vector<int> obstacleMap){
	int index = tile.y*tileMap.GetWidth() + tile.x;
	if (obstacleMap[index] == -1){
		return false;
	}

	vector<vector<int>> heuristics;
	//Check access
	bool goodPosition = false;
	int counter = 0;
	for (int i = tile.x; i >= tile.x - attributes.width + 1; i--){
		Point accessPoint(i,tile.y - attributes.height);
		if (attributes.access[counter] == 1){
			index = (accessPoint.y)*tileMap.GetWidth() + accessPoint.x;
			if (obstacleMap[index] != -1){
				goodPosition = true;
			}
		}
		heuristics.emplace_back(CreateHeuristic(accessPoint));
		counter++;
	}
	for (int i = tile.y - attributes.height + 1; i <= tile.y; i++){
		Point accessPoint(tile.x - attributes.width, i);
		if (attributes.access[counter] == 1){
			index = (accessPoint.y)*tileMap.GetWidth() + accessPoint.x;
			if (obstacleMap[index] != -1){
				goodPosition = true;
			}
		}
		heuristics.emplace_back(CreateHeuristic(accessPoint));
		counter++;
	}
	for (int i = tile.x - attributes.width + 1; i <= tile.x; i++){
		Point accessPoint(i, tile.y + 1);
		if (attributes.access[counter] == 1){
			index = (accessPoint.y)*tileMap.GetWidth() + accessPoint.x;
			if (obstacleMap[index] != -1){
				goodPosition = true;
			}
		}
		heuristics.emplace_back(CreateHeuristic(accessPoint));
		counter++;
	}
	for (int i = tile.y; i >= tile.y - attributes.height + 1; i--){
		Point accessPoint(tile.x + 1, i);
		if (attributes.access[counter] == 1){
			index = (accessPoint.y)*tileMap.GetWidth() + accessPoint.x;
			if (obstacleMap[index] != -1){
				goodPosition = true;
			}
		}
		heuristics.emplace_back(CreateHeuristic(accessPoint));
		counter++;
	}
	heuristicArray = heuristics;
	if (goodPosition){
		type = "Object";
	}

	return goodPosition;

}


bool Object::Is(string type){
	return type == this->type;
}


vector<int> Object::CreateHeuristic(Point pos){
	vector<int> heuristic;
	int distX, distY;
	heuristic.resize(tileMap.GetWidth()*tileMap.GetHeight());

	for (int i = 0; i < heuristic.size(); i++){
		distX = i%tileMap.GetWidth() - pos.x;
		distY = ((int)(i / tileMap.GetWidth())) - pos.y;
		heuristic[i] = abs(distX) + abs(distY);
	}
	return heuristic;
}

vector<int> Object::GetHeuristic(int i){
	cout << "Hsize : " << heuristicArray.size() << endl;
	return heuristicArray[i];
}

bool Object::IsDead(){
	return false;
}

void Object::Editing(bool editing){

}
void Object::AddObjective(float x, float y, Point tile){

}
void Object::AddObjective(vector<int> path){

}

bool Object::IsCharacter(){
	return false;
}

string Object::Type(){
	return type;
}

string Object::GetChoice(){
	string a;
	return a;
}

int Object::GetHunger(){
	return 0;
}


void Object::SetHunger(int hunger){

}

ActionCharacter Object::GetAction(){
	return DECIDING_ROOM;
}

int Object::GetObjectIndex(){
	return -1;
}

void Object::UseObject(vector<unique_ptr<GameObject>> *objectArray, int index){

}

vector<int> Object::GetAttributes(){
	vector<int> att;
	att.emplace_back(attributes.activeHunger);
	att.emplace_back(attributes.activeMoney);
	att.emplace_back(attributes.activeSatisfaction);
	return att;
}

vector<Point> Object::GetAccessPoints(){
	return accessPoints;
}

vector<string> Object::GetTextAttributes(){
	vector<string> textAttributes;
	textAttributes.emplace_back(attributes.name);
	textAttributes.emplace_back(attributes.description);
	textAttributes.emplace_back(attributes.sprite);
	textAttributes.emplace_back(to_string(attributes.cost));
	return textAttributes;
}

Rect Object::GetBox(){
	return box;
}

Point Object::GetTile(){
	return tile;
}