#ifdef _XX_AUTOTEST_
#include "xx_timejob.h"
#if !defined(_WIN32)
#include <unistd.h>
#endif

#include "xx_dump.h"

USING_NS_CC;

bool XXTimeJob::g_do_screenshot = false;
bool XXTimeJob::g_do_dump = false;

XX_IMAGE_DATA* XXTimeJob::g_screenImg = NULL;
XX_DUMP_DATA* XXTimeJob::g_dumpData = NULL;

XXTimeJob::XXTimeJob() : m_uFrames(0) {
	m_winSize = XXCCDirector::funcDirectorGetInstance()->getWinSize();
	m_frameSize = XXCCDirector::funcDirectorGetInstance()->getOpenGLView()->getFrameSize();

	m_screenshot_width = m_frameSize.width;
	m_screenshot_height = m_frameSize.height;
}

XXTimeJob::~XXTimeJob() {
}


void XXTimeJob::ScheduleReg() {
	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(XXTimeJob::ScreenShot), this, 0.5, false);

	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(XXTimeJob::DumpData), this, 0.1, false);
}

void XXTimeJob::writePngEnd(XXCCRenderTexture* texture, const std::string& data) {
	std::string screenImgPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "screenshot.png";

	unsigned long nSize = 0;
	unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(screenImgPath.c_str(), "rb", &nSize);

	g_screenImg = new XX_IMAGE_DATA;
	g_screenImg->len = nSize;
	g_screenImg->imgBuf = new unsigned char[g_screenImg->len + 1];
	g_screenImg->imgBuf[g_screenImg->len] = '\0';

	memcpy(g_screenImg->imgBuf, pBuffer, g_screenImg->len);

	g_do_screenshot = false;
}


void XXTimeJob::screenShotFromOpenGL(const char* savePath) {
#ifdef _WIN32
	float zoomFactor = CCEGLView::sharedOpenGLView()->getFrameZoomFactor();
#else
	float zoomFactor = 1;
#endif
	int w = m_screenshot_width * zoomFactor;
	int h = m_screenshot_height * zoomFactor;

	int myDataLength = h * w * 4;

	GLubyte *buffer = new GLubyte[myDataLength + 1];
	memset(buffer, 0, myDataLength);

	glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	CCImage *image = new CCImage();
	GLubyte *buffer2 = (GLubyte *)malloc(myDataLength);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w * 4; x++) {
			buffer2[(h - 1 - y) * w * 4 + x] = buffer[y * 4 * w + x];
		}
	}
	
#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
	image->initWithImageData(buffer2, myDataLength,CCImage::kFmtRawData, w, h);
#else
	image->initWithRawData(buffer2, myDataLength, w, h, 3);
#endif
	image->saveToFile(savePath);

	free(buffer2);

	image->release();
}

void XXTimeJob::screenShotFromRenderTexture(const char* savePath) {
	// Get image form texture. Error:Black area in upper right corner.
	CCRenderTexture* texture = CCRenderTexture::create((int)m_screenshot_width, (int)m_screenshot_height);
	texture->setPosition(ccp(m_screenshot_width / 2, m_screenshot_height / 2));
	texture->begin();
	CCDirector::sharedDirector()->getRunningScene()->visit();
	texture->end();

#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
	bool bSaved = texture->saveToFile(savePath, kCCImageFormatPNG);
	if (!bSaved) {
		printf("save error");
	}

#else
	std::function<void(RenderTexture*, const std::string&)> save_end_func = std::bind(&XXTimeJob::writePngEnd, this, std::placeholders::_1, std::placeholders::_2);

	bool bSaved = texture->saveToFile("screenshot.png", kCCImageFormatPNG, false, save_end_func);
	if (!bSaved) {
		printf("save error");
	}
#endif

}

void XXTimeJob::DumpData(float t) {
	if (!g_do_dump) {
		return;
	}

	std::string dumpStr = "";

	dumpStr = Dump(g_dumpData->idStr.c_str());

	g_dumpData->data = dumpStr;

	g_do_dump = false;
}

void XXTimeJob::ScreenShot(float t) {
	if (!g_do_screenshot) {
		return;
	}

	if (m_screenshot_height <= 0 || m_screenshot_width <= 0) {
		return;
	}

	this->screenShotFromOpenGL((CCFileUtils::sharedFileUtils()->getWritablePath() + "screenshot.png").c_str());

	std::string screenImgPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "screenshot.png";

	unsigned long nSize = 0;
	unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(screenImgPath.c_str(), "rb", &nSize);

	g_screenImg = new XX_IMAGE_DATA;
	g_screenImg->len = nSize;
	g_screenImg->imgBuf = new unsigned char[g_screenImg->len + 1];
	g_screenImg->imgBuf[g_screenImg->len] = '\0';

	memcpy(g_screenImg->imgBuf, pBuffer, g_screenImg->len);

	g_do_screenshot = false;

	return;
}

bool XXTimeJob::GetScreenBuf(unsigned char** buf, int* len) {
	if (g_screenImg == NULL) {
		return false;
	}

	*buf = g_screenImg->imgBuf;
	*len = g_screenImg->len;

	return true;
}

void XXTimeJob::ReleaseScreenBuf() {
	if (g_screenImg == NULL) {
		return;
	}
	if (g_screenImg->imgBuf) {
		delete[] g_screenImg->imgBuf;
	}
	delete g_screenImg;
	g_screenImg = NULL;
}

void XXTimeJob::GetScreenSize(float* w, float* h) {

	*w = m_frameSize.width;
	*h = m_frameSize.height;
}

void XXTimeJob::DoScreenShot(float width, float height) {
	g_do_screenshot = true;

	if (width > 0) {
		m_screenshot_width = width;
	}

	if (height > 0) {
		m_screenshot_height = height;
	}

}

void XXTimeJob::DoDump(XX_DUMP_DATA* cData) {
	g_dumpData = cData;
	g_do_dump = true;
}

bool XXTimeJob::WaitScreenShotFinish(int timeout_t) {
	for (int i = 0; i < timeout_t; i++) {
		if (!g_do_screenshot) {
			return true;
		}
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
	return false;
}

bool XXTimeJob::WaitDumpFinish(int timeout_t) {
	for (int i = 0; i < timeout_t; i++) {
		if (!g_do_dump) {
			return true;
		}
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
	return false;
}



#endif //_XX_AUTOTEST_