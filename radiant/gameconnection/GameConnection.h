#include "icommandsystem.h"

class MessageTcp;

class GameConnection {
public:
	//connect to TDM instance if not connected yet
	//return false if failed to connect
	bool Connect();

	//send given request synchronously
	//returns response text when it becomes available
	std::string Execute(const std::string &request);

	//flush async commands (e.g. camera update) and wait for them to finish
	void Finish();

	static void ReloadMap(const cmd::ArgumentList& args);

private:
	//connection to TDM game
	std::unique_ptr<MessageTcp> _connection;
	//sequence number of the last send request (incremented sequentally)
	int _seqno = 0;

	//nonzero: action with this seqno sent to TDM, answer not recieved yet
	int _seqnoInProgress = 0;
	//response from current in-progress request will be saved here
	std::vector<char> _response;

	//true: cameraOutData has new camera position, should be sent to TDM
	bool _cameraOutPending = false;
	//data for camera position (setviewpos format: X Y Z pitch yaw roll)
	double _cameraOutData[6];


	int NewSeqno() { return ++_seqno; }

	//check how socket is doing, send pending async requests and accept responses
	void Think();

	//if there are any pending async commands (camera update), send one now (non-blocking)
	bool SendAsyncCommand();

	void WaitAction();

};
extern GameConnection g_gameConnection;
