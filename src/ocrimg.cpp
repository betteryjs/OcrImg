#include <tencentcloud/core/Credential.h>
#include <tencentcloud/core/profile/ClientProfile.h>
#include <tencentcloud/core/profile/HttpProfile.h>
#include <tencentcloud/ocr/v20181119/OcrClient.h>
#include <tencentcloud/ocr/v20181119/model/GeneralAccurateOCRRequest.h>
#include <tencentcloud/ocr/v20181119/model/GeneralAccurateOCRResponse.h>
#include <iostream>
#include <string>
#include <fstream>
#include "include/base64.h"
#include "include/json.hpp"

using namespace std;
using json = nlohmann::json;
using namespace TencentCloud;
using namespace TencentCloud::Ocr::V20181119;
using namespace TencentCloud::Ocr::V20181119::Model;
using namespace std;

#include <gflags/gflags.h>

#include <sys/stat.h>

inline int exist(const string name) {
    // if  exists return 1
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}


class OCRImages {
private:
    string secretId;
    string secretKey;
    string fileName;
    string content="";

public:
    OCRImages(const string & name):fileName(name){}
    ~OCRImages(){}
    void Run(){
        this->loadCredentialKey();
        this->SendRequests();
    }

    int loadCredentialKey() {
        if (exist("tencent_key.json")) {
            json tencent_key_json;
            ifstream tencent_key("tencent_key.json");
            tencent_key>>tencent_key_json;
            this->secretId = tencent_key_json["secretId"].get<string>();
            this->secretKey = tencent_key_json["secretKey"].get<string>();
            return 0;
        }else{
            return 1;
        }
    }



    int SendRequests(){
        Credential cred = Credential(this->secretId, this->secretKey);
        HttpProfile httpProfile = HttpProfile();
        httpProfile.SetEndpoint("ocr.tencentcloudapi.com");
        ClientProfile clientProfile = ClientProfile();
        clientProfile.SetHttpProfile(httpProfile);
        OcrClient client = OcrClient(cred, "ap-beijing", clientProfile);
        GeneralAccurateOCRRequest req = GeneralAccurateOCRRequest();
        fstream f;
        f.open(this->fileName, ios::in | ios::binary);
        f.seekg(0, std::ios_base::end);
        std::streampos sp = f.tellg();
        int size = sp;
        char *buffer = (char *) malloc(sizeof(char) * size);
        f.seekg(0, std::ios_base::beg);
        f.read(buffer, size);
//        cout << "file size:" << size << endl;
        string imgBase64 = base64_encode(buffer, size);
//        cout << "img base64 encode size:" << imgBase64.size() << endl;
        req.SetImageBase64(imgBase64);
        auto outcome = client.GeneralAccurateOCR(req);
        if (!outcome.IsSuccess()) {
            cout << outcome.GetError().PrintAll() << endl;
            return -1;
        }
        GeneralAccurateOCRResponse resp = outcome.GetResult();
        string str = resp.ToJsonString();
        auto j = json::parse(str);
        vector<string> DetectedTextList;
        for (auto content: j["TextDetections"]) {
            DetectedTextList.emplace_back(content["DetectedText"]);
        }
        for (string res: DetectedTextList) {
            this->content = this->content + res + '\n';
        }
        f.close();
        cout << endl;
        cout << endl;
        cout << this->content << endl;
        cout << endl;
        cout << endl;
        return 0;
    }


    string getCount() const{
        return this->content;
    }

};

DEFINE_bool(f, false, "cout file");
DEFINE_string(n, "filename", "file name");

int main(int argc, char **argv) {

    google::ParseCommandLineFlags(&argc, &argv, true);
    string filepath=string (FLAGS_n);
    if(!FLAGS_n.empty()){

        OCRImages*   ocrImages= new OCRImages(filepath);
        ocrImages->Run();
        if (!FLAGS_f) {
                string name;
                string fellname =string (FLAGS_n);
                string::size_type position = fellname.find('.');
                if (position != fellname.npos) {
                    name = string(fellname, 0, position);
                }
            fstream  f2;

            f2.open(name + ".txt", ios::out);
                f2 << ocrImages->getCount();
                f2.close();
        }
        delete ocrImages;
    }

    google::ShutDownCommandLineFlags();
    return 0;
}
