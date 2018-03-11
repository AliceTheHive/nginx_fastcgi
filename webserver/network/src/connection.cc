#include "connection.h"

#include <sys/uio.h>

#include "constant.h"


namespace Network
{
	CConnection::CConnection(int32_t fd)
	{
		m_fd = fd;
		EnableInput();
		DisableOutput();

		m_local_address = "";
		m_local_port = 0;
		m_remote_address = "";
		m_remote_port = 0;
		
		if(-1 != m_fd)
		{
			sockaddr_in local_addr;
			socklen_t local_length = sizeof(local_addr);
			if(0 == getsockname(m_fd, static_cast<sockaddr *>(&local_addr), &local_length))
			{
				m_local_address = inet_ntoa(local_addr.sin_addr);
				m_local_port = ntohs(local_addr.sin_port);
			}

			sockaddr_in remote_addr;
			socklen_t remote_length = sizeof(remote_addr);
			if(0 == getpeername(m_fd, static_cast<sockaddr *>(&remote_addr), &remote_length))
			{
				m_remote_address = inet_ntoa(remote_addr.sin_addr);
				m_remote_port = ntohs(remote_addr.sin_port);
			}
		}
	}

	virtual CConnection::~CConnection()
	{
		
	}

	virtual void CConnection::InputNotify()
	{
		uint8_t buff[kDefaultBufferLength] = {'\0'};
		int64_t recv_length = -1;
		do
		{
			recv_length = read(m_fd, buff, sizeof(buff));
			if(recv_length > 0)
			{
				CBuffer *one_buffer = new CBuffer(recv_length);
				if(NULL == one_buffer)
				{
					log_error("Not enough memory for new CBuffer, length [%llu].", static_cast<unsigned long long int>(recv_length));
					delete this;
					break;
				}
				
				one_buffer->SetData(buff, recv_length);
				m_recv_buffers.PushBack(one_buffer);

				CPackets packets;
				int64_t decode_length = Decode(packets);
				if(decode_length > 0)
				{
					m_recv_buffers.EraseFront(decode_length);
					for(CPackets::iterator pit = packets.begin(); pit != packets.end(); ++ pit)
					{
						OnPacket(*(*pit));
						delete(*pit);
					}
				}
				else if(decode_length < 0)
				{
					OnDecodeError();
				}
				break;
			}
			else if(0 == recv_length)
			{
				log_error("The remote process initiative close fd [%d], local address [%s], local port [%u], remote address [%s], remote port [%u].", m_fd, m_local_address.c_str(), m_local_port, m_remote_address.c_str(), m_remote_port);
				delete this;
				break;
			}
			else
			{
				if(EINTR == errno
				   || EAGAIN == errno
				   || EINPROGRESS == errno
				   || EWOULDBLOCK == errno)
				{
					break;
				}
				else
				{
					log_error("The connection [%d] recv failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
					delete this;
					break;
				}
			}
		} while(0);
	}

	virtual void CConnection::OutputNotify()
	{
		int64_t send_length = -1;
		if(!m_send_buffers.empty())
		{
			uint32_t iov_count = 0;
			int64_t iov_length = 0;
			iovec iovs[kDefaultIOVIov_Count] = {'\0'};
			for(CBuffers::iterator bit = m_send_buffers.begin(); (bit != m_send_buffers.end()) && (iov_count < kDefaultIOVCount); ++ bit)
			{
				iovs[iov_count].iov_base = (*bit)->GetData();
				iovs[iov_count].iov_len = (*bit)->DataLength();
				++ iov_count;
				iov_length += (*bit)->DataLength();
			}

			if(iov_length > 0)
			{
				send_length = writev(m_fd, iovs, iov_count);
				if(send_length > 0)
				{
					m_send_buffers.EraseFront(send_length);
				}
				else if(0 == send_length)
				{
					log_debug("writev data is zero.");
					return;
				}
				else
				{
					if(EINTR == errno
					   || EAGAIN == errno
					   || EINPROGRESS == errno
					   || EWOULDBLOCK == errno)
					{
						log_debug("writev this data next.");
						return;
					}
					else
					{
						log_error("The connection [%d] writev data failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
						delete this;
						return;
					}
				}
			}
		}

		if(m_send_buffers.empty())
		{
			EnableOutput();
			m_owner->ModifyEvent(this);
		}
	}

	virtual void CConnection::OnPacket(CPacket &packet)
	{
		
	}

	virtual void CConnection::OnDecodeError()
	{
		Close();
		delete this;
	}
};
