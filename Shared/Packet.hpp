#ifndef __PACKET_HPP__
#define __PACKET_HPP__
#include "enet/enet.h"
#include <cstring>
#include <vector>
#include "Vector.hpp"

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

  /** left bit-shift operators*/
  
  template <typename T, class = typename std::enable_if_t<((std::is_integral_v<T>) || (std::is_unsigned_v<T>)),void> >
  Packet& operator<<(T data){
    T res = static_cast<T>(HostToNet<T>(data));
    Append(&res,sizeof(res));
    return (*this);
  }

  Packet& operator<<(const char* data){
    const auto len = std::strlen(data);
    *this << len;

    Append(data,(len * sizeof(uint8_t)));

    return (*this);
  }

  Packet& operator<<(Vec2 vector){
    (*this) << vector.x << vector.y;
    return (*this);
  }

  /** right bit-shift operators*/
  
  /**
   * @brief handles integral and unsigned types
  */
  template <typename T, class = typename std::enable_if_t<((std::is_integral_v<T>) || (std::is_unsigned_v<T>)),void> >
  Packet& operator>>(T& data){
    memcpy(&data,&m_Bytes[m_ReadPos],sizeof(T));
    data = NetToHost<T>(data);
    m_ReadPos += sizeof(data);
    return (*this);
  }

  Packet& operator>>(char* data){
     std::uint32_t size = 0;
    *this >> size;

    if(size == 0) return (*this);

    std::memcpy(data, &m_Bytes[m_ReadPos], size);
    data[size] = '\0';

    m_ReadPos += size;

    return (*this);
  }

  Packet& operator>>(Vec2& vector){
    (*this) >> vector.x >> vector.y;
    return (*this);
  }

  bool IsEmpty() const{
    return m_Bytes.empty();
  }

private:
  template<typename T, class = typename std::enable_if_t<(std::is_integral_v<T> || std::is_unsigned_v<T>)> >
  T NetToHost(T& data){
    if constexpr(sizeof(T) == sizeof(uint16_t)){
      return static_cast<T>((T)ntohs(data));
    }else if constexpr(sizeof(T) == sizeof(uint32_t)){
      return static_cast<T>((T)ntohl(data));
    }
    
    return static_cast<T>(data);
  }

  template<typename T, class = typename std::enable_if_t<(std::is_integral_v<T> || std::is_unsigned_v<T>)> >
  T HostToNet(T& data){
    if constexpr(sizeof(T) == sizeof(uint16_t)){
      return static_cast<T>((T)htons(data));
    }else if constexpr(sizeof(T) == sizeof(uint32_t)){
      return static_cast<T>((T)htonl(data));
    }

    return static_cast<T>(data);
  }
  
private:
  std::vector<std::byte> m_Bytes;
  size_t m_ReadPos;
};

[[nodiscard]] ENetPacket* create_packet(const Packet& packet,enet_uint32 packet_flags);

void send_packet(ENetPeer *peer, const Packet &packet,enet_uint8 channel_id, enet_uint32 packet_flags);
};     // namespace Network

#endif //!__PACKET_HPP__