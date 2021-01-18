/* -*-c++-*- OpenSceneGraph - Copyright (C) 2009 Wang Rui
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/
#ifndef OSGVIEWER_GRAPHICSWINDOWQT
#define OSGVIEWER_GRAPHICSWINDOWQT

#include <osgQt/Export.h>
#include <osgQt/Version.h>
#include <osgViewer/GraphicsWindow>

class QWidget;
class QGLWidget;

namespace osgQt {

class OSGQT_EXPORT GraphicsWindowQt : public osgViewer::GraphicsWindow
{
public:
	GraphicsWindowQt(osg::GraphicsContext::Traits* traits, QWidget* parent = NULL, const QGLWidget* shareWidget = NULL);
	virtual ~GraphicsWindowQt();

	void* getWndHandle() const;
	QGLWidget* getGLWidget() { return _widget; }
	const QGLWidget* getGLWidget() const { return _widget; }

	struct WindowData : public osg::Referenced
	{
		WindowData(QGLWidget* widget = NULL, QWidget* parent = NULL) : _widget(widget), _parent(parent) {}
		QGLWidget* _widget; QWidget* _parent;
	};

	bool init(QWidget* parent, const QGLWidget* shareWidget);
	static osg::GraphicsContext::Traits* createTraits(const QGLWidget* widget);

	virtual bool setWindowRectangleImplementation(int x, int y, int width, int height);
	virtual void getWindowRectangle(int& x, int& y, int& width, int& height);
	virtual bool setWindowDecorationImplementation(bool windowDecoration);
	virtual bool getWindowDecoration() const;
	virtual void grabFocus();
	virtual void grabFocusIfPointerInWindow();
	virtual void raiseWindow();
	virtual void setWindowName(const std::string& name);
	virtual std::string getWindowName();
	virtual void useCursor(bool cursorOn);
	virtual void setCursor(MouseCursor cursor);
	virtual bool getTouchEventsEnabled() const;
	virtual void setTouchEventsEnabled(bool e);

	virtual bool valid() const;
	virtual bool realizeImplementation();
	virtual bool isRealizedImplementation() const;
	virtual void closeImplementation();
	virtual bool makeCurrentImplementation();
	virtual bool releaseContextImplementation();
	virtual void swapBuffersImplementation();
	virtual void runOperations();

	virtual void requestWarpPointer(float x, float y);

protected:
	bool _realized;
	bool _ownsWidget;
	QGLWidget* _widget;
	friend class GLWidget;
};

}

#endif
