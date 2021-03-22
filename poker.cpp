#include <iostream>
#include <assert.h>
#include <ostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

#define ENUM_LABEL_NAME(n) case n: return #n

class Player;
class Hand;
class Card;
class Game;

static const bool gAbreviate = true;

enum Suit
{
	MinSuit = 0,

	Hearts = 0,
	Clubs,
	Diamonds,
	Spades,

	MaxSuit
};

enum Rank
{
	MinRank = 0,

	Two = 0,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,
	Ten,
	Jack,
	Queen,
	King,
	Ace,
	MaxRank	
};
const char* toString(Rank r)
{
	switch(r)
	{
		ENUM_LABEL_NAME(Ace);
		ENUM_LABEL_NAME(Two);
		ENUM_LABEL_NAME(Three);
		ENUM_LABEL_NAME(Four);
		ENUM_LABEL_NAME(Five);
		ENUM_LABEL_NAME(Six);
		ENUM_LABEL_NAME(Seven);
		ENUM_LABEL_NAME(Eight);
		ENUM_LABEL_NAME(Nine);
		ENUM_LABEL_NAME(Ten);
		ENUM_LABEL_NAME(Jack);
		ENUM_LABEL_NAME(Queen);
		ENUM_LABEL_NAME(King);	
		default:
			assert(!"Invalid Card");
	}
	return "Invalid";
}
const char* toStringShort(Rank r)
{
	switch(r)
	{
		default:
		case MaxRank:
			return "invalid";
		case Two:    return "2";
		case Three:  return "3";
		case Four:   return "4";
		case Five:   return "5";
		case Six:    return "6";
		case Seven:  return "7";
		case Eight:	 return "8";
		case Nine:   return "9";
		case Ten:    return "T";
		case Jack:   return "J";
		case Queen:  return "Q";
		case King:   return "K";
		case Ace:    return "A";
	}
	return "Invalid Rank";
}

const char* toString(Suit r)
{
	switch(r)
	{
		ENUM_LABEL_NAME(Hearts);
		ENUM_LABEL_NAME(Clubs);
		ENUM_LABEL_NAME(Diamonds);
		ENUM_LABEL_NAME(Spades);
		default:
			assert(!"Invalid Card");
	}
	return "Invalid";
}

enum Score
{
	MinScore = 0,

	High_card,     		// track highest rank
	One_pair, 			// (count freq == 2) == 1
	Two_pair, 			// (count freq == 2) == 2
	Three_of_a_kind,    // (count freq == 3) > 0
	Straight, 			// while(freq == 1) length++; length == 5
	Flush,              // find rank freq == 5
	Full_house,         // if all freq
	Four_of_a_kind,     // (count freq) == 4
	Straight_flush,     // while(freq == 1) length++; length == 5 && max(rank freq == 5)
	Royal_flush         // white(freq == 1) length++; length == 5 && maz(rank freq == 5) && startRank == Ten
};

const char* toString(Score s)
{
	switch(s)
	{
		ENUM_LABEL_NAME(High_card);    		// track highest rank
		ENUM_LABEL_NAME(One_pair); 			// (count freq == 2) == 1
		ENUM_LABEL_NAME(Two_pair); 			// (count freq == 2) == 2
		ENUM_LABEL_NAME(Three_of_a_kind);   // (count freq == 3) > 0
		ENUM_LABEL_NAME(Straight); 			// while(freq == 1) length++; length == 5
		ENUM_LABEL_NAME(Flush);             // find rank freq == 5
		ENUM_LABEL_NAME(Full_house);        // if all freq
		ENUM_LABEL_NAME(Four_of_a_kind);    // (count freq) == 4
		ENUM_LABEL_NAME(Straight_flush);    // while(freq == 1) length++; length == 5 && max(rank freq == 5)
		ENUM_LABEL_NAME(Royal_flush);       // white(freq == 1) length++; length == 5 && maz(rank freq == 5) && startRank == Ten
		default:
			assert(!"Invalid Score");
	}
	return "Invalid Score";
}

class Card
{
public:
	explicit Card(int s, int r) 
		: Card(static_cast<Suit>(s), static_cast<Rank>(r) )
	{
		assert(s >= MinSuit && s < MaxSuit );
		assert(r >= MinRank && r < MaxRank );
	}
	explicit Card( Suit s, Rank r) 
		: m_Suit(s), m_Rank(r)
	{}
	
	Rank rank() const { return m_Rank; }
	Suit suit() const { return m_Suit; }

	friend ostream& operator<<(ostream& stream, Card& card);
private:
	Rank    m_Rank;
	Suit  	m_Suit;
};

typedef vector<shared_ptr<Card>> Cards;

class Deck
{
public:
	Deck()
	{
		for( int i = MinSuit; i < MaxSuit; i++ )
		{
			for( int j = MinRank; j < MaxRank; j++ )
			{
				m_Cards.push_back(shared_ptr<Card>(new Card(i,j)));
			}
		}

		assert(m_Cards.size() == 52 );
	}

	void shuffle()
	{
  		srand(time(nullptr));
		for(int i = 0; i < m_Cards.size(); ++i )
		{
			int r = rand() % m_Cards.size();
			swap(m_Cards[r], m_Cards[i]);
		}
	}

	void deal(shared_ptr<Player> hand);
	void deal(Cards& out)
	{
		auto r = m_Cards.back();
		m_Cards.pop_back();
		out.push_back(r);
	}

	friend ostream& operator<<(ostream& stream, Deck& deck);
private:
	Cards m_Cards;
};

template<typename T> T MAX(T a, T b) { return a > b ? a : b; }

class Hand
{
public:
	Hand()
	{}

	Hand(const Card* cards, int nCards)
	{
		for(int i = 0; i < nCards; ++i )
		{
			receiveCardCopy(cards + i);
		}
	}

	Hand(const Cards& cards)
		: m_Cards(cards) 
	{}
	
	void receiveCardCopy(const Card* c)
	{
		// Make copy.
		m_Cards.push_back(shared_ptr<Card>(new Card(*c)));
	}

	void receiveCard(shared_ptr<Card> c)
	{
		m_Cards.push_back(c);
	}

	void showHand() const
	{
		cout << "[";
		for(auto i:m_Cards)
		{
			cout << *i << ", "; 
		}
		cout << "]";
	}
	
	pair<Score,int> score() const
	{
		Cards empty;
		return score(empty);
	}
	pair<Score,int> score(Cards flop) const
	{
		int suitCount[MaxSuit] = {0};		
		int rankCount[MaxRank] = {0};		
		int highestRank = 0;
		int maxRankFreq = 0;

		for( auto i : m_Cards )
		{
			suitCount[ i->suit() ]++;
			rankCount[ i->rank() ]++;
			highestRank = MAX<int>( i->rank(), highestRank );
			maxRankFreq = MAX( rankCount[ i->rank() ], maxRankFreq );
		}

		for( auto i : flop )
		{
			suitCount[ i->suit() ]++;
			rankCount[ i->rank() ]++;
			highestRank = MAX<int>( i->rank(), highestRank );
			maxRankFreq = MAX( rankCount[ i->rank() ], maxRankFreq );
		}

		// High_card,     		// track highest rank
		pair<Score,int> score(High_card,highestRank);

		// Stat frequencies.
		int rankFreqCount[6] = {0};
		int suitFreqCount[6] = {0};
		for( int i = 0; i < MaxRank; ++i )
		{
			rankFreqCount[2] += rankCount[ i ] == 2;
			rankFreqCount[3] += rankCount[ i ] == 3;
			rankFreqCount[4] += rankCount[ i ] == 4;
			suitFreqCount[5] += suitCount[ i ] == 5;
		}
		
		// Assume Pairs.
		if( rankFreqCount[2] == 1 )
			score.first = One_pair;
		else if( rankFreqCount[2] == 2)
			score.first = Two_pair;

		// 3, 4, 5 (flush) of a kind.
		if( rankFreqCount[3] > 0 )
			score.first = Three_of_a_kind;
		if( rankFreqCount[4] > 0 )
			score.first = Four_of_a_kind;

		// Fullhouse.
		if( rankFreqCount[3] && rankFreqCount[2] )
			score.first = Full_house;
		
		// Straights...
		auto begin = find_if( rankCount, rankCount+MaxRank, [](int i) { return i > 0; }	);	
		auto end = find_if( begin, rankCount+MaxRank, [](int i) { return i == 0; } );
		int length = distance(begin, end);
		Rank startingRank = begin != rankCount+MaxRank ? (Rank) distance(rankCount,begin) : MinRank; 

		// Flush.
		if( suitFreqCount[5] > 0 )
		{
			score.first  = Flush;
			score.second = suitFreqCount[5];
		}
		// Straight? Yes?
		if( length == 5 )
		{
			score.first = Straight;
			if( suitFreqCount[5] > 0 )
			{
				score.first = Straight_flush;
		
				if( startingRank == Ten )
				{
					score.first = Royal_flush;
				}
			}
		}

		// Straight?
		//Straight, 			// while(freq == 1) length++; length == 5
		//Full_house,         // if all freq
		//Straight_flush,     // while(freq == 1) length++; length == 5 && max(rank freq == 5)
		//Royal_flush         // white(freq == 1) length++; length == 5 && maz(rank freq == 5) && startRank == Ten
	
		return score;
	}

private:
	Cards m_Cards;
};

class Player
{
public:
	Player(string name)
		: m_Name(name)
		, m_Hand(new Hand() )
	{}

	void receiveCard(shared_ptr<Card> c)
	{
		m_Hand->receiveCard(c);
	}

	void showHand(const Cards& flop)
	{
		cout << m_Name;
		m_Hand->showHand();
		auto s = m_Hand->score(flop);
		cout << toString(s.first) << " : " << s.second;
	}

	const string& name() const
	{
		return m_Name;
	}

	pair<Score,int> score(Cards& cards) const
	{
		return m_Hand->score(cards);
	}

private:
	string m_Name;
	shared_ptr<Hand> m_Hand;
};

class Rules
{
public:
};

class Game
{
public:
	void addPlayer(shared_ptr<Player> player)
	{
		m_Players.push_back(player);
	}

	void
	play()
	{
		static const int dealtCardsPerRound = 2;
		static const int numOfRounds = 4;
		static const int cardsPerRound[numOfRounds] = {0, 3, 1, 1};

		// Shuffle.
		m_Deck.shuffle();

		// Deal x cards. (pre flop).
		for( int i = 0; i < dealtCardsPerRound; ++i )
		{
			for( auto p : m_Players )
			{
				m_Deck.deal(p);
			}
		}

		for( int roundNumber = 0; roundNumber < numOfRounds; ++roundNumber )
		{
			cout << "Round Number: " << (roundNumber+1) << endl;
			
			for( int i = 0; i < cardsPerRound[ roundNumber ]; ++i )
			{
				m_Deck.deal(m_Board);
			}

			for( auto p : m_Players )
			{
				p->showHand(m_Board);
				cout << endl;
			}

			cout << "Board: ";
			for( auto i : m_Board )
			{
				cout << *i << ", ";
			}
			cout << endl;
		}

		// Calculate winner.
		pair<Score,int> score(MinScore,-1);
		shared_ptr<Player> winner(nullptr);
		for( auto i:m_Players )
		{
			pair<Score,int> current = i->score(m_Board);
			if( current.first > score.first || (current.first == score.first && current.second > score.second ) )
			{
				winner = i;
				score = current;
			}
		}
		cout << "Winner: " << winner->name() << endl;
	}

private:
	typedef vector<shared_ptr<Player>> Players;
	Players m_Players;
	Deck m_Deck;
	Cards m_Board;
};

#define CHECK(v,expected) { if(v!=expected) { cout << "Expected: " << toString(expected) << " but received " << toString(v) << endl; exit(-1); } }

int main(int argc, char** argv)
{
	{
		static const Card c[] = { Card(Diamonds,Ace), Card(Diamonds, King), Card(Diamonds, Queen), Card(Diamonds, Jack), Card(Diamonds, Ten) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Royal_flush );
	}
	{
		static const Card c[] = { Card(Diamonds,Nine), Card(Diamonds, King), Card(Diamonds, Queen), Card(Diamonds, Jack), Card(Diamonds, Ten) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Straight_flush );
	}
	{ // Four of a kind.
		static const Card c[] = { Card(Diamonds,Ace), Card(Spades, Ace), Card(Clubs, Ace), Card(Hearts, Ace), Card(Diamonds, Ten) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Four_of_a_kind );
	}
	{ // Full-house.
		static const Card c[] = { Card(Diamonds,Four), Card(Spades, Four), Card(Clubs, Six), Card(Hearts, Six), Card(Diamonds, Six) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Full_house );
	}

	{ // Flush.
		static const Card c[] = { Card(Diamonds,Four), Card(Diamonds, Two), Card(Diamonds, Six), Card(Diamonds, Nine), Card(Diamonds, Ace) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Flush );
	}

	{ // Straight.
		static const Card c[] = { Card(Diamonds,Four), Card(Spades, Five), Card(Clubs, Six), Card(Hearts, Seven), Card(Diamonds, Eight) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Straight );
	}
	{ // Three-of-a-kind.
		static const Card c[] = { Card(Diamonds,Four), Card(Spades, Five), Card(Clubs, Six), Card(Hearts, Six), Card(Diamonds, Six) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Three_of_a_kind );
	}
	{ // Two Pair
		static const Card c[] = { Card(Diamonds,Ace), Card(Spades, Ace), Card(Clubs, Two), Card(Hearts, Two), Card(Diamonds, Ten) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, Two_pair );
	}
	{ // One Pair
		static const Card c[] = { Card(Diamonds,Ace), Card(Spades, Ace), Card(Clubs, Three), Card(Hearts, Two), Card(Diamonds, Ten) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, One_pair );
	}

	{ // High-hand.
		static const Card c[] = { Card(Hearts,King), Card(Clubs, Seven), Card(Hearts, Five), Card(Spades, Three), Card(Clubs, Two) };
		static const Hand h( c, 5 );	
		CHECK( h.score().first, High_card );
	}

	for(long i = 0; i < 1; ++i )
	{
		shared_ptr<Game> game( new Game() );

		game->addPlayer(shared_ptr<Player>(new Player("  Andy: ")));
		game->addPlayer(shared_ptr<Player>(new Player("   Rob: ")));
		game->addPlayer(shared_ptr<Player>(new Player(" James: ")));
		game->addPlayer(shared_ptr<Player>(new Player("Soleil: ")));
		game->addPlayer(shared_ptr<Player>(new Player("   Mum: ")));
		game->addPlayer(shared_ptr<Player>(new Player("   Dad: ")));

		game->play();
 	}	
		
	cout << "Complete" << endl;
	return 0;
}


ostream& operator<<(ostream& stream, Deck& deck)
{
	for( auto i : deck.m_Cards )
		stream << *i << endl;
	return stream; 
}

ostream& operator<<(ostream& stream, Card& card)
{
	if( !gAbreviate )
		stream << toString(card.m_Rank) << " of " << toString(card.m_Suit);
	else
	{
		const char* suitStr = toString(card.m_Suit);
		const char* rankStr = toStringShort(card.m_Rank);
		string shortSuit, shortRank;
		std::transform( rankStr, rankStr+1,back_inserter(shortRank), ::toupper);
		std::transform( suitStr, suitStr+1,back_inserter(shortSuit), ::tolower);
		stream << shortRank << shortSuit;
	}

	return stream; 
}

void Deck::deal(shared_ptr<Player> hand)
{
	auto c = m_Cards.back();
	m_Cards.pop_back();
	hand->receiveCard(c);
}
