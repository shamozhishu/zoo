#pragma once

#include <map>
#include <string>
#include <QTimer>
#include <zoo/Support.h>
#include <zoo/Singleton.h>
#include <QtXml/QDomElement>
#include "PublicEnum.h"

using namespace std;

class DoF;
class Scene;
class WarManager : public QObject, public zoo::Singleton<WarManager>
{
	Q_OBJECT
public:
	WarManager(string resPath, string mainTable);
	~WarManager();
	const string& getResPath() const;
	bool enterScene(int sceneId);
	void exitCurScene();
	void saveCurScene();

protected:
	void loadDoF(QDomElement& elem, DoF* parent);
	void saveDoF(QDomElement& elem, DoF* parent, QDomDocument& doc);
	void writeFile(ENTITY_TYPE entType, stringstream& ss, zoo::TableCSV* pTable);

protected slots:
	void tick();

private:
	string _resPath;
	string _mainTable;
	Scene* _curScene;
	QTimer _tickTimer;
	map<int, Scene*> _scenes;
	string _entNames[ENTITY_COUNT];
};
