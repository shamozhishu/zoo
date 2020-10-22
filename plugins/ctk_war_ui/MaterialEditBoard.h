#ifndef __MATERIAL_EDIT_BOARD_H__
#define __MATERIAL_EDIT_BOARD_H__

#include <QWidget>

namespace Ui {
	class MaterialEditBoard;
}

class MaterialWgt;
class MaterialDisplayWgt;
class MaterialEditBoard : public QWidget
{
public:
	MaterialEditBoard();
	~MaterialEditBoard();

private:
	MaterialWgt* _matWgt;
	Ui::MaterialEditBoard* _ui;
	struct Material* _curSelMat;
	MaterialDisplayWgt* _matDisplayWgt;
};

#endif // __MATERIAL_EDIT_BOARD_H__
