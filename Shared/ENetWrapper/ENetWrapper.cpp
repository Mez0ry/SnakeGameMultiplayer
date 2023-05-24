#include "ENetWrapper.hpp"

ENet::ClientWrapper::ClientWrapper() {
  if (enet_initialize() != 0) {
    std::cerr<<"Failed to initialize enet"<<'\n';
    std::exit(1);
  }
}

ENet::ClientWrapper::~ClientWrapper() { enet_deinitialize(); }

void ENet::ClientWrapper::SetAddress(const std::string &host, size_t port) {
  enet_address_set_host(&m_address, host.c_str());
  m_address.port = port;
}

void ENet::ClientWrapper::SetAddress(uint32_type host, size_t port) {
  m_address.host = host;
  m_address.port = port;
}

bool ENet::ClientWrapper::CreateHost(size_t peerCount, size_t channelLimit, uint32_type incomingBandwidth, uint32_type outgoingBandwidth) {
  m_client = enet_host_create(NULL, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
  if (m_client == NULL) {
    return false;
  }

  return true;
}

bool ENet::ClientWrapper::ConnectHost(size_t channelCount, enet_uint32 data /*= 0*/) {
  m_peer = enet_host_connect(m_client, &m_address, channelCount, data);
  if (m_peer == NULL) {
    return false;
  }

  return true;
}

void ENet::ClientWrapper::DestroyHost(){
    enet_host_destroy(m_client);
}


//ServerWrapper
ENet::ServerWrapper::ServerWrapper(){
  if (enet_initialize() != 0) {
    std::cerr<<"Failed to initialize enet"<<'\n';
    std::exit(1);
  }
}

ENet::ServerWrapper::~ServerWrapper(){
  enet_deinitialize();
}

void ENet::ServerWrapper::SetAddress(const std::string &host, size_t port){
  enet_address_set_host(&m_address, host.c_str());
  m_address.port = port;
}
void ENet::ServerWrapper::SetAddress(uint32_type host, size_t port){
  m_address.host = host;
  m_address.port = port;
}

bool ENet::ServerWrapper::CreateHost(size_t peerCount, size_t channelLimit, uint32_type incomingBandwidth, uint32_type outgoingBandwidth){
  m_server = enet_host_create(&m_address, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
  if (m_server == NULL) {
    return false;
  }

  return true;
}


void ENet::ServerWrapper::DestroyHost(){
 enet_host_destroy(m_server);
}
