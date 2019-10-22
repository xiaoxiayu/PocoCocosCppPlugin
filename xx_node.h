#ifndef _XX_AUTOTEST_NODE_H
#define _XX_AUTOTEST_NODE_H

#include <string>
#include "cocos2d.h"

#include "xx_define.h"

#include "json/document.h"
#include "json/writer.h"

enum XXNodeType
{
	E_QA_Unknown = -1,

	E_QA_Control = 50,
	E_QA_Layer,
	E_QA_MenuItem,
	E_QA_Scene,
	E_QA_AtlasNode,
	E_QA_LabelTTF,
	E_QA_SpriteBatchNode,
	E_QA_Sprite,
	E_QA_Guide,
	E_QA_GuideTouchNode,
	E_QA_INode
};


class XXNode {
	cocos2d::XXCCNode* _node;
	cocos2d::XXCCSize _screenSize;

	void stringCompare(std::string cmpStr, const char* nodeStr);

public:
	XXNode(cocos2d::XXCCNode* node);
	~XXNode();

	void getPos(float* pos);

	void getSize(float* data);

	void getScale(float* data);

	void getRotation3D(float* data);

	void getZOrders(float* data);

	void getSkew(float* data);

	void getAnchorPoint(float* pos);

	std::string getName(const char* nodeType);

	std::string getNodeTypeStr(int nodeType);
	void getNodeType(int& nodeType);

	void setPayload(std::string nodeName, int nodeType, int detailType, rapidjson::Writer<rapidjson::StringBuffer>& writer);

	void setChildren(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	void setDumpString(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	bool GetFilteredNodeInfo(rapidjson::Writer<rapidjson::StringBuffer>& writer, std::vector<std::string>& nodeInfos);

	float getScreenWidth() { return _screenSize.width;}
	float getScreenHeight() { return _screenSize.height;}

	bool GetChildrenInfo(rapidjson::Writer<rapidjson::StringBuffer>& writer, std::vector<std::string>& nodeInfos);
};


#endif // _XX_AUTOTEST_NODE_H