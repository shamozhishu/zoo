#include <zooCmd/InputAdapter.h>

namespace zooCmd {

std::map<int, int> InputAdapter::_keyboardMap;
void InputAdapter::remapKeyboard(int zoocmdKey, int remapKey)
{
	_keyboardMap[remapKey] = zoocmdKey;
}

void InputAdapter::clearKeyboardMap()
{
	_keyboardMap.clear();
}

}
