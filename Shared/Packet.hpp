#ifndef __PACKET_HPP__
#define __PACKET_HPP__
#include "enet/enet.h"
#include <cstring>
#include <vector>
#include <iostream>

namespace Network {

template <typename T, typename = int,typename = int>
struct is_physics_vector : std::false_type{

};

template <typename T>
struct is_physics_vector<T,decltype((void) T::y, 0),decltype((void) T::x, 0)> : std::true_type{

};

template <typename T>
constexpr bool is_physics_vector_v = Network::is_physics_vector<T>::value;

class Packet {
public:
  Packet();
  ~Packet();

  void Append(const void *data, size_t size);

  const void *Data() const;
  
  size_t GetBytesSize() const {
    return m_Bytes.size();
  }

  size_t GetReadPos() const {
    return m_ReadPos;
  }

  /** left bit-shift operator*/

  Packet& operator<<(uint8_t data){
    Append(&data,sizeof(data));
    return (*this);
  }

  Packet& operator<<(uint16_t data){
    auto res = htons((uint16_t)data);

    Append(&res,sizeof(res));
    return (*this);
  }

  Packet& operator<<(uint32_t data){
    auto res = (uint32_t)(htonl((uint32_t)data));
    
    Append(&res,sizeof(res));
    return (*this);
  }

  Packet& operator<<(int8_t data){
    Append(&data,sizeof(data));
    return (*this);
  }

  Packet& operator<<(int16_t data){
    auto res = (int16_t)htons((uint16_t)data);

    Append(&res,sizeof(res));
    return (*this);
  }

  Packet& operator<<(int32_t data){
    auto res = (int32_t)htonl((uint32_t)data);

    Append(&res,sizeof(res));
    return (*this);
  }

  /** right bit-shift operator*/
  Packet& operator>>(uint8_t& data){
    auto size = sizeof(data);
    memcpy(&data,&m_Bytes[m_ReadPos],size);
    m_ReadPos += size;

    return (*this);
  }

  Packet& operator>>(uint16_t& data){
    memcpy(&data,&m_Bytes[m_ReadPos],sizeof(data));
    data = (uint16_t)ntohs(data);
    m_ReadPos += sizeof(data);
    return (*this);
  }

  Packet& operator>>(uint32_t& data){
    memcpy(&data,&m_Bytes[m_ReadPos],sizeof(data));
    data = ntohl(data);
    m_ReadPos += sizeof(data);
    return (*this);
  }

  Packet& operator>>(int8_t& data){
    memcpy(&data,&m_Bytes[m_ReadPos],sizeof(data));
    m_ReadPos += sizeof(data);
    return (*this);
  }

  Packet& operator>>(int16_t& data){
    memcpy(&data,&m_Bytes[m_ReadPos],sizeof(data));
    data = (int16_t)ntohs((uint16_t)data);
    m_ReadPos += sizeof(data);
    return (*this);
  } 

  Packet& operator>>(int32_t& data){
    memcpy(&data,&m_Bytes[m_ReadPos],sizeof(data));
    data = (int32_t)ntohl((uint32_t)data);
    m_ReadPos += sizeof(data);
    return (*this);
  }

  Packet& operator <<(const char* data){

    const auto len = (uint32_t)(std::strlen(data));
    *this << len;

    Append(data,(len * sizeof(uint8_t)));

    return (*this);
  }

  Packet& operator >>(char* data){
     std::uint32_t size = 0;
    *this >> size;

    if(size == 0) return (*this);

    if ((size > 0) )
    {
        std::memcpy(data, &m_Bytes[m_ReadPos], size);
        data[size] = '\0';

        m_ReadPos += size;
    }

    return (*this);
  }

  template <typename Vector,typename = std::enable_if_t<std::is_same_v<int,std::conditional_t<is_physics_vector_v<Vector> ,int,std::nullptr_t> >,std::nullptr_t> > 
  Packet& operator<<(Vector data){
    (*this) << data.x << data.y;
    return (*this);
  }

  template <typename Vector,typename = std::enable_if_t<std::is_same_v<int,std::conditional_t<is_physics_vector_v<Vector> ,int,std::nullptr_t> >,std::nullptr_t> > 
  Packet& operator>>(Vector& data){
    (*this) >> data.x >> data.y;
    return (*this);
  }

  bool IsEmpty() const{
    return m_Bytes.empty();
  }
  
private:
  std::vector<std::byte> m_Bytes;
  size_t m_ReadPos;
};

[[nodiscard]] ENetPacket* create_packet(const Packet& packet,enet_uint32 packet_flags);

void send_packet(ENetPeer *peer, const Packet &packet,enet_uint8 channel_id, enet_uint32 packet_flags);
};     // namespace Network

#endif //!__PACKET_HPP__