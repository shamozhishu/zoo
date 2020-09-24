#ifndef __WINDOW_CAPTURE_CALLBACK_H__
#define __WINDOW_CAPTURE_CALLBACK_H__

#include <zooCmd_osg/OsgEarthSupport.h>

namespace zooCmd_osg {

class CaptureOperation : public osg::Referenced
{
public:
	virtual void operator()(const osg::Image& image, const unsigned int context_id) = 0;
};

class WriteToFile : public CaptureOperation
{
public:
	enum SavePolicy
	{
		OVERWRITE,
		SEQUENTIAL_NUMBER
		// ... any others?
	};

	WriteToFile(const std::string& filename, const std::string& extension, SavePolicy savePolicy = SEQUENTIAL_NUMBER);

	virtual void operator()(const osg::Image& image, const unsigned int context_id);

	void setSavePolicy(SavePolicy savePolicy) { _savePolicy = savePolicy; }
	SavePolicy getSavePolicy() const { return _savePolicy; }

protected:

	WriteToFile& operator = (const WriteToFile&) { return *this; }

	const std::string _filename;
	const std::string _extension;

	SavePolicy _savePolicy;

	std::vector<unsigned int> _contextSaveCounter;
};

class WindowCaptureCallback : public osg::Camera::DrawCallback
{
public:
	enum Mode
	{
		READ_PIXELS,
		SINGLE_PBO,
		DOUBLE_PBO,
		TRIPLE_PBO
	};

	enum FramePosition
	{
		START_FRAME,
		END_FRAME
	};

	WindowCaptureCallback(int numFrames, Mode mode, FramePosition position, GLenum readBuffer);

	FramePosition getFramePosition() const { return _position; }

	void setCaptureOperation(CaptureOperation* operation);
	CaptureOperation* getCaptureOperation() { return _contextDataMap.begin()->second->_captureOperation.get(); }

	void setFramesToCapture(int numFrames) { _numFrames = numFrames; }
	int getFramesToCapture() const { return _numFrames; }

	virtual void operator()(osg::RenderInfo& renderInfo) const;

	struct ContextData : public osg::Referenced
	{
		ContextData(osg::GraphicsContext* gc, Mode mode, GLenum readBuffer);

		void getSize(osg::GraphicsContext* gc, int& width, int& height);

		void updateTimings(osg::Timer_t tick_start,
			osg::Timer_t tick_afterReadPixels,
			osg::Timer_t tick_afterMemCpy,
			osg::Timer_t tick_afterCaptureOperation,
			unsigned int dataSize);

		void read();
		void readPixels();
		void singlePBO(osg::GLExtensions* ext);
		void multiPBO(osg::GLExtensions* ext);

		typedef std::vector< osg::ref_ptr<osg::Image> >             ImageBuffer;
		typedef std::vector< GLuint > PBOBuffer;

		osg::GraphicsContext*   _gc;
		unsigned int            _index;
		Mode                    _mode;
		GLenum                  _readBuffer;

		GLenum                  _pixelFormat;
		GLenum                  _type;
		int                     _width;
		int                     _height;

		unsigned int            _currentImageIndex;
		ImageBuffer             _imageBuffer;

		unsigned int            _currentPboIndex;
		PBOBuffer               _pboBuffer;

		unsigned int            _reportTimingFrequency;
		unsigned int            _numTimeValuesRecorded;
		double                  _timeForReadPixels;
		double                  _timeForMemCpy;
		double                  _timeForCaptureOperation;
		double                  _timeForFullCopy;
		double                  _timeForFullCopyAndOperation;
		osg::Timer_t            _previousFrameTick;

		osg::ref_ptr<CaptureOperation> _captureOperation;
	};

	typedef std::map<osg::GraphicsContext*, osg::ref_ptr<ContextData> > ContextDataMap;

	ContextData* createContextData(osg::GraphicsContext* gc) const;
	ContextData* getContextData(osg::GraphicsContext* gc) const;

	Mode                        _mode;
	FramePosition               _position;
	GLenum                      _readBuffer;
	mutable OpenThreads::Mutex  _mutex;
	mutable ContextDataMap      _contextDataMap;
	mutable int                 _numFrames;

	osg::ref_ptr<CaptureOperation> _defaultCaptureOperation;
};

}

#endif // __WINDOW_CAPTURE_CALLBACK_H__
