#ifndef STREAMMANAGER_H_
#define STREAMMANAGER_H_

#include "CommandProcessor.h"
#include "Recognizer.h"
#include "Stream.h"
#include "bot.h"

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "opencv2/core/core.hpp"

namespace hs {

const std::string CMD_DECK_FORMAT = "%s's current decklist: %s";

const std::string DECK_PICK_FORMAT = "Pick %02i: (%s, %s, %s)";
const std::string DECK_PICKED_FORMAT = " --> %s picked %s";

const std::string MSG_CLASS_POLL = "Which class should %s pick next?";
const int MSG_CLASS_POLL_ERROR_RETRY_COUNT = 1;
const std::string MSG_CLASS_POLL_ERROR = "Could not connect to strawpoll, retrying %d more time(s)";
const std::string MSG_CLASS_POLL_ERROR_GIVEUP = "Could not create a strawpoll";
const std::string MSG_CLASS_POLL_VOTE = "Vote for %s's next class: %s";
const std::string MSG_CLASS_POLL_VOTE_REPEAT = "relink: %s";

const std::string MSG_WINS_POLL = "How many wins do you think %s will be able to achieve with this deck?";
const std::string MSG_WINS_POLL_VOTE = "How many wins do you think %s will get? %s";
const std::string MSG_WINS_POLL_VOTE_REPEAT = "relink: %s";

const std::string MSG_GAME_START = "!score -as %s -vs %s -%s";
const std::string MSG_GAME_END = "!score -%s";

const std::string DEFAULT_DECKURL = "draft is in progress (if I missed draft, have a mod do !deckforcepublish for a partial draft)";
const std::string STATE_PATH = "state.xml";

class CommandProcessor;

class StreamManager {
friend class CommandProcessor;

public:
	struct Deck {
		std::string url;
		std::vector<std::string> cards;
		std::vector<std::vector<std::string> > picks;

		bool isValid() {return !url.empty();}
		void clear() {url=DEFAULT_DECKURL; cards.clear(); picks.clear();}

		unsigned int state;
	};

	struct Game {
		std::string player;
		std::string opponent;
		std::string fs;
		std::string end;
		int wins;
		int losses;

		unsigned int state;
	};

	StreamManager(StreamPtr stream, clever_bot::botPtr bot);
	~StreamManager();
	void loadState();
	void saveState();
	void setStream(StreamPtr stream);
	void startAsyn();
	void wait();
	void run();
	std::string processCommand(const std::string& user, const std::string& cmd, bool isMod, bool isSuperUser);

	std::string getDeckURL();
	void setDeckURL(std::string deckURL);

private:
	std::string createDeckString(Deck deck);
	boost::shared_ptr<CommandProcessor> cp;
	StreamPtr stream;
	clever_bot::botPtr bot;
	RecognizerPtr recognizer;

	bool param_strawpolling;
	bool param_backupscoring;
	unsigned int param_debug_level;

	int numThreads;
	boost::thread_group processingThreads;

	std::string sName; //streamer name
	Deck currentDeck;
	Game currentGame;

	//make sure the execution of commands doesn't interfere with other vars
    boost::mutex stateMutex;

    void disable(unsigned int& state, unsigned int recognizer) {state &= (~recognizer);}
    void enable(unsigned int& state, unsigned int recognizer) {state |= recognizer;}
};

typedef boost::shared_ptr<StreamManager> StreamManagerPtr;

}


#endif /* STREAMMANAGER_H_ */
