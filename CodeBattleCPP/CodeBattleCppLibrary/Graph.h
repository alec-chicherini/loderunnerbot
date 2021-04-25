#pragma once
//#define DEBUG_GRAPHFROMMAP
//#define DEBUG_CREATEROUTE
//#define DEBUG_POSSIBLEROUTES
//#define DEBUG_CHECKCURRENTROUTE
//#define DEBUG_FINDCLEARROUTE
#define DEBUG
#include "GameBoard.h"
#include <vector>
#include <tuple>
#include <algorithm>
#include <list>
#include <map>

#ifdef DEBUG
#include "timedelay/timedelay.h"
#include "timedelay/timedelay.cpp"
#endif

using BE = BoardElement;

auto amIShadow = [&](BE B) {
	if (
		B == BE::HERO_SHADOW_DIE ||
		B == BE::HERO_SHADOW_DRILL_LEFT ||
		B == BE::HERO_SHADOW_DRILL_RIGHT ||
		B == BE::HERO_SHADOW_LADDER ||
		B == BE::HERO_SHADOW_LEFT ||
		B == BE::HERO_SHADOW_RIGHT ||
		B == BE::HERO_SHADOW_FALL_LEFT ||
		B == BE::HERO_SHADOW_FALL_RIGHT ||
		B == BE::HERO_SHADOW_PIPE_LEFT ||
		B == BE::HERO_SHADOW_PIPE_RIGHT
		) return true;
	else return false;
};

auto isGold = [&](BE& B) {
	if (
		B == BE::GREEN_GOLD ||
		B == BE::RED_GOLD ||
		B == BE::YELLOW_GOLD ||
		B == BE::THE_SHADOW_PILL
		) return true;
	else return false;
};

auto isRouteClear = [&](BE& B) {
	if (
		B == BE::GREEN_GOLD ||
		B == BE::RED_GOLD ||
		B == BE::YELLOW_GOLD||
		B == BE::LADDER ||
		B == BE::PIPE ||
		B == BE::PORTAL||
		B == BE::NONE||
		B == BE::THE_SHADOW_PILL||
		B == BE::BRICK

		) return true;
	else return false;
};

auto isWalkable = [&](BE B) {
	if (
		B == BE::BRICK ||
		B == BE::INDESTRUCTIBLE_WALL ||
		B == BE::LADDER ||
		B == BE::ENEMY_LADDER ||
		B == BE::HERO_LADDER ||
		B == BE::HERO_SHADOW_LADDER ||
		B == BE::OTHER_HERO_LADDER ||
		B == BE::OTHER_HERO_SHADOW_LADDER
		) return true;
	else return false;
};

auto isNONE = [&](BE B) {
	if (
		B == BE::NONE
		) return true;
	else return false;
};


auto isPipe = [&](BE B) {
	if (
		B == BE::PIPE ||
		B == BE::ENEMY_PIPE_LEFT ||
		B == BE::ENEMY_PIPE_RIGHT ||
		B == BE::HERO_PIPE_LEFT ||
		B == BE::HERO_PIPE_RIGHT ||
		B == BE::HERO_SHADOW_PIPE_LEFT ||
		B == BE::HERO_SHADOW_PIPE_RIGHT ||
		B == BE::OTHER_HERO_PIPE_LEFT ||
		B == BE::OTHER_HERO_PIPE_RIGHT ||
		B == BE::OTHER_HERO_SHADOW_PIPE_LEFT ||
		B == BE::OTHER_HERO_SHADOW_PIPE_RIGHT
		) return true;
	else return false;
};

auto isLikeNONE = [&](BE B) {
	if (
		B == BE::GREEN_GOLD ||
		B == BE::RED_GOLD ||
		B == BE::YELLOW_GOLD ||
		B == BE::THE_SHADOW_PILL ||
		B == BE::PORTAL ||

		B == BE::HERO_DIE ||
		B == BE::HERO_DRILL_LEFT ||
		B == BE::HERO_DRILL_RIGHT ||
		B == BE::HERO_SHADOW_DRILL_LEFT ||
		B == BE::HERO_SHADOW_DRILL_RIGHT ||
		B == BE::HERO_LEFT ||
		B == BE::HERO_RIGHT ||
		B == BE::HERO_SHADOW_LEFT ||
		B == BE::HERO_SHADOW_RIGHT ||
		B == BE::HERO_FALL_LEFT ||
		B == BE::HERO_FALL_RIGHT ||
		B == BE::HERO_SHADOW_FALL_LEFT ||
		B == BE::HERO_SHADOW_FALL_RIGHT ||

		B == BE::OTHER_HERO_DIE ||
		B == BE::OTHER_HERO_SHADOW_DIE ||
		B == BE::OTHER_HERO_LEFT ||
		B == BE::OTHER_HERO_RIGHT ||
		B == BE::OTHER_HERO_SHADOW_LEFT ||
		B == BE::OTHER_HERO_SHADOW_RIGHT ||

		B == BE::ENEMY_LEFT ||
		B == BE::ENEMY_RIGHT

		) return true;
	else return false;
};

auto isLadder = [&](BE B) {
	if (
		B == BE::LADDER ||
		B == BE::ENEMY_LADDER ||
		B == BE::HERO_LADDER ||
		B == BE::HERO_SHADOW_LADDER ||
		B == BE::OTHER_HERO_LADDER ||
		B == BE::OTHER_HERO_SHADOW_LADDER

		) return true;
	else return false;
};

auto isOtherHero = [&](BE B) {
	if (
		
		B == BE::OTHER_HERO_DRILL_LEFT ||
		B == BE::OTHER_HERO_DRILL_RIGHT ||
		B == BE::OTHER_HERO_LEFT ||
		B == BE::OTHER_HERO_RIGHT ||
		B == BE::OTHER_HERO_LADDER ||
	  //B == BE::OTHER_HERO_FALL_LEFT ||
	  //B == BE::OTHER_HERO_FALL_RIGHT ||
		B == BE::OTHER_HERO_PIPE_LEFT ||
		B == BE::OTHER_HERO_PIPE_RIGHT

		) return true;
	else return false;
};

class Graph
{
	
private:
	using Edges = typename std::vector<std::pair<BoardPoint, BoardPoint>>;

	Edges E;
	std::vector<BoardPoint> V;//Vertexes
	Edges currentRoute;
	std::vector<Edges> possibleRoutes;

public:
	void cleanUp() 
	{
		E.clear();
		V.clear();
		currentRoute.clear();
		possibleRoutes.clear();

	};

	void cleanUp_withoutGraph()
	{

		currentRoute.clear();
		possibleRoutes.clear();

	};
	bool isCurrentRouteEmpty() { return currentRoute.empty(); }

	bool isSecondBrick(const GameBoard& board) 
	{
		if (currentRoute.size() > 1) {
			auto second = board.getElementAt(currentRoute[0].second);
			if (second == BE::BRICK)return true;
			else return false;
		}
		else return false;
	};



	auto isReachable (BoardPoint&& BP)
	{
		auto it = std::find_if(V.begin(), V.end(),
			[&](BoardPoint& VertexPoint)
			{
				return ((BP.getX() == VertexPoint.getX()) &&
					(BP.getY() - 1 == VertexPoint.getY()));
			});

		return true;
	};

	bool checkCurrentRoute(const GameBoard& board)
	{
		bool result=true;
#ifdef DEBUG_CHECKCURRENTROUTE
		std::cout << std::endl;
		std::cout << "checkCurrentRoute started" << std::endl;
#endif
		for (auto& r : currentRoute) {
			auto current = board.getElementAt(r.second);
			if (!isRouteClear(current)) {
				result = false;
				break;
			}
		};
		
#ifdef DEBUG_CHECKCURRENTROUTE
		std::cout << "checkCurrentRoute result:" <<std::boolalpha<< result<<std::endl;
#endif
		return result;

	};

	bool findCleanRoute(const GameBoard& board)
	{

		bool result = false;
#ifdef DEBUG_FINDCLEARROUTE
		auto dump = possibleRoutes;
		timedelay timers;
		timers.addTimer("findCleanRoute");
		std::cout << std::endl << "findeCleanRoute started:" << std::endl;
		int skip_counter = 1;
#endif
		if (possibleRoutes.size())
		{
			possibleRoutes.erase(possibleRoutes.begin());
			currentRoute = possibleRoutes[0];
		}
		

		while (possibleRoutes.size())
		{
			if (!checkCurrentRoute(board))
			{
#ifdef DEBUG_FINDCLEARROUTE
				skip_counter++;
#endif
				possibleRoutes.erase(possibleRoutes.begin());
				if(possibleRoutes.size())
				  currentRoute = possibleRoutes[0];
			}
			else { result = true; break; }
		}

#ifdef DEBUG_FINDCLEARROUTE
		std::cout << std::endl << "findeCleanRoute skipped = " << skip_counter
						       <<" result = "<<std::boolalpha<<result
							   <<" time passed :"<< timers.readTimer("findCleanRoute") 
							   << std::endl;
		//int counter_1=0;
		//if(result==false)
		//	for (auto& r : dump)
		//	{
		//		std::cout << "possibleRoutes [" << counter_1++ << "] = ";
		//		for (auto& v : r)std::cout << "([" << v.first.getX() << "," << v.first.getY()
		//							       << "],["<< v.second.getX() << "," << v.second.getY() << "])-->";
		//		std::cout << std::endl;
		//	}
#endif
		
		return result;
	};

	BoardPoint makeStep()
	{
		if (currentRoute.size() == 0)return BoardPoint(0, 0);
		auto step = currentRoute.front().second;
		currentRoute.erase(currentRoute.begin());
		return std::move(step);
	};

	inline Edges AdjacencyList(BoardPoint BP)
	{
		Edges res;
		for (auto& e : E)
		{
			if (e.first == BP) res.push_back(e);
		}
		return res;
	};

	void createRoutesWithBFS(const BoardPoint& current, const GameBoard& board, bool amIshadow, float timer_value=0.5f)
	{
#ifdef DEBUG
		timedelay timers;
		timers.addTimer("BFS");
		std::cout <<std::endl<< "Finding routes started " << std::endl;
#endif
		currentRoute.clear();

		auto BoardPointComparator = [&](const BoardPoint& BP1, const BoardPoint& BP2) 
		{
			if (BP1.getX() != BP2.getX())return (BP1.getX() < BP2.getX());
			else return (BP1.getY() < BP2.getY());
			
		};
		std::map<BoardPoint, Edges,decltype(BoardPointComparator)> allRoutes(BoardPointComparator);
		
		std::list<BoardPoint> queue;
		std::vector<std::vector<bool>> visited; 
		std::vector<std::vector<bool>> inqueue;

		visited.resize(board.getBoardSize());
		for (auto& v : visited)v.resize(board.getBoardSize(),false);

		inqueue.resize(board.getBoardSize());
		for (auto& v : inqueue)v.resize(board.getBoardSize(),false);

		queue.push_front(current);

		inqueue[current.getX()][current.getY()] = true;

		bool timePassed = false;
		bool routeComplete = false;
		possibleRoutes.clear();
		currentRoute.clear();

		while (queue.size() && !timePassed)
		{

			auto c =queue.back(); queue.pop_back();

			inqueue[c.getX()][c.getY()] = false;
			visited[c.getX()][c.getY()] = true;

			auto AdjList = AdjacencyList(c);
			for (auto& p : AdjList)
			{
				auto routeToP = [&]
				{
					Edges res;

					if(p.first!=current)
					res = allRoutes[p.first];

					res.push_back(p);

					return std::move(res);
				};

				allRoutes[p.second] = routeToP();

				auto CURRENT = board.m_map[p.second.getY()][p.second.getX()];

				if (!amIshadow) {
					if (isGold(CURRENT))
					{
						routeComplete = true;

						possibleRoutes.push_back(allRoutes[p.second]);

					


#ifdef DEBUG_CREATEROUTE
						std::cout << "Gold found at:" << "[" << p.second.getX() << "," << p.second.getY() << "]" << std::endl;
#endif
					}
				}
				else
				{
					if (isGold(CURRENT)||isOtherHero(CURRENT))
					{
						routeComplete = true;

						possibleRoutes.push_back(allRoutes[p.second]);

						


#ifdef DEBUG_CREATEROUTE
						std::cout << "Gold found at:" << "[" << p.second.getX() << "," << p.second.getY() << "]" << std::endl;
#endif
					}
				}

				if (!visited[p.second.getX()][p.second.getY()] &&
					!inqueue[p.second.getX()][p.second.getY()])
				{
					queue.push_front(p.second);
					inqueue[p.second.getX()][p.second.getY()] = true;
				}
#ifdef DEBUG_CREATEROUTE_2
				std::cout << "allRoutes map begin" << std::endl;
				for (auto& m : allRoutes)
				{
					std::cout << "[" << m.first.getX() << "," << m.first.getY() << "]";
					for(auto&v:m.second)
						std::cout << "([" << v.first.getX() << "," << v.first.getY() << "],[" << v.second.getX() << "," << v.second.getY() << "])-->";

					std::cout << std::endl;
				}
				std::cout << "allRoutes map end" << std::endl;
#endif
				
				if (timers.readTimer("BFS") > timer_value)
				{
					timePassed = true;
				}
				else 
				{
					routeComplete = false;
				}
			}

#ifdef DEBUG_CREATEROUTE
			std::cout << "queue.size() = " << queue.size() << " routeComplete = " << routeComplete << std::endl;
#endif

		}	//main while bfs search cycle

		if(possibleRoutes.size()>0)
		currentRoute = possibleRoutes[0];

#ifdef DEBUG_POSSIBLEROUTES

		int counter = 0;
		std::cout << "possibleRoutes :";
		for (auto& r : possibleRoutes)
		{
			std::cout << "[" << counter++ << "].size()= " << r.size() << " -- ";
		}
		std::cout << std::endl;

#endif

#ifdef DEBUG

		std::cout <<"Finding routes finished. Possible routes: " << possibleRoutes.size()
				  <<" Time passed:"<< timers.readTimer("BFS") << " s " << std::endl;
		//this->print();
#endif
	};
	
	bool BuildGraphFromMap(const GameBoard& board)
	{

#ifdef DEBUG
		timedelay timers;
		timers.addTimer("started");
#endif
		std::cout << "building graph started" << std::endl;

		int SIZE = board.m_map.size();
		for (int row = 0; row < SIZE; row++)
			for (int column = 0; column < SIZE; column++)
			{
#ifdef DEBUG_GRAPHFROMMAP

		std::cout << "[" << column << "," << row << "] => ";
#endif
				auto CURRENT = board.m_map[row][column];
				if (CURRENT == BE::INDESTRUCTIBLE_WALL) {
#ifdef DEBUG_GRAPHFROMMAP

					std::cout << " BE::INDESTRUCTIBLE_WALL" << std::endl;
#endif
					continue;
				}

				auto L = board.m_map[row][column - 1];
				auto R = board.m_map[row][column + 1];
				auto T = board.m_map[row - 1][column];
				auto B = board.m_map[row + 1][column];

				auto BL = board.m_map[row + 1][column - 1];
				auto BR = board.m_map[row + 1][column + 1];

				//current == none
			    if ((CURRENT == BE::NONE) || (isLikeNONE(CURRENT)))
				{
#ifdef DEBUG_GRAPHFROMMAP

					std::cout << " BE::NONE => ";
#endif
					if (isWalkable(B))
					{
						if (L != BE::BRICK && L != BE::INDESTRUCTIBLE_WALL)
						{
							E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column - 1,row }));
							V.push_back(BoardPoint{ column - 1,row });
#ifdef DEBUG_GRAPHFROMMAP

							std::cout << " V.push_back( " << column - 1<<","<<row<<") =>";
#endif
						};
						if (R != BE::BRICK && R != BE::INDESTRUCTIBLE_WALL)
						{
							E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column + 1,row }));
							V.push_back(BoardPoint{ column + 1,row });
#ifdef DEBUG_GRAPHFROMMAP

							std::cout << " V.push_back( " << column + 1 << "," << row << ") =>";
#endif
						};
					}
					 if (isLadder(B)||(B == BE::NONE) || isPipe(B) || isLikeNONE(B))
					{
						if (isReachable(BoardPoint{ column,row }))
						{
							E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
							V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

							std::cout << " V.push_back( " << column  << "," << row+1 << ") =>";
#endif
						};

					}

					if (B == BE::BRICK)
					{
						if (isWalkable(BL) || isWalkable(BR))
						{
							E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
							V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

							std::cout << " V.push_back( " << column << "," << row + 1 << ") =>";
#endif
						};

					};
#ifdef DEBUG_GRAPHFROMMAP

					std::cout << std::endl;
#endif
				}

				//current == ladder
				else if (isLadder(CURRENT))
				{
#ifdef DEBUG_GRAPHFROMMAP

					std::cout << "BE::LADDER =>";
#endif
					if (L != BE::BRICK && L != BE::INDESTRUCTIBLE_WALL)
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column - 1,row }));
						V.push_back(BoardPoint{ column - 1,row });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column -1 << "," << row  << ") =>";
#endif
					};
					if (R != BE::BRICK && R != BE::INDESTRUCTIBLE_WALL)
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column + 1,row }));
						V.push_back(BoardPoint{ column + 1,row });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column + 1 << "," << row << ") =>";
#endif
					};
					if (isLadder(T))
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column ,row - 1 }));
						V.push_back(BoardPoint{ column , row - 1 });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column  << "," << row-1 << ") =>";
#endif
					};
					if (isLadder(B))
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column ,row + 1 }));
						V.push_back(BoardPoint{ column , row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column  << "," << row+1 << ") =>";
#endif
					}
					else if ((B == BE::NONE) || isPipe(B) || isLikeNONE(B))
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
						V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column  << "," << row+1 << ") =>";
#endif
					}
					else if (B == BE::BRICK)
					{
						if (isWalkable(BL) || isWalkable(BR))
						{
							E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
							V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

							std::cout << " V.push_back( " << column  << "," << row +1<< ") =>";
#endif
						};

					};
#ifdef DEBUG_GRAPHFROMMAP

					std::cout << std::endl;
#endif

				}

				//current == pipe
				else if (isPipe(CURRENT))
				{
#ifdef DEBUG_GRAPHFROMMAP

					std::cout << "BE::PIPE =>";
#endif
					if (L != BE::BRICK && L != BE::INDESTRUCTIBLE_WALL)
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column - 1,row }));
						V.push_back(BoardPoint{ column - 1,row });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column-1 << "," << row  << ") =>";
#endif
					};
					if (R != BE::BRICK && R != BE::INDESTRUCTIBLE_WALL)
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column + 1,row }));
						V.push_back(BoardPoint{ column + 1,row });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column + 1 << "," << row << ") =>";
#endif
					};

					 if ((B == BE::NONE) || isPipe(B) || isLikeNONE(B))
					{
						E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
						V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

						std::cout << " V.push_back( " << column  << "," << row+1 << ") =>";
#endif
					}
					else if (B == BE::BRICK)
					{
						if (isWalkable(BL) || isWalkable(BR))
						{
							E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
							V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

							std::cout << " V.push_back( " << column << "," << row+1 << ") =>";
#endif
						};

					};
#ifdef DEBUG_GRAPHFROMMAP

					 std::cout << std::endl;
#endif

				}

				else if (CURRENT == BE::BRICK)
				{
#ifdef DEBUG_GRAPHFROMMAP

				std::cout << "BE::BRICK =>" ;
#endif
					if (isReachable(BoardPoint{ column,row }))
					{
						if ((B == BE::NONE) || isPipe(B) || isLikeNONE(B)||isLadder(B))
						{
							E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
							V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

							std::cout << " V.push_back( " << column << "," << row + 1 << ") =>";
#endif
						}
						else if (B == BE::BRICK)
						{
							if (
								isReachable(BoardPoint{ column - 1,row }) && isWalkable(BL) ||
								isReachable(BoardPoint{ column + 1,row }) && isWalkable(BR)
								)
							{
								E.push_back(std::make_pair(BoardPoint{ column,row }, BoardPoint{ column,row + 1 }));
								V.push_back(BoardPoint{ column,row + 1 });
#ifdef DEBUG_GRAPHFROMMAP

								std::cout << " V.push_back( " << column << "," << row + 1 << ") =>";
#endif

							};
						}
					}
#ifdef DEBUG_GRAPHFROMMAP

					std::cout << std::endl;
#endif
				}
			};
			
			//delete duplicates
			std::sort(V.begin(), V.end(), [&](BoardPoint& BP1, BoardPoint& BP2)
				{
					if (BP1.getX() != BP2.getX())return (BP1.getX() < BP2.getX());
					else return (BP1.getY() < BP2.getY());
						
				}
			);

			V.erase(std::unique(V.begin(), V.end()), V.end());

#ifdef DEBUG
			
			std::cout<<"Building graph finished : "<<timers.readTimer("started")<<" s "<<std::endl;
			//this->print();
#endif
			return true;
	};//BuildGraphFromMap

	void print()
	{
		std::cout << "V = ";
		for (auto& v : V)std::cout << "[" << v.getX() << "," << v.getY() << "],";
		std::cout << std::endl;
	};

	void printCurrentRoute()
	{
		std::cout << "currentRoute = ";
		for (auto& v : currentRoute)std::cout << "([" << v.first.getX() << "," << v.first.getY() << "],[" << v.second.getX() << "," << v.second.getY() << "])-->";
		std::cout << std::endl;
	};


};

