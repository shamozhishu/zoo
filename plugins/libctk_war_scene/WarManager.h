#pragma once

#include <map>
#include <string>
#include <QTimer>
#include <zoo/Singleton.h>

using namespace std;

class Scene;
class WarManager : public QObject, public zoo::Singleton<WarManager>
{
	Q_OBJECT
public:
	WarManager(string resPath, string mainTable);
	~WarManager();
	const string& getResPath() const;
	bool enterScene(int sceneId);
	void exitScene(int sceneId);

protected slots:
	void tick();

private:
	bool _startup;
	string _resPath;
	Scene* _curScene;
	QTimer _tickTimer;
	map<int, Scene*> _scenes;
};
