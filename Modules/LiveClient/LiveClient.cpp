#include "stdafx.h"
#include "LiveClient.h"
#include "LiveIpc.h"
#include "LiveWorker.h"

namespace LiveClient
{
    uv_loop_t *g_uv_loop = NULL;

    string g_strRtpIP;            //< RTP服务IP
    int    g_nRtpBeginPort;       //< RTP监听的起始端口，必须是偶数
    int    g_nRtpPortNum;         //< RTP使用的个数，从strRTPPort开始每次加2，共strRTPNum个
    int    g_nRtpCatchPacketNum;  //< rtp缓存的包的数量
    int    g_nRtpStreamType;      //< rtp包的类型，传给libLive。ps h264

    vector<int>     m_vecRtpPort;     //< RTP可用端口，使用时从中取出，使用结束重新放入
    CriticalSection m_csRTP;          //< RTP端口锁

    extern LIVECLIENT_CB ipc_cb;

    void Init(void* uv){
        g_uv_loop = (uv_loop_t *)uv;

        /** 进程间通信 */
        LiveIpc::Init();

        /** 配置内容 */
        g_strRtpIP           = Settings::getValue("RtpClient","IP");                    //< RTP服务IP
        g_nRtpBeginPort      = Settings::getValue("RtpClient","BeginPort",10000);       //< RTP监听的起始端口，必须是偶数
        g_nRtpPortNum        = Settings::getValue("RtpClient","PortNum",1000);          //< RTP使用的个数，从strRTPPort开始每次加2，共strRTPNum个
        g_nRtpCatchPacketNum = Settings::getValue("RtpClient", "CatchPacketNum", 100);  //< rtp缓存的包的数量
        g_nRtpStreamType     = Settings::getValue("RtpClient", "Filter", 0);            //< rtp类型，是ps还是h264

        Log::debug("RtpConfig IP:%s, BeginPort:%d,PortNum:%d,CatchPacketNum:%d"
            , g_strRtpIP.c_str(), g_nRtpBeginPort, g_nRtpPortNum, g_nRtpCatchPacketNum);
        m_vecRtpPort.clear();
        for (int i=0; i<g_nRtpPortNum; ++i) {
            m_vecRtpPort.push_back(g_nRtpBeginPort+i*2);
        }

        if(g_nRtpStreamType == 0)
            g_stream_type = STREAM_PS;
        else if(g_nRtpStreamType == 1)
            g_stream_type = STREAM_H264;
    }

    string GetClientsInfo() 
    {
        return GetAllWorkerClientsInfo();
    }

    void GetDevList(){
        LiveIpc::GetDevList();
    }

    void QueryDirtionary(){
        LiveIpc::QueryDirtionary();
    }

    void SetCallBack(LIVECLIENT_CB cb){
        ipc_cb = cb;
    }

    ILiveWorker* GetWorker(string strCode){
        CLiveWorker* worker = GetLiveWorker(strCode);
        if(nullptr == worker)
            worker = CreatLiveWorker(strCode);
        return (ILiveWorker*)worker;
    }
}