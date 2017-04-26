CPPFLAGS=-I/user/cse422b/fs14/include/gstreamer-0.10 \
	-I/user/cse422b/fs14/include/glib-2.0 \
	-I/user/cse422b/fs14/lib/glib-2.0/include \
	-I/user/cse422b/fs14/include/libxml2

CXXFLAGS=$(CPPFLAGS) -g
LDFLAGS=-L/user/cse422b/fs14/lib -lgstreamer-0.10 -lgstapp-0.10  -lglib-2.0 -lgobject-2.0 \
	-Wl,-rpath,/user/cse422b/fs14/lib

CLIENT=streamClient
CLIENT_OBJS=VideoPlayer.o \
	streamClient.o \
	PlaylistEntry.o \
	Playlist.o \
	HTTPMessage.o \
	HTTPRequest.o \
	HTTPResponse.o \
	TCPSocket.o \
	URL.o

TEST_CLIENT=simpleClient
TEST_CLIENT_OBJS=VideoPlayer.o \
	simpleClient.o


all: $(CLIENT) $(TEST_CLIENT)

%.o : %.cc %.h
	g++ -c $< $(CXXFLAGS) -o $@ -DBUFFER_SIZE=40960


$(CLIENT): $(CLIENT_OBJS)
	g++ -o $@ $^ $(LDFLAGS)

$(TEST_CLIENT): $(TEST_CLIENT_OBJS)
	g++ -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(CLIENT) $(CLIENT_OBJS) $(TEST_CLIENT) $(TEST_CLIENT_OBJS)
