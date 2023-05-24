#include "Game.hpp"

Game::Game() : m_bRunning(true)
{
	nodelay(m_Window.GetWindow(),TRUE); //used to set getch to non-blocking mode
	keypad(m_Window.GetWindow(), true); // initialise the keyboard: we can use arrows for directions
	noecho(); // user input is not displayed on the screen
	curs_set(0);

  m_ClientData.id = 0;

  m_ClientWrapper.SetAddress("127.0.0.1", 1337);
  m_ClientWrapper.CreateHost(1, 2, 0, 0);

  m_ClientWrapper.ConnectHost(2, 0);

  Serializer des;
  if(Serializer::Deserialize(des,"../client_data.cfg")){
    auto& context = des["ClientData"];

    m_ClientData.sUsername = context["username"].GetAs<std::string>(0);
  }
  m_Snakes.insert(std::make_pair(m_ClientData.id,Snake()));
}

Game::~Game()
{
  clear();
  m_ClientWrapper.DestroyHost();
  nodelay(m_Window.GetWindow(), false);
	getch();
}

void Game::Play()
{
  using Clock = std::chrono::high_resolution_clock;
  using milliseconds = std::chrono::milliseconds;

  while (m_bRunning)
  {
    auto curr_time = Clock::now();

    InputHandler();
    Update();
    Render();

    int frame_ticks = std::chrono::duration_cast<milliseconds>((Clock::now() - curr_time)).count();
    if(m_frameDelay > frame_ticks){
      usleep((m_frameDelay - frame_ticks) * 1000);
    }
  }

}

void Game::InputHandler()
{
  int8_t key = getch();

  if(key >= 0){
    m_key = (uint8_t)key;
  }

  Network::Packet direction_packet;

  switch(m_key){
    case GM_KEY_LEFT:{
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientData.id << Vec2(-1,0) << m_Snakes[m_ClientData.id].GetPosition();
      break;
    }
    case GM_KEY_RIGHT:{
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientData.id << Vec2(1,0) << m_Snakes[m_ClientData.id].GetPosition();
      break;
    }
    case GM_KEY_UP:{
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientData.id << Vec2(0,-1) << m_Snakes[m_ClientData.id].GetPosition();
      break;
    }
    case GM_KEY_DOWN:{
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientData.id << Vec2(0,1) << m_Snakes[m_ClientData.id].GetPosition();
      break;
    }
  }
  
  if(!direction_packet.IsEmpty()){
    Network::send_packet(m_ClientWrapper.GetPeer(),direction_packet,0,ENET_PACKET_FLAG_RELIABLE);
  }

  auto& event = m_ClientWrapper.GetEvent();
  
  while(enet_host_service(m_ClientWrapper.GetHost(),&event,1) > 0){
    switch(event.type){
      case ENET_EVENT_TYPE_CONNECT:{
        const char* msg = "[info] successfully connected to the server";
        size_t len = strlen(msg);
        m_Window.UpdateResolution();

        move(m_Window.GetWindowHeight() * 0.8f,(m_Window.GetWindowWidth() * 0.6) - (len / 2));
        printw(msg);

        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT:{
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE:{

        Network::Packet packet;
        packet.Append(event.packet->data,event.packet->dataLength);
        
        uint8_t ev;
        packet >> ev;

        switch((Event)ev){
          case Event::CLIENT_ID:{
            if(m_ClientData.id == 0){
              packet >> m_ClientData.id;
              m_Snakes.insert(std::make_pair(m_ClientData.id,Snake()));
              if(m_Snakes.count(0) > 0){
                m_Snakes.erase(0);
              }

              Network::Packet username_packet;
              username_packet << static_cast<uint8_t>(Event::USERNAME) << m_ClientData.id << m_ClientData.sUsername.c_str();
              
              Network::send_packet(m_ClientWrapper.GetPeer(),username_packet,0,ENET_PACKET_FLAG_RELIABLE);
            }
            break;
          }
          case Event::MAP_SIZE:{
            Vec2 map_size;
            packet >> map_size;
            m_Map.SetMapSize(map_size);

            break;
          }
          case Event::BROADCAST_POSITION:{
            uint32_t client_id{};
            uint8_t entity_type;
            packet >> client_id >> entity_type;

            switch((EntityType)entity_type){
              case EntityType::SNAKE:{
                Vec2 pos;
                packet >> pos;

                if(m_Snakes.count(client_id) < 1){
                 m_Snakes.insert(std::make_pair(client_id,Snake()));
                }

                m_Snakes[client_id].SetPosition(pos);
                break;
              }
              case EntityType::FOOD:{
                for(int i = 0;i<8;i++){
                  uint32_t food_id = 0;
                  Vec2 food_pos;
                  packet >> food_id >> food_pos;

                  m_FoodMap.insert(std::make_pair(food_id,Food()));
                  m_FoodMap[food_id].SetPosition(food_pos);
                }
                break;
              }
            }
            break;
          }

          case Event::BROADCAST_SCORE:{
            uint32_t client_id{};

            int score;
            packet >> client_id >> score;

            if(m_GameScoreMap.count(client_id) < 1){
             m_GameScoreMap.insert(std::make_pair(client_id,GameScore()));
            }

            m_GameScoreMap[client_id].SetScore(score);

            break;
          }

          case Event::USERNAME:{
            uint32_t client_id;
            char username[1024];
            memset(&username,'\0',strlen(username));

            packet >> client_id >> username;

            if(m_UserNameMap.count(client_id) < 1){
              m_UserNameMap.insert(std::make_pair(client_id,username));
            }
            break;
          }
        }

        break;
      }
    }
  }
}

void Game::Update()
{
  if(m_ClientData.id != 0 && m_Snakes.count(m_ClientData.id) > 0){
    
    auto& pos = m_Snakes[m_ClientData.id].GetPosition();
    
    uint8_t event = (uint8_t)Event::BROADCAST_POSITION;
    int x = pos.x;
    int y = pos.y;
    
    Network::Packet packet;
    packet << event << m_ClientData.id << static_cast<uint8_t>(EntityType::SNAKE) << x << y;

    Network::send_packet(m_ClientWrapper.GetPeer(),packet,0,ENET_PACKET_FLAG_RELIABLE);
  }

  m_Map.Update();
}

void Game::Render()
{
  for(auto& snake : m_Snakes){
    Map::RenderEntity(m_Map,snake.second);
  }

  for(auto& food : m_FoodMap){
    Map::RenderEntity(m_Map,food.second);
  }
  
  std::vector<std::pair<int,std::string>> scores;
  for(auto& pair : m_GameScoreMap){
    scores.push_back(std::make_pair(pair.second.GetScore(),m_UserNameMap[pair.first]));
  }

  std::sort(scores.begin(), scores.end(),std::greater<>());

  int win_w{m_Window.GetWindowWidth()},win_h{m_Window.GetWindowHeight()};
  
  for(int i = 0,offset = 0;i<scores.size();i++,offset += 2){
    auto score_str = std::to_string(scores[i].first);
    auto username_str = scores[i].second;

    std::string msg("Username: " + username_str + " Score: " + score_str);

    move((win_h * 0.2) + offset,(win_w * 0.6) - (msg.length() / 2));
    printw(msg.c_str());
  }

  m_Map.Render();

  refresh();
}