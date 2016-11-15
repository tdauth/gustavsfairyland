/**
 * A simple HTTP server which lists all searching players and connects them.
 *
 * HTTP Commands:
 * "list" - Lists all available games.
 * "connect/id" - Connects to one of the available games.
 * "create" - Creates a new game. Replies with the game ID.
 * "delete/id" - Deletes a game.
 *
 * HTTP Game Commands:
 * "waitforclip" - Requests the next clip in form of a server URL to the file which can be downloaded.
 * "sendclip" - Uploads a clip file for the current game for all other players.
 */
#include <iostream>

#include <QtNetwork>

#include "httpserver.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	myHTTPserver server;

	return a.exec();
}
