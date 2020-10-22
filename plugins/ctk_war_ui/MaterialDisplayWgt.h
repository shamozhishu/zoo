#ifndef __MATERIAL_DISPLAY_WGT_H__
#define __MATERIAL_DISPLAY_WGT_H__

#include <QtWidgets/QWidget>

struct Material;
namespace zoo
{
	class Entity;
}

class MaterialDisplayWgt : public QWidget
{
public:
	MaterialDisplayWgt(QWidget* parent = Q_NULLPTR);
	~MaterialDisplayWgt();
	Material* getDisplayedMat() const;
	void refreshMaterialSphere(bool turnOnLight);
	void generateMaterialSphere(std::string materialFile);

private:
	bool _turnOnLight;
	Material* _displayedMat;
	zoo::Entity* _materialSphere;
};

#endif // __MATERIAL_DISPLAY_WGT_H__
