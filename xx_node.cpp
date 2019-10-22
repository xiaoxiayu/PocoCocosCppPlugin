#ifdef _XX_AUTOTEST_
#include "xx_node.h"


#include <iostream>
#include <regex>



USING_NS_CC;


bool IsFiniteNumber(double x) {
    return (x <= DBL_MAX && x >= -DBL_MAX);
}


XXNode::XXNode(XXCCNode* node) {
	auto glview = XXCCDirector::funcDirectorGetInstance()->getOpenGLView();
	_screenSize = glview->getFrameSize();

	_node = node;
	_node->retain();
	
};

XXNode::~XXNode() { 
	_node->release(); 
	
};

void XXNode::getPos(float* pos) {
#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
	CCPoint convert_pos = _node->convertToWorldSpaceAR(CCPoint(0, 0));
#else
	Vec2 convert_pos = _node->convertToWorldSpaceAR(Vec2(0, 0));
#endif
	
	//Size screen_size = Director::getInstance()->getWinSize();

	float tem_x = convert_pos.x / _screenSize.width; 
	float tem_y = 1 - (convert_pos.y / _screenSize.height);
	if (!IsFiniteNumber(tem_x)) {
		tem_x = 0;
	}
	if (!IsFiniteNumber(tem_y)) {
		tem_y = 0;
	}

	pos[0] = tem_x;
	pos[1] = tem_y;
}

void XXNode::getSize(float* data) {
	XXCCSize size = _node->getContentSize();
	if (!IsFiniteNumber(size.width)) {
		size.width = 0;
	}
	if (!IsFiniteNumber(size.height)) {
		size.height = 0;
	}
	size.width = size.width / _screenSize.width;
	size.height = size.height / _screenSize.height;
	data[0] = size.width;
	data[1] = size.height;
}

void XXNode::getScale(float* data) {
	float x = _node->getScaleX();
	float y = _node->getScaleY();
	if (!IsFiniteNumber(x)) {
		x = 0;
	}
	if (!IsFiniteNumber(y)) {
		y = 0;
	}
	data[0] = x;
	data[1] = y;
}

void XXNode::getRotation3D(float* data) {
#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
	float x = _node->getRotationX();
	float y = _node->getRotationY();
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
#else
	float x = _node->getRotationSkewX();
	float y = _node->getRotationSkewY();
	if (x == y) {
		Vec3 v = _node->getRotation3D();
		data[0] = v.x;
		data[1] = v.y;
		data[2] = v.z;
	}
#endif
	
}

void XXNode::getZOrders(float* data) {
	
#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
	data[0] = _node->getZOrder();
	data[1] = _node->getZOrder();
#else
	data[0] = _node->getGlobalZOrder();
	data[1] = _node->getLocalZOrder();
#endif
}

void XXNode::getSkew(float* data) {
	data[0] = _node->getSkewX();
	data[1] = _node->getSkewY();
}

void XXNode::getAnchorPoint(float* pos) {
#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
	CCPoint anchorPoint = _node->getAnchorPoint();
#else
	Vec2 anchorPoint = _node->getAnchorPoint();
#endif

	float tem_x = anchorPoint.x;
	float tem_y = 1 - anchorPoint.y;

	pos[0] = tem_x;
	pos[1] = tem_y;
}

static int _g_i = 0;
std::string XXNode::getName(const char* nodeType) {
	char name[1024];
	memset(name, 0, 1024);

#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
	sprintf(name, "<%s>", nodeType);
	return name;
#else
	std::string name = _node->getName();
	std::string desc = _node->getDescription();
	if (name.compare("") == 0) {
		name = desc;
	}
	return name;
#endif
}


std::string XXNode::getNodeTypeStr(int nodeType) {
	switch(nodeType) {
	case E_QA_Control:
		return "Control";
	case E_QA_Unknown:
		return "Unknown";
	case E_QA_Layer:
		return "Layer";
	case E_QA_MenuItem:
		return "MenuItem";
	case E_QA_Scene:
		return "Scene";
	case E_QA_AtlasNode:
		return "AtlasNode";
	case E_QA_LabelTTF:
		return "LabelTTF";
	case E_QA_SpriteBatchNode:
		return "SpriteBatchNode";
	case E_QA_Sprite:
		return "Sprite";
	case E_QA_Guide:
		return "Guide";
	case E_QA_GuideTouchNode:
		return "GuideTouchNode";
	case E_QA_INode:
		return "INode";
	default:
		return "Control";
	}
}

void XXNode::getNodeType(int& nodeType) {
	XXCCLayer* layer = dynamic_cast<XXCCLayer*>(_node);
	if (layer != NULL) {
		nodeType = E_QA_Layer;
		return;
	}
		
	XXCCMenuItem* item = dynamic_cast<XXCCMenuItem*>(_node);
	if (item != NULL) {
		nodeType = E_QA_MenuItem;
		return;
	}

	XXCCScene* scene = dynamic_cast<XXCCScene*>(_node);
	if (scene != NULL) {
		nodeType = E_QA_Scene;
		return;
	}

	XXCCAtlasNode* atlas = dynamic_cast<XXCCAtlasNode*>(_node);
	if (atlas != NULL) {
		nodeType = E_QA_AtlasNode;
		return;
	}

	XXCCLabelTTF* ttf = dynamic_cast<XXCCLabelTTF*>(_node);
	if (ttf != NULL) {
		nodeType = E_QA_LabelTTF;
		return;
	}

	XXCCSpriteBatchNode* batch = dynamic_cast<XXCCSpriteBatchNode*>(_node);
	if (batch != NULL) {
		nodeType = E_QA_SpriteBatchNode;
		return;
	}
#if TESTIN_USING_CC_EXTENSION == 1
	XXCCScale9Sprite* sprite9 = dynamic_cast<XXCCScale9Sprite*>(node);
	if (sprite9 != NULL)
		return "TCScale9Sprite";
#endif
#ifdef COCOS2D_CPP_3X
	XXCCLabel* label = dynamic_cast<XXCCLabel*>(_node);
	if (label != NULL)
		return "Label";
#endif
	XXCCSprite* sprite = dynamic_cast<XXCCSprite*>(_node);
	if (sprite != NULL) {
		nodeType = E_QA_Sprite;
		return;
	}
	nodeType = E_QA_Unknown;
	return;
}


void XXNode::setPayload(std::string nodeName, int nodeType, int detailType, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
	writer.Key("payload");
	writer.StartObject();

	writer.Key("screen");
	writer.StartArray();
	writer.Int(this->getScreenWidth());
	writer.Int(this->getScreenHeight());
	writer.EndArray();

	writer.Key("rotation");

	float rotationX = _node->getRotationX();
	float rotationY = _node->getRotationY();
	if (rotationX == rotationY) {
		writer.Double(_node->getRotation());
	} else {
		writer.Double(_node->getRotationX());
	}
	
	writer.Key("tag");
	writer.Int(_node->getTag());

	writer.Key("visible");
	writer.Bool(_node->isVisible());

#if 0
	writer.Key("desc");

#ifdef COCOS2D_CPP_3X
	writer.String(_node->getDescription().c_str());
#else
	char desc[1024];
	memset(desc, 0, 1024);

	sprintf(desc, "<%d | Tag = %d>", nodeType, _node->getTag());
	writer.String(desc);
#endif
#endif // 0

	writer.Key("type");
	writer.String(this->getNodeTypeStr(nodeType).c_str());

	writer.Key("name");
	writer.String(nodeName.c_str());

	float pos[2] = { 0 };
	getPos(pos);
	writer.Key("pos");
	//writer.SetMaxDecimalPlaces(3);
	writer.StartArray();
	
	writer.Double(pos[0]);
	writer.Double(pos[1]);
	writer.EndArray();

	float size[2] = { 0 };
	getSize(size);
	writer.Key("size");
	writer.StartArray();
	writer.Double(size[0]);
	writer.Double(size[1]);
	writer.EndArray();

	float scale[2] = { 0 };
	getScale(scale);
	writer.Key("scale");
	writer.StartArray();
	writer.Double(scale[0]);
	writer.Double(scale[1]);
	writer.EndArray();

	float rotation3D[3] = { 0 };
	getRotation3D(rotation3D);
	writer.Key("rotation3D");
	writer.StartObject();
	writer.Key("x");
	writer.Double(rotation3D[0]);
	writer.Key("y");
	writer.Double(rotation3D[1]);
	writer.Key("z");
	writer.Double(rotation3D[2]);
	writer.EndObject();

	float zOrders[2] = { 0 };
	getZOrders(zOrders);
	writer.Key("zOrders");
	writer.StartObject();
	writer.Key("global");
	writer.Double(zOrders[0]);
	writer.Key("local");
	writer.Double(zOrders[1]);
	writer.EndObject();

	float skew[2] = { 0 };
	getSkew(skew);
	writer.Key("skew");
	writer.StartArray();
	writer.Double(skew[0]);
	writer.Double(skew[1]);
	writer.EndArray();

	float anchorPoint[2] = { 0 };
	getAnchorPoint(anchorPoint);
	writer.Key("anchorPoint");
	writer.StartArray();
	writer.Double(anchorPoint[0]);
	writer.Double(anchorPoint[1]);
	writer.EndArray();

	writer.EndObject();
}

void XXNode::setChildren(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
	int childrenCnt = _node->getChildrenCount();
	if (childrenCnt > 0) {
		writer.Key("children");
		writer.StartArray();

#if COCOS2D_VERSION >= 0x00020100 && COCOS2D_VERSION < 0x00030000
		CCObject* child;
		CCARRAY_FOREACH(_node->getChildren(), child)
		{
			CCNode* sub = (CCNode*)child;
			bool bVisible = sub->isVisible();

			writer.StartObject();

			XXNode childNode(sub);
			childNode.setDumpString(writer);

			writer.EndObject();
		}
#else
		Label* label = dynamic_cast<Label*>(_node);
		for (auto& child : _node->getChildren()) {
			if (label != NULL) {
				TCSprite* sprite = dynamic_cast<TCSprite*>(child);
				if (sprite != NULL) {
					//not parse Sprite under Label
					continue;
				}
			}
			//parseNodeToXML(doc, element, child);

			writer.StartObject();

			XXNode childNode(child);
			childNode.setDumpString(writer);

			writer.EndObject();
		}
#endif // 
		writer.EndArray();
	}
}



void XXNode::setDumpString(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
	int nodeType = -1;
	getNodeType(nodeType);
	
	int detailType = E_QA_Unknown;

	char dataTypeArr[3] = { 0 };
	char detaDetailTypeArr[3] = { 0 };

	sprintf(dataTypeArr, "%d", nodeType);
	sprintf(detaDetailTypeArr, "%d", detailType);

	std::string nodeName = dataTypeArr;
	nodeName += "-";
	nodeName += detaDetailTypeArr;
	
	writer.Key("name");
	writer.String(nodeName.c_str());
	
	setPayload(nodeName, nodeType, detailType, writer);
	
	setChildren(writer);
}


#endif //#ifdef _XX_AUTOTEST_