#include "Packet.hpp"
Network::Packet::Packet() : m_ReadPos(0){

}

Network::Packet::~Packet(){

}

void Network::Packet::Append(const void *data, size_t size){
    if(data != nullptr && size > 0){
        size_t end_index = m_Bytes.size();
        m_Bytes.resize(end_index + size);
        std::memcpy(&m_Bytes[end_index],data,size);
    }
}

const void * Network::Packet::Data() const{
    return (IsEmpty()) ? nullptr : m_Bytes.data();
}

ENetPacket* Network::create_packet(const Packet& packet,enet_uint32 packet_flags){
   ENetPacket* pPacket = enet_packet_create(packet.Data(),packet.GetBytesSize(),packet_flags);
   if(pPacket == NULL){
    std::cerr<<"Failed to create a packet" << '\n';
   }
   return pPacket;
}

void Network::send_packet(ENetPeer *peer, const Packet &packet,enet_uint8 channel_id, enet_uint32 packet_flags){
  size_t data_len = packet.GetBytesSize();
  ENetPacket* enet_packet = create_packet(packet,packet_flags);

  enet_packet_resize(enet_packet,data_len);

  enet_peer_send(peer,channel_id,enet_packet); // internal impl of this function handles deallocation of packet
}