#ifdef _XX_AUTOTEST_

#include "xx_server.h"
#include "xx_node.h"

#include "json/document.h"
#include "json/writer.h"

#include "xx_dump.h"

USING_NS_CC;

/** Base64 */
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}

std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
}

	return ret;
}



void XXDataSend(CActiveSocket* pClient, std::string data) {
	XXNetData sendData;
	memset(sendData.dataStr, 0, MAX_PACKET);

	int dumpStrLen = data.length();
	int totalStrLen = dumpStrLen + 4;
	if (totalStrLen > MAX_PACKET) {
		int strOffset = 0;
		int willSendedLen = totalStrLen;
		while (willSendedLen > 0) {
			int sendLen = 0;
			if (willSendedLen <= MAX_PACKET) {
				sendLen = willSendedLen;
			}
			else {
				sendLen = MAX_PACKET % willSendedLen;
			}

			if (strOffset == 0) {
				int firstStrLen = sendLen - 4;
				std::string sendStr = data.substr(strOffset, firstStrLen);
				sprintf(sendData.dataStr, "%s", sendStr.c_str());
				sendData.dataHeader.len = dumpStrLen;
				pClient->Send((unsigned char *)&sendData, sendLen);
				strOffset += firstStrLen;
				willSendedLen -= sendLen;
			}
			else {
				std::string sendStr = data.substr(strOffset, sendLen);
				pClient->Send((unsigned char *)sendStr.c_str(), sendLen);
				strOffset += sendLen;
				willSendedLen -= sendLen;
			}
		}
	}
	else {
		memcpy(sendData.dataStr, data.c_str(), data.length());
		sendData.dataHeader.len = data.length();
		pClient->Send((unsigned char *)&sendData, sendData.dataHeader.len + 4);
	}
}



#ifdef _WIN32
void usleep(__int64 usec) 
{ 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}
#endif

//CLock g_lock;
class XXRespondAction {
public:
	XXRespondAction() {};
	~XXRespondAction() {};


	static void safeDump(XXTimeJob* screen, const char* idStr, rapidjson::Value* filter, CActiveSocket* pClient) {

		int bytesReceived = 0;
		int nparsed = -1;

		XX_DUMP_DATA cData;
		cData.bDumpEnd = false;
		cData.data = "";
		cData.filter = filter;
		cData.idStr = idStr;

		screen->DoDump(&cData);

		if (!screen->WaitDumpFinish()) {
			return;
		}

		XXDataSend(pClient, cData.data);
	}

	static void dump(const char* idStr, rapidjson::Value* filter, CActiveSocket* pClient) {
		int bytesReceived = 0;
		int nparsed = -1;

		std::string dumpStr = "";
		
		dumpStr = Dump(idStr);

		XXDataSend(pClient, dumpStr);
	}


	static void getScreenSize(CActiveSocket* pClient, std::string id_s, XXTimeJob* screen) {
		float width = 0, height = 0;
		screen->GetScreenSize(&width, &height);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();

		writer.Key("jsonrpc");
		writer.Double(2.0);
		writer.Key("id");
		writer.String(id_s.c_str());

		writer.Key("result");
		writer.StartArray();
		writer.Double(width);
		writer.Double(height);
		writer.EndArray();

		writer.EndObject();

		XXDataSend(pClient, buffer.GetString());
	}

	static void getSDKVersion(CActiveSocket* pClient, std::string id_s) {
		char buf[1024] = { 0 };
		sprintf(buf, "{\"result\":\"1.0.7\", \"jsonrpc\" : \"2.0\", \"id\" : %s}", id_s.c_str());

		XXDataSend(pClient, buf);
	}


	static void getRenderData(CActiveSocket* pClient, int interval) {
		char buf[1024] = { 0 };
		
		int time_i = 30;
		if (interval == 0) {
			interval = 1;
			time_i = 1;
		}

		float* deltaTimes = new float[time_i * interval];
		float* sfps = new float[time_i * interval];

		memset(deltaTimes, 0, time_i * interval);
		memset(sfps, 0, time_i * interval);

		int t_i = 0;
		for (int j = 0; j < interval; j++) {
			for (int i = 0; i < time_i; i++) {
				float deltaTime = XXCCDirector::funcDirectorGetInstance()->getDeltaTime();
				float sfp = XXCCDirector::funcDirectorGetInstance()->getSecondsPerFrame();

				deltaTimes[t_i] = deltaTime;
				sfps[t_i] = sfp;

				usleep(33333);
				t_i++;
			}
		}
		
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();

		writer.Key("jsonrpc");
		writer.Double(2.0);

		writer.Key("DeltaTime");
		writer.StartArray();

		t_i = 0;
		for (int j = 0; j < interval; j++) {
			for (int i = 0; i < time_i; i++) {
				writer.Double(deltaTimes[t_i]);
				t_i++;
			}
		}

		writer.EndArray();

		writer.Key("SPF");
		writer.StartArray();

		t_i = 0;
		for (int j = 0; j < interval; j++) {
			for (int i = 0; i < time_i; i++) {
				writer.Double(sfps[t_i]);
				t_i++;
			}
		}

		writer.EndArray();

		writer.EndObject();

		delete[] sfps;
		delete[] deltaTimes;

		XXDataSend(pClient,  buffer.GetString());
	}

	static void unknownFunction(CActiveSocket* pClient, std::string id_s, std::string clientIp, std::string funcName) {
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();

		writer.Key("jsonrpc");
		writer.Double(2.0);
		writer.Key("id");
		writer.String(id_s.c_str());

		writer.Key("error");
		writer.StartObject();
		writer.Key("message");

		char msg[1024] = { 0 };
		sprintf(msg, "No such rpc method \"%s\", reqid: %s, client:%s\"", funcName.c_str(), id_s.c_str(), clientIp.c_str());

		writer.Key(msg);
		writer.EndObject();

		writer.EndObject();

		XXDataSend(pClient, buffer.GetString());
	}


	static void screenShot(CActiveSocket* pClient, std::string id_s, XXTimeJob* screen) {
		screen->DoScreenShot();

		if (!screen->WaitScreenShotFinish()) {
			return;
		}

		int bufLen = 0;
		unsigned char* imgBuf = NULL;
		bool bRet = screen->GetScreenBuf(&imgBuf, &bufLen);
		if (!bRet) {
			return;
		}

		std::string bsStr = base64_encode(imgBuf, bufLen);

		screen->ReleaseScreenBuf();

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();

		writer.Key("jsonrpc");
		writer.Double(2.0);
		writer.Key("id");
		writer.String(id_s.c_str());

		writer.Key("result");
		writer.StartArray();
		writer.String(bsStr.c_str());
		writer.String("png");
		writer.EndArray();

		writer.EndObject();

		std::string dumpString = buffer.GetString();

		XXDataSend(pClient, dumpString);
	}
};


void* do_action(void* data) {
	XX_ADATA* pData = (XX_ADATA*)data;
	CActiveSocket* pClient = pData->pClient;

	while (true) {
		XXNetData recvData;

		int numBytes = pClient->Receive(1024);
		if (numBytes <= 0) {
			pClient->Close();

			delete pClient;
			delete data;
			return NULL;
		}

		char tmpRes[1024 + 1];
		memset(tmpRes, 0, 1024 + 1);
		memcpy(tmpRes, pClient->GetData(), numBytes);
		//recv_str += tmpRes;

		XX_Header_t* recvHeader = (XX_Header_t*)tmpRes;
		int recvLen = recvHeader->len;
		if (recvLen > 100000) {
			pClient->Close();
			delete pClient;
			delete data;
			return NULL;
		}

		char recvDataStr[1024] = { 0 };
		memcpy(recvDataStr, tmpRes + 4, recvLen);

		rapidjson::Document d;
		d.Parse(recvDataStr);
		
		if (!d.HasMember("id") || !d.HasMember("method")) {
			pClient->Close();

			delete pClient;
			delete data;
			return NULL;
		}

		rapidjson::Value& id_v = d["id"];
		rapidjson::Type vType = id_v.GetType();
		//const char* idStr = "";
		char idStr[512] = { 0 };
		if (vType == rapidjson::Type::kStringType) {
			sprintf(idStr, "%s", id_v.GetString());
		}
		else if (vType == rapidjson::Type::kNumberType) {
			sprintf(idStr, "%d", id_v.GetInt());
		}
		else {
			sprintf(idStr, "%s", "");
		}

		rapidjson::Value& s = d["method"];
		const char* methodStr = s.GetString();

		if (strcmp(methodStr, "dump") == 0 || strcmp(methodStr, "Dump") == 0) {
			std::string filter = "";
			rapidjson::Value* filterValue = NULL;
			if (d.HasMember("filter")) {
				filterValue = &d["filter"];
			}

			bool bSafeDump = true;
			if (d.HasMember("safe")) {
				rapidjson::Value& _s = d["safe"];
				bSafeDump = _s.GetBool();
			}
			if (bSafeDump) {
				XXRespondAction::safeDump(pData->pScreen, idStr, filterValue, pClient);
			} else {
				XXRespondAction::dump(idStr, filterValue, pClient);
			}			
		}
		else if (strcmp(methodStr, "GetScreenSize") == 0) {
			XXRespondAction::getScreenSize(pClient, idStr, pData->pScreen);
		}
		else if (strcmp(methodStr, "Screenshot") == 0) {
			XXRespondAction::screenShot(pClient, idStr, pData->pScreen);
		}
		else if (strcmp(methodStr, "GetSDKVersion") == 0) {
			XXRespondAction::getSDKVersion(pClient, idStr);
		} else if (strcmp(methodStr, "GetRenderData") == 0) {
			int interval_i = 0;
			if (d.HasMember("interval")) {
				rapidjson::Value& interval_s = d["interval"];
				interval_i = interval_s.GetInt();
			}
			
			XXRespondAction::getRenderData(pClient, interval_i);
		}
		else {
			XXRespondAction::unknownFunction(pClient, idStr, "255.255.255.0", methodStr);
		}

	}

}


bool XXServerRun(XXTimeJob* screen) {
	CPassiveSocket socket;
	std::string host_ip = "0.0.0.0";

	socket.Initialize();

	socket.Listen(host_ip.c_str(), XX_SERVER_PORT);

	while (true) {
		std::string str = "";
		std::string recv_str = "";
		CCLog("XX RUNING");
		socket.Select();

		XX_ADATA* actionData = new XX_ADATA;
		actionData->pScreen = screen;
		actionData->pClient = socket.Accept();
		if (actionData->pClient == NULL) {
			CCLog("XX ERROR1");
			continue;
		}
		CCLog("XX LINK");
#ifdef _WIN32
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)do_action, (void*)actionData, 0, NULL);
#else
		pthread_t thread = 0;
		pthread_create(&thread, NULL, do_action, (void*)actionData);
#endif

#ifdef WIN32
		if (10038 == GetLastError()) {
			CCLog("XX ERROR0");
			break;
		}
#else
		if (9 == errno) {
			break;
		}
#endif
	}

	socket.Close();

	return true;
}
#endif // _XX_AUTOTEST_