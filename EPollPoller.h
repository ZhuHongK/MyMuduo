#pragma once

#include "Poller.h"
#include "Timestamp.h"

#include <vector>
#include <sys/epoll.h>


/**
 * epoll的使用
 * epoll_create 创建epoll的fd
 * epoll_ctl  添加你想让epoll监听的fd以及针对fd感兴趣的事件  add/mod/del
 * epoll_wait  
 */
class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *Loop);
    ~EPollPoller() override;

    // 重写基类Poller的抽象方法
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    // 填写活跃的连接
    void fillActiveCahnnels(int numEvents, ChannelList *activeChnnels) const;

    // 更新channel通道
    void update(int operation, Channel * channel);

    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_;
};