#include "Game.hpp"

Game::Game() : m_bRunning(true)
{
	nodelay(m_Window.GetWindow(),TRUE); //used to set getch to non-blocking mode
	keypad(m_Window.GetWindow(), true); // initialise the keyboard: we can use arrows for directions
	noecho(); // user input is not displayed on the screen
	curs_set(0);

  m_ClientWrapper.SetAddress("127.0.0.1", 1337);
  m_ClientWrapper.CreateHost(1, 2, 0, 0);

  m_ClientWrapper.ConnectHost(2, 0);

  m_ClientsData.push_back(std::make_shared<ClientData>(ClientData()));
  m_ClientsData.front()->id = 0;
  
  Serializer des;
  if(Serializer::Deserialize(des,"../client_data.cfg")){
    auto& context = des["ClientData"];

    m_ClientsData.front()->sUsername = context["username"].GetAs<std::string>(0);
  }
  m_Snakes.insert(std::make_pair(m_ClientsData.front()->id,Snake()));
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
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientsData.front()->id << Vec2(-1,0) << m_Snakes[m_ClientsData.front()->id].GetPosition();
      break;
    }
    case GM_KEY_RIGHT:{
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientsData.front()->id << Vec2(1,0) << m_Snakes[m_ClientsData.front()->id].GetPosition();
      break;
    }
    case GM_KEY_UP:{
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientsData.front()->id << Vec2(0,-1) << m_Snakes[m_ClientsData.front()->id].GetPosition();
      break;
    }
    case GM_KEY_DOWN:{
      direction_packet << static_cast<uint8_t>(Event::DIRECTION) << m_ClientsData.front()->id << Vec2(0,1) << m_Snakes[m_ClientsData.front()->id].GetPosition();
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
          case Event::CLIENT_CONNECTED:{
            if(m_ClientsData.front()->id == 0){
              packet >> m_ClientsData.front()->id;
              m_Snakes.insert(std::make_pair(m_ClientsData.front()->id,Snake()));
              if(m_Snakes.count(0) > 0){
                m_Snakes.erase(0);
              }

              Network::Packet username_packet;
              username_packet << static_cast<uint8_t>(Event::USERNAME) << m_ClientsData.front()->id << m_ClientsData.front()->sUsername.c_str();
              
              Network::send_packet(m_ClientWrapper.GetPeer(),username_packet,0,ENET_PACKET_FLAG_RELIABLE);
            }
            break;
          }
          case Event::CLIENT_DISCONNECTED:{
            uint32_t disconnected_id{};
            packet >> disconnected_id;

            if(m_Snakes.count(disconnected_id)){
              m_Snakes.erase(disconnected_id);
            }

            auto it = std::find_if(m_ClientsData.begin(),m_ClientsData.end(),[&](const auto& client){
              return (client->id == disconnected_id);
            });

            if(it != m_ClientsData.end()){
              m_ClientsData.erase(it);
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
                uint32_t food_amount{};
                packet >> food_amount;

                for(int i = 0; i < food_amount;i++){
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
            
             std::weak_ptr<ClientData> weak_client = GetClientById(client_id);

            if(auto client = weak_client.lock()){
              client->score = score;
            }

            break;
          }

          case Event::BROADCAST_STATS:{
            uint32_t client_size{};
            packet >> client_size;

            for(size_t i = 0;i<client_size;i++){
              uint32_t client_id{};
              Stats stats;
              packet >> client_id >> stats.Wins >> stats.Losses;
              
              std::weak_ptr<ClientData> weak_client = GetClientById(client_id);

              if(std::shared_ptr<ClientData> client = weak_client.lock()){
                  client->stats = stats;
              }else{
                if(weak_client.expired()){
                  m_ClientsData.push_back(std::make_shared<ClientData>(ClientData()));
                  m_ClientsData.back()->id = client_id;
                  m_ClientsData.back()->stats = stats;
                }
              }
              
            }
            break;
          }
          case Event::USERNAME:{
            uint32_t client_id;
            char username[1024];
            memset(&username,'\0',strlen(username));

            packet >> client_id >> username;

            std::weak_ptr<ClientData> weak_client = GetClientById(client_id);

            if(std::shared_ptr<ClientData> client = weak_client.lock()){
                client->sUsername = username;
            }else{
              if(weak_client.expired()){
                m_ClientsData.push_back(std::make_shared<ClientData>(ClientData()));
                m_ClientsData.back()->id = client_id;
                m_ClientsData.back()->sUsername = username;
              }
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
  if(m_ClientsData.front()->id != 0 && m_Snakes.count(m_ClientsData.front()->id) > 0){
    
    auto& pos = m_Snakes[m_ClientsData.front()->id].GetPosition();
    
    uint8_t event = (uint8_t)Event::BROADCAST_POSITION;
    int x = pos.x;
    int y = pos.y;
    
    Network::Packet packet;
    packet << event << m_ClientsData.front()->id << static_cast<uint8_t>(EntityType::SNAKE) << x << y;

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

  int win_w{m_Window.GetWindowWidth()},win_h{m_Window.GetWindowHeight()};

  for(int i = 0,offset_y = 0;i < m_ClientsData.size();i++,offset_y += 2){
   std::string score_str = std::to_string(m_ClientsData[i]->score);
   auto username = m_ClientsData[i]->sUsername;
   auto wins = std::to_string(m_ClientsData[i]->stats.Wins);
   auto losses = std::to_string(m_ClientsData[i]->stats.Losses);

   std::string msg("Username: " + username + " -"  " Score: " + score_str + "," + " Wins: " + wins + "," + " Losses: " + losses + ".");
   move((win_h / 2) + offset_y,(win_w * 0.6) - (msg.length() / 2));
   printw(msg.c_str());
  }

  m_Map.Render();
  
  refresh();
}