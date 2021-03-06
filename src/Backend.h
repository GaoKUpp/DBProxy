#ifndef _BACKEND_CLIENT_H
#define _BACKEND_CLIENT_H

#include <memory>
#include <queue>
#include <vector>
#include <string>

#include <brynet/net/EventLoop.h>
#include <brynet/net/TCPService.h>

#include "BaseSession.h"

class BaseWaitReply;
struct parse_tree;
struct BackendParseMsg;

/*  链接db服务器的(网络线程-网络层)会话  */
class BackendSession : public BaseSession, public std::enable_shared_from_this<BackendSession>
{
public:
    BackendSession(brynet::net::TcpConnection::Ptr session, int id);
    ~BackendSession();

    void                                        forward(
                                                    const std::shared_ptr<BaseWaitReply>& waitReply, 
                                                    const std::shared_ptr<std::string>& sharedStr, 
                                                    const char* b, 
                                                    size_t len);
    int                                         getID() const;

private:
    size_t                                      onMsg(const char* buffer, size_t len) override;
    void                                        onEnter() override;
    void                                        onClose() override;

    void                                        processReply(
                                                    const std::shared_ptr<parse_tree>& redisReply, 
                                                    std::shared_ptr<std::string>& responseBinary, 
                                                    const char* replyBuffer, 
                                                    size_t replyLen);

private:
    const int                                   mID;
    std::shared_ptr<parse_tree>                 mRedisParse;
    std::shared_ptr<std::string>                mCache;

    std::queue<std::weak_ptr<BaseWaitReply>>    mPendingWaitReply;
};

std::shared_ptr<BackendSession>                 randomServer();
std::shared_ptr<BackendSession>                 findBackendByID(int id);

#endif