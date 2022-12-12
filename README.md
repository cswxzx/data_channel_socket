# data_channel_socket
conversion webrtc data channel to socket

Convert WebRTC's channel to a socket and use the socket to read and write.

## depend
  socket_pair

  https:\/\/github.com\/cswxzx\/socket_pair.git

example

```
#include "data_channel_socket.h"

int main() {
    auto channel = peer_connection->CreateDataChannelOrError(...);
    DataChannelSocket dcs(channel);
    dcs.start();
    SOCKET fd = dcs.get_source_handle();
    recv(fd, ...);
    send(fd, ...);
    wait(...);
    dcs.stop();
    return 0;
}

```
