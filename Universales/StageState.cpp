#include "StageState.h"
#include "EndState.h"
#include "Music.h"
#include "StateData.h"
#include "Permonkey.h"
#include "CostComparator.h"
#include "Camera.h"
#include "Object.h"

#include <fstream>
#include <string>
#include <vector>

StageState::StageState() : tileSet(152,76), tileMap("map/tileMap.txt", &tileSet),
moneyText("font/enhanced_dot_digital-7.ttf", 40, Text::TEXT_BLENDED, "-", WHITE, 100), occupancyMap(tileMap.GetWidth(), tileMap.GetWidth()), sheet(PERMONKEY), subGuiEdit("img/icons/door.png", "img/icons/wall.jpg"),
okTile("img/tileset/tile_grama_1.png"), noTile("img/tileset/tile_grama_2.png"),
music("music/can_o do vinho.ogg"),
click("music/click_interface.wav"){
	cMusic = 0;
	music.Play(1);
	cout << music.isOverInd << endl;
	instance = this;
	string file, file2, tile, line, endLine("\n"), initFile("img/tileset/");
	FILE *tileFile, *objectFile;
	Point roomBegin, roomEnd;
	ifstream objFile, roomFile;
	char c;
	vector<string> objectList, roomList;

	obstacleMap = occupancyMap.Update(&tileMap, &objectArray);

	creationTimer.Restart();

	rType = CORRIDOR;

	roomBegin.SetPoint(0, 0);
	roomEnd.SetPoint(25, 25);

	action = NONE;


	bg.Open("img/fundo_espaco.png");

	//Room *newRoom = new Room(roomBegin, roomEnd, &tileMap, &objectArray, roomArray.size(), rType);
	//roomArray.emplace_back(newRoom);

	file = "img/tileset/tilelist.txt";
	tileFile = fopen(file.c_str(), "r");
    if(tileFile == NULL)
        exit(EXIT_FAILURE);

    while (!feof(tileFile)) {
        fscanf(tileFile, "%c", &c);
        line = c;
        while(line.find(endLine)== string::npos){
            fscanf(tileFile, "%c", &c);
            line = line + c;
        }

        line.pop_back();
        tile = initFile+line;
        if(tile.compare(initFile)!= 0)
            tileSet.Open(tile);
    }
	fclose(tileFile);
	cout << "Aqui inicio" << endl;
	switch (StateData::langInd){
	case 0:
		objFile.open("obj/pt-br-objects.txt");
		break;
	case 1:
		objFile.open("obj/en-objects.txt");
		break;
	}
	cout << "Aqui inicio" << endl;
	

	while (!objFile.eof()){
		getline(objFile, file);
		objectList.emplace_back(file);
	}

	
	for (int i = 0; i < objectList.size(); i++){
		cout << objectList[i] << endl;
	}

	cout << "Ended" << endl;

	ParseObject(objectList);

	roomFile.open("room/rooms.txt");

	while (!roomFile.eof()){
		getline(roomFile, file);
		roomList.emplace_back(file);
	}


	for (int i = 0; i < roomList.size(); i++){
		cout << roomList[i] << endl;
	}

	ParseRoom(roomList);

	Point monkeyTile(4, 4);
	Point teste(4, 4);
	monkeyTile = tileMap.GetTileCenter(monkeyTile);
	Permonkey* pM = new Permonkey(monkeyTile.x, monkeyTile.y, teste, tileMap, objList);
	objectArray.emplace_back(pM);

	data = new StateData();
	data->money = 5000;
	data->fame = 128;
	moneyText.SetText(to_string(data->money));
	moneyText.SetPos(20, 20, false, false);
}

StageState::~StageState() {
	delete data;
	objectArray.clear();
	roomArray.clear();
	//Music::Clear();
	//Sprite::Clear();
}

void StageState::Update(float dt) {
	if (music.isOverInd){
		cMusic++;
		if (cMusic == musicNames.size()){
			cMusic = 0;
		}
		music.Open(musicNames[cMusic]);
		music.Play(1);
	}
	int size = 0;
	int cost = 0;
	Input();
	creationTimer.Update(dt);
	switch (action)	{
	case NONE:
		break;
	case TILE_SELECT:
		p = tileMap.GetTile(InputManager::GetInstance().GetMouseX()-Camera::pos.x,InputManager::GetInstance().GetMouseY()-Camera::pos.y);
        selectionBox.SetBegin(p);
		selectionBox.Update(&tileMap);
        p = tileMap.GetTileCenter(p);
		break;
	case GUI_ROOM:
		roomSheet.UpdateObjectSheet();
		break;
	case CONSTRUCT_ROOM:
		selectionBox.Update(&tileMap);
		size = (abs(selectionBox.end.x - selectionBox.begin.x) + 1)*(abs(selectionBox.end.y - selectionBox.begin.y) + 1);
		cost = size * 5;
		break;
	case DESTROY_ROOM:
		break;
	case AREA_SELECT:
		break;
	case BUY:
		buySheet.UpdateObjectSheet();
		break;
	case EDIT_OBJECT:{
		Point tiles = Point(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y);
		tiles = tileMap.GetTileCenterFromScreen(tiles);
		objectArray[selectedObject]->MoveTo(tiles.x, tiles.y);
	}
		break;
	default:
		break;
	}
	UpdateArray(dt,&tileMap);
	
	for (int i = 0; i < objectArray.size(); i++){

		if (objectArray[i]->IsCharacter()){
			if (objectArray[i]->GetAction() == DECIDING_ROOM){
				charChoice = objectArray[i]->GetChoice();
				charRoom = charChoice;

				if (!roomArray.empty()){
					for (int j = 0; j < roomArray.size(); j++){
						if ((roomArray[j]->GetState() == charRoom) && (roomArray[j]->roomState != Room::EDITING)){
							int x, y;
							Point t;
							x = objectArray[i]->box.x + objectArray[i]->box.w / 2;
							y = objectArray[i]->box.y + objectArray[i]->box.h;
							t = tileMap.GetTile(x, y);
							vector<int> path = PathAStar(t.x, t.y, roomArray[j]->GetDoor(), heuristicsArray[roomArray[j]->GetID()]);
							if (!path.empty())
								objectArray[i]->AddObjective(path);
							else
								cout << "No path!!!!" << endl;
						}
					}
				}
			}
			else{ 
				if (objectArray[i]->GetAction() == DECIDING_OBJECT){
					int objectIndex = objectArray[i]->GetObjectIndex();
					if (objectIndex != -1){
						Point objective = Point(objectArray[objectIndex]->GetTile().x, objectArray[objectIndex]->GetTile().y - 1);
						Point t = objectArray[i]->GetTile();
						cout << "Tile : " << t.x << "," << t.y << endl;
						vector<int> path = PathAStar(t.x, t.y, objective, objectArray[objectIndex]->GetHeuristic(0));
						if (!path.empty())
							objectArray[i]->AddObjective(path);
						else
							cout << "No path!!!!" << endl;
					}
				}
			}
		}
	}
	if (creationTimer.Get() > 10){
		//CreateCharacter(5, 5);
		creationTimer.Restart();
	}
	
	if (cost > 0)	costText.SetText(to_string(-cost));
	else costText.SetText(" ");
	costText.SetPos(InputManager::GetInstance().GetMouseX() + 10, InputManager::GetInstance().GetMouseY() + 5, false, false);
}

void StageState::Render() {
	bg.Render(0, 0);
	bg.Render(bg.GetWidth(), 0);
	bg.Render(bg.GetWidth(), bg.GetHeight());
	bg.Render(0, bg.GetHeight());
	tileMap.Render(Camera::pos.GetXpoint(), Camera::pos.GetYpoint());

	moneyText.SetText(to_string(data->money));
	moneyText.SetPos(20, 20, false, false);
	moneyText.Render();

	SDL_Rect fillRect = { 20, 70, 100, 10 };
	if (data->fame <= 255){
		SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255 - data->fame, 0, data->fame, 255);
	}
	if (data->fame >= 255){
		SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 255, 255, 255);
	}
	SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &fillRect);
	
	for(int i = 0; i < roomArray.size(); i++){
        roomArray[i]->Render(&tileMap);
    }
	switch (action)	{
	case NONE:
		RenderArray();
		occupancyMap.Render(&tileMap);
		break;
	case TILE_SELECT:
		selectionBox.Render(&tileMap);
		RenderArray();
		break;
	case GUI_ROOM:
		RenderArray();
		roomSheet.RenderRoomSheet();
		break;
	case CONSTRUCT_ROOM:
		selectionBox.Render(&tileMap);
		RenderArray();
		break;
	case DESTROY_ROOM:
		RenderArray();
		break;
	case AREA_SELECT:
		RenderArray();
		break;
	case GUI_A:
		RenderArray();
		gui.Render();
		break;
	case SUB_GUI_EDIT :
		RenderArray();
		subGuiEdit.Render();
		break;
	case EDIT_DOOR:
		RenderArray();
		break;
	case BUY:
		RenderArray();
		buySheet.RenderObjectSheet();
		break;
	case EDIT_OBJECT:{
			vector<Point> tiles = objectArray[selectedObject]->GetAccessPoints();
			for (int i = 0; i < tiles.size(); i++){
				Point center = tileMap.GetTileCenter(tiles[i]);
				center.x = center.x - tileMap.GetTileWidth()/2;
				center.y = center.y - tileMap.GetTileHeight()/2;

				if (obstacleMap[(tiles[i].y)*tileMap.GetWidth() + tiles[i].x] != -1){
					okTile.Render(center.x + Camera::pos.x, center.y+Camera::pos.y);
				}
				else{
					noTile.Render(center.x + Camera::pos.x, center.y + Camera::pos.y);
				}
			}

		}

		RenderArray();
		break;
	default:
		RenderArray();
		break;
	}
	if (sheet.GetRender()){
		sheet.SetHunger(to_string(objectArray[selectedCharacter]->GetHunger()));
		sheet.SetSatisfaction(to_string(int(objectArray[selectedCharacter]->satisfaction)));
		sheet.SetMoney(to_string(objectArray[selectedCharacter]->money));
		sheet.Render();
	}
	if (objSheet.GetRender()){
		objSheet.Open(objectArray[selectedCharacter]->GetTextAttributes());
		objSheet.Render();
	}

	//Dinheiro e fama
	
		
	moneyText.SetText(to_string(data->money));
	moneyText.SetPos(20, 20, false, false);
	moneyText.Render();

	costText.Render();

	if (data->fame <= 255){
		SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255 - data->fame, 0, data->fame, 255);
	}
	if (data->fame >= 255){
		SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 255, 255, 255);
	}
	SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &fillRect);
	int anchorx = data->fame * 100 / 255;
	SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 153, 153, 0, 255);
	fillRect = { 20 + anchorx, 68, 3, 14 };
	SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &fillRect);
}

void StageState::Input() {
	if ((InputManager::GetInstance().KeyPress(SDLK_ESCAPE)) || (InputManager::GetInstance().ShouldQuit())){
		PauseState *state = new PauseState();
		Game::GetInstance().Push(state);
	}

	if (InputManager::GetInstance().KeyPress(SDLK_c)){
		Point tile(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y);
		tile = tileMap.GetTile(tile.x, tile.y);
		CreateCharacter(tile.x, tile.y);
	}

	if (InputManager::GetInstance().KeyPress(SDLK_d)){
		DestroyCharacter(objectArray.size()-1);
	}

	if (InputManager::GetInstance().KeyPress(SDLK_RETURN)){
		CheatState *state = new CheatState(this, data);
		Game::GetInstance().Push(state);
	}
	if(InputManager::GetInstance().MouseRelease(SDL_BUTTON_LEFT)){

		switch (action){
		case NONE:
			ptile = tileMap.GetTile(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y);
			p = tileMap.GetTileCenter(ptile);
			//objectArray[0]->AddObjective(p.x, p.y, ptile);
			SelectCharacter();
			for (int i = 0; i < roomArray.size(); i++){
				roomArray[i]->EditRoom(false);
				for (int j = 0; j < objectArray.size(); j++){
					if (objectArray[j]->Is("Wall")){
						objectArray[j]->Editing(false);
					}
				}
			}
			break;
		case GUI_A:
			if(gui.BuildIconPressed()){
				click.Play(1);
				action = GUI_ROOM;
				gui.SetState(ROOMS);
            }
			else{
				if (gui.DestroyIconPressed()){
					click.Play(1);
					action = DESTROY_ROOM;
				}
				else{
					if (gui.BuyIconPressed()){
						click.Play(1);
						action = BUY;
					}
					else{
						if (gui.EditIconPressed()){
							click.Play(1);
							subGuiEdit.SetPositionSubGUI(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY());
							action = SUB_GUI_EDIT;
							subGuiEdit.SetState(EDIT);
						}
						else {
							action = NONE;
						}
					}
				}
			}
			break;
		case SUB_GUI_EDIT:
			if (subGuiEdit.DoorIconPressed()){
				click.Play(1);
				cout << "editando porta..." << endl;
				action = EDIT_DOOR;
			}
			else{
				if (subGuiEdit.WallIconPressed()){
					click.Play(1);
					cout << "editando parede..." << endl;
					action = EDIT_WALL;
				}
				else {
					action = NONE;
				}
			}
			break;
		case TILE_SELECT:
			break;
		case GUI_ROOM:
			if (!roomSheet.IsInside(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY())){
				action = NONE;
			}
			if (roomSheet.IsBuy(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY())){
				action = TILE_SELECT;
				roomAttributes = roomSheet.GetAttributes();
				selectionBox.SetSprite(roomAttributes.tileSprite);
			}
			gui.SetState(BASIC);

			break;
		case CONSTRUCT_ROOM:
			{
			action = NONE;
			Point aux;

			if (selectionBox.begin.y > selectionBox.end.y) {
				aux = selectionBox.begin;
				selectionBox.begin = selectionBox.end;
				selectionBox.end = aux;
			}

			if ((selectionBox.begin.x > selectionBox.end.x) && (selectionBox.begin.y < selectionBox.end.y)) {
				aux = selectionBox.begin;
				selectionBox.begin.x = selectionBox.end.x;
				selectionBox.end.x = aux.x;
			}
			RoomAttributes asd;
			asd.cost = 50;
			asd.tileSprite = "img/tileset/tile_madeira.png";
			asd.type = "Pirate";
			Room *newRoom = new Room(selectionBox.begin, selectionBox.end, &tileMap, &objectArray, roomArray.size(), roomAttributes);
			data->money -= newRoom->cost;
            roomArray.emplace_back(newRoom);
			vector<int> heuristc = occupancyMap.CreateHeuristic(&tileMap, roomArray[roomArray.size() - 1]->GetDoor());
			heuristicsArray.emplace(newRoom->GetID(),heuristc);
			obstacleMap = occupancyMap.Update(&tileMap, &objectArray);
			//Point tileC = tileMap.GetTile(objectArray[0]->box.x + objectArray[0]->box.w / 2, objectArray[0]->box.y + objectArray[0]->box.h);
			//vector<int> path = PathAStar(tileC.x, tileC.y, newRoom->GetID());
			//objectArray[0]->AddObjective(path);
			}
			break;
		case DESTROY_ROOM:
			for (int i = 0; i < roomArray.size(); i++){
				p = tileMap.GetTile(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y);
				if (roomArray[i]->IsInside(p)){
					heuristicsArray.erase(roomArray[i]->GetID());
					cout << "Destroying..." << endl;
					DestroyRoom(roomArray[i]->GetID());
					roomArray.erase(roomArray.begin() + i);
					i = roomArray.size();
				}
				
			}
			action = NONE;
			break;
		case BUY:
			if (!buySheet.IsInside(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY())){
				action = NONE;
			}
			if (buySheet.IsBuy(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY())){
				Point Tiles = Point(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y);
				Tiles = tileMap.GetTileCenterFromScreen(Tiles);
				Object* object = new Object(Tiles.x, Tiles.y, buySheet.GetAttributes(), tileMap);
				selectedObject = objectArray.size();
				objectArray.emplace_back(object);
				action = EDIT_OBJECT;
			}
			//data->money -= 100;
			break;
		case AREA_SELECT:
			break;
		case EDIT_DOOR:
			for (int i = 0; i < roomArray.size(); i++){
				p = tileMap.GetTile(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y);
				if (roomArray[i]->IsInside(p)){
					cout << "editando quarto: " << roomArray[i]->GetID() << endl;
					roomArray[i]->EditRoom(true);
					for (int j = 0; j < objectArray.size(); j++){
						if (objectArray[j]->Is("Wall")){
							if ((p.x == objectArray[j]->tile.x) && (p.y == objectArray[j]->tile.y))
								roomArray[i]->SetDoor(p.x, p.y, roomArray[i]->GetID(), &objectArray);
						}
					}
					
					for (int j = 0; j < objectArray.size(); j++){
						if (objectArray[j]->Is("Wall")){
							if (objectArray[j]->roomID == roomArray[i]->GetID()) {
								objectArray[j]->Editing(true);
								if (objectArray[j]->tile.DisPoints(objectArray[j]->tile, roomArray[i]->GetDoor()) == 0) {
									objectArray.erase(objectArray.begin() + j); 
								}
							}
						}
					}
					vector<int> heuristc = occupancyMap.CreateHeuristic(&tileMap, roomArray[i]->GetDoor());
					heuristicsArray[roomArray[i]->GetID()] = heuristc;
					obstacleMap = occupancyMap.Update(&tileMap, &objectArray);
					obstacleMap = occupancyMap.CleanTile(&tileMap, roomArray[i]->GetDoor());
				}
			}
			action = NONE;
			break;
		case EDIT_WALL:
			for (int i = 0; i < roomArray.size(); i++){
				p = tileMap.GetTile(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y);
				if (roomArray[i]->IsInside(p)){
					cout << "editando quarto: " << roomArray[i]->GetID() << endl;
					roomArray[i]->EditRoom(true);
					for (int j = 0; j < objectArray.size(); j++){
						if (objectArray[j]->Is("Wall")){
							if (objectArray[j]->roomID == roomArray[i]->GetID()) {
								objectArray[j]->Editing(true);
							}
						}
					}
				}
			}
			action = NONE;
			break;
		case EDIT_OBJECT:
			if (objectArray[selectedObject]->SettlePos(obstacleMap)){
				obstacleMap = occupancyMap.Update(&tileMap, &objectArray);
				action = NONE;
			}
		default:
			break;
		}

	}

	if(InputManager::GetInstance().MousePress(SDL_BUTTON_RIGHT)){
		if (sheet.GetRender() || objSheet.GetRender()){
			if (!sheet.GetBox().IsInside(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY())){
				sheet.SetRender(false);
				Camera::Unfollow();
			}

			if (!objSheet.GetBox().IsInside(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY()))
				objSheet.SetRender(false);
		}
		else{
			switch (action)
			{
			case NONE:
				gui.SetPosition(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY());
				action = GUI_A;
				break;
			case TILE_SELECT:
				break;
			case CONSTRUCT_ROOM:
				break;
			case DESTROY_ROOM:
				break;
			case AREA_SELECT:
				break;
			case GUI_A:
				gui.SetPosition(InputManager::GetInstance().GetMouseX(), InputManager::GetInstance().GetMouseY());
				break;
			case SUB_GUI_EDIT:
				action = NONE;
				break;
			default:
				break;
			}
		}
	}

	if(InputManager::GetInstance().IsMouseDown(SDL_BUTTON_LEFT)){

		switch (action){
		case NONE:
			break;
		case TILE_SELECT:
			action = CONSTRUCT_ROOM;
			break;
		case CONSTRUCT_ROOM:
			break;
		case DESTROY_ROOM:
			break;
		case AREA_SELECT:
			break;
		default:
			break;
		}

	}

	if(InputManager::GetInstance().MouseRelease(SDL_BUTTON_RIGHT)){

		switch (action){
		case NONE:
			//action = TILE_SELECT;
			break;
		case TILE_SELECT:
			break;
		case CONSTRUCT_ROOM:
			break;
		case DESTROY_ROOM:
			break;
		case AREA_SELECT:
			break;
		default:
			break;
		}

	}
	
}


void StageState::DestroyRoom(int roomID){
    cout << roomID << endl;
	Point pos;
    for(int i = 0; i<objectArray.size(); i++){
        if(objectArray[i]->roomID  == roomID){
			if (objectArray[i]->Type() == "LEFT_CORNER" || objectArray[i]->Type() == "RIGHT_CORNER"){
				if (objectArray[i]->Type() == "LEFT_CORNER"){
					pos.x = objectArray[i]->box.x;
					pos.y = objectArray[i]->box.y + objectArray[i]->box.h;

						pos = tileMap.GetTile(pos.x, pos.y);
					}
					else{
						pos.x = objectArray[i]->box.x + objectArray[i]->box.w;
						pos.y = objectArray[i]->box.y + objectArray[i]->box.h;

						pos = tileMap.GetTile(pos.x, pos.y);

					}
				}
			else{
				pos.x = objectArray[i]->box.x + objectArray[i]->box.w / 2;
				pos.y = objectArray[i]->box.y + objectArray[i]->box.h;
				pos = tileMap.GetTile(pos.x, pos.y);
			}
			occupancyMap.CleanTile(&tileMap, pos);
            objectArray.erase(objectArray.begin() + i);
            i--;
        }
    }
}

vector<int> StageState::PathAStar(int posX, int posY, Point door, vector<int> heuristic){
	int index, cost, actualPos, exploredSize = 0, movements = 0;
	Point posCost;
	bool arrived = false;
	priority_queue<Point, vector<Point>, CostComparator> costQueue;
	unordered_map<int, int> exploredTiles;
	unordered_map<int, int> allPaths;
	vector<int> path;

	index = posY*tileMap.GetWidth() + posX;
	if (door.x == posX && door.y == posY){
		path.emplace_back(index);
		return path;
	}
	actualPos = index;
	cost = heuristic[index];
	posCost.SetPoint(index, movements, cost);
	exploredTiles.emplace(index, -1);
	allPaths.emplace(actualPos, -1);
	costQueue.emplace(posCost);

	if (posX == door.x && posY == door.y) {
		arrived = true;
		cout << "Achou : " << posX << "," << posY << endl;
		cout << "Movements : " << movements << endl;
	}

	while (!arrived){
		costQueue.pop();
		movements++;
		//Explore sides
		//Up Right
		posY--;
		index = posY*tileMap.GetWidth() + posX;
		if (allPaths.find(index) == allPaths.end() && posX >= 0 && posY >= 0 && obstacleMap[index] != -1 && posY < 25 && posX < 25){
			cost = heuristic[index] + movements;
			posCost.SetPoint(index, movements, cost);
			costQueue.emplace(posCost);
			allPaths.emplace(index, actualPos);
		}
		//Down Right
		posY++;
		posX++;
		index = posY*tileMap.GetWidth() + posX;
		if (allPaths.find(index) == allPaths.end() && posX >= 0 && posY >= 0 && obstacleMap[index] != -1 && posY < 25 && posX < 25){
			cost = heuristic[index] + movements;
			posCost.SetPoint(index, movements, cost);
			costQueue.emplace(posCost);
			allPaths.emplace(index, actualPos);
		}
		//Down Left
		posY++;
		posX--;
		index = posY*tileMap.GetWidth() + posX;
		if (allPaths.find(index) == allPaths.end() && posX >= 0 && posY >= 0 && obstacleMap[index] != -1 && posY < 25 && posX < 25){
			cost = heuristic[index] + movements;
			posCost.SetPoint(index, movements, cost);
			costQueue.emplace(posCost);
			allPaths.emplace(index, actualPos);
		}
		//Down Right
		posY--;
		posX--;
		index = posY*tileMap.GetWidth() + posX;
		if (allPaths.find(index) == allPaths.end() && posX >= 0 && posY >= 0 && obstacleMap[index] != -1 && posY < 25 && posX < 25){
			cost = heuristic[index] + movements;
			posCost.SetPoint(index, movements, cost);
			costQueue.emplace(posCost);
			allPaths.emplace(index, actualPos);
		}

		//Get next tile
		if (costQueue.empty()){
			cout << "Vazio!!!!" << endl;
		}
		posCost = costQueue.top();
		index = posCost.x;
		posY = (int)(index / tileMap.GetWidth());
		posX = index % tileMap.GetWidth();

		movements = posCost.y;
		if (posX == door.x && posY == door.y) {
			arrived = true;
			cout << "Achou : " << posX << "," << posY << endl;
			cout << "Movements : "<< movements << endl;
		}
		
		cost = posCost.z;
		exploredTiles.emplace(index, actualPos);

		if (exploredSize == exploredTiles.size()){
			break;
		}
		exploredSize = exploredTiles.size();

		actualPos = index;
		cout << posX << "," << posY << endl;
		
	}
	if (arrived){
		path.emplace_back(index);
		while (allPaths[index] != -1){
			path.emplace_back(allPaths[index]);
			index = allPaths[index];
		}
	}
	else{
		cout << "No path" << endl;
	}

	for (int i = path.size()-1; i >= 0; i--){
		//cout << "Path : " << (int)(path[i] / tileMap.GetWidth()) << "," << path[i] % tileMap.GetWidth() << endl;
	}
	return path;
}

void StageState::CreateCharacter(int x, int y){
	Point tile(x, y);
	tile = tileMap.GetTileCenter(tile);
	Permonkey* pM = new Permonkey(tile.x, tile.y, tile, tileMap, objList);
	objectArray.emplace_back(pM);
}

void StageState::DestroyCharacter(int id){
	objectArray.erase(objectArray.begin() + id);
}

void StageState::SelectCharacter(){
	for (int i = 0; i < objectArray.size(); i++){
		if (objectArray[i]->IsCharacter()){
			if (objectArray[i]->box.IsInside(InputManager::GetInstance().GetMouseX()-Camera::pos.x, InputManager::GetInstance().GetMouseY()-Camera::pos.y)){
				Camera::Follow(objectArray[i].get());
				selectedCharacter = i;
				sheet.SetRace(to_string(i));
				sheet.SetHunger(to_string(objectArray[i]->GetHunger()));
				sheet.SetSatisfaction(to_string(int(objectArray[i]->satisfaction)));
				sheet.SetMoney(to_string(objectArray[i]->money));
				sheet.SetRender(true);
				objSheet.SetRender(false);
				cout << "Character is : " << objectArray[i]->hunger << endl;
				break;
			}
		}
		if (objectArray[i]->Is("Object")){
			if (objectArray[i]->GetBox().IsInside(InputManager::GetInstance().GetMouseX() - Camera::pos.x, InputManager::GetInstance().GetMouseY() - Camera::pos.y)){
				objSheet.SetRender(true);
				sheet.SetRender(false);
				selectedCharacter = i;
				
			}
		}
	}
}

void StageState::ParseObject(vector<string> objList){
	Attributes attributes;
	ifstream objectFile;
	string attribute;
	size_t size;
	int counter;
	unordered_map<string, Attributes> list;
	unordered_map<string, unordered_map<string, Attributes>> typeList;
	for (int i = 0; i < objList.size(); i++){
		objectFile.open(objList[i]);
		cout << "New object : " << objList[i] << "." << endl;
		counter = 0;
		while (!objectFile.eof()){
			getline(objectFile, attribute);
			size = attribute.find(":");
			if (size != string::npos){
				counter++;
				attribute = attribute.substr(size + 1);
				switch (counter){
				case 1:
					attributes.name = attribute;
					break;
				case 2:
					attributes.cost = stoi(attribute);
					break;
				case 3:
					attributes.type = attribute;
					break;
				case 4:
					attributes.activeHunger = stoi(attribute);
					break;
				case 5:
					attributes.activeSatisfaction = stof(attribute);
					break;
				case 6:
					attributes.activeMoney = stoi(attribute);
					break;
				case 7:
					attributes.radius = stoi(attribute);
					break;
				case 8:
					attributes.passiveHunger = stoi(attribute);
					break;
				case 9:
					attributes.passiveSatisfaction = stof(attribute);
					break;
				case 10:
					attributes.passiveMoney = stoi(attribute);
					break;
				case 11:
					attributes.sprite = attribute;
					break;
				case 12:
					attributes.description = attribute;
					break;
				case 13:{
					string a;
					a = attribute[0];
					attributes.access[0] = stoi(a);
					a = attribute[1];
					attributes.access[1] = stoi(a);
					a = attribute[2];
					attributes.access[2] = stoi(a);
					a = attribute[3];
					attributes.access[3] = stoi(a);
				}
					break;
				default:
					break;
				}
			}
		}
		objectFile.close();
		for (int j = 1; j <= counter; j++){
			switch (j){
			case 1:
				cout << "name: " << attributes.name << endl;
				break;
			case 2:
				cout << "cost: " << attributes.cost << endl;
				break;
			case 3:
				cout << "type: " << attributes.type << endl;
				break;
			case 4:
				cout << "AHunger: " << attributes.activeHunger << endl;
				break;
			case 5:
				cout << "ASatisfaction: " << attributes.activeSatisfaction << endl;
				break;
			case 6:
				cout << "AMoney: " << attributes.activeMoney << endl;
				break;
			case 7:
				cout << "Radius: " << attributes.radius << endl;
				break;
			case 8:
				cout << "pHunger: " << attributes.passiveHunger << endl;
				break;
			case 9:
				cout << "pSatisfaction: " << attributes.passiveSatisfaction << endl;
				break;
			case 10:
				cout << "pMoney: " << attributes.passiveMoney << endl;
				break;
			case 11:
				cout << "Sprite: " << attributes.sprite << endl;
				break;
			case 12:
				cout << "Description: " << attributes.description << endl;
				break;
			case 13:
				cout << "Access: " << attributes.access[0] << attributes.access[1] << attributes.access[2] << attributes.access[3] << endl;
			default:
				break;
			}
		}
		cout << endl;
		list.emplace(attributes.name, attributes);
		typeList[attributes.type].emplace(attributes.name, attributes);
		this->objList[attributes.type].emplace_back(attributes.name);
		
	}
	cout << endl;
	buySheet.SetTypeList(typeList);
}

void StageState::ParseRoom(vector<string> roomList){
	RoomAttributes attributes;
	ifstream roomFile;
	string attribute;
	size_t size;
	int counter;
	unordered_map<string, RoomAttributes> list;
	unordered_map<string, unordered_map<string, RoomAttributes>> typeList;
	for (int i = 0; i < objList.size(); i++){
		roomFile.open(roomList[i]);
		cout << "New room : " << roomList[i] << "." << endl;
		counter = 0;
		while (!roomFile.eof()){
			getline(roomFile, attribute);
			size = attribute.find(":");
			if (size != string::npos){
				counter++;
				attribute = attribute.substr(size + 1);
				switch (counter){
				case 1:
					attributes.type = attribute;
					break;
				case 2:
					attributes.cost = stoi(attribute);
					break;
				case 3:
					attributes.tileSprite = attribute;
					break;
				default:
					break;
				}
			}
		}
		
		roomFile.close();
		for (int j = 1; j <= counter; j++){
			switch (j){
			case 1:
				cout << "name: " << attributes.type << endl;
				break;
			case 2:
				cout << "cost: " << attributes.cost << endl;
				break;
			case 3:
				cout << "type: " << attributes.tileSprite << endl;
				break;
			}
		}
		cout << endl;
		list.emplace(attributes.type, attributes);
		typeList[attributes.type].emplace(attributes.type, attributes);
		this->roomList[attributes.type].emplace_back(attributes.type);

	}
	cout << endl;
	roomSheet.SetRoomList(list);

}
void StageState::Load(){
	objectArray.clear();
	//unico jeito at� o momento em que o 'save' n�o implica em crash.

	ifstream loadFile("save.bin", ios::in | ios::binary);

	int size = 0;
	loadFile.read(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		Room *room = new Room(loadFile,&tileMap,&objectArray,roomArray.size());
		roomArray.emplace_back(room);

	}
	cout << "Saiu p" << endl;
	//---------------------------------------------------------
	loadFile.read(reinterpret_cast<char*> (&size), sizeof(int));
	obstacleMap.resize(size);
	for (int p = 0; p < size; p++){
		loadFile.read(reinterpret_cast<char*> (&obstacleMap[p]), sizeof(int));
	}
	//---------------------------------------------------------
	loadFile.read(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		char tipo[15];
		loadFile.read(reinterpret_cast<char*> (&tipo), sizeof(tipo));
		string type(tipo);
		cout << type << endl;
		if (type == "PerMonkey"){
			Permonkey *pm = new Permonkey(loadFile, tileMap, objList);
			objectArray.emplace_back(pm);
		}
		if (type == "Object"){
			Object *ob = new Object(loadFile, tileMap);
			objectArray.emplace_back(ob);
		}
	}
	loadFile.read(reinterpret_cast<char*> (&data->money), sizeof(int));
	loadFile.read(reinterpret_cast<char*> (&data->fame), sizeof(int));
	cout << "end" << endl;
	loadFile.close();
};
void StageState::Save(){
	ofstream saveFile("save.bin", ios::out | ios::binary);

	int size = roomArray.size();
	saveFile.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < roomArray.size(); p++){
		roomArray[p]->Save(saveFile);
	}

	size = obstacleMap.size();
	saveFile.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		saveFile.write(reinterpret_cast<char*> (&obstacleMap[p]), sizeof(int));
	}
	size = objectArray.size();
	saveFile.write(reinterpret_cast<char*> (&size), sizeof(int));
	for (int p = 0; p < size; p++){
		//if (objectArray[p]->Type() == "PerMonkey"){
			char tipo[15];
			strcpy(tipo, objectArray[p]->Type().c_str());
			saveFile.write(reinterpret_cast<char*> (&tipo), sizeof(tipo));
			objectArray[p]->Save(saveFile);
		//}
	}
	saveFile.write(reinterpret_cast<char*> (&data->money), sizeof(int));
	saveFile.write(reinterpret_cast<char*> (&data->fame), sizeof(int));
	saveFile.close();
};
StageState *StageState::instance = nullptr;
vector<string> StageState::musicNames = { "music/can_o do vinho.ogg", "music/feliz1.2.ogg" };
