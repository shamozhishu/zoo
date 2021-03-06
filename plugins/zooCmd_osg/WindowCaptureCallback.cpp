﻿#include "WindowCaptureCallback.h"

namespace zooCmd_osg {

WriteToFile::WriteToFile(const std::string& filename,
	const std::string& extension,
	SavePolicy savePolicy)
	: _filename(filename), _extension(extension), _savePolicy(savePolicy)
{
}

void WriteToFile::operator()(const osg::Image& image, const unsigned int context_id)
{
	if (_savePolicy == SEQUENTIAL_NUMBER)
	{
		if (_contextSaveCounter.size() <= context_id)
		{
			unsigned int oldSize = _contextSaveCounter.size();
			_contextSaveCounter.resize(context_id + 1);
			// Initialize all new values to 0 since context ids may not be consecutive.
			for (unsigned int i = oldSize; i <= context_id; i++)
				_contextSaveCounter[i] = 0;
		}
	}

	std::stringstream filename;
	filename << _filename << "_" << context_id;

	if (_savePolicy == SEQUENTIAL_NUMBER)
		filename << "_" << _contextSaveCounter[context_id];

	filename << "." << _extension;

	osgDB::writeImageFile(image, filename.str());

	OSG_INFO << "ScreenCaptureHandler: Taking a screenshot, saved as '" << filename.str() << "'" << std::endl;

	if (_savePolicy == SEQUENTIAL_NUMBER)
	{
		_contextSaveCounter[context_id]++;
	}
}

WindowCaptureCallback::ContextData::ContextData(osg::GraphicsContext* gc, Mode mode, GLenum readBuffer)
	: _gc(gc),
	_index(_gc->getState()->getContextID()),
	_mode(mode),
	_readBuffer(readBuffer),
	_pixelFormat(GL_RGBA),
	_type(GL_UNSIGNED_BYTE),
	_width(0),
	_height(0),
	_currentImageIndex(0),
	_currentPboIndex(0),
	_reportTimingFrequency(100),
	_numTimeValuesRecorded(0),
	_timeForReadPixels(0.0),
	_timeForMemCpy(0.0),
	_timeForCaptureOperation(0.0),
	_timeForFullCopy(0.0),
	_timeForFullCopyAndOperation(0.0),
	_previousFrameTick(0)
{
	_previousFrameTick = osg::Timer::instance()->tick();

	osg::NotifySeverity level = osg::INFO;

	if (gc->getTraits())
	{
		if (gc->getTraits()->alpha)
		{
			OSG_NOTIFY(level) << "ScreenCaptureHandler: Selected GL_RGBA read back format" << std::endl;
			_pixelFormat = GL_RGBA;
		}
		else
		{
			OSG_NOTIFY(level) << "ScreenCaptureHandler: Selected GL_RGB read back format" << std::endl;
			_pixelFormat = GL_RGB;
		}
	}

	getSize(gc, _width, _height);

	//OSG_NOTICE<<"Window size "<<_width<<", "<<_height<<std::endl;

	// single buffered image
	_imageBuffer.push_back(new osg::Image);

	// double buffer PBO.
	switch (_mode)
	{
	case(READ_PIXELS):
		OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, without PixelBufferObject." << std::endl;
		break;
	case(SINGLE_PBO):
		OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, with a single PixelBufferObject." << std::endl;
		_pboBuffer.push_back(0);
		break;
	case(DOUBLE_PBO):
		OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, with a double buffer PixelBufferObject." << std::endl;
		_pboBuffer.push_back(0);
		_pboBuffer.push_back(0);
		break;
	case(TRIPLE_PBO):
		OSG_NOTIFY(level) << "ScreenCaptureHandler: Reading window using glReadPixels, with a triple buffer PixelBufferObject." << std::endl;
		_pboBuffer.push_back(0);
		_pboBuffer.push_back(0);
		_pboBuffer.push_back(0);
		break;
	default:
		break;
	}
}

void WindowCaptureCallback::ContextData::getSize(osg::GraphicsContext* gc, int& width, int& height)
{
	if (gc->getTraits())
	{
		width = gc->getTraits()->width;
		height = gc->getTraits()->height;
	}
}

void WindowCaptureCallback::ContextData::updateTimings(osg::Timer_t tick_start,
	osg::Timer_t tick_afterReadPixels,
	osg::Timer_t tick_afterMemCpy,
	osg::Timer_t tick_afterCaptureOperation,
	unsigned int /*dataSize*/)
{
	_timeForReadPixels = osg::Timer::instance()->delta_s(tick_start, tick_afterReadPixels);
	_timeForMemCpy = osg::Timer::instance()->delta_s(tick_afterReadPixels, tick_afterMemCpy);
	_timeForCaptureOperation = osg::Timer::instance()->delta_s(tick_afterMemCpy, tick_afterCaptureOperation);

	_timeForFullCopy = osg::Timer::instance()->delta_s(tick_start, tick_afterMemCpy);
	_timeForFullCopyAndOperation = osg::Timer::instance()->delta_s(tick_start, tick_afterCaptureOperation);
}

void WindowCaptureCallback::ContextData::read()
{
	osg::GLExtensions* ext = osg::GLExtensions::Get(_gc->getState()->getContextID(), true);

	if (ext->isPBOSupported && !_pboBuffer.empty())
	{
		if (_pboBuffer.size() == 1)
		{
			singlePBO(ext);
		}
		else
		{
			multiPBO(ext);
		}
	}
	else
	{
		readPixels();
	}
}

void WindowCaptureCallback::ContextData::readPixels()
{
	unsigned int nextImageIndex = (_currentImageIndex + 1) % _imageBuffer.size();
	unsigned int nextPboIndex = _pboBuffer.empty() ? 0 : (_currentPboIndex + 1) % _pboBuffer.size();

	int width = 0, height = 0;
	getSize(_gc, width, height);
	if (width != _width || _height != height)
	{
		//OSG_NOTICE<<"   Window resized "<<width<<", "<<height<<std::endl;
		_width = width;
		_height = height;
	}

	osg::Image* image = _imageBuffer[_currentImageIndex].get();

	osg::Timer_t tick_start = osg::Timer::instance()->tick();

#if 1
	image->readPixels(0, 0, _width, _height,
		_pixelFormat, _type);
#endif

	osg::Timer_t tick_afterReadPixels = osg::Timer::instance()->tick();

	if (_captureOperation.valid())
	{
		(*_captureOperation)(*image, _index);
	}

	osg::Timer_t tick_afterCaptureOperation = osg::Timer::instance()->tick();
	updateTimings(tick_start, tick_afterReadPixels, tick_afterReadPixels, tick_afterCaptureOperation, image->getTotalSizeInBytes());

	_currentImageIndex = nextImageIndex;
	_currentPboIndex = nextPboIndex;
}

void WindowCaptureCallback::ContextData::singlePBO(osg::GLExtensions* ext)
{
	unsigned int nextImageIndex = (_currentImageIndex + 1) % _imageBuffer.size();

	int width = 0, height = 0;
	getSize(_gc, width, height);
	if (width != _width || _height != height)
	{
		//OSG_NOTICE<<"   Window resized "<<width<<", "<<height<<std::endl;
		_width = width;
		_height = height;
	}

	GLuint& pbo = _pboBuffer[0];

	osg::Image* image = _imageBuffer[_currentImageIndex].get();
	if (image->s() != _width ||
		image->t() != _height)
	{
		//OSG_NOTICE<<"ScreenCaptureHandler: Allocating image "<<std::endl;
		image->allocateImage(_width, _height, 1, _pixelFormat, _type);

		if (pbo != 0)
		{
			//OSG_NOTICE<<"ScreenCaptureHandler: deleting pbo "<<pbo<<std::endl;
			ext->glDeleteBuffers(1, &pbo);
			pbo = 0;
		}
	}


	if (pbo == 0)
	{
		ext->glGenBuffers(1, &pbo);
		ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo);
		ext->glBufferData(GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), 0, GL_STREAM_READ);

		//OSG_NOTICE<<"ScreenCaptureHandler: Generating pbo "<<pbo<<std::endl;
	}
	else
	{
		ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo);
	}

	osg::Timer_t tick_start = osg::Timer::instance()->tick();

#if 1
	glReadPixels(0, 0, _width, _height, _pixelFormat, _type, 0);
#endif

	osg::Timer_t tick_afterReadPixels = osg::Timer::instance()->tick();

	GLubyte* src = (GLubyte*)ext->glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB,
		GL_READ_ONLY_ARB);
	if (src)
	{
		memcpy(image->data(), src, image->getTotalSizeInBytes());
		ext->glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
	}

	ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);

	osg::Timer_t tick_afterMemCpy = osg::Timer::instance()->tick();

	if (_captureOperation.valid())
	{
		(*_captureOperation)(*image, _index);
	}

	osg::Timer_t tick_afterCaptureOperation = osg::Timer::instance()->tick();
	updateTimings(tick_start, tick_afterReadPixels, tick_afterMemCpy, tick_afterCaptureOperation, image->getTotalSizeInBytes());

	_currentImageIndex = nextImageIndex;
}

void WindowCaptureCallback::ContextData::multiPBO(osg::GLExtensions* ext)
{
	unsigned int nextImageIndex = (_currentImageIndex + 1) % _imageBuffer.size();
	unsigned int nextPboIndex = (_currentPboIndex + 1) % _pboBuffer.size();

	int width = 0, height = 0;
	getSize(_gc, width, height);
	if (width != _width || _height != height)
	{
		//OSG_NOTICE<<"   Window resized "<<width<<", "<<height<<std::endl;
		_width = width;
		_height = height;
	}

	GLuint& copy_pbo = _pboBuffer[_currentPboIndex];
	GLuint& read_pbo = _pboBuffer[nextPboIndex];

	osg::Image* image = _imageBuffer[_currentImageIndex].get();
	if (image->s() != _width ||
		image->t() != _height)
	{
		//OSG_NOTICE<<"ScreenCaptureHandler: Allocating image "<<std::endl;
		image->allocateImage(_width, _height, 1, _pixelFormat, _type);

		if (read_pbo != 0)
		{
			//OSG_NOTICE<<"ScreenCaptureHandler: deleting pbo "<<read_pbo<<std::endl;
			ext->glDeleteBuffers(1, &read_pbo);
			read_pbo = 0;
		}

		if (copy_pbo != 0)
		{
			//OSG_NOTICE<<"ScreenCaptureHandler: deleting pbo "<<copy_pbo<<std::endl;
			ext->glDeleteBuffers(1, &copy_pbo);
			copy_pbo = 0;
		}
	}


	bool doCopy = copy_pbo != 0;
	if (copy_pbo == 0)
	{
		ext->glGenBuffers(1, &copy_pbo);
		ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, copy_pbo);
		ext->glBufferData(GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), 0, GL_STREAM_READ);

		//OSG_NOTICE<<"ScreenCaptureHandler: Generating pbo "<<read_pbo<<std::endl;
	}

	if (read_pbo == 0)
	{
		ext->glGenBuffers(1, &read_pbo);
		ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, read_pbo);
		ext->glBufferData(GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), 0, GL_STREAM_READ);

		//OSG_NOTICE<<"ScreenCaptureHandler: Generating pbo "<<read_pbo<<std::endl;
	}
	else
	{
		ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, read_pbo);
	}

	osg::Timer_t tick_start = osg::Timer::instance()->tick();

#if 1
	glReadPixels(0, 0, _width, _height, _pixelFormat, _type, 0);
#endif

	osg::Timer_t tick_afterReadPixels = osg::Timer::instance()->tick();

	if (doCopy)
	{

		ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, copy_pbo);

		GLubyte* src = (GLubyte*)ext->glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB,
			GL_READ_ONLY_ARB);
		if (src)
		{
			memcpy(image->data(), src, image->getTotalSizeInBytes());
			ext->glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
		}

		if (_captureOperation.valid())
		{
			(*_captureOperation)(*image, _index);
		}
	}

	ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);

	osg::Timer_t tick_afterMemCpy = osg::Timer::instance()->tick();

	updateTimings(tick_start, tick_afterReadPixels, tick_afterMemCpy, tick_afterMemCpy, image->getTotalSizeInBytes());

	_currentImageIndex = nextImageIndex;
	_currentPboIndex = nextPboIndex;
}

WindowCaptureCallback::WindowCaptureCallback(int numFrames, Mode mode, FramePosition position, GLenum readBuffer)
	: _mode(mode),
	_position(position),
	_readBuffer(readBuffer),
	_numFrames(numFrames)
{
}

WindowCaptureCallback::ContextData* WindowCaptureCallback::createContextData(osg::GraphicsContext* gc) const
{
	WindowCaptureCallback::ContextData* cd = new WindowCaptureCallback::ContextData(gc, _mode, _readBuffer);
	cd->_captureOperation = _defaultCaptureOperation;
	return cd;
}

WindowCaptureCallback::ContextData* WindowCaptureCallback::getContextData(osg::GraphicsContext* gc) const
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
	osg::ref_ptr<ContextData>& data = _contextDataMap[gc];
	if (!data) data = createContextData(gc);

	return data.get();
}

void WindowCaptureCallback::setCaptureOperation(CaptureOperation* operation)
{
	_defaultCaptureOperation = operation;

	// Set the capture operation for each ContextData.
	for (ContextDataMap::iterator it = _contextDataMap.begin(); it != _contextDataMap.end(); ++it)
	{
		it->second->_captureOperation = operation;
	}
}

void WindowCaptureCallback::operator()(osg::RenderInfo& renderInfo) const
{
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
	glReadBuffer(_readBuffer);
#endif

	osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
	osg::ref_ptr<ContextData> cd = getContextData(gc);
	cd->read();

	// If _numFrames is > 0 it means capture that number of frames.
	if (_numFrames > 0)
	{
		--_numFrames;
		if (_numFrames == 0)
		{
			// the callback must remove itself when it's done.
			if (_position == START_FRAME)
				renderInfo.getCurrentCamera()->setInitialDrawCallback(0);
			if (_position == END_FRAME)
				renderInfo.getCurrentCamera()->setFinalDrawCallback(0);
		}
	}

	int prec = osg::notify(osg::INFO).precision(5);
	OSG_INFO << "ScreenCaptureHandler: "
		<< "copy=" << (cd->_timeForFullCopy*1000.0f) << "ms, "
		<< "operation=" << (cd->_timeForCaptureOperation*1000.0f) << "ms, "
		<< "total=" << (cd->_timeForFullCopyAndOperation*1000.0f) << std::endl;
	osg::notify(osg::INFO).precision(prec);

	cd->_timeForFullCopy = 0;
}

}
