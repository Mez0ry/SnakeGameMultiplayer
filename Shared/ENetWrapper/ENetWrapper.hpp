#ifndef MEZ_ENET_WRAPPER
#define MEZ_ENET_WRAPPER
#include "enet/enet.h"
#include <string>
#include <iostream>

namespace ENet {
using uint32_type = enet_uint32;
using uint16_type = enet_uint16;
using uint8_type = enet_uint8;

class ENetWrapper {
public:
  ENetWrapper() = default;
  ~ENetWrapper() = default;

public:
  ENetWrapper(ENetWrapper &&move) noexcept;
  ENetWrapper &operator=(ENetWrapper &&move) noexcept;
  void swap(ENetWrapper &other) noexcept;
  ENetWrapper(ENetWrapper const &) = delete;
  ENetWrapper &operator=(ENetWrapper const &) = delete;
};

class ClientWrapper : ENetWrapper {
public:
  ClientWrapper();
  ~ClientWrapper();

  void SetAddress(const std::string &host, size_t port);
  void SetAddress(uint32_type host, size_t port);

  /**
   * @brief Creates a host for communicating to peers
   * @param
   * Address - inner parameter, which set to NULL and if its NULL then no peers
   * can connect to the host
   * @param
   * peerCount	the maximum number of peers that should be allocated for the
   * host.
   * @param
   * channelLimit	the maximum number of channels allowed; if 0, then this
   * is equivalent to ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
   * @param
   * incomingBandwidth	downstream bandwidth of the host in bytes/second; if 0,
   * ENet will assume unlimited bandwidth.
   * @param
   * outgoingBandwidth	upstream bandwidth of the host in bytes/second; if 0,
   * ENet will assume unlimited bandwidth.
   * @return
   * true on success and false on failure
   */
  bool CreateHost(size_t peerCount, size_t channelLimit, uint32_type incomingBandwidth, uint32_type outgoingBandwidth);

  /**
   * @brief Initiates a connection to a foreign host. Remark: The peer returned
   * will have not completed the connection until enet_host_service() notifies
   * of an ENET_EVENT_TYPE_CONNECT event for the peer
   * @param
   * channelCount	number of channels to allocate
   * @param
   * data	user data supplied to the receiving host
   * @return
   * true on success false on failure
   */
  bool ConnectHost(size_t channelCount, uint32_type data = 0);

  /**
   * @brief destroys host
   */
  void DestroyHost();
public:
[[__nodiscard__]] ENetHost* GetHost()  {return m_client;}
[[__nodiscard__]] ENetPeer* GetPeer() const {return m_peer;}
  ENetEvent& GetEvent() {return m_event;}
  ENetAddress& GetAddress() {return m_address;}

private:
  ENetAddress m_address;
  ENetHost *m_client;
  ENetPeer *m_peer;
  ENetEvent m_event;
};

class ServerWrapper : ENetWrapper {
public:
  ServerWrapper();
  ~ServerWrapper();

  void SetAddress(const std::string &host, size_t port);
  void SetAddress(uint32_type host, size_t port);

  /**
   * @brief Creates a host for communicating to peers
   * can connect to the host
   * @param
   * peerCount	the maximum number of peers that should be allocated for the
   * host.
   * @param
   * channelLimit	the maximum number of channels allowed; if 0, then this
   * is equivalent to ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
   * @param
   * incomingBandwidth	downstream bandwidth of the host in bytes/second; if 0,
   * ENet will assume unlimited bandwidth.
   * @param
   * outgoingBandwidth	upstream bandwidth of the host in bytes/second; if 0,
   * ENet will assume unlimited bandwidth.
   * @return
   * true on success and false on failure
   */
  bool CreateHost(size_t peerCount, size_t channelLimit, uint32_type incomingBandwidth, uint32_type outgoingBandwidth);

  /**
   * @brief destroys host
   */
  void DestroyHost();
public:
[[__nodiscard__]] ENetHost* GetHost()  {return m_server;}
  ENetEvent& GetEvent() {return m_event;}
  ENetAddress& GetAddress() {return m_address;}
private:
  ENetAddress m_address;
  ENetHost *m_server;
  ENetEvent m_event;
};

};     // namespace ENet
#endif //! MEZ_ENET_WRAPPER