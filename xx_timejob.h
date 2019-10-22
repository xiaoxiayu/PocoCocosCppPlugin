#ifndef _XX_AUTOTEST_TIMEJOB_H
#define _XX_AUTOTEST_TIMEJOB_H

#include <string>

#include "json/document.h"
#include "json/writer.h"

#include "xx_define.h"

typedef struct XX_IMAGE_DATA {
	unsigned char*	imgBuf;
	int				len;
} XX_IMAGE_DATA;

typedef struct XX_SAFE_DUMP_DATA {
	std::string			idStr;
	rapidjson::Value*	filter;

	bool				bDumpEnd;

	std::string			data;
} XX_DUMP_DATA;


#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
class XXTimeJob : public cocos2d::CCObject {
#else
class XXTimeJob : public cocos2d::Ref {
#endif

	cocos2d::XXCCSize m_winSize;
	cocos2d::XXCCSize m_frameSize;

	float		m_screenshot_width;
	float		m_screenshot_height;

	int			m_uFrames;
public:
	static bool					g_do_screenshot;
	static XX_IMAGE_DATA*	g_screenImg;
	static bool					g_do_dump;

	static XX_DUMP_DATA*		g_dumpData;

private:
	
	void			screenShotFromRenderTexture(const char* savePath);
	void			screenShotFromOpenGL(const char* savePath);

public:
	XXTimeJob();
	~XXTimeJob();

	void			ScheduleReg();

	void			ScreenShot(float t);
	void			DumpData(float t);

	void			GetScreenSize(float* w, float* h);

	void			writePngEnd(cocos2d::XXCCRenderTexture* texture, const std::string& data);

	void			DoScreenShot(float width = 0, float height = 0);

	bool			WaitScreenShotFinish(int timeout_t = 15);

	bool			WaitDumpFinish(int timeout_t = 15);

	bool			GetScreenBuf(unsigned char** buf, int* len);

	void			ReleaseScreenBuf();

	void			DoDump(XX_DUMP_DATA* cData);

};


#endif // _XX_AUTOTEST_TIMEJOB_H