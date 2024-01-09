#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"

#include <error.h>

// channel未添加到poller中
const int kNew = -1;    // channel的成员index_ = -1
// channel已添加到poller中
const int kAddd = 1;
// channel从poller中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize) // vector<epoll_event>
{
    if (epollfd_ < 0)
    {
        LOG_FATAL("epoll_create error:%d \n", errno);
    }
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{

}

// channel update remove => EventLoop updateChannel removeChannel => Poller updateChannel removeChannel
/**
 *          EventLoop
 * channelList     Poller
 *                  ChannelMap <fd, channel*> epollfd
 */
void EPollPoller::updateChannel(Channel *channel) 
{
    const int index = channel->index();
    LOG_INFO("fd=%d events=%d index=%d \n", channel->fd(), Channel->events(), index);

    if (index == kNew || index = kDeleted)
    {
        if (index == kNew)
        {
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(kAddd);
        update(EPOLL_CTL_ADD, channel);
    }
    else  // channel 已经在poller上注册过了
    {
        int fd = channel->fd();
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel) 
{
    int fd = channel->fd();
    channels_.erase(fd);

    int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

// 填写活跃的连接
void EPollPoller::fillActiveCahnnels(int numEvents, ChannelList *activeChnnels) const
{

}

// 更新channel通道 epoll_ctl add/mod/del
void EPollPoller::update(int operation, Channel * channel)
{
    epoll_event event;
    memset(&event, 0, sizeof event); 

    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        }
        else
        {
            LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
        }
    }
}