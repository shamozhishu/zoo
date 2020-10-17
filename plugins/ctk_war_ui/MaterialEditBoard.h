#ifndef __MATERIAL_EDIT_BOARD_H__
#define __MATERIAL_EDIT_BOARD_H__

#include <QWidget>
#include <component/war/WarComponents.h>

namespace Ui {
	class MaterialEditBoard;
}

class MaterialDisplayWgt;
class MaterialEditBoard : public QWidget
{
public:
	MaterialEditBoard();
	~MaterialEditBoard();

private:
	Material* _curSelMat;
	Ui::MaterialEditBoard* _ui;
	MaterialDisplayWgt* _matDisplayWgt;
};

#endif // __MATERIAL_EDIT_BOARD_H__
